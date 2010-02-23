#ifndef _BASE_OBJECTS_H_
#define _BASE_OBJECTS_H_

#include <Python.h>

/* data is a pointer to the underlying Irssi record */
/* base_name is the type name of the object returned from the type member
   it can be SERVER, CHANNEL, QUERY, etc. Note: base_name isn't freed, so
   it cannot point to heap memory */
/* cleanup_installed: 1 = a cleanup signal handler is installed, 0 = not installed */
#define PyIrssi_HEAD(type)  \
    PyObject_HEAD           \
    type *data;             \
    const char *base_name;  \
    int cleanup_installed; 

/* for uninheritable objects without a type id (Ban, Log, etc) */
#define PyIrssiFinal_HEAD(type) \
    PyObject_HEAD               \
    type *data;                 \
    int cleanup_installed; 

/* to access data from any irssi object */
typedef struct
{
    PyObject_HEAD
    void *data;
} PyIrssiObject;

#define DATA(obj) (obj? ((PyIrssiObject *)obj)->data : NULL)

/* base for classes with a type */
typedef struct
{
    int type;
} IRSSI_BASE_REC;

typedef struct
{
    PyIrssi_HEAD(IRSSI_BASE_REC)
} PyIrssiBase;


/* base for classes with type and a chat type */
typedef struct
{
    int type;
    int chat_type;
} IRSSI_CHAT_REC;

typedef struct
{
    PyIrssi_HEAD(IRSSI_CHAT_REC)
} PyIrssiChatBase;

extern PyTypeObject PyIrssiBaseType;
extern PyTypeObject PyIrssiChatBaseType;

#define pybase_check(op) PyObject_TypeCheck(op, &PyIrssiBaseType)
#define pychatbase_check(op) PyObject_TypeCheck(op, &PyIrssiChatBaseType)
#define py_instp(tp, to) ((tp *) (to)->tp_alloc(to, 0))
#define py_inst(tp, to) py_instp(tp, &to)

int base_objects_init(void);

#define RET_NULL_IF_INVALID(data)                                              \
    if (data == NULL)                                                          \
        return PyErr_Format(PyExc_RuntimeError, "wrapped object is invalid")

#define RET_N1_IF_INVALID(data)                                         \
do {                                                                    \
    if (data == NULL)                                                   \
    {                                                                   \
        PyErr_Format(PyExc_RuntimeError, "wrapped object is invalid");  \
        return -1;                                                      \
    }                                                                   \
} while (0)

#define RET_AS_STRING_OR_NONE(str)          \
do {                                        \
    if (str)                                \
        return PyString_FromString(str);    \
    else                                    \
    {                                       \
        Py_INCREF(Py_None);                 \
        return Py_None;                     \
    }                                       \
} while (0)


#define RET_AS_STRING_OR_EMPTY(str) return PyString_FromString(str? str : "")

#define RET_AS_OBJ_OR_NONE(obj) \
do {                            \
    PyObject *tmp = obj;        \
    if (!tmp)                   \
        tmp = Py_None;          \
    Py_INCREF(tmp);             \
    return tmp;                 \
} while (0)

#define INVALID_MEMBER(member)  \
    return PyErr_Format(PyExc_RuntimeError, "invalid member id, %d", member)

#endif
