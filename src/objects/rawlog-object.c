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
#include "rawlog-object.h"
#include "pycore.h"

/* monitor "????" signal */
static void rawlog_cleanup(RAWLOG_REC *ban)
{
    /* XXX */
}

static void PyRawlog_dealloc(PyRawlog *self)
{
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *PyRawlog_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyRawlog *self;

    self = (PyRawlog *)type->tp_alloc(type, 0);
    if (!self)
        return NULL;

    return (PyObject *)self;
}

/* XXX: Need function to create the rawlog */

/* Getters */
PyDoc_STRVAR(PyRawlog_logging_doc,
    "The raw log is being written to file currently."
);
static PyObject *PyRawlog_logging_get(PyRawlog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyBool_FromLong(self->data->logging);
}

PyDoc_STRVAR(PyRawlog_nlines_doc,
    "Number of lines in rawlog."
);
static PyObject *PyRawlog_nlines_get(PyRawlog *self, void *closure)
{
    RET_NULL_IF_INVALID(self->data);
    return PyInt_FromLong(self->data->nlines);
}

/* specialized getters/setters */
static PyGetSetDef PyRawlog_getseters[] = {
    {"logging", (getter)PyRawlog_logging_get, NULL,
        PyRawlog_logging_doc, NULL},
    {"nlines", (getter)PyRawlog_nlines_get, NULL,
        PyRawlog_nlines_doc, NULL},
    {NULL}
};

/* Methods */
PyDoc_STRVAR(PyRawlog_get_lines_doc,
    "get_lines() -> list of str\n"
    "\n"
    "Return a list of lines for rawlog.\n"
);
static PyObject *PyRawlog_get_lines(PyRawlog *self, PyObject *args)
{
    PyObject *lines = NULL;
    GSList *node;

    RET_NULL_IF_INVALID(self->data);
    
    lines = PyList_New(0);
    if (!lines)
        return NULL;

    for (node = self->data->lines; node; node = node->next)
    {
        int ret;
        PyObject *line = PyString_FromString(node->data);

        if (!line)
        {
            Py_XDECREF(lines);
            return NULL;
        }

        ret = PyList_Append(lines, line);
        Py_DECREF(line);
        if (ret != 0)
        {
            Py_XDECREF(lines);
            return NULL;
        }
    }
  
    return lines;
}

PyDoc_STRVAR(PyRawlog_destroy_doc,
    "destroy() -> None\n"
    "\n"
    "Destroy rawlog\n"
);
static PyObject *PyRawlog_destroy(PyRawlog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    rawlog_destroy(self->data);
   
    /*XXX: what about signal handler ? */
    self->data = NULL;

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_input_doc,
    "input(str) -> None\n"
    "\n"
    "Send str to rawlog as input text.\n"
);
static PyObject *PyRawlog_input(PyRawlog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", NULL};
    char *str = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &str))
        return NULL;

    rawlog_input(self->data, str);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_output_doc,
    "output(str) -> None\n"
    "\n"
    "Send str to rawlog as output text.\n"
);
static PyObject *PyRawlog_output(PyRawlog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", NULL};
    char *str = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &str))
        return NULL;

    rawlog_output(self->data, str);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_redirect_doc,
    "redirect(str) -> None\n"
    "\n"
    "Send str to rawlog as redirection text."
);
static PyObject *PyRawlog_redirect(PyRawlog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"str", NULL};
    char *str = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &str))
        return NULL;

    rawlog_redirect(self->data, str);

    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_open_doc,
    "open(fname) -> None\n"
    "\n"
    "Start logging new messages in rawlog to specified file.\n"
);
static PyObject *PyRawlog_open(PyRawlog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"fname", NULL};
    char *fname = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &fname))
        return NULL;

    rawlog_open(self->data, fname);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_close_doc,
    "close() -> None\n"
    "\n"
    "Stop logging to file\n"
);
static PyObject *PyRawlog_close(PyRawlog *self, PyObject *args)
{
    RET_NULL_IF_INVALID(self->data);

    rawlog_close(self->data);
    
    Py_RETURN_NONE;
}

PyDoc_STRVAR(PyRawlog_save_doc,
    "save(fname) -> None\n"
    "\n"
    "Save the current rawlog history to specified file.\n"
);
static PyObject *PyRawlog_save(PyRawlog *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"fname", NULL};
    char *fname = "";

    RET_NULL_IF_INVALID(self->data);

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, 
           &fname))
        return NULL;

    rawlog_save(self->data, fname);
    
    Py_RETURN_NONE;
}
/* Methods for object */
static PyMethodDef PyRawlog_methods[] = {
    {"get_lines", (PyCFunction)PyRawlog_get_lines, METH_NOARGS,
        PyRawlog_get_lines_doc},
    {"destroy", (PyCFunction)PyRawlog_destroy, METH_NOARGS,
        PyRawlog_destroy_doc},
    {"input", (PyCFunction)PyRawlog_input, METH_VARARGS | METH_KEYWORDS,
        PyRawlog_input_doc},
    {"output", (PyCFunction)PyRawlog_output, METH_VARARGS | METH_KEYWORDS,
        PyRawlog_output_doc},
    {"redirect", (PyCFunction)PyRawlog_redirect, METH_VARARGS | METH_KEYWORDS,
        PyRawlog_redirect_doc},
    {"open", (PyCFunction)PyRawlog_open, METH_VARARGS | METH_KEYWORDS,
        PyRawlog_open_doc},
    {"close", (PyCFunction)PyRawlog_close, METH_NOARGS,
        PyRawlog_close_doc},
    {"save", (PyCFunction)PyRawlog_save, METH_VARARGS | METH_KEYWORDS,
        PyRawlog_save_doc},
    {NULL}  /* Sentinel */
};

PyTypeObject PyRawlogType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "irssi.Rawlog",            /*tp_name*/
    sizeof(PyRawlog),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyRawlog_dealloc, /*tp_dealloc*/
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
    "PyRawlog objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    PyRawlog_methods,             /* tp_methods */
    0,                      /* tp_members */
    PyRawlog_getseters,        /* tp_getset */
    0,          /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    PyRawlog_new,                 /* tp_new */
};


/* window item wrapper factory function */
PyObject *pyrawlog_new(void *rlog)
{
    PyRawlog *pyrlog;

    pyrlog = py_inst(PyRawlog, PyRawlogType);
    if (!pyrlog)
        return NULL;

    pyrlog->data = rlog;

    return (PyObject *)pyrlog;
}

int rawlog_object_init(void) 
{
    g_return_val_if_fail(py_module != NULL, 0);

    if (PyType_Ready(&PyRawlogType) < 0)
        return 0;
    
    Py_INCREF(&PyRawlogType);
    PyModule_AddObject(py_module, "Rawlog", (PyObject *)&PyRawlogType);

    return 1;
}
