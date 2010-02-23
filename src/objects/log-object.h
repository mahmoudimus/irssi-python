#ifndef _LOG_OBJECT_H_
#define _LOG_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _LOG_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct _LOG_REC)
} PyLog;

extern PyTypeObject PyLogType;

int log_object_init(void);
PyObject *pylog_new(void *log);
#define pylog_check(op) PyObject_TypeCheck(op, &PyLogType)

#endif
