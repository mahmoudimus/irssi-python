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
#include "pysource.h"

typedef struct _PY_SOURCE_REC
{
    int tag;
    GSList **tag_list;
    int fd;
    PyObject *func;
    PyObject *data;
} PY_SOURCE_REC;

static PY_SOURCE_REC *py_source_rec_new(GSList **tag_list, int fd, PyObject *func, PyObject *data)
{
    PY_SOURCE_REC *rec;

    rec = g_new0(PY_SOURCE_REC, 1);
    rec->tag_list = tag_list;
    rec->fd = fd;
    rec->func = func;
    rec->data = data;

    Py_INCREF(func);
    Py_XINCREF(data);

    return rec;
}

static int py_remove_tag(GSList **list, int handle)
{
    GSList *node;

    node = g_slist_find(*list, GINT_TO_POINTER(handle));
    if (!node)
        return 0;

    *list = g_slist_delete_link(*list, node);

    return 1;
}

static void py_source_destroy(PY_SOURCE_REC *rec)
{
    g_return_if_fail(py_remove_tag(rec->tag_list, rec->tag) == 1);
    Py_DECREF(rec->func);
    Py_XDECREF(rec->data);
    g_free(rec);
}

static int py_handle_ret(PyObject *ret)
{
    int res;

    if (!ret)
    {
        PyErr_Print();
        res = FALSE; 
    }
    else
    {
        res = PyObject_IsTrue(ret);
        Py_DECREF(ret);
    }

    return res;
}

static int py_timeout_proxy(PY_SOURCE_REC *rec)
{
    PyObject *ret;

    g_return_val_if_fail(rec != NULL, FALSE);
    
    if (rec->data)
        ret = PyObject_CallFunction(rec->func, "O", rec->data);
    else
        ret = PyObject_CallFunction(rec->func, "");

    return py_handle_ret(ret);
}

static int py_io_proxy(GIOChannel *src, GIOCondition condition, PY_SOURCE_REC *rec)
{
    PyObject *ret;

    g_return_val_if_fail(rec != NULL, FALSE);

    if (rec->data)
        ret = PyObject_CallFunction(rec->func, "iiO", rec->fd, condition, rec->data);
    else
        ret = PyObject_CallFunction(rec->func, "ii", rec->fd, condition);

    return py_handle_ret(ret);
}

int pysource_timeout_add_list(GSList **list, int msecs, PyObject *func, PyObject *data)
{
    PY_SOURCE_REC *rec;

    g_return_val_if_fail(func != NULL, -1);

    rec = py_source_rec_new(list, -1, func, data);
    rec->tag = g_timeout_add_full(G_PRIORITY_DEFAULT, msecs, 
            (GSourceFunc)py_timeout_proxy, rec, 
            (GDestroyNotify)py_source_destroy);
    
    *list = g_slist_append(*list, GINT_TO_POINTER(rec->tag));
    
    return rec->tag;
}

int pysource_io_add_watch_list(GSList **list, int fd, int cond, PyObject *func, PyObject *data)
{
    PY_SOURCE_REC *rec;
    GIOChannel *channel;

    g_return_val_if_fail(func != NULL, 1);

    rec = py_source_rec_new(list, fd, func, data);
    channel = g_io_channel_unix_new(fd);
    rec->tag = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond, 
            (GIOFunc)py_io_proxy, rec,
            (GDestroyNotify)py_source_destroy);
    g_io_channel_unref(channel);
   
    *list = g_slist_append(*list, GINT_TO_POINTER(rec->tag));
    
    return rec->tag;
}
