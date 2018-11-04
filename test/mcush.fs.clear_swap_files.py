#!/usr/bin/env python
'''clear swap files'''
import os
from mcush import *

if __name__ == '__main__':
    cmd = "find %s -name '%s*' -delete"% \
            (os.path.join(Env.TEST_DIR, ""), Env.TEST_PREFIX)
    os.system( cmd )

