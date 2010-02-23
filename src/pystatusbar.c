/* 
    irssi-python

    Copyright (C) 2006 Christopher Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "pystatusbar.h"
#include "pyirssi.h"
#include "factory.h"

typedef struct
{
    char *name;
    PyObject *script;
    PyObject *handler;
} PY_BAR_ITEM_REC;

/* Map: item name -> bar item obj */
static GHashTable *py_bar_items = NULL;

static void py_add_bar_handler(const char *iname, PyObject *script, PyObject *handler)
{
    PY_BAR_ITEM_REC *sitem;

    sitem = g_new0(PY_BAR_ITEM_REC, 1);
    sitem->name = g_strdup(iname);
    sitem->script = script;
    sitem->handler = handler;
    Py_INCREF(script);
    Py_INCREF(handler);

    g_hash_table_insert(py_bar_items, sitem->name, sitem);
}

static void py_destroy_handler(PY_BAR_ITEM_REC *sitem)
{
    statusbar_item_unregister(sitem->name);

    g_free(sitem->name); /* destroy key */
    Py_DECREF(sitem->script);
    Py_DECREF(sitem->handler);
    g_free(sitem);
}

static void py_statusbar_proxy_call(SBAR_ITEM_REC *item, int sizeonly, PY_BAR_ITEM_REC *sitem)
{
    PyObject *pybaritem;
    PyObject *ret;

    g_return_if_fail(PyCallable_Check(sitem->handler));

    pybaritem = pystatusbar_item_new(item);
    if (!pybaritem)
    {
        PyErr_Print();
        pystatusbar_item_unregister(sitem->name);
    }

    ret = PyObject_CallFunction(sitem->handler, "Oi", pybaritem, sizeonly);
    if (!ret)
    {
        PyErr_Print();
        pystatusbar_item_unregister(sitem->name);
    }
    else
        Py_DECREF(ret);
}

static void py_statusbar_proxy(SBAR_ITEM_REC *item, int sizeonly)
{
    PY_BAR_ITEM_REC *sitem;    

    sitem = g_hash_table_lookup(py_bar_items, item->config->name);
    if (sitem)
        py_statusbar_proxy_call(item, sizeonly, sitem);
    else
    {
        statusbar_item_default_handler(item, sizeonly, NULL, "", TRUE);
        g_critical("unknown handler for Python statusbar proxy: %s", item->config->name);
    }
}

void pystatusbar_item_register(PyObject *script, const char *sitem, 
        const char *value, PyObject *func)
{
    if (func)
    {
        g_return_if_fail(PyCallable_Check(func));
        py_add_bar_handler(sitem, script, func);
    }

    statusbar_item_register(sitem, value, func? py_statusbar_proxy : NULL);
}

/* remove selected status bar item handler */
void pystatusbar_item_unregister(const char *iname)
{
    if (!g_hash_table_remove(py_bar_items, iname))
        statusbar_item_unregister(iname);
}

/* remove all statusbar item handlers for script */
/* XXX: Only status bar items registered with a handler are stored in the hash table.
 * Items registered with only a value are not stored, so there is no way to unregister 
 * them when the script is unloaded.
 */
static int py_check_clean(char *key, PY_BAR_ITEM_REC *value, PyObject *script)
{
    if (value->script == script)
        return 1;

    return 0;
}

void pystatusbar_cleanup_script(PyObject *script)
{
    g_hash_table_foreach_remove(py_bar_items, (GHRFunc)py_check_clean, script);
}

void pystatusbar_init(void)
{
    g_return_if_fail(py_bar_items == NULL);

    /* key is freed by destroy_handler */
    py_bar_items = g_hash_table_new_full(g_str_hash, g_str_equal,
            NULL, (GDestroyNotify)py_destroy_handler);
}

/* XXX: this must be called after cleaning up all the loaded scripts */
void pystatusbar_deinit(void)
{
    g_return_if_fail(py_bar_items != NULL);
    g_return_if_fail(g_hash_table_size(py_bar_items) == 0);

    g_hash_table_destroy(py_bar_items);
    py_bar_items = NULL;
}

