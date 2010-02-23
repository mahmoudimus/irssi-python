#ifndef _SERVER_OBJECT_H_
#define _SERVER_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _SERVER_REC;

#define PyServer_HEAD(type) \
    PyIrssi_HEAD(type)      \
    PyObject *connect;      \
    PyObject *rawlog;       

typedef struct
{
    PyServer_HEAD(struct _SERVER_REC)
} PyServer;

extern PyTypeObject PyServerType;

int server_object_init(void);
PyObject *pyserver_sub_new(void *server, PyTypeObject *subclass);
PyObject *pyserver_new(void *server);

#define pyserver_check(op) PyObject_TypeCheck(op, &PyServerType)

#endif
