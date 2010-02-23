#ifndef _PYTHEMES_H_
#define _PYTHEMES_H_

#include <Python.h>

struct _TEXT_DEST_REC;

int pythemes_printformat(struct _TEXT_DEST_REC *dest, const char *script, const char *format, PyObject *argtup);
int pythemes_register(const char *script, PyObject *list);
void pythemes_unregister(const char *script);
int pythemes_init(void);

#endif
