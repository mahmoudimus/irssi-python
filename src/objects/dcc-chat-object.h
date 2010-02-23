#ifndef _DCC_CHAT_OBJECT_H_
#define _DCC_CHAT_OBJECT_H_

#include <Python.h>
#include "dcc-object.h"

/* forward */
struct CHAT_DCC_REC;

typedef struct
{
    PyDcc_HEAD(struct CHAT_DCC_REC)
} PyDccChat;

extern PyTypeObject PyDccChatType;

PyObject *pydcc_chat_new(void *dcc);
#define pydcc_chat_check(op) PyObject_TypeCheck(op, &PyDccChatType)
int dcc_chat_object_init(void);

#endif
