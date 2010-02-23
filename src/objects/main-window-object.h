#ifndef _MAIN_WINDOW_OBJECT_H_
#define _MAIN_WINDOW_OBJECT_H_

#include <Python.h>
#include "base-objects.h"
#include "pyirssi.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
    PyObject *active;
} PyMainWindow;

extern PyTypeObject PyMainWindowType;

int main_window_object_init(void);
PyObject *pymain_window_new(MAIN_WINDOW_REC *mw);
#define pymain_window_check(op) PyObject_TypeCheck(op, &PyMainWindowType)

#endif
