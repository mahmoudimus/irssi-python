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
#include "factory.h"
#include "pyirssi.h"
#include "pycore.h"
#include "pyutils.h"

static void server_cleanup(SERVER_REC *server)
{
    PyServer *pyserver = signal_get_user_data();

    if (server == pyserver->data)
    {
        if (pyserver->connect)
            ((PyConnect *)pyserver->connect)->data = NULL;

        if (pyserver->rawlog)
            ((PyRawlog *)pyserver->rawlog)->data = NULL;

        pyserver->data = NULL;
        pyserver->cleanup_installed = 0;
        signal_remove_data("server disconnected", server_cleanup, pyserver);
    }
}

static void PyServer_dealloc(PyServer *self)
{
    if (self->cleanup_installed)
        signal_remove_data("server disconnected", server_cleanup, self);

    Py_XDECREF(self->connect);
    Py_XDECREF(self->rawlog);
    
    self->ob_type->tp_free((PyObject*)self);
}

/* Getters */
PyDoc_STRVAR(PyServer_connect_time_doc,
    "Time when connect() to server finished"
);
static PyObject *PyServer_connect_time_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromLong(self->data->connect_time);
}

PyDoc_STRVAR(PyServer_real_connect_time_doc,
    "Time when server sent 'connected' message"
);
static PyObject *PyServer_real_connect_time_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromLong(self->data->real_connect_time);
}

PyDoc_STRVAR(PyServer_tag_doc,
    "Unique server tag"
);
static PyObject *PyServer_tag_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->tag);
}

PyDoc_STRVAR(PyServer_nick_doc,
    "Current nick"
);
static PyObject *PyServer_nick_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->nick);
}

PyDoc_STRVAR(PyServer_connected_doc,
    "Is connection finished? 1|0"
);
static PyObject *PyServer_connected_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->connected);
}

PyDoc_STRVAR(PyServer_connection_lost_doc,
    "Did we lose the connection (1) or was the connection just /DISCONNECTed (0)"
);
static PyObject *PyServer_connection_lost_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->connection_lost);
}

PyDoc_STRVAR(PyServer_rawlog_doc,
    "Rawlog object for the server"
);
static PyObject *PyServer_rawlog_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->rawlog);
}

PyDoc_STRVAR(PyServer_connect_doc,
    "Connect object for the server"
);
static PyObject *PyServer_connect_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->connect);
}

PyDoc_STRVAR(PyServer_version_doc,
    "Server version"
);
static PyObject *PyServer_version_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->version);
}

PyDoc_STRVAR(PyServer_last_invite_doc,
    "Last channel we were invited to"
);
static PyObject *PyServer_last_invite_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->last_invite);
}

PyDoc_STRVAR(PyServer_server_operator_doc,
    "Are we server operator (IRC op) 1|0"
);
static PyObject *PyServer_server_operator_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->server_operator);
}

PyDoc_STRVAR(PyServer_usermode_away_doc,
    "Are we marked as away? 1|0"
);
static PyObject *PyServer_usermode_away_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->usermode_away);
}

PyDoc_STRVAR(PyServer_away_reason_doc,
    "Away reason message"
);
static PyObject *PyServer_away_reason_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->away_reason);
}

PyDoc_STRVAR(PyServer_banned_doc,
    "Were we banned from this server? 1|0"
);
static PyObject *PyServer_banned_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->banned);
}

PyDoc_STRVAR(PyServer_lag_doc,
    "Current lag to server in milliseconds"
);
static PyObject *PyServer_lag_get(PyServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->lag);
}

