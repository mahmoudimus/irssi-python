#ifndef _CHATNET_OBJECT_H_
#define _CHATNET_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _CHATNET_REC;

typedef struct
{
    PyIrssi_HEAD(struct _CHATNET_REC)
} PyChatnet;

extern PyTypeObject PyChatnetType;

int chatnet_object_init(void);
PyObject *pychatnet_sub_new(void *cn, PyTypeObject *subclass);
PyObject *pychatnet_new(void *cn);
#define pychatnet_check(op) PyObject_TypeCheck(op, &PyChatnetType)

#endif
