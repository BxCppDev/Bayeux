#!/usr/bin/python3

import sys;

def run (file_name_, prefix_, mode_):
	sys.stderr.write ("DEBUG: file_name = '%s'\n"  % file_name_)
	sys.stderr.write ("DEBUG: prefix    = '%s'\n"  % prefix_)
	sys.stderr.write ("DEBUG: mode      = '%s'\n"  % mode_)
	class_name = prefix_[len ("GSL_CONST_"):-1].lower ()
	sys.stderr.write ("DEBUG: class_name= '%s'\n"  % class_name)
	file = open (file_name_)

	sys.stdout.write ("  \n")
	if mode_ == "include" :
		sys.stdout.write ("  class %s\n" % (class_name))
		sys.stdout.write ("  {\n")
		sys.stdout.write ("  public:\n")
	else:
		pass
	sys.stdout.write ("  \n")
	
	for line in file :
		tokens =  line.split ();
		gsl_const = tokens[0]
		const_name = gsl_const[len (prefix_):].lower ()
		if mode_ == "include" :
			sys.stdout.write ("    static const double %s;\n" % (const_name))
		else:
			sys.stdout.write ("    const double %s::%s = %s;\n" %
					  (class_name, const_name, gsl_const))
			

	if mode_ == "include" :
		sys.stdout.write ("  }; // end of class %s\n" % (class_name))
	else:
		pass
	
	sys.stdout.write ("  \n")


if __name__ == '__main__' :
	run (sys.argv[1], sys.argv[2], sys.argv[3])
	sys.exit (0)


# Local Variables: 
# mode: python        
# coding: utf-8    
# End:             

# end