static PyGetSetDef PyServer_getseters[] = {
    {"connect_time", (getter)PyServer_connect_time_get, NULL,
        PyServer_connect_time_doc, NULL},
    {"real_connect_time", (getter)PyServer_real_connect_time_get, NULL,
        PyServer_real_connect_time_doc, NULL},
    {"tag", (getter)PyServer_tag_get, NULL,
        PyServer_tag_doc, NULL},
    {"nick", (getter)PyServer_nick_get, NULL,
        PyServer_nick_doc, NULL},
    {"connected", (getter)PyServer_connected_get, NULL,
        PyServer_connected_doc, NULL},
    {"connection_lost", (getter)PyServer_connection_lost_get, NULL,
        PyServer_connection_lost_doc, NULL},
    {"rawlog", (getter)PyServer_rawlog_get, NULL,
        PyServer_rawlog_doc, NULL},
    {"connect", (getter)PyServer_connect_get, NULL,
        PyServer_connect_doc, NULL},
    {"version", (getter)PyServer_version_get, NULL,
        PyServer_version_doc, NULL},
    {"last_invite", (getter)PyServer_last_invite_get, NULL,
        PyServer_last_invite_doc, NULL},
    {"server_operator", (getter)PyServer_server_operator_get, NULL,
        PyServer_server_operator_doc, NULL},
    {"usermode_away", (getter)PyServer_usermode_away_get, NULL,
        PyServer_usermode_away_doc, NULL},
    {"away_reason", (getter)PyServer_away_reason_get, NULL,
        PyServer_away_reason_doc, NULL},
    {"banned", (getter)PyServer_banned_get, NULL,
        PyServer_banned_doc, NULL},
    {"lag", (getter)PyServer_lag_get, NULL,
        PyServer_lag_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(print_doc,
    "prnt(channel, str, level) -> None\n"
    "\n"
    "Print to server\n"
);
static PyObject *PyServer_prnt(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"channel", "str", "level", NULL};
    char *str, *channel;
    int level = MSGLEVEL_CLIENTNOTICE;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss|i", kwlist, &channel, &str, &level))
        return NULL;

    printtext_string(self->data, channel, level, str);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(command_doc,
    "command(cmd) -> None\n"
    "\n"
    "Send command\n"
);
static PyObject *PyServer_command(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", NULL};
    char *cmd;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &cmd))
        return NULL;

    py_command(cmd, self->data, NULL);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(disconnect_doc,
    "disconnect() -> None\n"
    "\n"
    "Disconnect from server\n"
);
static PyObject *PyServer_disconnect(PyServer *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    server_disconnect(self->data);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(isnickflag_doc,
    "isnickflag(flag) -> bool\n"
    "\n"
    "Returns True if flag is a nick mode flag (@, + or % in IRC)\n"
);
static PyObject *PyServer_isnickflag(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"flag", NULL};
    char flag;
    int ret;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "c", kwlist, &flag))
        return NULL;

    ret = self->data->isnickflag(self->data, flag);
    
    return PyBool_FromLong(ret);
}

PyDoc_STRVAR(ischannel_doc,
    "ischannel(data) -> bool\n"
    "\n"
    "Returns True if start of `data' seems to mean channel.\n"
);
static PyObject *PyServer_ischannel(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"data", NULL};
    char *data;
    int ret;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &data))
        return NULL;

    ret = self->data->ischannel(self->data, data);
    
    return PyBool_FromLong(ret);
}

PyDoc_STRVAR(get_nick_flags_doc,
    "get_nick_flags() -> str\n"
    "\n"
    "Returns nick flag characters in order: op, voice, halfop (\"@+%\") in IRC\n"
);
static PyObject *PyServer_get_nick_flags(PyServer *self, PyObject *args)
{
    char *ret;

    RET_NULL_IF_INVALID(self->data);

    ret = (char *)self->data->get_nick_flags(self->data);

    return PyString_FromString(ret);
}

