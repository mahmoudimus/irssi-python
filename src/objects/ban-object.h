#ifndef _BAN_OBJECT_H_
#define _BAN_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
} PyBan;

extern PyTypeObject PyBanType;

int ban_object_init(void);
PyObject *pyban_new(void *ban);
#define pyban_check(op) PyObject_TypeCheck(op, &PyBanType)

#endif
