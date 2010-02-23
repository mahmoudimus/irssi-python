import irssi
import os
import time
import sys

child_pid = 0

def sig_pidwait(pid, status):
    if child_pid != pid:
        print 'pidwait dont know',pid
        return

    if os.WIFSIGNALED(status):
        print '%d killed by signal' % pid
    elif os.WIFEXITED(status):
        print '%d exited(%d)' % (pid, os.WEXITSTATUS(status))

    irssi.signal_remove('pidwait')

def read_child(fd, condition, out):
    data = os.read(fd, 512)
    if not data:
        return False
    out.write(data) 
    return True

def childfunc():
    """ do your stuff """
    for i in xrange(30):
        print 'ME CHILD', i
        time.sleep(1)    


def cmd_forkoff(data, server, witem):
    global child_pid
   
    rs, ws = os.pipe()
    re, we = os.pipe()

    pid = os.fork()
    if pid > 0:
        #parent
        child_pid = pid
        irssi.pidwait_add(pid)
        print 'forked off',pid
        irssi.signal_add('pidwait', sig_pidwait)

        #redirect child output
        irssi.io_add_watch(rs, read_child, sys.stdout)
        irssi.io_add_watch(re, read_child, sys.stderr)

    else:
        #child
        sys.stdout = os.fdopen(ws, 'w', 0)
        sys.stderr = os.fdopen(we, 'w', 0)

        childfunc()

        sys.stdout.close()
        sys.stderr.close()
        os._exit(5)

irssi.command_bind('forkoff', cmd_forkoff)
