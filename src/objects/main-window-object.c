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
#include "main-window-object.h"
#include "factory.h"
#include "pycore.h"

#define MW(data) ((MAIN_WINDOW_REC *) data)

/* monitor "mainwindow destroyed" signal */
static void main_window_cleanup(MAIN_WINDOW_REC *mw)
{
    PyMainWindow *pymw = signal_get_user_data();

    if (mw == pymw->data)
    {
        pymw->data = NULL;
        pymw->cleanup_installed = 0;
        signal_remove_data("mainwindow destroyed", main_window_cleanup, pymw);
    }
}

static void PyMainWindow_dealloc(PyMainWindow *self)
{
    if (self->cleanup_installed)
        signal_remove_data("mainwindow destroyed", main_window_cleanup, self); 

    Py_XDECREF(self->active);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyMainWindow_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyMainWindow *self;

    self = (PyMainWindow *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* getters */
PyDoc_STRVAR(PyMainWindow_active_doc,
    "active window object"
);
static PyObject *PyMainWindow_active_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->active);
}

PyDoc_STRVAR(PyMainWindow_first_line_doc,
    "first line used by this window (0..x) (includes statusbars)"
);
static PyObject *PyMainWindow_first_line_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(MW(self->data)->first_line);
}

PyDoc_STRVAR(PyMainWindow_last_line_doc,
    "last line used by this window (0..x) (includes statusbars)"
);
static PyObject *PyMainWindow_last_line_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(MW(self->data)->last_line);
}

PyDoc_STRVAR(PyMainWindow_width_doc,
    "width of the window (includes statusbars)"
);
static PyObject *PyMainWindow_width_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(MW(self->data)->width);
}

PyDoc_STRVAR(PyMainWindow_height_doc,
    "height of the window (includes statusbars)"
);
static PyObject *PyMainWindow_height_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(MW(self->data)->height);
}

PyDoc_STRVAR(PyMainWindow_statusbar_lines_doc,
    "???"
);
static PyObject *PyMainWindow_statusbar_lines_get(PyMainWindow *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(MW(self->data)->statusbar_lines);
}

/* specialized getters/setters */
static PyGetSetDef PyMainWindow_getseters[] = {
    {"active", (getter)PyMainWindow_active_get, NULL,
        PyMainWindow_active_doc, NULL},
    {"first_line", (getter)PyMainWindow_first_line_get, NULL,
        PyMainWindow_first_line_doc, NULL},
    {"last_line", (getter)PyMainWindow_last_line_get, NULL,
        PyMainWindow_last_line_doc, NULL},
    {"width", (getter)PyMainWindow_width_get, NULL,
        PyMainWindow_width_doc, NULL},
    {"height", (getter)PyMainWindow_height_get, NULL,
        PyMainWindow_height_doc, NULL},
    {"statusbar_lines", (getter)PyMainWindow_statusbar_lines_get, NULL,
        PyMainWindow_statusbar_lines_doc, NULL},
    {NULL}
};

/* Methods for object */
static PyMethodDef PyMainWindow_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyMainWindowType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.MainWindow",            /*tp_name*/
    sizeof(PyMainWindow),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyMainWindow_dealloc, /*tp_dealloc*/
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
    "PyMainWindow objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyMainWindow_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyMainWindow_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyMainWindow_new,                 /* tp_new */
};


/* main window wrapper factory function */
PyObject *pymain_window_new(MAIN_WINDOW_REC *mw)
{
    PyObject *pyactive = NULL;
    PyMainWindow *pymw;

    pyactive = pywindow_new(mw->active);
    if (!pyactive)
        return NULL;
    
    pymw = py_inst(PyMainWindow, PyMainWindowType);
    if (!pymw)
    {
        Py_DECREF(pyactive);
        return NULL;
    }

    pymw->active = pyactive;
    pymw->data = mw;
    pymw->cleanup_installed = 1;
    signal_add_last_data("mainwindow destroyed", main_window_cleanup, pymw);

    return (PyObject *)pymw;
}

int main_window_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyMainWindowType) < 0)
        return 0;
    
    Py_INCREF(&PyMainWindowType);
    PyModule_AddObject(py_module, "MainWindow", (PyObject *)&PyMainWindowType);

    return 1;
}
