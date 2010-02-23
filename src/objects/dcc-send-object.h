#ifndef _DCC_SEND_OBJECT_H_
#define _DCC_SEND_OBJECT_H_

#include <Python.h>
#include "dcc-object.h"

typedef struct
{
    PyDcc_HEAD(void)
} PyDccSend;

extern PyTypeObject PyDccSendType;

PyObject *pydcc_send_new(void *dcc);
#define pydcc_send_check(op) PyObject_TypeCheck(op, &PyDccSendType)
int dcc_send_object_init(void);

#endif
