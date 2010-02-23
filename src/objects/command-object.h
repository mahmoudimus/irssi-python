#ifndef _COMMAND_OBJECT_H_
#define _COMMAND_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
} PyCommand;

extern PyTypeObject PyCommandType;

int command_object_init(void);
PyObject *pycommand_new(void *command);
#define pycommand_check(op) PyObject_TypeCheck(op, &PyCommandType)

#endif
