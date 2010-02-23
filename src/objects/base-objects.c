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
#include "structmember.h"
#include "pymodule.h"
#include "base-objects.h"
#include "pyirssi.h"

/* This is the base type for Irssi objects with a type id. The user can find 
 * the type name, type id, and check if the object is wrapping a valid Irssi 
 * record. 
 */

static void PyIrssiBase_dealloc(PyIrssiBase *self)
{
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyIrssiBase_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyIrssiBase *self;

    self = (PyIrssiBase *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyIrssiBase_type_id_doc,
    "Irssi's type id for object"
);
static PyObject *PyIrssiBase_type_id_get(PyIrssiBase *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->type);
}

PyDoc_STRVAR(PyIrssiBase_type_doc,
    "Irssi's name for object"
);
static PyObject *PyIrssiBase_type_get(PyIrssiBase *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->base_name);
}

PyDoc_STRVAR(PyIrssiBase_valid_doc,
    "True if the object is valid"
);
static PyObject *PyIrssiBase_valid_get(PyIrssiBase *self, void *closure)
{
    if (self->data != NULL)
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

/* specialized getters/setters */
static PyGetSetDef PyIrssiBase_getseters[] = {
    {"type_id", (getter)PyIrssiBase_type_id_get, NULL,
        PyIrssiBase_type_id_doc, NULL},
    {"type", (getter)PyIrssiBase_type_get, NULL,
        PyIrssiBase_type_doc, NULL},
    {"valid", (getter)PyIrssiBase_valid_get, NULL,
        PyIrssiBase_valid_doc, NULL},
    {NULL}
};

/* Methods for object */
static PyMethodDef PyIrssiBase_methods[] = {
    {NULL}  
};

PyTypeObject PyIrssiBaseType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.IrssiBase",            /*tp_name*/
    sizeof(PyIrssiBase),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyIrssiBase_dealloc, /*tp_dealloc*/
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
    "PyIrssiBase objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyIrssiBase_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyIrssiBase_getseters,        /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyIrssiBase_new,                 /* tp_new */
};


/* IrssiChatBase is a base type for any object with a chat type. The user
   can find the chat type string name with the chat_type member or
   the type id with the chat_type_id member. It inherits from IrssiBase
   so the type, valid, and type_id members are visible to the user, too */

static void PyIrssiChatBase_dealloc(PyIrssiChatBase *self)
{
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyIrssiChatBase_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyIrssiChatBase *self;

    self = (PyIrssiChatBase *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyIrssiChatBase_chat_type_id_doc,
    "Chat Type id (int)"
);
static PyObject *PyIrssiChatBase_chat_type_id_get(PyIrssiChatBase *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->chat_type);
}

PyDoc_STRVAR(PyIrssiChatBase_chat_type_doc,
    "Chat name (str)"
);
static PyObject *PyIrssiChatBase_chat_type_get(PyIrssiChatBase *self, void *closure)
{
    CHAT_PROTOCOL_REC *rec;

    RET_NULL_IF_INVALID(self->data);

    rec = chat_protocol_find_id(self->data->chat_type);
    if (rec)
        RET_AS_STRING_OR_NONE(rec->name);
    else
        Py_RETURN_NONE;
}

/* specialized getters/setters */
static PyGetSetDef PyIrssiChatBase_getseters[] = {
    {"chat_type_id", (getter)PyIrssiChatBase_chat_type_id_get, NULL,
        PyIrssiChatBase_chat_type_id_doc, NULL},
    {"chat_type", (getter)PyIrssiChatBase_chat_type_get, NULL,
        PyIrssiChatBase_chat_type_doc, NULL},
    {NULL}
};

/* Methods */
static PyMethodDef PyIrssiChatBase_methods[] = {
    {NULL}  
};

PyTypeObject PyIrssiChatBaseType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.IrssiChatBase",            /*tp_name*/
    sizeof(PyIrssiChatBase),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyIrssiChatBase_dealloc, /*tp_dealloc*/
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
    "PyIrssiChatBase objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyIrssiChatBase_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyIrssiChatBase_getseters,        /* tp_getset */
    &PyIrssiBaseType,         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyIrssiChatBase_new,                 /* tp_new */
};

int base_objects_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyIrssiBaseType) < 0)
        return 0;
    if (PyType_Ready(&PyIrssiChatBaseType) < 0)
        return 0;

    Py_INCREF(&PyIrssiBaseType);
    Py_INCREF(&PyIrssiChatBaseType);
    PyModule_AddObject(py_module, "IrssiBase", (PyObject *)&PyIrssiBaseType);
    PyModule_AddObject(py_module, "IrssiChatBase", (PyObject *)&PyIrssiChatBaseType);

    return 1;
}
