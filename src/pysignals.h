#ifndef _PYSIGNALS_H_
#define _PYSIGNALS_H_
#include <Python.h>

/* forward */
struct _PY_SIGNAL_SPEC_REC;

typedef struct _PY_SIGNAL_REC
{
    struct _PY_SIGNAL_SPEC_REC *signal;
    char *command; /* used for command and variable signal */
    PyObject *handler;
    int is_signal;
} PY_SIGNAL_REC;

typedef enum
{
    PSG_COMMAND,
    PSG_SIGNAL,
    PSG_ALL,
} PSG_TYPE;

PY_SIGNAL_REC *pysignals_command_bind(const char *cmd, PyObject *func, 
        const char *category, int priority);
PY_SIGNAL_REC *pysignals_signal_add(const char *signal, PyObject *func, 
        int priority);
int pysignals_command_bind_list(GSList **list, const char *command, 
        PyObject *func, const char *category, int priority);
int pysignals_signal_add_list(GSList **list, const char *signal, 
        PyObject *func, int priority);
void pysignals_command_unbind(PY_SIGNAL_REC *rec);
void pysignals_signal_remove(PY_SIGNAL_REC *rec);
void pysignals_remove_generic(PY_SIGNAL_REC *rec);
int pysignals_remove_search(GSList **siglist, const char *name, 
        PyObject *func, PSG_TYPE type);
void pysignals_remove_list(GSList *siglist);
int pysignals_emit(const char *signal, PyObject *argtup);
int pysignals_continue(PyObject *argtup);
int pysignals_register(const char *name, const char *arglist);
int pysignals_unregister(const char *name);
void pysignals_init(void);
void pysignals_deinit(void);

#endif