PyDoc_STRVAR(send_message_doc,
    "send_message(target, msg, target_type) -> None\n"
    "\n"
    "Sends a message to nick/channel. target_type 0 = channel, 1 = nick\n"
);
static PyObject *PyServer_send_message(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"target", "msg", "target_type", NULL};
    char *target, *msg;
    int target_type;
    
    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssi", kwlist, &target, &msg, &target_type))
        return NULL;

    self->data->send_message(self->data, target, msg, target_type);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(channels_join_doc,
    "channels_join(channels, automatic=False) -> None\n"
    "\n"
    "Join to channels in server. `channels' may also contain keys for\n"
    "channels just like with /JOIN command. `automatic' specifies if this\n"
    "channel was joined 'automatically' or if it was joined because join\n"
    "was requested by user. If channel join is 'automatic', irssi doesn't\n"
    "jump to the window where the channel was joined.\n"
);
static PyObject *PyServer_channels_join(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"channels", "automatic", NULL};
    char *channels;
    int automatic = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, &channels, &automatic))
        return NULL;

    self->data->channels_join(self->data, channels, automatic);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyServer_window_item_find_doc,
    "window_item_find(name) -> WindowItem object or None\n"
    "\n"
    "Find window item that matches best to given arguments\n"
);
static PyObject *PyServer_window_item_find(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", NULL};
    char *name = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &name))
        return NULL;

    return py_irssi_chat_new(window_item_find(self->data, name), 1);
}

PyDoc_STRVAR(PyServer_window_find_item_doc,
    "window_find_item(name) -> Window object or None\n"
    "\n"
    "Find window which contains window item with specified name/server\n"
);
static PyObject *PyServer_window_find_item(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", NULL};
    char *name = "";
    WINDOW_REC *win;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &name))
        return NULL;

    win = window_find_item(self->data, name);
    if (win)
        return pywindow_new(win);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyServer_window_find_level_doc,
    "window_find_level(level) -> Window object or None\n"
    "\n"
    "Find window with level\n"
);
static PyObject *PyServer_window_find_level(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"level", NULL};
    int level = 0;
    WINDOW_REC *win;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, 
           &level))
        return NULL;

    win = window_find_level(self->data, level);
    if (win)
        return pywindow_new(win);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyServer_window_find_closest_doc,
    "window_find_closest(name, level) -> Window object or None\n"
    "\n"
    "Find window that matches best to given arguments. `name' can be either\n"
    "window name or name of one of the window items.\n"
);
static PyObject *PyServer_window_find_closest(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", "level", NULL};
    char *name = "";
    int level = 0;
    WINDOW_REC *win;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "si", kwlist, 
           &name, &level))
        return NULL;

    win = window_find_closest(self->data, name, level);
    if (win)
        return pywindow_new(win);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyServer_channels_doc,
    "channels() -> list of Channel objects\n"
    "\n"
    "Return list of channels for server\n"
);
static PyObject *PyServer_channels(PyServer *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    return py_irssi_chatlist_new(self->data->channels, 1);
}

PyDoc_STRVAR(PyServer_channel_find_doc,
    "channel_find(name) -> Channel object or None\n"
    "\n"
    "Find channel from this server\n"
);
static PyObject *PyServer_channel_find(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", NULL};
    char *name = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &name))
        return NULL;

    return py_irssi_chat_new(channel_find(self->data, name), 1);
}

PyDoc_STRVAR(PyServer_nicks_get_same_doc,
    "nicks_get_same(nick)\n"
    "\n"
    "Return all nick objects in all channels in server. List is in format:\n"
    "[(Channel, Nick), (Channel, Nick), ...]\n"
);
static PyObject *PyServer_nicks_get_same(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    char *nick = "";
    GSList *list, *node;
    PyObject *pylist = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &nick))
        return NULL;
    
    pylist = PyList_New(0);
    if (!pylist)
        return NULL;

    list = nicklist_get_same(self->data, nick);
    for (node = list; node != NULL; node = node->next->next)
    {
        int ret;
        PyObject *tup;

        tup = Py_BuildValue("(NN)", 
                py_irssi_chat_new(node->data, 1), 
                py_irssi_chat_new(node->next->data, 1));
        if (!tup)
        {
            Py_XDECREF(pylist);
            return NULL;
        }
        
        ret = PyList_Append(pylist, tup);
        Py_DECREF(tup);
        if (ret != 0)
        {
            Py_XDECREF(pylist);
            return NULL;
        }
    }

    return pylist;
}

