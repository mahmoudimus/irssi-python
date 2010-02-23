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
#include "pyirssi_irc.h"
#include "pymodule.h"
#include "textdest-object.h"
#include "factory.h"
#include "pycore.h"

static int pytextdest_setup(PyTextDest *pytdest, void *td, int owned);

/* XXX: no cleanup signal for textdest */
static void PyTextDest_dealloc(PyTextDest *self)
{
    Py_XDECREF(self->window);
    Py_XDECREF(self->server);
    
    if (self->owned)
    {
        g_free((char*)self->data->target);
        g_free(self->data);
    }
    
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyTextDest_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyTextDest *self;

    self = (PyTextDest *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* init function to create the textdest */
PyDoc_STRVAR(PyTextDest_doc,
    "__init__(target, level=MSGLEVEL_CLIENTNOTICE, server=None, window=None)\n"
    "\n"
    "Create a TextDest\n"
);
static int PyTextDest_init(PyTextDest *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"target", "level", "server", "window", NULL};
    char *target;
    int level = MSGLEVEL_CLIENTNOTICE;
    PyObject *server = NULL, *window = NULL;
    TEXT_DEST_REC *dest;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|ioo", kwlist, 
            &target, &level, &server, &window))
        return -1;
 
    if (server == Py_None)
        server = NULL;
    if (window == Py_None)
        window = NULL;
    
    if (server && !pyserver_check(server))
    {
        PyErr_Format(PyExc_TypeError, "arg 3 isnt server");
        return -1;
    }
    
    if (window && !pywindow_check(window))
    {
        PyErr_Format(PyExc_TypeError, "arg 4 isnt window");
        return -1;
    }
    
    if (self->data)
    {
        PyErr_Format(PyExc_RuntimeError, "TextDest already wrapped");
        return -1;
    }
    
    dest = g_new0(TEXT_DEST_REC, 1);
    format_create_dest(dest, DATA(server), g_strdup(target), level, DATA(window));
   
    if (!pytextdest_setup(self, dest, 1))
        return -1;
    
    return 0;
}

/* Getters */
PyDoc_STRVAR(PyTextDest_window_doc,
    "Window where the text will be written"
);
static PyObject *PyTextDest_window_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->window);
}

PyDoc_STRVAR(PyTextDest_server_doc,
    "Target server"
);
static PyObject *PyTextDest_server_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->server);
}

PyDoc_STRVAR(PyTextDest_target_doc,
    "Target channel/query/etc name"
);
static PyObject *PyTextDest_target_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->target);
}

PyDoc_STRVAR(PyTextDest_level_doc,
    "Text level"
);
static PyObject *PyTextDest_level_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->level);
}

PyDoc_STRVAR(PyTextDest_hilight_priority_doc,
    "Priority for the hilighted text"
);
static PyObject *PyTextDest_hilight_priority_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->hilight_priority);
}

PyDoc_STRVAR(PyTextDest_hilight_color_doc,
    "Color for the hilighted text"
);
static PyObject *PyTextDest_hilight_color_get(PyTextDest *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->hilight_color);
}

/* specialized getters/setters */
static PyGetSetDef PyTextDest_getseters[] = {
    {"window", (getter)PyTextDest_window_get, NULL,
        PyTextDest_window_doc, NULL},
    {"server", (getter)PyTextDest_server_get, NULL,
        PyTextDest_server_doc, NULL},
    {"target", (getter)PyTextDest_target_get, NULL,
        PyTextDest_target_doc, NULL},
    {"level", (getter)PyTextDest_level_get, NULL,
        PyTextDest_level_doc, NULL},
    {"hilight_priority", (getter)PyTextDest_hilight_priority_get, NULL,
        PyTextDest_hilight_priority_doc, NULL},
    {"hilight_color", (getter)PyTextDest_hilight_color_get, NULL,
        PyTextDest_hilight_color_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyTextDest_prnt_doc,
    "prnt(str) -> None\n"
    "\n"
    "Print str to TextDest\n"
);
static PyObject *PyTextDest_prnt(PyTextDest *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", NULL};
    char *str = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &str))
        return NULL;

    printtext_dest(self->data, "%s", str);
    
    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyTextDest_methods[] = {
    {"prnt", (PyCFunction)PyTextDest_prnt, METH_VARARGS | METH_KEYWORDS,
        PyTextDest_prnt_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyTextDestType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.TextDest",            /*tp_name*/
    sizeof(PyTextDest),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyTextDest_dealloc, /*tp_dealloc*/
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
    PyTextDest_doc,           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyTextDest_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyTextDest_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyTextDest_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyTextDest_new,                 /* tp_new */
};

static int pytextdest_setup(PyTextDest *pytdest, void *td, int owned)
{
    PyObject *window, *server;
    TEXT_DEST_REC *tdest = td;

    if (tdest->window)
    {
        window = pywindow_new(tdest->window);
        if (!window)
            return 0;
    }

    server = py_irssi_chat_new(tdest->server, 1);
    if (!server)
    {
        Py_DECREF(window);
        return 0;
    }

    pytdest->data = td;
    pytdest->window = window;
    pytdest->server = server;
    pytdest->owned = owned;

    return 1;
}

/* TextDest factory function */
PyObject *pytextdest_new(void *td)
{
    PyTextDest *pytdest;

    pytdest = py_inst(PyTextDest, PyTextDestType);
    if (!pytdest)
        return NULL;

    if (!pytextdest_setup(pytdest, td, 0))
    {
        Py_DECREF(pytdest);
        return NULL;
    }
    
    return (PyObject *)pytdest;
}

int textdest_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyTextDestType) < 0)
        return 0;
    
    Py_INCREF(&PyTextDestType);
    PyModule_AddObject(py_module, "TextDest", (PyObject *)&PyTextDestType);

    return 1;
}
