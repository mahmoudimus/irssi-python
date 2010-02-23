#ifndef _CHANNEL_OBJECT_H_
#define _CHANNEL_OBJECT_H_

#include <Python.h>
#include "window-item-object.h"

/* forward */
struct _CHANNEL_REC;

typedef struct
{
    PyWindowItem_HEAD(struct _CHANNEL_REC)
} PyChannel;

extern PyTypeObject PyChannelType;

int channel_object_init(void);
PyObject *pychannel_sub_new(void *chan, const char *name, PyTypeObject *type);
PyObject *pychannel_new(void *chan);
#define pychannel_check(op) PyObject_TypeCheck(op, &PyChannelType)

#endif