PyDoc_STRVAR(PyServer_queries_doc,
    "queries() -> list of Query objects\n"
    "\n"
    "Return a list of queries for server.\n"
);
static PyObject *PyServer_queries(PyServer *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);
    
    return py_irssi_chatlist_new(self->data->queries, 1);
}

PyDoc_STRVAR(PyServer_query_find_doc,
    "query_find(nick) -> Query object or None\n"
    "\n"
    "Find a query on this server.\n"
);
static PyObject *PyServer_query_find(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    char *nick = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &nick))
        return NULL;

    return py_irssi_chat_new(query_find(self->data, nick), 1);
}

PyDoc_STRVAR(PyServer_mask_match_doc,
    "mask_match(mask, nick, user, host) -> bool\n"
    "\n"
    "Return true if mask matches nick!user@host\n"
);
static PyObject *PyServer_mask_match(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"mask", "nick", "user", "host", NULL};
    char *mask = "";
    char *nick = "";
    char *user = "";
    char *host = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssss", kwlist, 
           &mask, &nick, &user, &host))
        return NULL;

    return PyBool_FromLong(mask_match(self->data, mask, nick, user, host));
}

PyDoc_STRVAR(PyServer_mask_match_address_doc,
    "mask_match_address(mask, nick, address) -> bool\n"
    "\n"
    "Return True if mask matches nick!address\n"
);
static PyObject *PyServer_mask_match_address(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"mask", "nick", "address", NULL};
    char *mask = "";
    char *nick = "";
    char *address = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss", kwlist, 
           &mask, &nick, &address))
        return NULL;

    return PyBool_FromLong(mask_match_address(self->data, mask, nick, address));
}

PyDoc_STRVAR(PyServer_masks_match_doc,
    "masks_match(masks, nick, address) -> bool\n"
    "\n"
    "Return True if any mask in the masks (string separated by spaces)\n"
    "matches nick!address\n"
);
static PyObject *PyServer_masks_match(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"masks", "nick", "address", NULL};
    char *masks = "";
    char *nick = "";
    char *address = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss", kwlist, 
           &masks, &nick, &address))
        return NULL;

    return PyBool_FromLong(masks_match(self->data, masks, nick, address));
}

PyDoc_STRVAR(PyServer_ignore_check_doc,
    "ignore_check(nick, host, channel, text, level) -> bool\n"
    "\n"
    "Return True if ignore matches\n"
);
static PyObject *PyServer_ignore_check(PyServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", "host", "channel", "text", "level", NULL};
    char *nick = "";
    char *host = "";
    char *channel = "";
    char *text = "";
    int level = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssssi", kwlist, 
           &nick, &host, &channel, &text, &level))
        return NULL;

    return PyBool_FromLong(ignore_check(self->data, 
                nick, host, channel, text, level));
}

