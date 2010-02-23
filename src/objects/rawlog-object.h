#ifndef _RAWLOG_OBJECT_H_
#define _RAWLOG_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _RAWLOG_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct _RAWLOG_REC)
    int owned;
} PyRawlog;

extern PyTypeObject PyRawlogType;

int rawlog_object_init(void);
PyObject *pyrawlog_new(void *rlog);
#define pyrawlog_check(op) PyObject_TypeCheck(op, &PyRawlogType)

#endif
