/* 
    irssi-python

    Copyright (C) 2006 Christopher Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <Python.h>
#include <frameobject.h>
#include <string.h>
#include "pyirssi.h"
#include "pyloader.h"
#include "pyutils.h"
#include "pyscript-object.h"

/* List of loaded modules */
static PyObject *script_modules;

/* List of load paths for scripts */
static GSList *script_paths = NULL;

static PyObject *py_get_script(const char *name, int *id);
static int py_load_module(PyObject *module, const char *path);
static char *py_find_script(const char *name);

/* Add to the list of script load paths */
void pyloader_add_script_path(const char *path)
{
    PyObject *ppath = PySys_GetObject("path");
    if (ppath)
    {
        PyList_Append(ppath, PyString_FromString(path));
        script_paths = g_slist_append(script_paths, g_strdup(path));
    }
}

/* Loads a file into a module; it is not inserted into sys.modules */
static int py_load_module(PyObject *module, const char *path) 
{
    PyObject *dict, *ret, *fp;

    if (PyModule_AddStringConstant(module, "__file__", (char *)path) < 0)
        return 0;
    
    dict = PyModule_GetDict(module);
    
    if (PyDict_SetItemString(dict, "__builtins__", PyEval_GetBuiltins()) < 0)
        return 0;

    /* Dont use the standard library to avoid incompatabilities with 
       the FILE structure and Python */
    fp = PyFile_FromString((char *)path, "r");
    if (!fp)
        return 0;

    ret = PyRun_File(PyFile_AsFile(fp), path, Py_file_input, dict, dict);
    Py_DECREF(fp);  /* XXX: I assume that the file is closed when refs drop to zero? */ 
    if (!ret)
        return 0;

    Py_DECREF(ret);
    return 1;

}

/* looks up name in Irssi script directories
   returns full path or NULL if not found */
static char *py_find_script(const char *name)
{
    GSList *node;
    char *fname;
    char *path = NULL;

    //XXX: what if there's another ext?
    if (!file_has_ext(name, "py"))
        fname = g_strdup_printf("%s.py", name);
    else
        fname = (char *)name;
    
    /*XXX: use case insensitive path search? */
    for (node = script_paths; node && !path; node = node->next)
    {
        path = g_strdup_printf("%s/%s", (char *)node->data, fname);

        if (!g_file_test(path, G_FILE_TEST_IS_REGULAR))
        { 
            g_free(path);
            path = NULL;
        }
    }

    if (fname != name)
        g_free(fname);

    return path;
}

/* Load a script manually using PyRun_File.
 * This expects a null terminated array of strings 
 * (such as from g_strsplit) of the command line.
 * The array needs at least one item
 */
static int py_load_script_path_argv(const char *path, char **argv)
{
    PyObject *module = NULL, *script = NULL;
    char *name = NULL; 

    name = file_get_filename(path);
    module = PyModule_New(name);
    g_free(name);
    if (!module)
        goto error;

    script = pyscript_new(module, argv);
    Py_DECREF(module);
    if (!script)
        goto error;
   
    /* insert script obj into module dict, load file */
    if (PyModule_AddObject(module, "_script", script) != 0)
        goto error;

    Py_INCREF(script);
    
    if (!py_load_module(module, path))
        goto error;
    
    if (PyList_Append(script_modules, script) != 0)
        goto error;

    printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "loaded script %s", argv[0]); 
    /* PySys_WriteStdout("load %s, script -> 0x%x\n", argv[0], script); */

    Py_DECREF(script);

    return 1;

error:
    if (PyErr_Occurred())
        PyErr_Print();
    
    printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "error loading script %s", argv[0]); 

    if (script)
    {
        /* make sure to clean up any formats, signals, commands that may have been
           attached before the exception took place */
        pyscript_cleanup(script);
        Py_DECREF(script);
    }
    
    return 0;
}

static int py_load_script_path(const char *path)
{
    int ret;
    char *argv[2];

    argv[0] = file_get_filename(path);
    argv[1] = NULL;

    if (py_get_script(argv[0], NULL) != NULL)
        pyloader_unload_script(argv[0]);

    ret = py_load_script_path_argv(path, argv);
    g_free(argv[0]);

    return ret;
}

int pyloader_load_script_argv(char **argv)
{
    char *path;
    int ret;

    if (py_get_script(argv[0], NULL) != NULL)
        pyloader_unload_script(argv[0]);
   
    path = py_find_script(argv[0]);
    if (!path)
    {
        printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "script %s does not exist", argv[0]); 
        return 0;
    }

    ret = py_load_script_path_argv(path, argv);
    g_free(path);

    return ret;
}

int pyloader_load_script(char *name)
{
    char *argv[2];

    argv[0] = name;
    argv[1] = NULL;

    return pyloader_load_script_argv(argv);
}

