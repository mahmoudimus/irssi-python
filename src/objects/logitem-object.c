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
#include "logitem-object.h"
#include "pycore.h"

/* no special cleanup -- value copy is made */

static void PyLogitem_dealloc(PyLogitem *self)
{
    Py_XDECREF(self->type);
    Py_XDECREF(self->name);
    Py_XDECREF(self->servertag);

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyLogitem_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyLogitem *self;

    self = (PyLogitem *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyLogitem_type_doc,
    "0=target, 1=window refnum"
);
static PyObject *PyLogitem_type_get(PyLogitem *self, void *closure)
{
    RET_AS_OBJ_OR_NONE(self->type);
}

PyDoc_STRVAR(PyLogitem_name_doc,
    "Name"
);
static PyObject *PyLogitem_name_get(PyLogitem *self, void *closure)
{
    RET_AS_OBJ_OR_NONE(self->name);
}

PyDoc_STRVAR(PyLogitem_servertag_doc,
    "Server tag"
);
static PyObject *PyLogitem_servertag_get(PyLogitem *self, void *closure)
{
    RET_AS_OBJ_OR_NONE(self->servertag);
}

/* specialized getters/setters */
static PyGetSetDef PyLogitem_getseters[] = {
    {"type", (getter)PyLogitem_type_get, NULL,
        PyLogitem_type_doc, NULL},
    {"name", (getter)PyLogitem_name_get, NULL,
        PyLogitem_name_doc, NULL},
    {"servertag", (getter)PyLogitem_servertag_get, NULL,
        PyLogitem_servertag_doc, NULL},
    {NULL}
};

/* Methods for object */
static PyMethodDef PyLogitem_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyLogitemType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Logitem",            /*tp_name*/
    sizeof(PyLogitem),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyLogitem_dealloc, /*tp_dealloc*/
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
    "PyLogitem objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyLogitem_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyLogitem_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyLogitem_new,                 /* tp_new */
};


/* log item factory function */
PyObject *pylogitem_new(void *log)
{
    LOG_ITEM_REC *li = log;
    PyLogitem *pylog = NULL;

    pylog = py_inst(PyLogitem, PyLogitemType);
    if (!pylog)
        return NULL;

    pylog->type = PyInt_FromLong(li->type);
    if (!pylog->type)
        goto error;

    pylog->name = PyString_FromString(li->name);
    if (!pylog->name)
        goto error;

    if (li->servertag)
    {
        pylog->servertag = PyString_FromString(li->servertag);
        if (!pylog->servertag)
            goto error;
    }

    return (PyObject *)pylog;

error:
    Py_XDECREF(pylog);
    return NULL;
}

int logitem_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyLogitemType) < 0)
        return 0;
    
    Py_INCREF(&PyLogitemType);
    PyModule_AddObject(py_module, "Logitem", (PyObject *)&PyLogitemType);

    return 1;
}
