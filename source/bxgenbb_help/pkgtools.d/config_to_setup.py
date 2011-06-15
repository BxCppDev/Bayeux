#!/usr/bin/env python
# -*- mode: python; -*-

import sys
import string

# Usage:
#
#   echo -e 'a="foo"\nb="billy"' | config_to_setup.py [c]sh
#
#   cat file.conf | config_to_setup.py sh  > setup.sh
#   cat file.conf | config_to_setup.py csh > setup.csh
#

if __name__ == '__main__' :
    shell=sys.argv[1]
    lines = sys.stdin.readlines ()
    for line in lines :
        l = line.strip ()
        if len (l) and l[0] != '#' :
            tokens = l.split ('=', 2)
            propname=tokens[0]
            propvalue=tokens[1]
            if shell == 'sh' :
                sys.stdout.write ('export %s=' % (propname))
            elif shell == 'csh' :
                sys.stdout.write ('setenv %s ' % (propname))
            else :
                sys.stderr.write ('config_to_setup.py: Invalid shell family \'%s\' !\n' % shell)
                sys.exit (1)
            sys.stdout.write ('%s ' % (propvalue)) 
            sys.stdout.write ('\n')
        pass
        
# end
