#ifndef _PYSOURCE_H_
#define _PYSOURCE_H_

#include <Python.h>

/* condition is G_INPUT_READ or G_INPUT_WRITE */
int pysource_io_add_watch_list(GSList **list, int fd, int cond, PyObject *func, PyObject *data);
int pysource_timeout_add_list(GSList **list, int msecs, PyObject *func, PyObject *data);

#endif
