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
#include "irc-server-object.h"
#include "factory.h"
#include "pyirssi_irc.h"
#include "pycore.h"
#include "pyutils.h"

/* cleanup and dealloc inherited from base Server */

/* Getters */
PyDoc_STRVAR(PyIrcServer_real_address_doc,
    "Address the IRC server gives"
);
static PyObject *PyIrcServer_real_address_get(PyIrcServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->real_address);
}

PyDoc_STRVAR(PyIrcServer_usermode_doc,
    "User mode in server"
);
static PyObject *PyIrcServer_usermode_get(PyIrcServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->usermode);
}

PyDoc_STRVAR(PyIrcServer_userhost_doc,
    "Your user host in server"
);
static PyObject *PyIrcServer_userhost_get(PyIrcServer *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->userhost);
}

static PyGetSetDef PyIrcServer_getseters[] = {
    {"real_address", (getter)PyIrcServer_real_address_get, NULL,
        PyIrcServer_real_address_doc, NULL},
    {"usermode", (getter)PyIrcServer_usermode_get, NULL,
        PyIrcServer_usermode_doc, NULL},
    {"userhost", (getter)PyIrcServer_userhost_get, NULL,
        PyIrcServer_userhost_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(get_channels_doc,
    "get_channels() -> str\n"
    "\n"
    "Return a string of all channels (and keys, if any have them) in server,\n"
    "like '#a,#b,#c,#d x,b_chan_key,x,x' or just '#e,#f,#g'\n"
);
static PyObject *PyIrcServer_get_channels(PyIrcServer *self, PyObject *args)
{
    char *list;
    PyObject *ret;

    RET_NULL_IF_INVALID(self->data);

    list = irc_server_get_channels(self->data);
    ret = PyString_FromString(list);
    g_free(list);

    return ret;
}

PyDoc_STRVAR(send_raw_doc,
    "send_raw(cmd) -> None\n"
    "\n"
    "Send raw message to server, it will be flood protected so you\n"
    "don't need to worry about it.\n"
);
static PyObject *PyIrcServer_send_raw(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", NULL};
    char *cmd;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &cmd))
        return NULL;

    irc_send_cmd(self->data, cmd);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(send_raw_now_doc,
    "send_raw_now(cmd) -> None\n"
    "\n"
    "Send raw message to server immediately without flood protection.\n"
);
static PyObject *PyIrcServer_send_raw_now(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", NULL};
    char *cmd;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &cmd))
        return NULL;

    irc_send_cmd_now(self->data, cmd);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(send_raw_split_doc,
    "send_raw_split(cmd, nickarg, max_nicks) -> None\n"
    "\n"
    "Split the `cmd' into several commands so `nickarg' argument has only\n"
    "`max_nicks' number of nicks.\n"
    "\n"
    "Example:\n"
    "server.send_raw_split('KICK #channel nick1,nick2,nick3 :byebye', 3, 2)\n"
    "\n"
    "Irssi will send commands 'KICK #channel nick1,nick2 :byebye' and\n"
    "'KICK #channel nick3 :byebye' to server.\n"
);
static PyObject *PyIrcServer_send_raw_split(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", "nickarg", "max_nicks", NULL};
    char *cmd;
    int nickarg;
    int max_nicks;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sii", kwlist, &cmd, &nickarg, &max_nicks))
        return NULL;

    irc_send_cmd_split(self->data, cmd, nickarg, max_nicks);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(ctcp_send_reply_doc,
    "ctcp_send_reply(data) -> None\n"
    "\n"
    "Send CTCP reply. This will be 'CTCP flood protected' so if there's too\n"
    "many CTCP requests in buffer, this reply might not get sent. The data\n"
    "is the full raw command to be sent to server, like\n"
    "'NOTICE nick :\001VERSION irssi\001'\n"
);
static PyObject *PyIrcServer_ctcp_send_reply(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"data", NULL};
    char *data;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &data))
        return NULL;

    ctcp_send_reply(self->data, data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(isupport_doc,
    "isupport(name) -> str or None\n"
    "\n"
    "Returns the value of the named item in the ISUPPORT (005) numeric to the\n"
    "script. If the item is not present returns undef, if the item has no value\n"
    "then '' is returned use defined server.isupport('name') if you need to\n"
    "check whether a property is present.\n"
    "See http://www.ietf.org/internet-drafts/draft-brocklesby-irc-isupport-01.txt\n"  
    "for more information on the ISUPPORT numeric.\n"
);
static PyObject *PyIrcServer_isupport(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", NULL};
    char *name;
    char *found;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &name))
        return NULL;

    found = g_hash_table_lookup(self->data->isupport, name);

    RET_AS_STRING_OR_NONE(found);
}

