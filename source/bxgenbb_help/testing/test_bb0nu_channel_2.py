#!/usr/bin/env python3

import os
import os.path
import sys
import math

def zzz (var1, var2 = None) :
    f = open (os.path.expandvars ("${tmp_dir}/se82_bb0nu.data"))
    lines = f.readlines ()
    for line in lines :
        tokens = line.split ()
        e1   = float (tokens[0])
        p1x  = float (tokens[1])
        p1y  = float (tokens[2])
        p1z  = float (tokens[3])
        p1   = math.sqrt (p1x * p1x + p1y * p1y + p1z * p1z)
        e2   = float (tokens[4])
        p2x  = float (tokens[5])
        p2y  = float (tokens[6])
        p2z  = float (tokens[7])
        p2   = math.sqrt (p2x * p2x + p2y * p2y + p2z * p2z)
        esum = float (tokens[8])
        cost = float (tokens[9])
        #XXX
        p1cost = p1z / p1
        p1phy  = math.atan2 (p1y, p1x)
        p2cost = p2z / p2
        p2phy  = math.atan2 (p2y, p2x)
        if var1 == "e1" :
            print e1
        elif var1 == "p1" :
            print p1
        elif var1 == "p1x" :
            print p1x
        elif var1 == "p1y" :
            print p1y
        elif var1 == "p1z" :
            print p1z
        elif var1 == "p1cost" :
            print p1cost
        elif var1 == "p1phy" :
            print p1phy
        elif var1 == "e2" :
            print e2
        elif var1 == "p2" :
            print p2
        elif var1 == "p2x" :
            print p2x
        elif var1 == "p2y" :
            print p2y
        elif var1 == "p2z" :
            print p2z
        elif var1 == "p2cost" :
            print p2cost
        elif var1 == "p2phy" :
            print p2phy
        elif var1 == "esum" :
            print esum
        elif var1 == "cost" :
            print cost
        pass
    return

if __name__ == "__main__" :
    #sys.stderr.write ("Hello !\n")
    zzz (sys.argv[1])
    #sys.stderr.write ("Bye !\n")    
    sys.exit (0)

# end
