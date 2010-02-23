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
#include "pycore.h"
#include "irc-channel-object.h"
#include "factory.h"

/* PyIrcChannel destructor is inherited from PyChannel */

/* specialized getters/setters */
static PyGetSetDef PyIrcChannel_getseters[] = {
    {NULL}
};

/* Methods */
PyDoc_STRVAR(bans_doc,
    "bans() -> list of Ban objects\n"
    "\n"
    "Returns a list of bans in the channel.\n"
);
static PyObject *PyIrcChannel_bans(PyIrcChannel *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    return py_irssi_objlist_new(self->data->banlist, 1, (InitFunc)pyban_new);
}

PyDoc_STRVAR(ban_get_mask_doc,
    "ban_get_mask(nick, ban_type=0) -> str\n"
    "\n"
    "Get ban mask for 'nick'.\n"
);
static PyObject *PyIrcChannel_ban_get_mask(PyIrcChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", "ban_type", NULL};
    char *nick, *str;
    int ban_type = 0;
    PyObject *ret;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, &nick, &ban_type))
        return NULL;

    str = ban_get_mask(self->data, nick, ban_type);
    if (!str)
        Py_RETURN_NONE;
    
    ret = PyString_FromString(str);
    g_free(str);

    return ret;
}

PyDoc_STRVAR(banlist_add_doc,
    "banlist_add(ban, nick, time) -> Ban object or None\n"
    "\n"
    "Add a new ban to channel. Return None if duplicate."
);
static PyObject *PyIrcChannel_banlist_add(PyIrcChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"ban", "nick", "time", NULL};
    char *ban, *nick;
    time_t btime;
    BAN_REC *newban;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssk", kwlist, &ban, &nick, &btime))
        return NULL;

    newban = banlist_add(self->data, ban, nick, btime);
    /* XXX: return none or throw error? */
    if (!newban)
        Py_RETURN_NONE;

    return pyban_new(newban);
}

PyDoc_STRVAR(banlist_remove_doc,
    "banlist_remove(ban, nick) -> None\n"
    "\n"
    "Remove a new ban from channel.\n"
);
static PyObject *PyIrcChannel_banlist_remove(PyIrcChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"ban", "nick", NULL};
    char *ban, *nick;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &ban, &nick))
        return NULL;

    banlist_remove(self->data, ban, nick);
    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyIrcChannel_methods[] = {
    {"bans", (PyCFunction)PyIrcChannel_bans, METH_NOARGS, 
        bans_doc},
    {"ban_get_mask", (PyCFunction)PyIrcChannel_ban_get_mask, METH_VARARGS | METH_KEYWORDS, 
        ban_get_mask_doc},
    {"banlist_add", (PyCFunction)PyIrcChannel_banlist_add, METH_VARARGS | METH_KEYWORDS, 
        banlist_add_doc},
    {"banlist_remove", (PyCFunction)PyIrcChannel_banlist_remove, METH_VARARGS | METH_KEYWORDS, 
        banlist_remove_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyIrcChannelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.IrcChannel",            /*tp_name*/
    sizeof(PyIrcChannel),             /*tp_basicsize*/
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
    "PyIrcChannel objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyIrcChannel_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyIrcChannel_getseters,        /* tp_getset */
    &PyChannelType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* irc channel factory function */
PyObject *pyirc_channel_new(void *chan)
{
    static const char *BASE_NAME = "CHANNEL";
    return pychannel_sub_new(chan, BASE_NAME, &PyIrcChannelType);
}

int irc_channel_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyIrcChannelType) < 0)
        return 0;
    
    Py_INCREF(&PyIrcChannelType);
    PyModule_AddObject(py_module, "IrcChannel", (PyObject *)&PyIrcChannelType);

    return 1;
}
