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
#include "nick-object.h"
#include "pyirssi.h"
#include "pycore.h"
#include "pyutils.h"

static void nick_cleanup(CHANNEL_REC *chan, NICK_REC *nick)
{
    PyNick *pynick = signal_get_user_data();

    if (nick == pynick->data)
    {
        pynick->data = NULL;
        pynick->cleanup_installed = 0;
        signal_remove_data("nicklist remove", nick_cleanup, pynick);
    }
}

static void PyNick_dealloc(PyNick *self)
{
    if (self->cleanup_installed)
        signal_remove_data("nicklist remove", nick_cleanup, self);

    self->ob_type->tp_free((PyObject*)self);
}

/* Getters */
PyDoc_STRVAR(PyNick_send_massjoin_doc,
    "Waiting to be sent in a 'massjoin' signal, True or False"
);
static PyObject *PyNick_send_massjoin_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->send_massjoin);
}

PyDoc_STRVAR(PyNick_nick_doc,
    "Plain nick"
);
static PyObject *PyNick_nick_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->nick);
}

PyDoc_STRVAR(PyNick_host_doc,
    "Host address"
);
static PyObject *PyNick_host_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->host);
}

PyDoc_STRVAR(PyNick_realname_doc,
    "Real name"
);
static PyObject *PyNick_realname_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->realname);
}

PyDoc_STRVAR(PyNick_hops_doc,
    "Hop count to the server the nick is using"
);
static PyObject *PyNick_hops_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->hops);
}

PyDoc_STRVAR(PyNick_gone_doc,
    "User status"
);
static PyObject *PyNick_gone_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->gone);
}

PyDoc_STRVAR(PyNick_serverop_doc,
    "User status"
);
static PyObject *PyNick_serverop_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->serverop);
}

PyDoc_STRVAR(PyNick_op_doc,
    "User status"
);
static PyObject *PyNick_op_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->op);
}

PyDoc_STRVAR(PyNick_voice_doc,
    "User status"
);
static PyObject *PyNick_voice_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->voice);
}

PyDoc_STRVAR(PyNick_halfop_doc,
    "User status"
);
static PyObject *PyNick_halfop_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->halfop);
}

PyDoc_STRVAR(PyNick_last_check_doc,
    "timestamp when last checked gone/ircop status."
);
static PyObject *PyNick_last_check_get(PyNick *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(self->data->last_check);
}

/* specialized getters/setters */
static PyGetSetDef PyNick_getseters[] = {
    {"send_massjoin", (getter)PyNick_send_massjoin_get, NULL,
        PyNick_send_massjoin_doc, NULL},
    {"nick", (getter)PyNick_nick_get, NULL,
        PyNick_nick_doc, NULL},
    {"host", (getter)PyNick_host_get, NULL,
        PyNick_host_doc, NULL},
    {"realname", (getter)PyNick_realname_get, NULL,
        PyNick_realname_doc, NULL},
    {"hops", (getter)PyNick_hops_get, NULL,
        PyNick_hops_doc, NULL},
    {"gone", (getter)PyNick_gone_get, NULL,
        PyNick_gone_doc, NULL},
    {"serverop", (getter)PyNick_serverop_get, NULL,
        PyNick_serverop_doc, NULL},
    {"op", (getter)PyNick_op_get, NULL,
        PyNick_op_doc, NULL},
    {"voice", (getter)PyNick_voice_get, NULL,
        PyNick_voice_doc, NULL},
    {"halfop", (getter)PyNick_halfop_get, NULL,
        PyNick_halfop_doc, NULL},
    {"last_check", (getter)PyNick_last_check_get, NULL,
        PyNick_last_check_doc, NULL},
    {NULL}
};

static PyMethodDef PyNick_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyNickType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Nick",            /*tp_name*/
    sizeof(PyNick),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyNick_dealloc, /*tp_dealloc*/
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
    "PyNick objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyNick_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyNick_getseters,        /* tp_getset */
    &PyIrssiChatBaseType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* nick factory function */
PyObject *pynick_sub_new(void *nick, PyTypeObject *subclass)
{
    static const char *name = "NICK";
    PyNick *pynick = NULL;

    pynick = py_instp(PyNick, subclass); 
    if (!pynick)
        return NULL;

    pynick->data = nick;
    pynick->base_name = name;
    signal_add_last_data("nicklist remove", nick_cleanup, pynick);
    pynick->cleanup_installed = 1;

    return (PyObject *)pynick;
}

PyObject *pynick_new(void *nick)
{
    return pynick_sub_new(nick, &PyNickType);
}

int nick_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyNickType) < 0)
        return 0;
    
    Py_INCREF(&PyNickType);
    PyModule_AddObject(py_module, "Nick", (PyObject *)&PyNickType);

    return 1;
}
