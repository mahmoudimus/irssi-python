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
#include "pyirssi.h"
#include "pymodule.h"
#include "theme-object.h"
#include "factory.h"
#include "pycore.h"

/* monitor "theme destroyed" signal */
static void theme_cleanup(THEME_REC *rec)
{
    PyTheme *pytheme = signal_get_user_data();
    if (pytheme->data == rec)
    {
        pytheme->data = NULL;
        pytheme->cleanup_installed = 0;
        signal_remove_data("theme destroyed", theme_cleanup, pytheme);
    }
}

static void PyTheme_dealloc(PyTheme *self)
{
    if (self->cleanup_installed)
        signal_remove_data("theme destroyed", theme_cleanup, self);
    
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyTheme_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTheme *self;

    self = (PyTheme *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
/* specialized getters/setters */
static PyGetSetDef PyTheme_getseters[] = {
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyTheme_format_expand_doc,
    "format_expand(format, flags=0) -> str or None\n"
);
static PyObject *PyTheme_format_expand(PyTheme *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"format", "flags", NULL};
    char *format = "";
    int flags = 0;
    char *ret;
    PyObject *pyret;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, 
           &format, &flags))
        return NULL;

    if (flags == 0)
        ret = theme_format_expand(self->data, format);
    else
        ret = theme_format_expand_data(self->data, (const char **)&format, 'n', 'n',
                NULL, NULL, EXPAND_FLAG_ROOT | flags);

    if (ret)
    {
        pyret = PyString_FromString(ret);
        g_free(ret);
        return pyret;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyTheme_get_format_doc,
    "get_format(module, tag) -> str\n"
);
static PyObject *PyTheme_get_format(PyTheme *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"module", "tag", NULL};
    char *module = "";
    char *tag = "";
    THEME_REC *theme = self->data;
    FORMAT_REC *formats;
    MODULE_THEME_REC *modtheme; 
    int i;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, 
           &module, &tag))
        return NULL;

    formats = g_hash_table_lookup(default_formats, module);
    if (!formats)
        return PyErr_Format(PyExc_KeyError, "unknown module, %s", module);

    for (i = 0; formats[i].def; i++)
    {
        if (formats[i].tag && !g_strcasecmp(formats[i].tag, tag))
        { 
            modtheme = g_hash_table_lookup(theme->modules, module);
            if (modtheme && modtheme->formats[i])
                return PyString_FromString(modtheme->formats[i]);
            else 
                return PyString_FromString(formats[i].def);
        }
    }
   
    return PyErr_Format(PyExc_KeyError, "unknown format tag, %s", tag);    
}

/* Methods for object */
static PyMethodDef PyTheme_methods[] = {
    {"format_expand", (PyCFunction)PyTheme_format_expand, METH_VARARGS | METH_KEYWORDS,
        PyTheme_format_expand_doc},
    {"get_format", (PyCFunction)PyTheme_get_format, METH_VARARGS | METH_KEYWORDS,
        PyTheme_get_format_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyThemeType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Theme",            /*tp_name*/
    sizeof(PyTheme),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyTheme_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    0,           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyTheme_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyTheme_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyTheme_new,                 /* tp_new */
};

/* Theme factory function */
PyObject *pytheme_new(void *td)
{
    PyTheme *pytheme;

    pytheme = py_inst(PyTheme, PyThemeType);
    if (!pytheme)
        return NULL;

    pytheme->data = td;
    signal_add_last_data("theme destroyed", theme_cleanup, pytheme);
    pytheme->cleanup_installed = 1;

    return (PyObject *)pytheme;
}

int theme_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyThemeType) < 0)
        return 0;
    
    Py_INCREF(&PyThemeType);
    PyModule_AddObject(py_module, "Theme", (PyObject *)&PyThemeType);

    return 1;
}
