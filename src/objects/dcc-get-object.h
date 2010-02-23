#ifndef _DCC_GET_OBJECT_H_
#define _DCC_GET_OBJECT_H_

#include <Python.h>
#include "dcc-object.h"

typedef struct
{
    PyDcc_HEAD(void)
} PyDccGet;

extern PyTypeObject PyDccGetType;

PyObject *pydcc_get_new(void *dcc);
#define pydcc_get_check(op) PyObject_TypeCheck(op, &PyDccGetType)
int dcc_get_object_init(void);

#endif
