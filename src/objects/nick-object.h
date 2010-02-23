#ifndef _NICK_OBJECT_H_
#define _NICK_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct _NICK_REC;

typedef struct
{
    PyIrssi_HEAD(struct _NICK_REC)
} PyNick;

extern PyTypeObject PyNickType;

int nick_object_init(void);
PyObject *pynick_sub_new(void *nick, PyTypeObject *subclass);
PyObject *pynick_new(void *nick);
#define pynick_check(op) PyObject_TypeCheck(op, &PyNickType)

#endif
