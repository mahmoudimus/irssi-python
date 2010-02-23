"""
    Translation of Perl script by Peder Stray
"""

import irssi
import os
from os import path
import shutil

def sig_dcc_closed(dcc):
    if not isinstance(dcc, irssi.DccGet) or not path.isfile(dcc.file):
        return

    dir = path.dirname(dcc.file)
    dir = path.join(dir, 'done') 
    file = path.basename(dcc.file)

    if dcc.transfd < dcc.size:
        remain = 0
        if dcc.size:
            remain = 100 - dcc.transfd / dcc.size * 100 
        print '%%gDCC aborted %%_%s%%_, %%R%d%%%%%%g remaining%%n' % \
                (file, remain)
        return

    if not path.isdir(dir):
        os.mkdir(dir, 0755)

    shutil.move(dcc.file, dir)

    print '%%gDCC moved %%_%s%%_ to %%_%s%%_%%n' % (file, dir)

irssi.signal_add('dcc closed', sig_dcc_closed)