static PyObject *py_get_script(const char *name, int *id)
{
    int i;

    g_return_val_if_fail(script_modules != NULL, NULL);
    
    for (i = 0; i < PyList_Size(script_modules); i++)
    {
        PyObject *script;
        char *sname;

        script = PyList_GET_ITEM(script_modules, i);
        sname = pyscript_get_name(script);

        if (sname && !strcmp(sname, name))
        {
            if (id)
                *id = i;
            return script;
        }
    }

    return NULL;
}

int pyloader_unload_script(const char *name)
{
    int id;
    PyObject *script = py_get_script(name, &id);

    if (!script)
    {
        printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "%s is not loaded", name); 
        return 0;
    }

    /* PySys_WriteStdout("unload %s, script -> 0x%x\n", name, script); */
    
    pyscript_cleanup(script);

    if (PySequence_DelItem(script_modules, id) < 0)
    {
        PyErr_Print();
        printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "error unloading script %s", name); 
        return 0;
    }

    /* Probably a good time to call the garbage collecter to clean up reference cycles */
    PyGC_Collect();
    printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "unloaded script %s", name); 
    
    return 1; 
}

/* Traverse stack backwards to find the nearest valid _script object in globals */
PyObject *pyloader_find_script_obj(void)
{
    PyFrameObject *frame;

    for (frame = PyEval_GetFrame(); frame != NULL; frame = frame->f_back)
    {
        PyObject *script;

        g_return_val_if_fail(frame->f_globals != NULL, NULL);
        script = PyDict_GetItemString(frame->f_globals, "_script");

        if (script && pyscript_check(script))
        {
            /*
            PySys_WriteStdout("Found script at %s in %s, script -> 0x%x\n",
                    PyString_AS_STRING(frame->f_code->co_name), 
                    PyString_AS_STRING(frame->f_code->co_filename), script);
            */
            return script;
        }
    }

    return NULL;
}

char *pyloader_find_script_name(void)
{
    PyObject *script = pyloader_find_script_obj();

    if (!script)
        return NULL;

    return pyscript_get_name(script);
}

GSList *pyloader_list(void)
{
    int i;
    GSList *list = NULL;

    g_return_val_if_fail(script_modules != NULL, NULL);

    for (i = 0; i < PyList_Size(script_modules); i++)
    {
        PyObject *scr;
        char *name, *file;

        scr = PyList_GET_ITEM(script_modules, i);
        name = pyscript_get_name(scr);
        file = pyscript_get_filename(scr);

        if (name && file)
        {
            PY_LIST_REC *rec;
            rec = g_new0(PY_LIST_REC, 1);

            rec->name = g_strdup(name);
            rec->file = g_strdup(file);
            list = g_slist_append(list, rec); 
        }
    }

    return list;
}

void pyloader_list_destroy(GSList **list)
{
    GSList *node;

    if (*list == NULL)
        return;
    
    for (node = *list; node != NULL; node = node->next)
    {
        PY_LIST_REC *rec = node->data;

        g_free(rec->name);
        g_free(rec->file);
        g_free(rec);
    }
   
    g_slist_free(*list);
    
    *list = NULL;
}

void pyloader_auto_load(void)
{
    GSList *node;
    char *autodir;
    const char *name; 
    GDir *gd;
    
    for (node = script_paths; node; node = node->next)
    {
        autodir = g_strdup_printf("%s/autorun", (char *)node->data);
        gd = g_dir_open(autodir, 0, NULL);
        g_free(autodir);

        if (!gd)
            continue;

        while ((name = g_dir_read_name(gd)))
        {
            char *path = g_strdup_printf("%s/autorun/%s", (char*)node->data, name);

            if (!strcmp(file_get_ext(name), "py"))
                py_load_script_path(path);

            g_free(path);
        }
        
        g_dir_close(gd);
    }
}

int pyloader_init(void)
{
    char *pyhome;

    g_return_val_if_fail(script_paths == NULL, 0);
    g_return_val_if_fail(script_modules == NULL, 0);

    script_modules = PyList_New(0);
    if (!script_modules)
        return 0;
    
    /* Add script location to the load path */
    pyhome = g_strdup_printf("%s/scripts", get_irssi_dir());
    pyloader_add_script_path(pyhome);
    g_free(pyhome);
   
    /* typically /usr/local/share/irssi/scripts */
    pyloader_add_script_path(SCRIPTDIR);
   
    return 1;
}

static void py_clear_scripts()
{
    int i;
    
    for (i = 0; i < PyList_Size(script_modules); i++)
    {
        PyObject *scr = PyList_GET_ITEM(script_modules, i);
        pyscript_cleanup(scr);
    }

    Py_DECREF(script_modules);
}

void pyloader_deinit(void)
{
    GSList *node;

    g_return_if_fail(script_paths != NULL);
    g_return_if_fail(script_modules != NULL);

    for (node = script_paths; node != NULL; node = node->next)
        g_free(node->data);
    g_slist_free(script_paths);
    script_paths = NULL;

    py_clear_scripts();
}
