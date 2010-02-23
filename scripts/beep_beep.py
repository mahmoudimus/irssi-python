"""
    Translation of Perl script by Georg Lukas
"""

import irssi
import os

might_beep = True

def beep_overflow_timeout():
    global might_beep
    might_beep = True

    return False

def sig_beep():
    global might_beep

    beep_cmd = irssi.settings_get_str('beep_cmd')
    if not beep_cmd:
        return

    beep_flood = irssi.settings_get_int('beep_flood')
    if beep_flood <= 0:
        beep_flood = 1000
   
    if might_beep:
        os.system(beep_cmd)
        might_beep = False
        irssi.timeout_add(beep_flood, beep_overflow_timeout)

    irssi.signal_stop()

irssi.settings_add_str("lookandfeel", "beep_cmd", "play ~/.irssi/scripts/beep_beep.wav 2>&1 > /dev/null &")
irssi.settings_add_int("lookandfeel", "beep_flood", 250)
irssi.signal_add("beep", sig_beep)

