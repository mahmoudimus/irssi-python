#ifndef _SBAR_ITEM_OBJECT_H_
#define _SBAR_ITEM_OBJECT_H_

#include <Python.h>
#include "base-objects.h"

/* forward */
struct SBAR_ITEM_REC;

typedef struct
{
    PyIrssiFinal_HEAD(struct SBAR_ITEM_REC)
    PyObject *window;
} PyStatusbarItem;

extern PyTypeObject PyStatusbarItemType;

int statusbar_item_object_init(void);
PyObject *pystatusbar_item_new(void *sbar_item);
#define pystatusbar_item_check(op) PyObject_TypeCheck(op, &PyStatusbarItemType)

#endif
