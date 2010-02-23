#ifndef _DCC_OBJECT_H_
#define _DCC_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

#define PyDcc_HEAD(type)    \
    PyIrssi_HEAD(type)      \
    PyObject *server;       \
    PyObject *chat;

typedef struct
{
    PyDcc_HEAD(void)
} PyDcc;

extern PyTypeObject PyDccType;

PyObject *pydcc_sub_new(void *dcc, const char *name, PyTypeObject *subclass);
PyObject *pydcc_new(void *dcc);
#define pydcc_check(op) PyObject_TypeCheck(op, &PyDccType)
int dcc_object_init(void);

#endif
