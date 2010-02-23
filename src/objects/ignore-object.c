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
#include "ignore-object.h"
#include "factory.h"
#include "pycore.h"

/* monitor "ignore destroy" signal */
static void ignore_cleanup(IGNORE_REC *ignore)
{
    PyIgnore *pyignore = signal_get_user_data();

    if (ignore == pyignore->data)
    {
        pyignore->data = NULL;
        pyignore->cleanup_installed = 0;
        signal_remove_data("ignore destroy", ignore_cleanup, pyignore);
    }
}

static void PyIgnore_dealloc(PyIgnore *self)
{
    if (self->cleanup_installed)
        signal_remove_data("ignore destroy", ignore_cleanup, self);

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyIgnore_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyIgnore *self;

    self = (PyIgnore *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyIgnore_mask_doc,
    "Ignore mask"
);
static PyObject *PyIgnore_mask_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->mask);
}

PyDoc_STRVAR(PyIgnore_servertag_doc,
    "Ignore only in server"
);
static PyObject *PyIgnore_servertag_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->servertag);
}

PyDoc_STRVAR(PyIgnore_pattern_doc,
    "Ignore text patern"
);
static PyObject *PyIgnore_pattern_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->pattern);
}

PyDoc_STRVAR(PyIgnore_level_doc,
    "Ignore level"
);
static PyObject *PyIgnore_level_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->level);
}

PyDoc_STRVAR(PyIgnore_exception_doc,
    "This is an exception ignore"
);
static PyObject *PyIgnore_exception_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->exception);
}

PyDoc_STRVAR(PyIgnore_regexp_doc,
    "Regexp pattern matching"
);
static PyObject *PyIgnore_regexp_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->regexp);
}

PyDoc_STRVAR(PyIgnore_fullword_doc,
    "Pattern matches only full words"
);
static PyObject *PyIgnore_fullword_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->fullword);
}

PyDoc_STRVAR(PyIgnore_replies_doc,
    "Ignore replies to nick in channel"
);
static PyObject *PyIgnore_replies_get(PyIgnore *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->replies);
}

/* specialized getters/setters */
static PyGetSetDef PyIgnore_getseters[] = {
    {"mask", (getter)PyIgnore_mask_get, NULL,
        PyIgnore_mask_doc, NULL},
    {"servertag", (getter)PyIgnore_servertag_get, NULL,
        PyIgnore_servertag_doc, NULL},
    {"pattern", (getter)PyIgnore_pattern_get, NULL,
        PyIgnore_pattern_doc, NULL},
    {"level", (getter)PyIgnore_level_get, NULL,
        PyIgnore_level_doc, NULL},
    {"exception", (getter)PyIgnore_exception_get, NULL,
        PyIgnore_exception_doc, NULL},
    {"regexp", (getter)PyIgnore_regexp_get, NULL,
        PyIgnore_regexp_doc, NULL},
    {"fullword", (getter)PyIgnore_fullword_get, NULL,
        PyIgnore_fullword_doc, NULL},
    {"replies", (getter)PyIgnore_replies_get, NULL,
        PyIgnore_replies_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyIgnore_channels_doc,
    "channels() -> list of str\n"
    "\n"
    "Ignore only in channels (list of names)\n"
);
static PyObject *PyIgnore_channels(PyIgnore *self, PyObject *args)
{
    char **p;
    PyObject *list;

    RET_NULL_IF_INVALID(self->data);

    list = PyList_New(0);
    if (!list)
        return NULL;
    
    for (p = self->data->channels; *p; p++)
    {
        int ret;
        PyObject *str;

        str = PyString_FromString(*p);
        if (!str)
        {
            Py_XDECREF(list);
            return NULL;
        }

        ret = PyList_Append(list, str);
        Py_DECREF(str);
        if (ret != 0)
        {
            Py_XDECREF(list);
            return NULL;
        }
    }
   
    return list;
}

PyDoc_STRVAR(PyIgnore_add_rec_doc,
    "add_rec() -> None\n"
    "\n"
    "Add ignore record"
);
static PyObject *PyIgnore_add_rec(PyIgnore *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    ignore_add_rec(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyIgnore_update_rec_doc,
    "update_rec() -> None\n"
    "\n"
    "Update ignore record in configuration"
);
static PyObject *PyIgnore_update_rec(PyIgnore *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    ignore_update_rec(self->data);
    
    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyIgnore_methods[] = {
    {"add_rec", (PyCFunction)PyIgnore_add_rec, METH_NOARGS,
        PyIgnore_add_rec_doc},
    {"update_rec", (PyCFunction)PyIgnore_update_rec, METH_NOARGS,
        PyIgnore_update_rec_doc},
    {"channels", (PyCFunction)PyIgnore_channels, METH_NOARGS,
        PyIgnore_channels_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyIgnoreType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Ignore",            /*tp_name*/
    sizeof(PyIgnore),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyIgnore_dealloc, /*tp_dealloc*/
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
    "PyIgnore objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyIgnore_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyIgnore_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyIgnore_new,                 /* tp_new */
};


/* ignore factory function */
PyObject *pyignore_new(void *ignore)
{
    PyIgnore *pyignore;

    pyignore = py_inst(PyIgnore, PyIgnoreType);
    if (!pyignore)
        return NULL;

    pyignore->data = ignore;
    pyignore->cleanup_installed = 1;
    signal_add_last_data("ignore destroy", ignore_cleanup, pyignore);

    return (PyObject *)pyignore;
}

int ignore_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyIgnoreType) < 0)
        return 0;
    
    Py_INCREF(&PyIgnoreType);
    PyModule_AddObject(py_module, "Ignore", (PyObject *)&PyIgnoreType);

    return 1;
}
