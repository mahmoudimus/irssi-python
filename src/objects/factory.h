#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <Python.h>
#include "pyscript-object.h"
#include "base-objects.h"
#include "window-item-object.h"
#include "channel-object.h"
#include "query-object.h"
#include "server-object.h"
#include "connect-object.h"
#include "irc-server-object.h"
#include "irc-connect-object.h"
#include "irc-channel-object.h"
#include "ban-object.h"
#include "nick-object.h"
#include "chatnet-object.h"
#include "reconnect-object.h"
#include "window-object.h"
#include "textdest-object.h"
#include "rawlog-object.h"
#include "log-object.h"
#include "logitem-object.h"
#include "ignore-object.h"
#include "dcc-object.h"
#include "dcc-chat-object.h"
#include "dcc-get-object.h"
#include "dcc-send-object.h"
#include "netsplit-object.h"
#include "netsplit-server-object.h"
#include "netsplit-channel-object.h"
#include "notifylist-object.h"
#include "process-object.h"
#include "command-object.h"
#include "theme-object.h"
#include "statusbar-item-object.h"
#include "main-window-object.h"

int factory_init(void);
void factory_deinit(void);

/*managed == 1: object invalidates itself
 *managed == 0: caller responsible for invalidating object
 *XXX: most objects invalidate themselves, ignoring "managed" switch,
 *     and some are never managed (Reconnect)
 */

/* For objects with a type member but no chat_type */
PyObject *py_irssi_new(void *typeobj, int managed);
/* For objects with both type and chat_type members */
PyObject *py_irssi_chat_new(void *typeobj, int managed);

typedef PyObject *(*InitFunc)(void *, int);
PyObject *py_irssi_objlist_new(GSList *node, int managed, InitFunc init);
#define py_irssi_chatlist_new(n, m) py_irssi_objlist_new(n, m, py_irssi_chat_new)
#define py_irssi_list_new(n, m) py_irssi_objlist_new(n, m, py_irssi_new)

#endif