PyDoc_STRVAR(PyIrcServer_netsplit_find_doc,
    "netsplit_find(nick, address) -> Netsplit object or None\n"
    "\n"
    "Check if nick!address is on the other side of netsplit. Netsplit records\n"
    "are automatically removed after 30 minutes (current default)..\n"
);
static PyObject *PyIrcServer_netsplit_find(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", "address", NULL};
    char *nick = "";
    char *address = "";
    NETSPLIT_REC *ns;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, 
           &nick, &address))
        return NULL;

    ns = netsplit_find(self->data, nick, address);
    if (ns)
        return pynetsplit_new(ns);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyIrcServer_netsplit_find_channel_doc,
    "netsplit_find_channel(nick, address, channel) -> NetsplitChannel object or None\n"
    "\n"
    "Find nick record for nick!address in channel `channel'.\n"
);
static PyObject *PyIrcServer_netsplit_find_channel(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", "address", "channel", NULL};
    char *nick = "";
    char *address = "";
    char *channel = "";
    NETSPLIT_CHAN_REC *nsc;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss", kwlist, 
           &nick, &address, &channel))
        return NULL;

    nsc = netsplit_find_channel(self->data, nick, address, channel);
    if (nsc)
        return pynetsplit_channel_new(nsc);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyIrcServer_notifylist_ison_doc,
    "notifylist_ison(nick) -> bool\n"
    "\n"
    "Check if nick is on server\n"
);
static PyObject *PyIrcServer_notifylist_ison(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"nick", NULL};
    char *nick = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &nick))
        return NULL;

    return PyBool_FromLong(notifylist_ison_server(self->data, nick));
}

/* expect a list of tuples [('str', 'str'), ...] */
static GSList *py_event_conv(PyObject *list)
{
    int i;
    GSList *ret = NULL;

    if (!PyList_Check(list))
    {
        PyErr_Format(PyExc_TypeError, "expect a list of tuples of two strings");  
        return NULL;
    }

    for (i = 0; i < PyList_Size(list); i++)
    {
        char *key;
        char *val;
        PyObject *tup = PyList_GET_ITEM(list, i);

        if (!PyTuple_Check(tup) || !PyArg_ParseTuple(tup, "ss", &key, &val))
        {
            GSList *node;

            for (node = ret; node; node = node->next)
                g_free(node->data);
            
            g_slist_free(ret);
            
            if (!PyErr_Occurred() || PyErr_ExceptionMatches(PyExc_TypeError))
            {
                PyErr_Clear();
                PyErr_SetString(PyExc_TypeError, "expect a list of tuples of two strings");
            }

            return NULL;
        }

        ret = g_slist_append(ret, g_strdup(key));
        ret = g_slist_append(ret, g_strdup(val));
    }

    return ret;
}

