#ifndef _NETSPLIT_SERVER_OBJECT_H_
#define _NETSPLIT_SERVER_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
} PyNetsplitServer;

extern PyTypeObject PyNetsplitServerType;

int netsplit_server_object_init(void);
PyObject *pynetsplit_server_new(void *nss);
#define pynetsplit_server_check(op) PyObject_TypeCheck(op, &PyNetsplitServerType)

#endif
