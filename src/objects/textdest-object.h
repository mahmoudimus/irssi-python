#ifndef _TEXTDEST_OBJECT_H_
#define _TEXTDEST_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _TEXT_DEST_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct _TEXT_DEST_REC)
    PyObject *window;
    PyObject *server;
    int owned;
} PyTextDest;

extern PyTypeObject PyTextDestType;

int textdest_object_init(void);
PyObject *pytextdest_new(void *td);
#define pytextdest_check(op) PyObject_TypeCheck(op, &PyTextDestType)

#endif
