#ifndef _IRC_CONNECT_OBJECT_H_
#define _IRC_CONNECT_OBJECT_H_

#include <Python.h>
#include "connect-object.h"

/* forward */
struct _IRC_SERVER_CONNECT_REC;

typedef struct
{
    PyIrssi_HEAD(struct _IRC_SERVER_CONNECT_REC)
} PyIrcConnect;

extern PyTypeObject PyIrcConnectType;

int irc_connect_object_init(void);
PyObject *pyirc_connect_new(void *connect, int managed);
#define pyirc_connect_check(op) PyObject_TypeCheck(op, &PyIrcConnectType)

#endif
