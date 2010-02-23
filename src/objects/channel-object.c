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
#include "factory.h"
#include "channel-object.h"
#include "pycore.h"

/* monitor "channel destroyed" signal */
static void chan_cleanup(CHANNEL_REC *chan)
{
    PyChannel *pychan = signal_get_user_data();

    if (chan == pychan->data)
    {
        pychan->data = NULL;
        pychan->cleanup_installed = 0;
        signal_remove_data("channel destroyed", chan_cleanup, pychan);
    }
}

static void PyChannel_dealloc(PyChannel *self)
{
    if (self->cleanup_installed)
        signal_remove_data("channel destroyed", chan_cleanup, self);

    self->ob_type->tp_free((PyObject*)self);
}

/* Getters */
PyDoc_STRVAR(PyChannel_topic_doc,
    "Channel topic"
);
static PyObject *PyChannel_topic_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->topic);
}

PyDoc_STRVAR(PyChannel_topic_by_doc,
    "Nick who set the topic"
);
static PyObject *PyChannel_topic_by_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->topic_by);
}

PyDoc_STRVAR(PyChannel_topic_time_doc,
    "Timestamp when the topic was set"
);
static PyObject *PyChannel_topic_time_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromLong(self->data->topic_time);
}

PyDoc_STRVAR(PyChannel_no_modes_doc,
    "Channel is modeless"
);
static PyObject *PyChannel_no_modes_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->no_modes);
}

PyDoc_STRVAR(PyChannel_mode_doc,
    "Channel mode"
);
static PyObject *PyChannel_mode_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->mode);
}

PyDoc_STRVAR(PyChannel_limit_doc,
    "Max. users in channel (+l mode)"
);
static PyObject *PyChannel_limit_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->limit);
}

PyDoc_STRVAR(PyChannel_key_doc,
    "Channel key (password)"
);
static PyObject *PyChannel_key_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->key);
}

PyDoc_STRVAR(PyChannel_chanop_doc,
    "You are channel operator"
);
static PyObject *PyChannel_chanop_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->chanop);
}

PyDoc_STRVAR(PyChannel_names_got_doc,
    "/NAMES list has been received"
);
static PyObject *PyChannel_names_got_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->names_got);
}

PyDoc_STRVAR(PyChannel_wholist_doc,
    "/WHO list has been received"
);
static PyObject *PyChannel_wholist_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->wholist);
}

PyDoc_STRVAR(PyChannel_synced_doc,
    "Channel is fully synchronized"
);
static PyObject *PyChannel_synced_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->synced);
}

PyDoc_STRVAR(PyChannel_joined_doc,
    "JOIN event for this channel has been received"
);
static PyObject *PyChannel_joined_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->joined);
}

PyDoc_STRVAR(PyChannel_left_doc,
    "You just left the channel (for 'channel destroyed' event)"
);
static PyObject *PyChannel_left_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->left);
}

PyDoc_STRVAR(PyChannel_kicked_doc,
    "You were just kicked out of the channel (for 'channel destroyed' event)"
);
static PyObject *PyChannel_kicked_get(PyChannel *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->kicked);
}

/* specialized getters/setters */
static PyGetSetDef PyChannel_getseters[] = {
    {"topic", (getter)PyChannel_topic_get, NULL,
        PyChannel_topic_doc, NULL},
    {"topic_by", (getter)PyChannel_topic_by_get, NULL,
        PyChannel_topic_by_doc, NULL},
    {"topic_time", (getter)PyChannel_topic_time_get, NULL,
        PyChannel_topic_time_doc, NULL},
    {"no_modes", (getter)PyChannel_no_modes_get, NULL,
        PyChannel_no_modes_doc, NULL},
    {"mode", (getter)PyChannel_mode_get, NULL,
        PyChannel_mode_doc, NULL},
    {"limit", (getter)PyChannel_limit_get, NULL,
        PyChannel_limit_doc, NULL},
    {"key", (getter)PyChannel_key_get, NULL,
        PyChannel_key_doc, NULL},
    {"chanop", (getter)PyChannel_chanop_get, NULL,
        PyChannel_chanop_doc, NULL},
    {"names_got", (getter)PyChannel_names_got_get, NULL,
        PyChannel_names_got_doc, NULL},
    {"wholist", (getter)PyChannel_wholist_get, NULL,
        PyChannel_wholist_doc, NULL},
    {"synced", (getter)PyChannel_synced_get, NULL,
        PyChannel_synced_doc, NULL},
    {"joined", (getter)PyChannel_joined_get, NULL,
        PyChannel_joined_doc, NULL},
    {"left", (getter)PyChannel_left_get, NULL,
        PyChannel_left_doc, NULL},
    {"kicked", (getter)PyChannel_kicked_get, NULL,
        PyChannel_kicked_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyChannel_nicks_doc,
    "nicks() -> list of Nick objects\n"
    "\n"
    "Return a list of nicks in the channel.\n"
);
static PyObject *PyChannel_nicks(PyChannel *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    return py_irssi_chatlist_new(nicklist_getnicks(self->data), 1);
}

PyDoc_STRVAR(PyChannel_nicks_find_mask_doc,
    "nicks_find_mask(mask) -> Nick object or None\n"
    "\n"
    "Find nick mask from nicklist, wildcards allowed.\n"
);
static PyObject *PyChannel_nicks_find_mask(PyChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"mask", NULL};
    char *mask = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &mask))
        return NULL;

    return py_irssi_chat_new(nicklist_find_mask(self->data, mask), 1);
}

