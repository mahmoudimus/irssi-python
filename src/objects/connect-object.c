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
#include "connect-object.h"
#include "pyirssi.h"
#include "pycore.h"
#include "pyutils.h"

static void connect_cleanup(SERVER_CONNECT_REC *connect)
{
    PyConnect *pyconn = signal_get_user_data();

    /*
    if (server == pyconn->data)
    {
        pyserver->data = NULL;
        pyserver->cleanup_installed = 0;
        signal_remove_data("server disconnected", connect_cleanup, pyserver);
    }
    */
}

static void PyConnect_dealloc(PyConnect *self)
{
    if (self->cleanup_installed)
        signal_remove_data("server disconnected", connect_cleanup, self);

    self->ob_type->tp_free((PyObject*)self);
}

/* Getters */
PyDoc_STRVAR(PyConnect_address_doc,
    "Address where we connected (irc.blah.org)"
);
static PyObject *PyConnect_address_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->address);
}

PyDoc_STRVAR(PyConnect_port_doc,
    "Port where we're connected"
);
static PyObject *PyConnect_port_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->port);
}

PyDoc_STRVAR(PyConnect_chatnet_doc,
    "Chat network"
);
static PyObject *PyConnect_chatnet_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->chatnet);
}

PyDoc_STRVAR(PyConnect_password_doc,
    "Password we used in connection."
);
static PyObject *PyConnect_password_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->password);
}

PyDoc_STRVAR(PyConnect_wanted_nick_doc,
    "Nick which we would prefer to use"
);
static PyObject *PyConnect_wanted_nick_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->nick);
}

PyDoc_STRVAR(PyConnect_username_doc,
    "User name"
);
static PyObject *PyConnect_username_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->username);
}

PyDoc_STRVAR(PyConnect_realname_doc,
    "Real name"
);
static PyObject *PyConnect_realname_get(PyConnect *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->realname);
}

/* Get/Set */
static PyGetSetDef PyConnect_getseters[] = {
    {"address", (getter)PyConnect_address_get, NULL,
        PyConnect_address_doc, NULL},
    {"port", (getter)PyConnect_port_get, NULL,
        PyConnect_port_doc, NULL},
    {"chatnet", (getter)PyConnect_chatnet_get, NULL,
        PyConnect_chatnet_doc, NULL},
    {"password", (getter)PyConnect_password_get, NULL,
        PyConnect_password_doc, NULL},
    {"wanted_nick", (getter)PyConnect_wanted_nick_get, NULL,
        PyConnect_wanted_nick_doc, NULL},
    {"username", (getter)PyConnect_username_get, NULL,
        PyConnect_username_doc, NULL},
    {"realname", (getter)PyConnect_realname_get, NULL,
        PyConnect_realname_doc, NULL},
    {NULL}
};

PyTypeObject PyConnectType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Connect",            /*tp_name*/
    sizeof(PyConnect),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyConnect_dealloc, /*tp_dealloc*/
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
    "PyConnect objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                      /* tp_methods */
    0,                      /* tp_members */
    PyConnect_getseters,        /* tp_getset */
    &PyIrssiChatBaseType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* server connect factory function (managed == 0, don't do signal cleanup, 1 == do sig cleanup */
PyObject *pyconnect_sub_new(void *connect, PyTypeObject *subclass, int managed)
{
    static const char *CONNECT_TYPE = "SERVER CONNECT";
    PyConnect *pyconn = NULL;

    g_return_val_if_fail(connect != NULL, NULL);
    
    pyconn = py_instp(PyConnect, subclass);
    if (!pyconn)
        return NULL;

    pyconn->base_name = CONNECT_TYPE;
    pyconn->data = connect;
    
    if (managed)
    {
        //XXX: how to handle cleanup?
        //signal_add_last_data("server disconnected", connect_cleanup, pyconn);
        //pyconn->cleanup_installed = 1;
    }

    return (PyObject *)pyconn;
}

PyObject *pyconnect_new(void *connect, int managed)
{
    return pyconnect_sub_new(connect, &PyConnectType, managed);
}

int connect_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyConnectType) < 0)
        return 0;
    
    Py_INCREF(&PyConnectType);
    PyModule_AddObject(py_module, "Connect", (PyObject *)&PyConnectType);

    return 1;
}
