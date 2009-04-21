#!/usr/bin/python
# -*- mode: python; -*-
# test.py

import sys
import os

from geomtools import *
from math import *

def test () :
	v1 = vector_3d (1., 0.5, 0.0)
	print "x=", v1.x ()
	print "y=", v1.y ()
	print "z=", v1.z ()
	print "v1=", v1
	v2 = vector_3d (1.0, 1.0, 1.0)
	
	v2 += v1;
	print "x=", v2.x ()
	print "y=", v2.y ()
	print "z=", v2.z ()
	print "v2=", v2
	v2 *= 3;
	print "x=", v2.x ()
	print "y=", v2.y ()
	print "z=", v2.z ()
	print "v2=", v2
	v2.set (1.0, 1.0, 1.0)

	v3 = (0.3 * v1 - v2 * 2) / 5 
	print "v3=", v3

	PI = acos (-1.0)
	r1 = rotation_3d ()
	create_rotation_3d (r1, PI, PI/3, 0)
	print "r1=", r1
	if is_valid_rotation_3d (r1) :
		print "r1 is valid"
	else:
		print "r1 is not valid"

	reset_rotation_3d (r1)
	print "r1 (reset)=", r1
	
	invalidate_rotation_3d (r1)
	print "r1 (invalidate)=", r1
	if is_valid_rotation_3d (r1) :
		print "r1 is valid"
	else:
		print "r1 is not valid"
	v4 = r1 * v3
	print "v4=", v4
	
	invalidate_vector_3d (v2)
	print "v2 (invalidate)=", v2
	if is_valid_vector_3d (v2) :
		print "v2 is valid"
	else:
		print "v2 is not valid"

if __name__ == '__main__':
	sys.stdout.write ("geomtools: running test.py\n" % ())
	test ()
	sys.exit (0)

# end of test.py
