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
#include "pymodule.h"
#include "base-objects.h"
#include "irc-connect-object.h"
#include "pyirssi_irc.h"
#include "pycore.h"
#include "pyutils.h"

/* cleanup and deallocation handled by Connect base */

/* Getters */
PyDoc_STRVAR(PyIrcConnect_alternate_nick_doc,
    "Alternate nick to use if default nick is taken"
);
static PyObject *PyIrcConnect_alternate_nick_get(PyIrcConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->alternate_nick);
}

/* Get/Set */
static PyGetSetDef PyIrcConnect_getseters[] = {
    {"alternate_nick", (getter)PyIrcConnect_alternate_nick_get, NULL,
        PyIrcConnect_alternate_nick_doc, NULL},
    {NULL}
};

PyTypeObject PyIrcConnectType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.IrcConnect",            /*tp_name*/
    sizeof(PyIrcConnect),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                          /*tp_dealloc*/
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
    "PyIrcConnect objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    PyIrcConnect_getseters,        /* tp_getset */
    &PyConnectType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};

PyObject *pyirc_connect_new(void *connect, int managed)
{
    return pyconnect_sub_new(connect, &PyIrcConnectType, managed);
}

int irc_connect_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyIrcConnectType) < 0)
        return 0;
    
    Py_INCREF(&PyIrcConnectType);
    PyModule_AddObject(py_module, "IrcConnect", (PyObject *)&PyIrcConnectType);

    return 1;
}
