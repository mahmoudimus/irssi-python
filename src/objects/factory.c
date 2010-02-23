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

#include <Python.h>
#include "pyirssi.h"
#include "factory.h"

/* Irssi object factory works for all items with at least a type member.
 *
 * Use py_irssi_new() or py_irssi_chat_new() to get a new wrapper for an
 * IrssiObject or an IrssiChatObject, respectively.
 *
 * For objects not descending from IrssiObject or IrssiChatObject, you must
 * use the object-specific init function directly.
 */

#define MAKEKEY(type, chat) ((chat << 16 ) | type)
#define GETTYPE(key) (key & 0xffff)
#define GETCHAT(key) ((key >> 16) & 0xffff)


GHashTable *init_map = NULL;

static int init_objects(void);
static void register_chat(CHAT_PROTOCOL_REC *rec);
static void unregister_chat(CHAT_PROTOCOL_REC *rec);
static void insert_map(int type, int chat_type, InitFunc func);
static int remove_chat(void *key, void *value, void *chat_typep);
static void register_nonchat(void);
static InitFunc find_map(int type, int chat_type);

static int init_objects(void)
{
    if (!pyscript_init())
        return 0;

    /* order is somewhat important here */
    if (!base_objects_init())
        return 0;

    if (!window_item_object_init())
        return 0;

    if (!channel_object_init())
        return 0;

    if (!query_object_init())
        return 0;
    
    if (!server_object_init())
        return 0;

    if (!connect_object_init())
        return 0;

    if (!irc_server_object_init())
        return 0;

    if (!irc_connect_object_init())
        return 0;
   
    if (!irc_channel_object_init())
        return 0;

    if (!ban_object_init())
        return 0;

    if (!nick_object_init())
        return 0;

    if (!chatnet_object_init())
        return 0;

    if (!reconnect_object_init())
        return 0;

    if (!window_object_init())
        return 0;

    if (!textdest_object_init())
        return 0;

    if (!rawlog_object_init())
        return 0;

    if (!log_object_init())
        return 0;

    if (!logitem_object_init())
        return 0;

    if (!ignore_object_init())
        return 0;

    if (!dcc_object_init())
        return 0;

    if (!dcc_chat_object_init())
        return 0;
    
    if (!dcc_get_object_init())
        return 0;

    if (!dcc_send_object_init())
        return 0;
    
    if (!netsplit_object_init())
        return 0;

    if (!netsplit_server_object_init())
        return 0;

    if (!netsplit_channel_object_init())
        return 0;

    if (!notifylist_object_init())
        return 0;

    if (!process_object_init())
        return 0;

    if (!command_object_init())
        return 0;

    if (!theme_object_init())
        return 0;

    if (!statusbar_item_object_init())
        return 0;
    
    if (!main_window_object_init())
        return 0;

    return 1;
}

static InitFunc find_map(int type, int chat_type)
{
    unsigned hash;

	g_return_val_if_fail(type <= 0xffff, NULL);
	g_return_val_if_fail(chat_type <= 0xffff, NULL);

    hash = MAKEKEY(type, chat_type);
    return g_hash_table_lookup(init_map, GUINT_TO_POINTER(hash));
}

static void insert_map(int type, int chat_type, InitFunc func)
{
    unsigned hash;

	g_return_if_fail(type <= 0xffff);
	g_return_if_fail(chat_type <= 0xffff);

    hash = MAKEKEY(type, chat_type);
    g_hash_table_insert(init_map, GUINT_TO_POINTER(hash), func);
}

static void register_chat(CHAT_PROTOCOL_REC *rec)
{
    int type;
    int chat_type;	
    int is_irc = 0;

    /* chat_type == rec->id ??? */
    chat_type = chat_protocol_lookup(rec->name);
	g_return_if_fail(chat_type >= 0 && chat_type < 0xffff);

    if (!g_strcasecmp(rec->name, "IRC"))
        is_irc = 1;
    
	type = module_get_uniq_id("SERVER", 0);
    if (is_irc)
        insert_map(type, chat_type, (InitFunc)pyirc_server_new);
    else
        insert_map(type, chat_type, (InitFunc)pyserver_new);

	type = module_get_uniq_id("SERVER CONNECT", 0);
    if (is_irc)
        insert_map(type, chat_type, (InitFunc)pyirc_connect_new);
    else
        insert_map(type, chat_type, (InitFunc)pyconnect_new);

	type = module_get_uniq_id_str("WINDOW ITEM TYPE", "CHANNEL");
    if (is_irc)
        insert_map(type, chat_type, (InitFunc)pyirc_channel_new); 
    else
        insert_map(type, chat_type, (InitFunc)pychannel_new);

	type = module_get_uniq_id_str("WINDOW ITEM TYPE", "QUERY");
    insert_map(type, chat_type, (InitFunc)pyquery_new);

	type = module_get_uniq_id("CHATNET", 0);
    insert_map(type, chat_type, (InitFunc)pychatnet_new); 

	type = module_get_uniq_id("NICK", 0);
    insert_map(type, chat_type, (InitFunc)pynick_new); 
}

