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
#include "window-object.h"
#include "factory.h"
#include "pycore.h"
#include "pyutils.h"

/* monitor "window destroyed" signal */
static void window_cleanup(WINDOW_REC *win)
{
    PyWindow *pywindow = signal_get_user_data();

    if (win == pywindow->data)
    {
        pywindow->data = NULL;
        pywindow->cleanup_installed = 0;
        signal_remove_data("window destroyed", window_cleanup, pywindow);
    }
}

static void PyWindow_dealloc(PyWindow *self)
{
    if (self->cleanup_installed)
        signal_remove_data("window destroyed", window_cleanup, self); 

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyWindow_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyWindow *self;

    self = (PyWindow *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyWindow_refnum_doc,
    "Reference number"
);
static PyObject *PyWindow_refnum_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->refnum);
}

PyDoc_STRVAR(PyWindow_name_doc,
    "Name"
);
static PyObject *PyWindow_name_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->name);
}

PyDoc_STRVAR(PyWindow_width_doc,
    "Width"
);
static PyObject *PyWindow_width_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->width);
}

PyDoc_STRVAR(PyWindow_height_doc,
    "Height"
);
static PyObject *PyWindow_height_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->height);
}

PyDoc_STRVAR(PyWindow_history_name_doc,
    "Name of named historylist for this window"
);
static PyObject *PyWindow_history_name_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->history_name);
}

PyDoc_STRVAR(PyWindow_active_doc,
    "Active window item"
);
static PyObject *PyWindow_active_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return py_irssi_chat_new(self->data->active, 1);
}

PyDoc_STRVAR(PyWindow_active_server_doc,
    "Active server"
);
static PyObject *PyWindow_active_server_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return py_irssi_chat_new(self->data->active_server, 1);
}

PyDoc_STRVAR(PyWindow_servertag_doc,
    "active_server must be either None or have this same tag"
    "(unless there's items in this window). This is used by"
	"/WINDOW SERVER -sticky"
);
static PyObject *PyWindow_servertag_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->servertag);
}

PyDoc_STRVAR(PyWindow_level_doc,
    "Current window level"
);
static PyObject *PyWindow_level_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->level);
}

PyDoc_STRVAR(PyWindow_sticky_refnum_doc,
    "True if reference number is sticky"
);
static PyObject *PyWindow_sticky_refnum_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->sticky_refnum);
}

PyDoc_STRVAR(PyWindow_data_level_doc,
    "Current data level"
);
static PyObject *PyWindow_data_level_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->data_level);
}

PyDoc_STRVAR(PyWindow_hilight_color_doc,
    "Current activity hilight color"
);
static PyObject *PyWindow_hilight_color_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->hilight_color);
}

PyDoc_STRVAR(PyWindow_last_timestamp_doc,
    "Last time timestamp was written in window"
);
static PyObject *PyWindow_last_timestamp_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(self->data->last_timestamp);
}

PyDoc_STRVAR(PyWindow_last_line_doc,
    "Last time text was written in window"
);
static PyObject *PyWindow_last_line_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(self->data->last_line);
}

PyDoc_STRVAR(PyWindow_theme_name_doc,
    "Active theme in window, None = default"
);
static PyObject *PyWindow_theme_name_get(PyWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->theme_name);
}

/* specialized getters/setters */
static PyGetSetDef PyWindow_getseters[] = {
    {"refnum", (getter)PyWindow_refnum_get, NULL,
        PyWindow_refnum_doc, NULL},
    {"name", (getter)PyWindow_name_get, NULL,
        PyWindow_name_doc, NULL},
    {"width", (getter)PyWindow_width_get, NULL,
        PyWindow_width_doc, NULL},
    {"height", (getter)PyWindow_height_get, NULL,
        PyWindow_height_doc, NULL},
    {"history_name", (getter)PyWindow_history_name_get, NULL,
        PyWindow_history_name_doc, NULL},
    {"active", (getter)PyWindow_active_get, NULL,
        PyWindow_active_doc, NULL},
    {"active_server", (getter)PyWindow_active_server_get, NULL,
        PyWindow_active_server_doc, NULL},
    {"servertag", (getter)PyWindow_servertag_get, NULL,
        PyWindow_servertag_doc, NULL},
    {"level", (getter)PyWindow_level_get, NULL,
        PyWindow_level_doc, NULL},
    {"sticky_refnum", (getter)PyWindow_sticky_refnum_get, NULL,
        PyWindow_sticky_refnum_doc, NULL},
    {"data_level", (getter)PyWindow_data_level_get, NULL,
        PyWindow_data_level_doc, NULL},
    {"hilight_color", (getter)PyWindow_hilight_color_get, NULL,
        PyWindow_hilight_color_doc, NULL},
    {"last_timestamp", (getter)PyWindow_last_timestamp_get, NULL,
        PyWindow_last_timestamp_doc, NULL},
    {"last_line", (getter)PyWindow_last_line_get, NULL,
        PyWindow_last_line_doc, NULL},
    {"theme_name", (getter)PyWindow_theme_name_get, NULL,
        PyWindow_theme_name_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyWindow_items_doc,
    "items() -> list of WindowItem objects\n"
    "\n"
    "Return a list of items in window.\n"
);
static PyObject *PyWindow_items(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);
    return py_irssi_chatlist_new(self->data->items, 1);
}

