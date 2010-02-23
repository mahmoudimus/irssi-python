import irssi

win0 = None
win1 = None

def cmd_wintest(data, server, witem):
    act_win = irssi.active_win()   
    act_server = irssi.active_server()   

    print 'active_win', act_win, 'ref', act_win.refnum
    print 'active_server', act_server

    items = act_win.items()
    print 'win.items()', items

    for i in items:
        print i, 'window ref', i.window().refnum, 'window name', i.window().name

    print
    print 'all windows'
    for i in irssi.windows():
        print 'window refnum', i.refnum, 'window name', i.name
    print

    f0 = irssi.window_find_name('melbo')
    f1 = irssi.window_find_name('(status)') 
    print 'irssi.window_find_name(melbo)', f0
    print 'irssi.window_find_name(status)', f1 

def cmd_opentest(data, server, witem):
    global win0, win1
    win0 = irssi.window_create(automatic=True)
    print 'window_create(automatic=True) ->', win0
    win1 = irssi.window_create(automatic=False)
    print 'window_create(automatic=False) ->', win1

def cmd_closetest(data, server, witem):
    print 'destroy win0 && win1'
    win0.destroy()
    win1.destroy() 

def cmd_postclose(*args):
    print 'post-close access'
    print win0.items()
    print win1.items()
    
irssi.command_bind('wintest', cmd_wintest)
irssi.command_bind('closetest', cmd_closetest)
irssi.command_bind('postclose', cmd_postclose)
irssi.command_bind('opentest', cmd_opentest)
