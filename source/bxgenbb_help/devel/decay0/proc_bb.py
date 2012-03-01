#!/usr/bin/python
# -*- mode: python; -*-

import sys
import os
import string
import re
import math

def main (argv_) :
    """Main function"""

    sys.stderr.write ("NOTICE: Entering the 'process_decay0.py' script...\n")
    if len (argv_) < 1 :
        sys.stderr.write ("ERROR: Missing Decay0 source file ! Abort !")
        return 1
    genbb_file = argv_[1].strip ()
    nevents = None
    if len (argv_) > 2 :
        nevents = int (argv_[2].strip ())
        sys.stderr.write ("NOTICE: GENBB data file has '%d'events\n" % nevents)

    sys.stderr.write ("NOTICE: GENBB data file name is '%s'\n" % genbb_file)
    f = open (genbb_file)

    if nevents == None:
        while True :
            line = f.readline ()
            l = line.strip ()
            sys.stderr.write ("NOTICE: line is '%s'\n" % l)
            ll = l.split (' ')
            if ll[0] == "First" :
                break
        line = f.readline ().strip ()
        sys.stderr.write ("NOTICE: line is '%s'\n" % line)
        l = re.sub ("[ \t]+", " ", line)
        sys.stderr.write ("NOTICE: l is '%s'\n" % l)
        ll = l.split (" ")
        nevents = int (ll[1])
        sys.stderr.write ("NOTICE: nevents = '%d'\n" % nevents)
        line = f.readline ()
        lines = f.readlines ()
        counts = len (lines)
        pass
    else:
        lines = f.readlines ()
    icount = 0
    iline = icount
    for icount in range (nevents) :
        l1 = lines[iline].strip ()
        l2 = re.sub ("[ \t]+", " ", l1)
        #sys.stderr.write ("NOTICE: l2 is '%s'\n" % l2)
        ll2 = l2.split (" ", 3)
        event_number = int (ll2[0])
        event_time = float (ll2[1])
        event_nparts = int (ll2[2])
        #sys.stderr.write ("NOTICE: event_number = '%d'\n" % event_number)
        #sys.stderr.write ("NOTICE: event_time   = '%f'\n" % event_time)
        #sys.stderr.write ("NOTICE: event_nparts = '%d'\n" % event_nparts)
        pvec = []
        esum = 0.0
        for ipart in range (event_nparts) :
            iline = iline + 1
            l1 = lines[iline].strip ()
            l2 = re.sub ("[ \t]+", " ", l1)
            #sys.stderr.write ("NOTICE: l2 is '%s'\n" % l2)
            ll2 = l2.split (" ", 5)
            part_type = int (ll2[0])
            part_px   = float (ll2[1])
            part_py   = float (ll2[2])
            part_pz   = float (ll2[3])
            part_time = float (ll2[4])
            #sys.stderr.write ("NOTICE:   part_type = '%d'\n" % part_type)
            #sys.stderr.write ("NOTICE:   part_px   = '%f'\n" % part_px)
            #sys.stderr.write ("NOTICE:   part_py   = '%f'\n" % part_py)
            #sys.stderr.write ("NOTICE:   part_pz   = '%f'\n" % part_pz)
            #sys.stderr.write ("NOTICE:   part_time = '%f'\n" % part_time)
            p2 = part_px*part_px + part_py*part_py + part_pz*part_pz
            pvec.append ({ "px" : part_px, "py" : part_py, "pz" : part_pz})
            me = 0.511
            e2 = math.sqrt(p2 + me * me)
            ke = e2 - me
            sys.stdout.write ("%f " % ke)
            esum = esum + ke
            pass
        p0 = math.sqrt (pvec[0]["px"]* pvec[0]["px"] \
                            + pvec[0]["py"]* pvec[0]["py"] \
                            + pvec[0]["pz"]* pvec[0]["pz"])
        p1 = math.sqrt (pvec[1]["px"]* pvec[1]["px"] \
                            + pvec[1]["py"]* pvec[1]["py"] \
                            + pvec[1]["pz"]* pvec[1]["pz"])
        p0p1 = pvec[0]["px"]* pvec[1]["px"] \
            + pvec[0]["py"]* pvec[1]["py"] \
            + pvec[0]["pz"]* pvec[1]["pz"]
        cost = p0p1 / (p0 * p1)
        sys.stdout.write ("%f %f\n" % (esum, cost))
        iline = iline + 1
        pass


if __name__ == "__main__" :
    ret = main (sys.argv)
    sys.exit (ret)

# end
