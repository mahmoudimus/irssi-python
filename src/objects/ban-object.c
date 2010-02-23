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
#include "ban-object.h"
#include "pycore.h"

/* monitor "ban remove" signal */
static void ban_cleanup(CHANNEL_REC *chan, BAN_REC *ban)
{
    PyBan *pyban = signal_get_user_data();

    if (ban == pyban->data)
    {
        pyban->data = NULL;
        pyban->cleanup_installed = 0;
        signal_remove_data("ban remove", ban_cleanup, pyban);
    }
}

static void PyBan_dealloc(PyBan *self)
{
    if (self->cleanup_installed)
        signal_remove_data("ban remove", ban_cleanup, self); 

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyBan_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyBan *self;

    self = (PyBan *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

PyDoc_STRVAR(PyBan_ban_doc,
    "The ban"
);
static PyObject *PyBan_ban_get(PyBan *self, void *closure)
{
    BAN_REC *data = self->data;
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(data->ban);
}

PyDoc_STRVAR(PyBan_setby_doc,
    "Nick of who set the ban"
);
static PyObject *PyBan_setby_get(PyBan *self, void *closure)
{
    BAN_REC *data = self->data;
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(data->setby);
}

PyDoc_STRVAR(PyBan_time_doc,
    "Timestamp when ban was set"
);
static PyObject *PyBan_time_get(PyBan *self, void *closure)
{
    BAN_REC *data = self->data;
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(data->time);
}

/* specialized getters/setters */
static PyGetSetDef PyBan_getseters[] = {
    {"ban", (getter)PyBan_ban_get, NULL,
        PyBan_ban_doc, NULL},
    {"setby", (getter)PyBan_setby_get, NULL,
        PyBan_setby_doc, NULL},
    {"time", (getter)PyBan_time_get, NULL,
        PyBan_time_doc, NULL},
    {NULL}
};

/* Methods for object */
static PyMethodDef PyBan_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyBanType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Ban",            /*tp_name*/
    sizeof(PyBan),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyBan_dealloc, /*tp_dealloc*/
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
    "PyBan objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyBan_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyBan_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyBan_new,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pyban_new(void *ban)
{
    PyBan *pyban;

    pyban = py_inst(PyBan, PyBanType);
    if (!pyban)
        return NULL;

    pyban->data = ban;
    pyban->cleanup_installed = 1;
    signal_add_last_data("ban remove", ban_cleanup, pyban);

    return (PyObject *)pyban;
}

int ban_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyBanType) < 0)
        return 0;
    
    Py_INCREF(&PyBanType);
    PyModule_AddObject(py_module, "Ban", (PyObject *)&PyBanType);

    return 1;
}
