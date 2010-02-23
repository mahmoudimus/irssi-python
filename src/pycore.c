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
#include <string.h>
#include <signal.h>
#include <assert.h>
#include "pyirssi.h"
#include "pycore.h"
#include "pyloader.h"
#include "pymodule.h"
#include "pysignals.h"
#include "pythemes.h"
#include "pystatusbar.h"
#include "pyconstants.h"
#include "factory.h"

static void cmd_default(const char *data, SERVER_REC *server, void *item)
{
    if (!*data)
        data = "list";

    command_runsub("py", data, server, item);
}

static void cmd_exec(const char *data)
{
    PyObject *co;
    PyObject *ret;
    PyObject *d;
    PyObject *m;
    char *cmd;

    if (!*data)
        cmd_return_error(CMDERR_NOT_ENOUGH_PARAMS);

    cmd = g_strconcat(data, "\n", NULL);
    
    m = PyImport_AddModule("__main__");
    if (!m)
        goto error;

    d = PyModule_GetDict(m);
    if (!d)
        goto error;

    co = Py_CompileString(cmd, "<stdin>", Py_single_input);
    if (!co)
        goto error;

    ret = PyEval_EvalCode((PyCodeObject *)co, d, d);
    Py_DECREF(co);
    Py_XDECREF(ret);

error:
    g_free(cmd);
    if (PyErr_Occurred())
        PyErr_Print();
}

static void cmd_load(const char *data)
{
    char **argv;

    argv = g_strsplit(data, " ", -1);
    if (*argv == NULL || **argv == '\0')
    {
        g_strfreev(argv);
        cmd_return_error(CMDERR_NOT_ENOUGH_PARAMS);
    }

    pyloader_load_script_argv(argv);
    g_strfreev(argv);
}

static void cmd_unload(const char *data)
{
    void *free_arg;
    char *script;

    if (!cmd_get_params(data, &free_arg, 1, &script))
        return;

    if (*script == '\0')
        cmd_param_error(CMDERR_NOT_ENOUGH_PARAMS);

    pyloader_unload_script(script); 
    
    cmd_params_free(free_arg);
}

static void cmd_list()
{
    char buf[128];
    GSList *list;

    list = pyloader_list();

    g_snprintf(buf, sizeof(buf), "%-15s %s", "Name", "File");

    if (list != NULL)
    {
        GSList *node;

        printtext_string(NULL, NULL, MSGLEVEL_CLIENTCRAP, buf);
        for (node = list; node != NULL; node = node->next)
        {
            PY_LIST_REC *item = node->data;
            g_snprintf(buf, sizeof(buf), "%-15s %s", item->name, item->file); 

            printtext_string(NULL, NULL, MSGLEVEL_CLIENTCRAP, buf);
        }
    }
    else
        printtext_string(NULL, NULL, MSGLEVEL_CLIENTERROR, "No python scripts are loaded");

    pyloader_list_destroy(&list);
}

#if 0
/* why doesn't this get called? */
static void intr_catch(int sig)
{
    printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "got sig %d", sig);
    PyErr_SetInterrupt();
}
#endif 

void python_init(void)
{
    Py_InitializeEx(0);

    pysignals_init();
    pystatusbar_init();
    if (!pyloader_init() || !pymodule_init() || !factory_init() || !pythemes_init()) 
    {
        printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, "Failed to load Python");
        return;
    }
    pyconstants_init();

    /*PyImport_ImportModule("irssi_startup");*/
    /* Install the custom output handlers, import hook and reload function */
    /* XXX: handle import error */
    PyRun_SimpleString(
            "import irssi_startup\n"
    );

    pyloader_auto_load();
    
    /* assert(signal(SIGINT, intr_catch) != SIG_ERR); */
    
    command_bind("py", NULL, (SIGNAL_FUNC) cmd_default);
    command_bind("py load", NULL, (SIGNAL_FUNC) cmd_load);
    command_bind("py unload", NULL, (SIGNAL_FUNC) cmd_unload);
    command_bind("py list", NULL, (SIGNAL_FUNC) cmd_list);
    command_bind("py exec", NULL, (SIGNAL_FUNC) cmd_exec);
    module_register(MODULE_NAME, "core");
}

void python_deinit(void)
{
    command_unbind("py", (SIGNAL_FUNC) cmd_default);
    command_unbind("py load", (SIGNAL_FUNC) cmd_load);
    command_unbind("py unload", (SIGNAL_FUNC) cmd_unload);
    command_unbind("py list", (SIGNAL_FUNC) cmd_list);
    command_unbind("py exec", (SIGNAL_FUNC) cmd_exec);

    pymodule_deinit();
    pyloader_deinit();
    pystatusbar_deinit();
    pysignals_deinit();
    Py_Finalize();
}
