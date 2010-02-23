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
#include "window-item-object.h"
#include "pyirssi.h"
#include "pycore.h"
#include "pyutils.h"
#include "factory.h"

/* Dealloc is overridden by sub types */

PyDoc_STRVAR(PyWindowItem_server_doc,
    "Active name for item"
);
static PyObject *PyWindowItem_server_get(PyWindowItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->server);
}

PyDoc_STRVAR(PyWindowItem_name_doc,
    "Name of the item"
);
static PyObject *PyWindowItem_name_get(PyWindowItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->visible_name);
}

PyDoc_STRVAR(PyWindowItem_createtime_doc,
    "Time the witem was created"
);
static PyObject *PyWindowItem_createtime_get(PyWindowItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromLong(self->data->createtime);
}

PyDoc_STRVAR(PyWindowItem_data_level_doc,
    "0=no new data, 1=text, 2=msg, 3=highlighted text"
);
static PyObject *PyWindowItem_data_level_get(PyWindowItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->data_level);
}

PyDoc_STRVAR(PyWindowItem_hilight_color_doc,
    "Color of the last highlighted text"
);
static PyObject *PyWindowItem_hilight_color_get(PyWindowItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->hilight_color);
}

/* specialized getters/setters */
static PyGetSetDef PyWindowItem_getseters[] = {
    {"server", (getter)PyWindowItem_server_get, NULL,
        PyWindowItem_server_doc, NULL},
    {"name", (getter)PyWindowItem_name_get, NULL,
        PyWindowItem_name_doc, NULL},
    {"createtime", (getter)PyWindowItem_createtime_get, NULL,
        PyWindowItem_createtime_doc, NULL},
    {"data_level", (getter)PyWindowItem_data_level_get, NULL,
        PyWindowItem_data_level_doc, NULL},
    {"hilight_color", (getter)PyWindowItem_hilight_color_get, NULL,
        PyWindowItem_hilight_color_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyWindowItem_prnt_doc,
    "prnt(str, level) -> None\n"
    "\n"
    "Print to window item\n"
);
static PyObject *PyWindowItem_prnt(PyWindowItem *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", "level", NULL};
    char *str;
    int level = MSGLEVEL_CLIENTNOTICE;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, &str, &level))
        return NULL;

    printtext_string(self->data->server, self->data->visible_name, level, str);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_command_doc,
    "command(cmd) -> None\n"
    "\n"
    "Send command to window item\n"
);
static PyObject *PyWindowItem_command(PyWindowItem *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", NULL};
    char *cmd;

    RET_NULL_IF_INVALID(self->data);
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &cmd))
        return NULL;

    py_command(cmd, self->data->server, self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_window_doc,
    "window() -> Window object or None\n"
    "\n"
    "Return parent window for window item\n"
);
static PyObject *PyWindowItem_window(PyWindowItem *self, PyObject *args)
{
    WINDOW_REC *win;

    RET_NULL_IF_INVALID(self->data);
   
    win = window_item_window(self->data);
    if (win)
        return pywindow_new(win);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_change_server_doc,
    "change_server(server) -> None\n"
    "\n"
    "Change server for window item\n"
);
static PyObject *PyWindowItem_change_server(PyWindowItem *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"server", NULL};
    PyObject *server = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &server))
        return NULL;

    if (!pyserver_check(server))
        return PyErr_Format(PyExc_TypeError, "arg must be server");
   
    window_item_change_server(self->data, ((PyServer*)server)->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_is_active_doc,
    "is_active() -> bool\n"
    "\n"
    "Returns true if window item is active\n"
);
static PyObject *PyWindowItem_is_active(PyWindowItem *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    return PyBool_FromLong(window_item_is_active(self->data));
}

PyDoc_STRVAR(PyWindowItem_set_active_doc,
    "set_active() -> None\n"
    "\n"
    "Set window item active\n"
);
static PyObject *PyWindowItem_set_active(PyWindowItem *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_item_set_active(window_item_window(self->data), self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_activity_doc,
    "activity(data_level, hilight_color) -> None\n"
    "\n"
);
static PyObject *PyWindowItem_activity(PyWindowItem *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"data_level", "hilight_color", NULL};
    int data_level = 0;
    char *hilight_color = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "is", kwlist, 
           &data_level, &hilight_color))
        return NULL;

    window_item_activity(self->data, data_level, hilight_color);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_destroy_doc,
    "destroy() -> None\n"
    "\n"
    "Destroy channel or query\n"
);
static PyObject *PyWindowItem_destroy(PyWindowItem *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_item_destroy(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindowItem_get_dcc_doc,
    "get_dcc() -> DccChat object or None\n"
    "\n"
    "If item is a query of a =nick, return DCC chat record of nick\n"
);
static PyObject *PyWindowItem_get_dcc(PyWindowItem *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);
    return py_irssi_new(self->data, 1);
}

/* Methods for object */
static PyMethodDef PyWindowItem_methods[] = {
    {"prnt", (PyCFunction)PyWindowItem_prnt, METH_VARARGS | METH_KEYWORDS, 
        PyWindowItem_prnt_doc},
    {"command", (PyCFunction)PyWindowItem_command, METH_VARARGS | METH_KEYWORDS, 
        PyWindowItem_command_doc},
    {"window", (PyCFunction)PyWindowItem_window, METH_NOARGS,
        PyWindowItem_window_doc},
    {"change_server", (PyCFunction)PyWindowItem_change_server, METH_VARARGS | METH_KEYWORDS,
        PyWindowItem_change_server_doc},
    {"is_active", (PyCFunction)PyWindowItem_is_active, METH_NOARGS,
        PyWindowItem_is_active_doc},
    {"set_active", (PyCFunction)PyWindowItem_set_active, METH_NOARGS,
        PyWindowItem_set_active_doc},
    {"activity", (PyCFunction)PyWindowItem_activity, METH_VARARGS | METH_KEYWORDS,
        PyWindowItem_activity_doc},
    {"destroy", (PyCFunction)PyWindowItem_destroy, METH_NOARGS,
        PyWindowItem_destroy_doc},
    {"get_dcc", (PyCFunction)PyWindowItem_get_dcc, METH_NOARGS,
        PyWindowItem_get_dcc_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyWindowItemType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.WindowItem",            /*tp_name*/
    sizeof(PyWindowItem),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                      /*tp_dealloc*/
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
    "PyWindowItem objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyWindowItem_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyWindowItem_getseters,        /* tp_getset */
    &PyIrssiChatBaseType,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pywindow_item_sub_new(void *witem, const char *name, PyTypeObject *subclass)
{
    WI_ITEM_REC *rec = witem;
    PyWindowItem *pywitem = NULL;
    PyObject *server;

    g_return_val_if_fail(witem != NULL, NULL);
   
    server = py_irssi_chat_new(rec->server, 1);
    if (!server)
        return NULL;

    pywitem = py_instp(PyWindowItem, subclass); 
    if (!pywitem)
        return NULL;

    pywitem->data = witem;
    pywitem->base_name = name;
    pywitem->server = server;

    return (PyObject *)pywitem;
}

PyObject *pywindow_item_new(void *witem)
{
    return pywindow_item_sub_new(witem, NULL, &PyWindowItemType);
}

int window_item_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyWindowItemType) < 0)
        return 0;
    
    Py_INCREF(&PyWindowItemType);
    PyModule_AddObject(py_module, "WindowItem", (PyObject *)&PyWindowItemType);

    return 1;
}
