#ifndef _PROCESS_OBJECT_H_
#define _PROCESS_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct PROCESS_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct PROCESS_REC)
    PyObject *target_win;
} PyProcess;

extern PyTypeObject PyProcessType;

int process_object_init(void);
PyObject *pyprocess_new(void *process);
#define pyprocess_check(op) PyObject_TypeCheck(op, &PyProcessType)

#endif
