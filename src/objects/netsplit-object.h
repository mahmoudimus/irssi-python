#ifndef _NETSPLIT_OBJECT_H_
#define _NETSPLIT_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
    PyObject *server;
} PyNetsplit;

extern PyTypeObject PyNetsplitType;

int netsplit_object_init(void);
PyObject *pynetsplit_new(void *ns);
#define pynetsplit_check(op) PyObject_TypeCheck(op, &PyNetsplitType)

#endif
