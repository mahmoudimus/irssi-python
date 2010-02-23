#ifndef _IGNORE_OBJECT_H_
#define _IGNORE_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _IGNORE_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct _IGNORE_REC)
} PyIgnore;

extern PyTypeObject PyIgnoreType;

int ignore_object_init(void);
PyObject *pyignore_new(void *ignore);
#define pyignore_check(op) PyObject_TypeCheck(op, &PyIgnoreType)

#endif
