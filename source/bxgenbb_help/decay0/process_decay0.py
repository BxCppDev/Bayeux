#!/usr/bin/python


import sys
import os
import string

decay0_author="V.I. Tretyak"
gplv3_txt="""c This program is free software; you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation; either version 3 of the License, or (at
c your option) any later version.
c 
c This program is distributed in the hope that it will be useful, but
c WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
c General Public License for more details.
c 
c You should have received a copy of the GNU General Public License
c along with this program; if not, write to the Free Software
c Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
c """

def main (argv_) :
    """Main function"""
    sys.stderr.write ("NOTICE: Entering the 'process_decay0.py' script...\n")
    if len (argv_) < 1 :
        sys.stderr.write ("ERROR: Missing Decay0 source file ! Abort !")
        return 1
    decay0_prog = argv_[1].strip ()
    sys.stderr.write ("NOTICE: Source file name is '%s'\n" % decay0_prog)
    file = open (decay0_prog)
    lines = file.readlines ()
    routines=[]
    program_name = ""
    is_program = False
    is_block_data = False
    current_routine = None
    current_routine_file = None
    current_routine_filename = None
    for line in lines :
        start_routine = False
        stop_routine = False
        routine_name = None
        l = line.strip ().split ()
        if line.strip ().lower () == "end" :
            stop_routine = True
        elif len (l) >= 2 :
            #sys.stderr.write ("DEVEL: l is '%s'\n" % l)
            tag = l[0].lower ()
            if tag == 'subroutine' or tag == 'function' or tag == 'program' or tag == 'block' :
                if tag == 'program' : 
                    is_program = True
                    is_block_data = False
                    routine_name = l[1]     
                    sys.stderr.write ("DEVEL: program is '%s'\n" % routine_name)
                    start_routine = True               
                elif tag == 'block' : 
                    is_block_data = True
                    is_program = False
                    routine_name = l[1]     
                    start_routine = True               
                    sys.stderr.write ("DEVEL: block is '%s'\n" % routine_name)
                else :
                    routine_name_list = l[1].split ('(')
                    if len (routine_name_list) > 1 :
                        routine_name = routine_name_list[0]
                        is_program = False     
                        is_block_data = False
                        start_routine = True               
                        routines.append (routine_name)
                        #sys.stderr.write ("DEVEL: routine is '%s'\n" % routine_name)
                    # if l[0] == 'function': 
                    #     #sys.stderr.write ("NOTICE: Found function '%s'\n" % routine_name)
                    #     pass
                    # else :
                    #     #sys.stderr.write ("NOTICE: Found subroutine '%s'\n" % routine_name)
                    #     pass

        if start_routine :
            current_routine_filename = "%s.%s" % (routine_name, "f")
            if is_program :
                sys.stderr.write ("DEVEL: start program '%s'\n" % routine_name)
                current_routine_filename = "prog_%s.%s" % (routine_name, "F")
            elif is_block_data :
                sys.stderr.write ("DEVEL: block data '%s'\n" % routine_name)
                current_routine_filename = "block_%s.%s" % (routine_name, "f")
            #sys.stderr.write ("DEVEL: current_routine_filename is '%s'\n" % current_routine_filename)
            current_routine = routine_name
            current_routine_file = open ("src/%s" % (current_routine_filename), "w");
            current_routine_file.write ("c %s \nc\n" % current_routine_filename )
            current_routine_file.write ("c This file was extracted from the 'decay0' program by %s\nc\n" % decay0_author)
            current_routine_file.write ("c Copyright 1995-2011 %s\nc\n" % decay0_author)
            current_routine_file.write ("%s\nc\n" % (gplv3_txt))
            gplv3_txt
            current_routine_file.write ("%s" % line )
        elif stop_routine :
            if current_routine != None :
                current_routine_file.write ("%s" % line )
                current_routine_file.write ("c\nc end of %s\n" % current_routine_filename)
                current_routine_file.write ("""c
c Local Variables: --
c mode: fortran --
c End: --
""")
                current_routine_file.close ();
                current_routine_file = None 
                current_routine_filename= None
                current_routine = None
        else:
            if current_routine_file != None :
                current_routine_file.write ("%s" % line)
 
    lib_list_file = open ("%s" % ("routines.lis"), "w");
    for routine_name in routines :
        if routine_name != program_name : 
            lib_list_file.write ("%s\n" % routine_name)
        pass


    sys.stderr.write ("NOTICE: Exiting the 'process_decay0.py' script.\n")
    return 0

if __name__ == "__main__" :
    ret = main (sys.argv)
    sys.exit (ret)

# end
