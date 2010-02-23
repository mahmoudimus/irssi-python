#ifndef _RECONNECT_OBJECT_H_
#define _RECONNECT_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/*XXX: no Reconnect cleanup/destroy sig. Maybe value copy the two members? */
typedef struct
{
    PyIrssiFinal_HEAD(void)
    PyObject *connect;
} PyReconnect;

extern PyTypeObject PyReconnectType;

int reconnect_object_init(void);
PyObject *pyreconnect_new(void *recon);
#define pyreconnect_check(op) PyObject_TypeCheck(op, &PyReconnectType)

#endif
