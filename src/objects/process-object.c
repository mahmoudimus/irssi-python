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
#include "process-object.h"
#include "pycore.h"

/* monitor "exec remove" signal */
static void process_cleanup(PROCESS_REC *process, int status)
{
    PyProcess *pyprocess = signal_get_user_data();

    if (process == pyprocess->data)
    {
        pyprocess->data = NULL;
        pyprocess->cleanup_installed = 0;
        signal_remove_data("exec remove", process_cleanup, pyprocess);
    }
}

static void PyProcess_dealloc(PyProcess *self)
{
    if (self->cleanup_installed)
        signal_remove_data("exec remove", process_cleanup, self); 

    Py_XDECREF(self->target_win);
    
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyProcess_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyProcess *self;

    self = (PyProcess *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* Getters */
PyDoc_STRVAR(PyProcess_id_doc,
    "ID for the process"
);
static PyObject *PyProcess_id_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->id);
}

PyDoc_STRVAR(PyProcess_name_doc,
    "Name for the process (if given)"
);
static PyObject *PyProcess_name_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->name);
}

PyDoc_STRVAR(PyProcess_args_doc,
    "The command that is being executed"
);
static PyObject *PyProcess_args_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->args);
}

PyDoc_STRVAR(PyProcess_pid_doc,
    "PID for the executed command"
);
static PyObject *PyProcess_pid_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->pid);
}

PyDoc_STRVAR(PyProcess_target_doc,
    "send text with /msg <target> ..."
);
static PyObject *PyProcess_target_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_STRING_OR_NONE(self->data->target);
}

PyDoc_STRVAR(PyProcess_target_win_doc,
    "print text to this window"
);
static PyObject *PyProcess_target_win_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    RET_AS_OBJ_OR_NONE(self->target_win);
}

PyDoc_STRVAR(PyProcess_shell_doc,
    "start the program via /bin/sh"
);
static PyObject *PyProcess_shell_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->shell);
}

PyDoc_STRVAR(PyProcess_notice_doc,
    "send text with /notice, not /msg if target is set"
);
static PyObject *PyProcess_notice_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->notice);
}

PyDoc_STRVAR(PyProcess_silent_doc,
    "don't print \"process exited with level xx\""
);
static PyObject *PyProcess_silent_get(PyProcess *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->silent);
}

/* specialized getters/setters */
static PyGetSetDef PyProcess_getseters[] = {
    {"id", (getter)PyProcess_id_get, NULL,
        PyProcess_id_doc, NULL},
    {"name", (getter)PyProcess_name_get, NULL,
        PyProcess_name_doc, NULL},
    {"args", (getter)PyProcess_args_get, NULL,
        PyProcess_args_doc, NULL},
    {"pid", (getter)PyProcess_pid_get, NULL,
        PyProcess_pid_doc, NULL},
    {"target", (getter)PyProcess_target_get, NULL,
        PyProcess_target_doc, NULL},
    {"target_win", (getter)PyProcess_target_win_get, NULL,
        PyProcess_target_win_doc, NULL},
    {"shell", (getter)PyProcess_shell_get, NULL,
        PyProcess_shell_doc, NULL},
    {"notice", (getter)PyProcess_notice_get, NULL,
        PyProcess_notice_doc, NULL},
    {"silent", (getter)PyProcess_silent_get, NULL,
        PyProcess_silent_doc, NULL},
    {NULL}
};

/* Methods */
/* Methods for object */
static PyMethodDef PyProcess_methods[] = {
    {NULL}  /* Sentinel */
};

PyTypeObject PyProcessType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Process",            /*tp_name*/
    sizeof(PyProcess),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyProcess_dealloc, /*tp_dealloc*/
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
    "PyProcess objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyProcess_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyProcess_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyProcess_new,                 /* tp_new */
};


/* process factory function */
PyObject *pyprocess_new(void *process)
{
    PyProcess *pyprocess;

    pyprocess = py_inst(PyProcess, PyProcessType);
    if (!pyprocess)
        return NULL;

    pyprocess->data = process;
    pyprocess->cleanup_installed = 1;
    signal_add_last_data("exec remove", process_cleanup, pyprocess);

    return (PyObject *)pyprocess;
}

int process_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyProcessType) < 0)
        return 0;
    
    Py_INCREF(&PyProcessType);
    PyModule_AddObject(py_module, "Process", (PyObject *)&PyProcessType);

    return 1;
}