/* register funcs for objects without a chat type */
static void register_nonchat(void)
{
    int type;
    int chat_type = 0xffff;

    type = module_get_uniq_id_str("DCC", "CHAT");
    insert_map(type, chat_type, (InitFunc)pydcc_chat_new);

    type = module_get_uniq_id_str("DCC", "GET");
    insert_map(type, chat_type, (InitFunc)pydcc_get_new);

    type = module_get_uniq_id_str("DCC", "SEND");
    insert_map(type, chat_type, (InitFunc)pydcc_send_new);

    type = module_get_uniq_id_str("DCC", "SERVER");
    insert_map(type, chat_type, (InitFunc)pydcc_new);
}

static int remove_chat(void *key, void *value, void *chat_typep)
{
    unsigned hash = GPOINTER_TO_UINT(key);
    int chat_type = GPOINTER_TO_INT(chat_type);

    if (GETCHAT(hash) == chat_type)
        return TRUE;

    return FALSE;
}

/* remove all items matching chat_type */
static void unregister_chat(CHAT_PROTOCOL_REC *rec)
{
    /*int chat_type = chat_protocol_lookup(rec->name);*/
    g_hash_table_foreach_remove(init_map, 
            (GHRFunc)remove_chat,
            GINT_TO_POINTER(rec->id));
}

PyObject *py_irssi_new(void *typeobj, int managed)
{
    IRSSI_BASE_REC *base = typeobj;
    InitFunc ifunc;
   
    if (!base)
        Py_RETURN_NONE;
    
    ifunc = find_map(base->type, 0xffff);

    if (ifunc)
        return ifunc(typeobj, managed);

    return PyErr_Format(PyExc_RuntimeError, "no initfunc for object type %d", base->type);
}

PyObject *py_irssi_chat_new(void *typeobj, int managed)
{
    IRSSI_CHAT_REC *chat = typeobj;
    InitFunc ifunc;
        
    if (!chat)
        Py_RETURN_NONE;
    
    ifunc = find_map(chat->type, chat->chat_type);

    if (ifunc)
        return ifunc(typeobj, managed);

    return PyErr_Format(PyExc_RuntimeError, "no initfunc for object type %d, chat_type %d", 
            chat->type, chat->chat_type);
}

PyObject *py_irssi_objlist_new(GSList *node, int managed, InitFunc init)
{
    PyObject *list = NULL;
  
    list = PyList_New(0);
    if (!list)
        goto error;
    
    for (; node != NULL; node = node->next)
    {
        int ret;
        PyObject *obj = init(node->data, managed);

        if (!obj)
            goto error;

        ret = PyList_Append(list, obj);
        Py_DECREF(obj);

        if (ret != 0)
            goto error;
    }

    return list;

error:
    Py_XDECREF(list);
    return NULL;
}

int factory_init(void)
{
    g_return_val_if_fail(init_map == NULL, 0);

    if (!init_objects())
        return 0;

    init_map = g_hash_table_new(g_direct_hash, g_direct_equal);
 	g_slist_foreach(chat_protocols, (GFunc) register_chat, NULL);
    register_nonchat();

	signal_add("chat protocol created", (SIGNAL_FUNC) register_chat);
	signal_add("chat protocol destroyed", (SIGNAL_FUNC) unregister_chat);
   
    return 1;
}

void factory_deinit(void)
{
    g_return_if_fail(init_map != NULL);

    g_hash_table_destroy(init_map);
    init_map = NULL;

	signal_remove("chat protocol created", (SIGNAL_FUNC) register_chat);
	signal_remove("chat protocol destroyed", (SIGNAL_FUNC) unregister_chat);
}