PyDoc_STRVAR(PyChannel_nick_find_doc,
    "nick_find(nick) -> Nick object or None\n"
    "\n"
    "Find nick from nicklist.\n"
);
static PyObject *PyChannel_nick_find(PyChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    char *nick = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &nick))
        return NULL;

    return py_irssi_chat_new(nicklist_find(self->data, nick), 1);
}

PyDoc_STRVAR(PyChannel_nick_remove_doc,
    "nick_remove(nick) -> None\n"
    "\n"
    "Remove nick from nicklist.\n"
);
static PyObject *PyChannel_nick_remove(PyChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    PyObject *nick = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &nick))
        return NULL;

    if (!pynick_check(nick))
        return PyErr_Format(PyExc_TypeError, "arg must be nick");

    nicklist_remove(self->data, ((PyNick*)nick)->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyChannel_nick_insert_obj_doc,
    "nick_insert(nick) -> None\n"
    "\n"
    "Insert nick object into nicklist.\n"
);
static PyObject *PyChannel_nick_insert_obj(PyChannel *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    PyObject *nick = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &nick))
        return NULL;

    if (!pynick_check(nick))
        return PyErr_Format(PyExc_TypeError, "arg must be nick");

    nicklist_insert(self->data, ((PyNick*)nick)->data);

    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyChannel_methods[] = {
    {"nicks", (PyCFunction)PyChannel_nicks, METH_NOARGS,
        PyChannel_nicks_doc},
    {"nicks_find_mask", (PyCFunction)PyChannel_nicks_find_mask, METH_VARARGS | METH_KEYWORDS,
        PyChannel_nicks_find_mask_doc},
    {"nick_find", (PyCFunction)PyChannel_nick_find, METH_VARARGS | METH_KEYWORDS,
        PyChannel_nick_find_doc},
    {"nick_remove", (PyCFunction)PyChannel_nick_remove, METH_VARARGS | METH_KEYWORDS,
        PyChannel_nick_remove_doc},
    {"nick_insert_obj", (PyCFunction)PyChannel_nick_insert_obj, METH_VARARGS | METH_KEYWORDS,
        PyChannel_nick_insert_obj_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyChannelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Channel",            /*tp_name*/
    sizeof(PyChannel),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyChannel_dealloc, /*tp_dealloc*/
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
    "PyChannel objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyChannel_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyChannel_getseters,        /* tp_getset */
    &PyWindowItemType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pychannel_sub_new(void *chan, const char *name, PyTypeObject *type) 
{
    PyObject *pychan;

    pychan = pywindow_item_sub_new(chan, name, type);
    if (pychan)
    {
        PyChannel *pych = (PyChannel *)pychan;
        signal_add_last_data("channel destroyed", chan_cleanup, pych);
        pych->cleanup_installed = 1;
    }

    return pychan;
}

PyObject *pychannel_new(void *chan)
{
    static const char *name = "CHANNEL";
    return pychannel_sub_new(chan, name, &PyChannelType);
}

int channel_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyChannelType) < 0)
        return 0;
    
    Py_INCREF(&PyChannelType);
    PyModule_AddObject(py_module, "Channel", (PyObject *)&PyChannelType);

    return 1;
}
