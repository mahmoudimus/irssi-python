#ifndef _NETSPLIT_CHANNEL_OBJECT_H_
#define _NETSPLIT_CHANNEL_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyObject_HEAD
    PyObject *name;
    int op, halfop; 
    int voice, other;
} PyNetsplitChannel;

extern PyTypeObject PyNetsplitChannelType;

int netsplit_channel_object_init(void);
PyObject *pynetsplit_channel_new(void *ns);
#define pynetsplit_channel_check(op) PyObject_TypeCheck(op, &PyNetsplitChannelType)

#endif
