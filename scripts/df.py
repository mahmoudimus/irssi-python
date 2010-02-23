"""
    Translation of script by Jochem Meyers
"""

import irssi
import os

output = ''

def get_disk_info():
    fp = os.popen('/bin/df -h')
    lines = fp.readlines()
    ret = []

    #dev, size, used, avail, pct, mnt_on
    maxm = irssi.settings_get_int('df_max_mounts')  
    for line in lines[1:maxm + 1]:
        ret.append(line.split())     

    return ret

def sb_df(item, get_size):
    item.default_handler(get_size, '{sb %s}' % output) 

def refresh_df():
    global output

    tmp = []
    for dev, size, used, avail, pct, mnt_on in get_disk_info():
        tmp.append(' [%s: A: %s U%%%%: %s]' % (dev, avail, pct))
   
    output = 'DF' + ''.join(tmp)
    irssi.statusbar_items_redraw('df')
    irssi.timeout_add(irssi.settings_get_int('df_refresh_time') * 1000, refresh_df)

    return False

irssi.statusbar_item_register('df', func=sb_df)
irssi.settings_add_int('misc', 'df_refresh_time', 60)
irssi.settings_add_int('misc', 'df_max_mounts', 6)
refresh_df()
