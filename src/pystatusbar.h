#ifndef _PYSTATUSBAR_H_
#define _PYSTATUSBAR_H_

#include <Python.h>

void pystatusbar_item_register(PyObject *script, const char *sitem, 
        const char *value, PyObject *func);
void pystatusbar_item_unregister(const char *iname);
void pystatusbar_cleanup_script(PyObject *script);
void pystatusbar_init(void);
void pystatusbar_deinit(void);

#endif