PyDoc_STRVAR(PyWindow_prnt_doc,
    "prnt(str, level=MSGLEVEL_CLIENTNOTICE) -> None\n"
    "\n"
    "Print to window\n"
);
static PyObject *PyWindow_prnt(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", "level", NULL};
    char *str = "";
    int level = MSGLEVEL_CLIENTNOTICE;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, 
           &str, &level))
        return NULL;

    printtext_string_window(self->data, level, str);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_command_doc,
    "command(cmd) -> None\n"
    "\n"
    "Send command to window\n"
);
static PyObject *PyWindow_command(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"cmd", NULL};
    char *cmd = "";
    WINDOW_REC *old;
    
    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &cmd))
        return NULL;

    old = active_win;
    active_win = self->data;
    py_command(cmd, active_win->active_server, active_win->active);
    if (g_slist_find(windows, old) != NULL)
        active_win = old;

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_item_add_doc,
    "item_add(item, automatic=False) -> None\n"
    "\n"
    "Add window item\n"
);
static PyObject *PyWindow_item_add(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", "automatic", NULL};
    PyObject *item = NULL;
    int automatic = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|i", kwlist, 
           &item, &automatic))
        return NULL;

    if (!pywindow_item_check(item))
        return PyErr_Format(PyExc_TypeError, "item must be window item");
    
    window_item_add(self->data, ((PyWindowItem*)item)->data, automatic);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_item_remove_doc,
    "item_remove(item) -> None\n"
    "\n"
    "Remove window item\n"
);
static PyObject *PyWindow_item_remove(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", NULL};
    PyObject *item = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &item))
        return NULL;

    if (!pywindow_item_check(item))
        return PyErr_Format(PyExc_TypeError, "item must be window item");

    window_item_remove(((PyWindowItem*)item)->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_item_destroy_doc,
    "item_destroy(item) -> None\n"
    "\n"
    "Destroy window item\n"
);
static PyObject *PyWindow_item_destroy(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", NULL};
    PyObject *item = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &item))
        return NULL;

    if (!pywindow_item_check(item))
        return PyErr_Format(PyExc_TypeError, "item must be window item");

    window_item_destroy(((PyWindowItem*)item)->data);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_item_prev_doc,
    "item_prev() -> None\n"
    "\n"
    "Change to previous window item\n"
);
static PyObject *PyWindow_item_prev(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_item_prev(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_item_next_doc,
    "item_next() -> None\n"
    "\n"
    "Change to next window item\n"
);
static PyObject *PyWindow_item_next(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_item_next(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_destroy_doc,
    "destroy() -> None\n"
    "\n"
    "Destroy the window.\n"
);
static PyObject *PyWindow_destroy(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_destroy(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_set_active_doc,
    "set_active() -> None\n"
    "\n"
    "Set window active.\n"
);
static PyObject *PyWindow_set_active(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    window_set_active(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_change_server_doc,
    "change_server(server) -> None\n"
    "\n"
    "Change server in window\n"
);
static PyObject *PyWindow_change_server(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"server", NULL};
    PyObject *server = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &server))
        return NULL;

    if (!pyserver_check(server))
        return PyErr_Format(PyExc_TypeError, "arg must be server");
   
    window_change_server(self->data, ((PyServer*)server)->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_set_refnum_doc,
    "set_refnum(refnum) -> None\n"
    "\n"
    "Set window refnum\n"
);
static PyObject *PyWindow_set_refnum(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"refnum", NULL};
    int refnum = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, 
           &refnum))
        return NULL;

    window_set_refnum(self->data, refnum);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_set_name_doc,
    "set_name(name) -> None\n"
    "\n"
    "Set window name\n"
);
static PyObject *PyWindow_set_name(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", NULL};
    char *name = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &name))
        return NULL;

    window_set_name(self->data, name);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_set_history_doc,
    "set_history(history) -> None\n"
    "\n"
    "Set window history\n"
);
static PyObject *PyWindow_set_history(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"history", NULL};
    char *history = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &history))
        return NULL;

    window_set_history(self->data, history);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_set_level_doc,
    "set_level(level) -> None\n"
    "\n"
    "Set window level\n"
);
static PyObject *PyWindow_set_level(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"level", NULL};
    int level = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, 
           &level))
        return NULL;

    window_set_level(self->data, level);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_activity_doc,
    "activity(data_level, hilight_color) -> None\n"
    "\n"
);
static PyObject *PyWindow_activity(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"data_level", "hilight_color", NULL};
    int data_level = 0;
    char *hilight_color = NULL;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i|s", kwlist, 
           &data_level, &hilight_color))
        return NULL;

    window_activity(self->data, data_level, hilight_color);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyWindow_get_active_name_doc,
    "get_active_name() -> str or None\n"
    "\n"
    "Return active item's name, or if none is active, window's name.\n"
);
static PyObject *PyWindow_get_active_name(PyWindow *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(window_get_active_name(self->data));
}

