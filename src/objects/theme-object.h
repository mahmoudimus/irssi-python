#ifndef _THEME_OBJECT_H_
#define _THEME_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

typedef struct
{
    PyIrssiFinal_HEAD(void)
} PyTheme;

extern PyTypeObject PyThemeType;

int theme_object_init(void);
PyObject *pytheme_new(void *td);
#define pytheme_check(op) PyObject_TypeCheck(op, &PyThemeType)

#endif
