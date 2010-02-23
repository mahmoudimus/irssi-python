#ifndef _WITEM_OBJECT_H_
#define _WITEM_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

#define PyWindowItem_HEAD(type) \
    PyIrssi_HEAD(type)          \
    PyObject *server;

/* forward */
struct _WI_ITEM_REC;

typedef struct
{
    PyWindowItem_HEAD(struct _WI_ITEM_REC)
} PyWindowItem;

extern PyTypeObject PyWindowItemType;

int window_item_object_init(void);
PyObject *pywindow_item_sub_new(void *witem, const char *name, PyTypeObject *subclass);
PyObject *pywindow_item_new(void *witem);
#define pywindow_item_check(op) PyObject_TypeCheck(op, &PyWindowItemType)

#endif
