#ifndef _IRC_SERVER_OBJECT_H_
#define _IRC_SERVER_OBJECT_H_

#include <Python.h>
#include "server-object.h"

/* forward */
struct _IRC_SERVER_REC;

typedef struct
{
    PyServer_HEAD(struct _IRC_SERVER_REC)
} PyIrcServer;

extern PyTypeObject PyIrcServerType;

int irc_server_object_init(void);
PyObject *pyirc_server_new(void *server);
#define pyirc_server_check(op) PyObject_TypeCheck(op, &PyIrcServerType)

#endif