/* Methods for object */
static PyMethodDef PyServer_methods[] = {
    {"prnt", (PyCFunction)PyServer_prnt, METH_VARARGS | METH_KEYWORDS, 
        print_doc}, 
    {"command", (PyCFunction)PyServer_command, METH_VARARGS | METH_KEYWORDS, 
        command_doc}, 
    {"disconnect", (PyCFunction)PyServer_disconnect, METH_NOARGS, 
        disconnect_doc}, 
    {"isnickflag", (PyCFunction)PyServer_isnickflag, METH_VARARGS | METH_KEYWORDS, 
        isnickflag_doc}, 
    {"ischannel", (PyCFunction)PyServer_ischannel, METH_VARARGS | METH_KEYWORDS, 
        ischannel_doc}, 
    {"get_nick_flags", (PyCFunction)PyServer_get_nick_flags, METH_NOARGS, 
        get_nick_flags_doc},
    {"send_message", (PyCFunction)PyServer_send_message, METH_VARARGS | METH_KEYWORDS, 
        send_message_doc}, 
    {"channels_join", (PyCFunction)PyServer_channels_join, METH_VARARGS | METH_KEYWORDS, 
        channels_join_doc}, 
    {"window_item_find", (PyCFunction)PyServer_window_item_find, METH_VARARGS | METH_KEYWORDS,
        PyServer_window_item_find_doc},
    {"window_find_item", (PyCFunction)PyServer_window_find_item, METH_VARARGS | METH_KEYWORDS,
        PyServer_window_find_item_doc},
    {"window_find_level", (PyCFunction)PyServer_window_find_level, METH_VARARGS | METH_KEYWORDS,
        PyServer_window_find_level_doc},
    {"window_find_closest", (PyCFunction)PyServer_window_find_closest, METH_VARARGS | METH_KEYWORDS,
        PyServer_window_find_closest_doc},
    {"channels", (PyCFunction)PyServer_channels, METH_NOARGS,
        PyServer_channels_doc},
    {"channel_find", (PyCFunction)PyServer_channel_find, METH_VARARGS | METH_KEYWORDS,
        PyServer_channel_find_doc},
    {"nicks_get_same", (PyCFunction)PyServer_nicks_get_same, METH_VARARGS | METH_KEYWORDS,
        PyServer_nicks_get_same_doc},
    {"queries", (PyCFunction)PyServer_queries, METH_NOARGS,
        PyServer_queries_doc},
    {"query_find", (PyCFunction)PyServer_query_find, METH_VARARGS | METH_KEYWORDS,
        PyServer_query_find_doc},
    {"mask_match", (PyCFunction)PyServer_mask_match, METH_VARARGS | METH_KEYWORDS,
        PyServer_mask_match_doc},
    {"mask_match_address", (PyCFunction)PyServer_mask_match_address, METH_VARARGS | METH_KEYWORDS,
        PyServer_mask_match_address_doc},
    {"masks_match", (PyCFunction)PyServer_masks_match, METH_VARARGS | METH_KEYWORDS,
        PyServer_masks_match_doc},
    {"ignore_check", (PyCFunction)PyServer_ignore_check, METH_VARARGS | METH_KEYWORDS,
        PyServer_ignore_check_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyServerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Server",            /*tp_name*/
    sizeof(PyServer),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyServer_dealloc, /*tp_dealloc*/
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
    "PyServer objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyServer_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyServer_getseters,        /* tp_getset */
    &PyIrssiChatBaseType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* server factory function 
   connect arg should point to a wrapped SERVER_CONNECT */
PyObject *pyserver_sub_new(void *server, PyTypeObject *subclass)
{
    static const char *SERVER_TYPE = "SERVER";
    SERVER_REC *srec = server;
    PyServer *pyserver = NULL;
    PyObject *rawlog = NULL;
    PyObject *connect = NULL;
    
    g_return_val_if_fail(server != NULL, NULL);

    connect = py_irssi_chat_new(srec->connrec, 0); 
    if (!connect)
        return NULL;

    /* FIXME */
    /*
    if (srec->rawlog)
    {
        rawlog = pyrawlog_new(srec->rawlog);
        if (!rawlog)
            return NULL;
    }
    */

    pyserver = py_instp(PyServer, subclass);
    if (!pyserver)
        return NULL;

    pyserver->base_name = SERVER_TYPE;
    pyserver->data = server;
    signal_add_last_data("server disconnected", server_cleanup, pyserver);
    pyserver->cleanup_installed = 1;
    pyserver->rawlog = rawlog;
    pyserver->connect = connect;

    return (PyObject *)pyserver;
}

PyObject *pyserver_new(void *server)
{
    return pyserver_sub_new(server, &PyServerType);
}

int server_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyServerType) < 0)
        return 0;
    
    Py_INCREF(&PyServerType);
    PyModule_AddObject(py_module, "Server", (PyObject *)&PyServerType);

    return 1;
}
