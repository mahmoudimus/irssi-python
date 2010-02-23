# type /pyload hello

import irssi

# data - contains the parameters for /HELLO
# server - the active server in window
# witem - the active window item (eg. channel, query)
#         or None if the window is empty
def cmd_hello(data, server, witem):
    if not server or not server.connected:
        irssi.prnt("Not connected to server")

    if data:
        server.command("MSG %s Hello!" % data)
    elif isinstance(witem, irssi.Channel) or isinstance(witem, irssi.Query):
        witem.command("MSG %s Hello!" % witem.name)
    else:
        irssi.prnt("Nick not given, and no active channel/query in window")

irssi.command_bind('hello', cmd_hello)
