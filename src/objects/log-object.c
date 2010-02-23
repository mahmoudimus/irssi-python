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
#include "log-object.h"
#include "factory.h"
#include "pycore.h"

static LOG_ITEM_REC *find_item(LOG_REC *log, PyLogitem *item);
static void log_cleanup(LOG_REC *log);
static int logtype(int *type, int target, int window);

/* find/convert a py log item */
static LOG_ITEM_REC *find_item(LOG_REC *log, PyLogitem *item)
{
    int type;
    char *name;
    char *servertag = NULL;

    if (!item->type || !item->name)
        return NULL;

    type = PyInt_AS_LONG(item->type);
    name = PyString_AS_STRING(item->name);
    if (item->servertag)
        servertag = PyString_AS_STRING(item->servertag);

    return log_item_find(log, type, name, servertag);
}

/* monitor "log remove" signal */
static void log_cleanup(LOG_REC *log)
{
    PyLog *pylog = signal_get_user_data();

    if (log == pylog->data)
    {
        pylog->data = NULL;
        pylog->cleanup_installed = 0;
        signal_remove_data("log remove", log_cleanup, pylog);
    }
}

static void PyLog_dealloc(PyLog *self)
{
    if (self->cleanup_installed)
        signal_remove_data("log remove", log_cleanup, self);

    if (self->data && !g_slist_find(logs, self->data))
    {
        printtext(NULL, NULL, MSGLEVEL_CRAP, "destroying orphan log %s", self->data->fname);
        log_close(self->data);
    }
    
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyLog_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyLog *self;

    self = (PyLog *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* function to create the log */
PyDoc_STRVAR(PyLog_doc,
    "__init__(fname, level=MSGLEVEL_ALL)\n"
    "\n"
    "Create a log\n"
);
static int PyLog_init(PyLog *self, PyObject *args, PyObject *kwds)
{
    char *fname;
    int level = MSGLEVEL_ALL;
    LOG_REC *log;

    static char *kwlist[] = {"fname", "level", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist,
            &fname, &level))
        return -1;

    /*XXX: anything better than RuntimeError ? */
    if (self->data || self->cleanup_installed)
    {
        PyErr_Format(PyExc_RuntimeError, "log already opened; close it first");
        return -1;
    }

    log = log_create_rec(fname, level);
    if (!log)
    {
        PyErr_Format(PyExc_RuntimeError, "failed to create log");
        return -1;
    }
   
    self->data = log;
    self->cleanup_installed = 1;
    signal_add_last_data("log remove", log_cleanup, self);
    
    return 0;
}

/* Getters */
PyDoc_STRVAR(PyLog_fname_doc,
    "Log file name"
);
static PyObject *PyLog_fname_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->fname);
}

PyDoc_STRVAR(PyLog_real_fname_doc,
    "The actual opened log file (after %d.%m.Y etc. are expanded)"
);
static PyObject *PyLog_real_fname_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->real_fname);
}

PyDoc_STRVAR(PyLog_opened_doc,
    "Log file is open"
);
static PyObject *PyLog_opened_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(self->data->opened);
}

PyDoc_STRVAR(PyLog_level_doc,
    "Log only these levels"
);
static PyObject *PyLog_level_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->level);
}

PyDoc_STRVAR(PyLog_last_doc,
    "Timestamp when last message was written"
);
static PyObject *PyLog_last_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyLong_FromUnsignedLong(self->data->last);
}

PyDoc_STRVAR(PyLog_autoopen_doc,
    "Automatically open log at startup"
);
static PyObject *PyLog_autoopen_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->autoopen);
}

PyDoc_STRVAR(PyLog_failed_doc,
    "Opening log failed last time"
);
static PyObject *PyLog_failed_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->failed);
}

PyDoc_STRVAR(PyLog_temp_doc,
    "Log isn't saved to config file"
);
static PyObject *PyLog_temp_get(PyLog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->temp);
}

/* specialized getters/setters */
static PyGetSetDef PyLog_getseters[] = {
    {"fname", (getter)PyLog_fname_get, NULL,
        PyLog_fname_doc, NULL},
    {"real_fname", (getter)PyLog_real_fname_get, NULL,
        PyLog_real_fname_doc, NULL},
    {"opened", (getter)PyLog_opened_get, NULL,
        PyLog_opened_doc, NULL},
    {"level", (getter)PyLog_level_get, NULL,
        PyLog_level_doc, NULL},
    {"last", (getter)PyLog_last_get, NULL,
        PyLog_last_doc, NULL},
    {"autoopen", (getter)PyLog_autoopen_get, NULL,
        PyLog_autoopen_doc, NULL},
    {"failed", (getter)PyLog_failed_get, NULL,
        PyLog_failed_doc, NULL},
    {"temp", (getter)PyLog_temp_get, NULL,
        PyLog_temp_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyLog_items_doc,
    "items() -> list of Log objects\n"
    "\n"
    "Return a list of log items\n"
);
static PyObject *PyLog_items(PyLog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);
    return py_irssi_objlist_new(self->data->items, 1, (InitFunc)pylogitem_new);
}

