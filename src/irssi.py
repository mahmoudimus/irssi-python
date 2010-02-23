"""
"""

import sys
import _irssi
from _irssi import *

def command_bind(*args, **kwargs):
    """ see Script.command_bind() """
    get_script().command_bind(*args, **kwargs)

def command_unbind(*args, **kwargs):
    """ see Script.command_unbind() """
    get_script().command_unbind(*args, **kwargs)

def signal_add(*args, **kwargs):
    """ see Script.signal_add() """
    get_script().signal_add(*args, **kwargs)

def signal_remove(*args, **kwargs):
    """ see Script.signal_remove() """
    get_script().signal_remove(*args, **kwargs)

def timeout_add(*args, **kwargs):
    """ see Script.timeout_add() """
    get_script().timeout_add(*args, **kwargs)

def io_add_watch(*args, **kwargs):
    """ see Script.io_add_watch() """
    get_script().io_add_watch(*args, **kwargs)

def statusbar_item_register(*args, **kwargs):
    """ see Script.statusbar_item_register() """
    get_script().statusbar_item_register(*args, **kwargs)

def settings_add_str(*args, **kwargs):
    """ see Script.settings_add_str() """
    get_script().settings_add_str(*args, **kwargs)

def settings_add_int(*args, **kwargs):
    """ see Script.settings_add_int() """
    get_script().settings_add_int(*args, **kwargs)

def settings_add_bool(*args, **kwargs):
    """ see Script.settings_add_bool() """
    get_script().settings_add_bool(*args, **kwargs)

def settings_add_time(*args, **kwargs):
    """ see Script.settings_add_time() """
    get_script().settings_add_time(*args, **kwargs)

def settings_add_level(*args, **kwargs):
    """ see Script.settings_add_level() """
    get_script().settings_add_level(*args, **kwargs)

def settings_add_size(*args, **kwargs):
    """ see Script.settings_add_size() """
    get_script().settings_add_size(*args, **kwargs)
