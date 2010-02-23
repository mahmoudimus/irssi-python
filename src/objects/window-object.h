#ifndef _WINDOW_OBJECT_H_
#define _WINDOW_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _WINDOW_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct _WINDOW_REC)
} PyWindow;

extern PyTypeObject PyWindowType;

int window_object_init(void);
PyObject *pywindow_new(void *win);
#define pywindow_check(op) PyObject_TypeCheck(op, &PyWindowType)

#endif
