#ifndef _PY_MODULE_H_
#define _PY_MODULE_H_

#include <Python.h>

/* This is global so that type objects and such can be easily attached */
extern PyObject *py_module;
int pymodule_init(void);
void pymodule_deinit(void);

#endif
