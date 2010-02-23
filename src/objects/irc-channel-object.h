#ifndef _IRC_CHANNEL_OBJECT_H_
#define _IRC_CHANNEL_OBJECT_H_

#include <Python.h>
#include "window-item-object.h"

/* forward */
struct _IRC_CHANNEL_REC;

typedef struct
{
    PyWindowItem_HEAD(struct _IRC_CHANNEL_REC)
} PyIrcChannel;

extern PyTypeObject PyIrcChannelType;

int irc_channel_object_init(void);
PyObject *pyirc_channel_new(void *chan);
#define pyirc_channel_check(op) PyObject_TypeCheck(op, &PyIrcChannelType)

#endif
