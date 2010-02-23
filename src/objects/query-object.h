#ifndef _QUERY_OBJECT_H_
#define _QUERY_OBJECT_H_

#include <Python.h>
#include "window-item-object.h"

/* forward */
struct _QUERY_REC;

typedef struct
{
    PyWindowItem_HEAD(struct _QUERY_REC)
} PyQuery;

extern PyTypeObject PyQueryType;

int query_object_init(void); 
PyObject *pyquery_new(void *query);
#define pyquery_check(op) PyObject_TypeCheck(op, &PyQueryType)

#endif