PyDoc_STRVAR(PyLog_update_doc,
    "update() -> None\n"
    "\n"
    "Add log to list of logs / save changes to config file.\n"
);
static PyObject *PyLog_update(PyLog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    log_update(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyLog_close_doc,
    "destroy() -> None\n"
    "\n"
    "Destroy the log file\n"
);
static PyObject *PyLog_close(PyLog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    log_close(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyLog_start_logging_doc,
    "start_logging() -> None\n"
    "\n"
    "Open log file and start logging.\n"
);
static PyObject *PyLog_start_logging(PyLog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    log_start_logging(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyLog_stop_logging_doc,
    "stop_logging() -> None\n"
    "\n"
    "Stop and close the log file.\n"
);
static PyObject *PyLog_stop_logging(PyLog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    log_stop_logging(self->data);

    Py_RETURN_NONE;
}

static int logtype(int *type, int target, int window)
{
    if (target || window)
    {
        if (target && window)
        {
            PyErr_SetString(PyExc_TypeError, "must specify target or window, not both");
            return 0;
        }

        *type = target? 0 : 1;
    }
    else if (*type < 0) 
    {
        PyErr_SetString(PyExc_TypeError, "must specify type, target, or window");
        return 0;
    }

    return 1;
}

PyDoc_STRVAR(PyLog_item_add_doc,
    "item_add(item, servertag=None, type=0, target=False, window=False) -> None\n"
    "\n"
    "Add a log item to log.\n"
    "\n"
    "Add a target item (nick, chan): \n"
    "   item_add('#linux', target=True)\n"
    "   item_add('#linux', type=0)\n"
    "\n"
    "Add a window ref: \n"
    "   item_add('2', window=True)\n"
    "   item_add('2', type=1)\n"
);
static PyObject *PyLog_item_add(PyLog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", "servertag", "type", "target", "window", NULL};
    char *item = "";
    char *servertag = NULL;
    int type = 0;
    int target = 0;
    int window = 0;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|ziii", kwlist, 
           &item, &servertag, &type, &target, &window))
        return NULL;

    if (!logtype(&type, target, window))
        return NULL;

    log_item_add(self->data, type, item, servertag);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyLog_item_destroy_doc,
    "item_destroy(item) -> None\n"
    "\n"
    "Remove log item from log.\n"
);
static PyObject *PyLog_item_destroy(PyLog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", NULL};
    PyObject *item = NULL;
    LOG_ITEM_REC *li;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, 
           &item))
        return NULL;

    if (!pylogitem_check(item))
        return PyErr_Format(PyExc_TypeError, "arg 1 should be log item");
    
    li = find_item(self->data, (PyLogitem *)item);
    if (!li)
        return PyErr_Format(PyExc_TypeError, "log item invalid or not found");
   
    log_item_destroy(self->data, li);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyLog_item_find_doc,
    "item_find(item, servertag=None, type=-1, target=False, window=False) -> item or None\n"
    "\n"
    "Find item from log.\n"
);
static PyObject *PyLog_item_find(PyLog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"item", "servertag", "type", "target", "window", NULL};
    char *item = "";
    char *server = NULL;
    int type = 0;
    int target = 0;
    int window = 0;
    LOG_ITEM_REC *li;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|ziii", kwlist, 
           &item, &server, &type, &target, &window))
        return NULL;

    if (!logtype(&type, target, window))
        return NULL;
    
    li = log_item_find(self->data, type, item, server);
    if (li)
        return pylogitem_new(li);
    
    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyLog_methods[] = {
    {"items", (PyCFunction)PyLog_items, METH_NOARGS,
        PyLog_items_doc},
    {"update", (PyCFunction)PyLog_update, METH_NOARGS,
        PyLog_update_doc},
    {"close", (PyCFunction)PyLog_close, METH_NOARGS,
        PyLog_close_doc},
    {"start_logging", (PyCFunction)PyLog_start_logging, METH_NOARGS,
        PyLog_start_logging_doc},
    {"stop_logging", (PyCFunction)PyLog_stop_logging, METH_NOARGS,
        PyLog_stop_logging_doc},
    {"item_add", (PyCFunction)PyLog_item_add, METH_VARARGS | METH_KEYWORDS,
        PyLog_item_add_doc},
    {"item_destroy", (PyCFunction)PyLog_item_destroy, METH_VARARGS | METH_KEYWORDS,
        PyLog_item_destroy_doc},
    {"item_find", (PyCFunction)PyLog_item_find, METH_VARARGS | METH_KEYWORDS,
        PyLog_item_find_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyLogType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Log",            /*tp_name*/
    sizeof(PyLog),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyLog_dealloc, /*tp_dealloc*/
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
    PyLog_doc,           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyLog_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyLog_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLog_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyLog_new,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pylog_new(void *log)
{
    PyLog *pylog;

    pylog = (PyLog *)PyLogType.tp_alloc(&PyLogType, 0);
    if (!pylog)
        return NULL;

    pylog->data = log;
    pylog->cleanup_installed = 1;
    signal_add_last_data("log remove", log_cleanup, pylog);

    return (PyObject *)pylog;
}

int log_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyLogType) < 0)
        return 0;
    
    Py_INCREF(&PyLogType);
    PyModule_AddObject(py_module, "Log", (PyObject *)&PyLogType);

    return 1;
}