PyDoc_STRVAR(PyWindow_item_find_doc,
    "item_find(server, name) -> WindowItem or None\n"
    "\n"
    "Find window item that matches best to given arguments\n"
);
static PyObject *PyWindow_item_find(PyWindow *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"server", "name", NULL};
    PyObject *server = NULL;
    char *name = "";
    WI_ITEM_REC *witem;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "Os", kwlist, 
           &server, &name))
        return NULL;

    if (!pyserver_check(server))
        return PyErr_Format(PyExc_TypeError, "arg 1 must be server");

    witem = window_item_find_window(self->data, ((PyServer*)server)->data, name);
    return py_irssi_chat_new(witem, 1);
}

static PyMethodDef PyWindow_methods[] = {
    {"items", (PyCFunction)PyWindow_items, METH_NOARGS,
        PyWindow_items_doc},
    {"prnt", (PyCFunction)PyWindow_prnt, METH_VARARGS | METH_KEYWORDS,
        PyWindow_prnt_doc},
    {"command", (PyCFunction)PyWindow_command, METH_VARARGS | METH_KEYWORDS,
        PyWindow_command_doc},
    {"item_add", (PyCFunction)PyWindow_item_add, METH_VARARGS | METH_KEYWORDS,
        PyWindow_item_add_doc},
    {"item_remove", (PyCFunction)PyWindow_item_remove, METH_VARARGS | METH_KEYWORDS,
        PyWindow_item_remove_doc},
    {"item_destroy", (PyCFunction)PyWindow_item_destroy, METH_VARARGS | METH_KEYWORDS,
        PyWindow_item_destroy_doc},
    {"item_prev", (PyCFunction)PyWindow_item_prev, METH_NOARGS,
        PyWindow_item_prev_doc},
    {"item_next", (PyCFunction)PyWindow_item_next, METH_NOARGS,
        PyWindow_item_next_doc},
    {"destroy", (PyCFunction)PyWindow_destroy, METH_NOARGS,
        PyWindow_destroy_doc},
    {"set_active", (PyCFunction)PyWindow_set_active, METH_NOARGS,
        PyWindow_set_active_doc},
    {"change_server", (PyCFunction)PyWindow_change_server, METH_VARARGS | METH_KEYWORDS,
        PyWindow_change_server_doc},
    {"set_refnum", (PyCFunction)PyWindow_set_refnum, METH_VARARGS | METH_KEYWORDS,
        PyWindow_set_refnum_doc},
    {"set_name", (PyCFunction)PyWindow_set_name, METH_VARARGS | METH_KEYWORDS,
        PyWindow_set_name_doc},
    {"set_history", (PyCFunction)PyWindow_set_history, METH_VARARGS | METH_KEYWORDS,
        PyWindow_set_history_doc},
    {"set_level", (PyCFunction)PyWindow_set_level, METH_VARARGS | METH_KEYWORDS,
        PyWindow_set_level_doc},
    {"activity", (PyCFunction)PyWindow_activity, METH_VARARGS | METH_KEYWORDS,
        PyWindow_activity_doc},
    {"get_active_name", (PyCFunction)PyWindow_get_active_name, METH_NOARGS,
        PyWindow_get_active_name_doc},
    {"item_find", (PyCFunction)PyWindow_item_find, METH_VARARGS | METH_KEYWORDS,
        PyWindow_item_find_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyWindowType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Window",            /*tp_name*/
    sizeof(PyWindow),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyWindow_dealloc, /*tp_dealloc*/
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
    "PyWindow objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyWindow_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyWindow_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyWindow_new,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pywindow_new(void *win)
{
    PyWindow *pywindow;

    pywindow = py_inst(PyWindow, PyWindowType);
    if (!pywindow)
        return NULL;

    pywindow->data = win;
    pywindow->cleanup_installed = 1;
    signal_add_last_data("window destroyed", window_cleanup, pywindow);

    return (PyObject *)pywindow;
}

int window_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyWindowType) < 0)
        return 0;
    
    Py_INCREF(&PyWindowType);
    PyModule_AddObject(py_module, "Window", (PyObject *)&PyWindowType);

    return 1;
}
