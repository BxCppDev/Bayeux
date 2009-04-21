#!/usr/bin/python
# -*- mode: python; -*-
# test.py

import sys
import os

import datatools

def test () :
	com = datatools.command ()
	com.init ("ls", True)
	com.add_argument ("-l")
	com.add_argument ("datatools.so")
	com.dump_stdout ()
	com.set_error ()
	com.set_error_message ("There are errors!")
	com.set_error_code (666)
	com.dump_stdout ()


	p = datatools.properties ()
	p.set_description ("Configuration")
	p.store_flag ("debug")
	p.store_flag ("buggy")
	p.set_flag ("nada")
	p.store_boolean ("Done", False, "Completeness")
	p.store_real ("Pi", 3.14159, "The Pi constant")
	p.dump_stdout ()
	p.unset_flag ("nada")

	if p.has_key ("toto") :
		print "p has 'toto'!"
	else :
		print "p has no 'toto'!"

	x = p.fetch_real ("Pi")
	print "x = ", x
	p.dump_stdout ()
		


if __name__ == '__main__':
	sys.stdout.write ("datatools: running Test.py\n" % ())
	test ()
	sys.exit (0)

# end of test.py
