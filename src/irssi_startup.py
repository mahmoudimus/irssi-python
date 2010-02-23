import sys, imp, __builtin__
import _irssi

class Output:
    def __init__(self, level):
        self.level = level
        self.buf = []
    def write(self, text):
        if not text:
            return
        self.buf.append(text)
        if '\n' == text[-1]:
            text = ''.join(self.buf)[:-1]
            for line in text.split('\\n'):
                _irssi.active_win().prnt(line, self.level)
            self.buf = []

sys.stdout = Output(level = _irssi.MSGLEVEL_CLIENTCRAP)
sys.stderr = Output(level = _irssi.MSGLEVEL_CLIENTERROR)
