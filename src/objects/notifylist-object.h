#ifndef _NOTIFYLIST_OBJECT_H_
#define _NOTIFYLIST_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
} PyNotifylist;

extern PyTypeObject PyNotifylistType;

int notifylist_object_init(void);
PyObject *pynotifylist_new(void *notifylist);
#define pynotifylist_check(op) PyObject_TypeCheck(op, &PyNotifylistType)

#endif
