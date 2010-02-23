#ifndef _LOG_ITEM_OBJECT_H_
#define _LOG_ITEM_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyObject_HEAD
    PyObject *type;
    PyObject *name;
    PyObject *servertag;
} PyLogitem;

extern PyTypeObject PyLogitemType;

int logitem_object_init(void);
PyObject *pylogitem_new(void *log);
#define pylogitem_check(op) PyObject_TypeCheck(op, &PyLogitemType)

#endif
