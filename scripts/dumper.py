# type /pyload dumper

import sys
import irssi

__script = None
__last_witem = None
__last_server = None

def cmd_pydumper(data, server, witem):
    assert isinstance(server, irssi.Server), "This should be a Server"
    assert isinstance(witem, irssi.WindowItem), "This should be a WindowItem"
    assert isinstance(witem, irssi.Query) or \
        isinstance(witem, irssi.Channel), \
        "... and be a Query or Channel"

    server.channels_join("#neblooh")
    #server.disconnect()
    sc = server.connect

    print 'witem.server', witem.server

    print 'Server.Connect', sc
    print 'connect.type', sc.type
    print 'connect.type_id', sc.type_id
    print 'connect.chat_type', sc.chat_type
    print 'connect.chat_type_id', sc.chat_type_id
    print 'connect.address', sc.address
    print 'connect.port', sc.port
    print 'connect.chatnet', sc.chatnet
    print 'connect.password', sc.password
    print 'connect.wanted_nick', sc.wanted_nick
    print 'connect.username', sc.username
    print 'connect.realname', sc.realname
    if isinstance(sc, irssi.IrcConnect):
        print 'IRC Connect items:'
        print 'connect.alternate_nick', sc.alternate_nick

    print
    print
    print 'Server', server
    print 'server.type', server.type
    print 'server.type_id', server.type_id
    print 'server.chat_type', server.chat_type
    print 'server.chat_type_id', server.chat_type_id
    print 'server.connect_time', server.connect_time
    print 'server.real_connect_time', server.real_connect_time
    print 'server.tag', server.tag
    print 'server.nick', server.nick
    print 'server.connected', server.connected
    print 'server.connection_lost', server.connection_lost
    print 'server.rawlog', server.rawlog
    print 'server.version', server.version
    print 'server.last_invite', server.server_operator
    print 'server.usermode_away', server.usermode_away
    print 'server.away_reason', server.away_reason
    print 'server.banned', server.banned
    print 'server.lag', server.lag
    if isinstance(server, irssi.IrcServer):
        print 'IRC Server items:'
        print 'server.real_address', server.real_address
        print 'server.usermode', server.usermode 
        print 'server.userhost', server.userhost 

    print
    print
    print 'Witem', witem
    print 'witem.type', witem.type
    print 'witem.type_id', witem.type_id
    print 'witem.chat_type', witem.chat_type
    print 'witem.chat_type_id', witem.chat_type_id
    print 'witem.server', witem.server
    print 'witem.name', witem.name
    print 'witem.createtime', witem.createtime
    print 'witem.data_level', witem.data_level
    print 'witem.hilight_color', witem.hilight_color

    #if witem.type == "CHANNEL":
    if isinstance(witem, irssi.Channel):
        print 'channel items:'
        print 'witem.topic', witem.topic
        print 'witem.topic_by', witem.topic_by
        print 'witem.topic_time', witem.topic_time
        print 'witem.no_modes', witem.no_modes
        print 'witem.mode', witem.mode
        print 'witem.limit', witem.limit
        print 'witem.key', witem.key
        print 'witem.chanop', witem.chanop
        print 'witem.names_got', witem.names_got
        print 'witem.wholist', witem.wholist
        print 'witem.synced', witem.synced
        #witem.destroy()
        print 'witem.joined', witem.joined
        print 'witem.left', witem.left
        print 'witem.kicked', witem.kicked
        if isinstance(witem, irssi.IrcChannel):
            print 'IRC channel:'
            print 'witem.bans', witem.bans()
            for ban in witem.bans():
                print 'ban.ban', ban.ban
                print 'ban.setby', ban.setby
                print 'ban.time', ban.time
                
    #elif witem.type == "QUERY":
    elif isinstance(witem, irssi.Query):
        print 'query items:'
        print 'witem.address', witem.address
        witem.change_server(server)
        #witem.change_server(witem)
        print 'witem.server_tag', witem.server_tag
        print 'witem.unwanted', witem.unwanted

    print
    print
    print 'is nick flag "@"?', server.isnickflag('@')
    print 'is nick flag "+"?', server.isnickflag('+')
    print 'is nick flag "%"?', server.isnickflag('%')

    print 'is channel "#fuggerd"', server.ischannel('#fuggerd')
    print 'is channel "&booh"', server.ischannel('&booh')
    print 'is channel "xbooh"', server.ischannel('xbooh')

    print 'nick flags', server.get_nick_flags()

    print irssi.chatnets()
    for cn in irssi.chatnets():
        print 'cn.type', cn.type
        print 'cn.chat_type', cn.chat_type
        print 'cn.name', cn.name
        print 'cn.nick', cn.nick
        print 'cn.username', cn.username 
        print 'cn.realname', cn.realname 
        print 'cn.own_host', cn.own_host 
        print 'cn.autosendcmd', cn.autosendcmd
        print

    print irssi.chatnet_find('ircnet')
    print irssi.servers()
    print irssi.reconnects()

    print irssi.windows()
    for win in irssi.windows():
        print 'win.refnum', win.refnum
        print 'win.name', win.name
        print 'win.width', win.width 
        print 'win.height', win.height
        print 'win.history_name', win.history_name
        print 'win.active', win.active
        print 'win.active_server', win.active_server
        print 'win.servertag', win.servertag
        print 'win.level', win.level
        print 'win.sticky_refnum', win.sticky_refnum
        print 'win.data_level', win.data_level
        print 'win.hilight_color', win.hilight_color
        print 'win.last_timestamp', win.last_timestamp
        print 'win.last_line', win.last_line
        print 'win.theme_name', win.theme_name
        print

    """
    print 'printing to channel'
    server.send_message('#booh', 'test msg chan', 0)
    server.send_message('#booh', 'test msg chan ER', 1)

    print 'printing to nick'
    server.send_message('melbo', 'test msg nick', 1)
    server.send_message('melbo', 'test msg nick ER', 0)
    """

    witem.prnt('hello there')
    global __last_witem
    __last_witem = witem
    global __last_server
    __last_server = server

    #new = irssi.IrssiChatBase()
    #print 'New', new.type_id

def cmd_crashme(data, server, witem):
    __last_server.prnt('#booh', 'what up??')
    __last_witem.prnt('imma crash mebbe?')

print dir(_script)
print _script.module
print _script.argv

irssi.command_bind('pydumper', cmd_pydumper)
irssi.command_bind('crashme', cmd_crashme)
