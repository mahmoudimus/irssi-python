#ifndef _CONNECT_OBJECT_H_
#define _CONNECT_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _SERVER_CONNECT_REC;

typedef struct
{
    PyIrssi_HEAD(struct _SERVER_CONNECT_REC)
} PyConnect;

extern PyTypeObject PyConnectType;

int connect_object_init(void);
PyObject *pyconnect_sub_new(void *connect, PyTypeObject *subtype, int managed);
PyObject *pyconnect_new(void *connect, int managed);
#define pyconnect_check(op) PyObject_TypeCheck(op, &PyConnectType)

#endif