PyDoc_STRVAR(PyIrcServer_redirect_event_doc,
    "redirect_event(command, signals, arg=None, count=1, remote=-1, failure_signal=None) -> None\n"
    "\n"
    "Specify that the next command sent to server will be redirected.\n"
    "NOTE: This command MUST be called before sending the command to server.\n"
    "\n"
    "`command' - Name of the registered redirection that we're using.\n"
    "\n"
    "`count' - How many times to execute the redirection. Some commands may\n"
    "send multiple stop events, like MODE #a,#b.\n"
    "\n"
    "`arg' - The argument to be compared in event strings. You can give multiple\n"
    "arguments separated with space.\n"
    "\n"
    "`remote' - Specifies if the command is a remote command, -1 = use default.\n"
    "\n"
    "`failure_signal' - If irssi can't find the stop signal for the redirection,\n"
    "this signal is called.\n"
    "\n"
    "`signals' - hash reference with \"event\" => \"redir signal\" entries.\n"
    "If the event is "", all the events belonging to the redirection but not\n"
    "specified here, will be sent there.\n"
    "\n"
    "Example:\n"
    "\n"
    "# ignore all events generated by whois query, except 311.\n"
    "\n"
    "server.redirect_event(\"whois\",\n"
    "    remote = 0,\n"
    "    arg = \"cras\",\n"
    "    signals = [\n"
    "        ('event 311', 'redir whois'),\n"
    "        ('', 'event empty') \n"
    "    ]\n"
    ")\n"
    "server.send_raw(\"WHOIS :cras\")\n"
);
static PyObject *PyIrcServer_redirect_event(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"command", "signals", "arg", "count", "remote", "failure_signal", NULL};
    char *command = "";
    int count = 1;
    char *arg = NULL;
    int remote = -1;
    char *failure_signal = NULL;
    PyObject *signals = NULL;
    GSList *gsignals;
    
    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO|ziiz", kwlist, 
           &command, &signals, &arg, &count, &remote, &failure_signal))
        return NULL;

    gsignals = py_event_conv(signals);
    if (!gsignals)
        return NULL;

    server_redirect_event(self->data, command, count, arg, remote, failure_signal, gsignals); 
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyIrcServer_redirect_get_signal_doc,
    "redirect_get_signal(event, args) -> str\n"
);
static PyObject *PyIrcServer_redirect_get_signal(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"event", "args", NULL};
    char *event = "";
    char *pargs = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, 
           &event, &pargs))
        return NULL;

    RET_AS_STRING_OR_NONE(server_redirect_get_signal(self->data, event, pargs));
}

PyDoc_STRVAR(PyIrcServer_redirect_peek_signal_doc,
    "redirect_peek_signal(event, args) -> str\n"
);
static PyObject *PyIrcServer_redirect_peek_signal(PyIrcServer *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"event", "args", NULL};
    char *event = "";
    char *pargs = "";
    int redirection;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, 
           &event, &pargs))
        return NULL;

    RET_AS_STRING_OR_NONE(server_redirect_peek_signal(self->data, event, pargs, &redirection));
}

/* Methods for object */
static PyMethodDef PyIrcServer_methods[] = {
    {"get_channels", (PyCFunction)PyIrcServer_get_channels, METH_NOARGS,
        get_channels_doc},
    {"send_raw", (PyCFunction)PyIrcServer_send_raw, METH_VARARGS | METH_KEYWORDS, 
        send_raw_doc},
    {"send_raw_now", (PyCFunction)PyIrcServer_send_raw_now, METH_VARARGS | METH_KEYWORDS, 
        send_raw_now_doc},
    {"send_raw_split", (PyCFunction)PyIrcServer_send_raw_split, METH_VARARGS | METH_KEYWORDS, 
        send_raw_split_doc},
    {"ctcp_send_reply", (PyCFunction)PyIrcServer_ctcp_send_reply, METH_VARARGS | METH_KEYWORDS, 
        ctcp_send_reply_doc},
    {"isupport", (PyCFunction)PyIrcServer_isupport, METH_VARARGS | METH_KEYWORDS, 
        isupport_doc},
    {"netsplit_find", (PyCFunction)PyIrcServer_netsplit_find, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_netsplit_find_doc},
    {"netsplit_find_channel", (PyCFunction)PyIrcServer_netsplit_find_channel, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_netsplit_find_channel_doc},
    {"notifylist_ison", (PyCFunction)PyIrcServer_notifylist_ison, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_notifylist_ison_doc},
    {"redirect_event", (PyCFunction)PyIrcServer_redirect_event, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_redirect_event_doc},
    {"redirect_get_signal", (PyCFunction)PyIrcServer_redirect_get_signal, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_redirect_get_signal_doc},
    {"redirect_peek_signal", (PyCFunction)PyIrcServer_redirect_peek_signal, METH_VARARGS | METH_KEYWORDS,
        PyIrcServer_redirect_peek_signal_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyIrcServerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.IrcServer",            /*tp_name*/
    sizeof(PyIrcServer),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
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
    "PyIrcServer objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyIrcServer_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyIrcServer_getseters,        /* tp_getset */
    &PyServerType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};

PyObject *pyirc_server_new(void *server)
{
    return pyserver_sub_new(server, &PyIrcServerType);
}

int irc_server_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyIrcServerType) < 0)
        return 0;
    
    Py_INCREF(&PyIrcServerType);
    PyModule_AddObject(py_module, "IrcServer", (PyObject *)&PyIrcServerType);

    return 1;
}
