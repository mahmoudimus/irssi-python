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
#include "statusbar-item-object.h"

/* monitor "statusbar item destroyed" signal */
static void statusbar_item_cleanup(SBAR_ITEM_REC *sbar_item)
{
    PyStatusbarItem *pysbar_item = signal_get_user_data();

    if (sbar_item == pysbar_item->data)
    {
        pysbar_item->data = NULL;
        pysbar_item->cleanup_installed = 0;
        signal_remove_data("statusbar item_destroy", statusbar_item_cleanup, pysbar_item);
    }
}

static void PyStatusbarItem_dealloc(PyStatusbarItem *self)
{
    if (self->cleanup_installed)
        signal_remove_data("sbar_itemlist remove", statusbar_item_cleanup, self); 

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyStatusbarItem_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyStatusbarItem *self;

    self = (PyStatusbarItem *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getter */
PyDoc_STRVAR(PyStatusbarItem_min_size_doc,
    "min size"
);
static PyObject *PyStatusbarItem_min_size_get(PyStatusbarItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->min_size);
}

PyDoc_STRVAR(PyStatusbarItem_max_size_doc,
    "max size"
);
static PyObject *PyStatusbarItem_max_size_get(PyStatusbarItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->max_size);
}

PyDoc_STRVAR(PyStatusbarItem_xpos_doc,
    "x position"
);
static PyObject *PyStatusbarItem_xpos_get(PyStatusbarItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->xpos);
}

PyDoc_STRVAR(PyStatusbarItem_size_doc,
    "size"
);
static PyObject *PyStatusbarItem_size_get(PyStatusbarItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->size);
}

PyDoc_STRVAR(PyStatusbarItem_window_doc,
    "parent window for statusbar item"
);
static PyObject *PyStatusbarItem_window_get(PyStatusbarItem *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->window);
}

/* setters */
static int py_setint(int *iv, PyObject *value)
{
    int tmp;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_AttributeError, "can't delete member");
        return -1;
    }

    if (!PyInt_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "value must be int");
        return -1;
    }
   
    tmp = PyInt_AsLong(value);
    if (PyErr_Occurred())
        return -1;
   
    *iv = tmp;

    return 0;
}

static int PyStatusbarItem_min_size_set(PyStatusbarItem *self, PyObject *value, void *closure)
{
    return py_setint(&self->data->min_size, value);
}

static int PyStatusbarItem_max_size_set(PyStatusbarItem *self, PyObject *value, void *closure)
{
    return py_setint(&self->data->max_size, value);
}

/* specialized getters/setters */
static PyGetSetDef PyStatusbarItem_getseters[] = {
    {"min_size", (getter)PyStatusbarItem_min_size_get, (setter)PyStatusbarItem_min_size_set,
        PyStatusbarItem_min_size_doc, NULL},
    {"max_size", (getter)PyStatusbarItem_max_size_get, (setter)PyStatusbarItem_max_size_set,
        PyStatusbarItem_max_size_doc, NULL},
    {"xpos", (getter)PyStatusbarItem_xpos_get, NULL,
        PyStatusbarItem_xpos_doc, NULL},
    {"size", (getter)PyStatusbarItem_size_get, NULL,
        PyStatusbarItem_size_doc, NULL},
    {"window", (getter)PyStatusbarItem_window_get, NULL,
        PyStatusbarItem_window_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyStatusbarItem_default_handler_doc,
    "default_handler(get_size_only, str=None, data="", escape_vars=True) -> None\n"
    "\n"
    "Run default handler of item to print to statusbar\n"
);
static PyObject *PyStatusbarItem_default_handler(PyStatusbarItem *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"get_size_only", "str", "data", "escape_vars", NULL};
    int get_size_only = 0;
    char *str = NULL;
    char *data = "";
    int escape_vars = TRUE;

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i|zsi", kwlist, 
           &get_size_only, &str, &data, &escape_vars))
        return NULL;

    if (str && !*str)
        str = NULL;

    statusbar_item_default_handler(self->data, get_size_only, str, data, escape_vars);
   
    Py_RETURN_NONE;
}

/* Methods for object */
static PyMethodDef PyStatusbarItem_methods[] = {
    {"default_handler", (PyCFunction)PyStatusbarItem_default_handler, METH_VARARGS | METH_KEYWORDS,
        PyStatusbarItem_default_handler_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyStatusbarItemType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.StatusbarItem",            /*tp_name*/
    sizeof(PyStatusbarItem),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyStatusbarItem_dealloc, /*tp_dealloc*/
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
    "PyStatusbarItem objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyStatusbarItem_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyStatusbarItem_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyStatusbarItem_new,                 /* tp_new */
};


/* sbar_item factory function */
PyObject *pystatusbar_item_new(void *sbar_item)
{
    SBAR_ITEM_REC *si;
    PyStatusbarItem *pysbar_item;
    PyObject *window = NULL;

    si = sbar_item;
    if (si->bar->parent_window)
    {
        window = pywindow_new(si->bar->parent_window);
        if (!window)
            return NULL;
    }
    
    pysbar_item = py_inst(PyStatusbarItem, PyStatusbarItemType);
    if (!pysbar_item)
        return NULL;

    pysbar_item->window = window;
    
    pysbar_item->data = sbar_item;
    pysbar_item->cleanup_installed = 1;
    signal_add_last_data("statusbar item destroyed", statusbar_item_cleanup, pysbar_item);

    return (PyObject *)pysbar_item;
}

int statusbar_item_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyStatusbarItemType) < 0)
        return 0;
    
    Py_INCREF(&PyStatusbarItemType);
    PyModule_AddObject(py_module, "StatusbarItem", (PyObject *)&PyStatusbarItemType);

    return 1;
}
