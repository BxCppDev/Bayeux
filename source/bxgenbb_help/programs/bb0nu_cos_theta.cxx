// -*- mode: c++; -*- 
// bb0nu_cos_theta.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/algorithm/string.hpp>

#include <genbb_help/genbb_mgr.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Hello, this is a sample program for class 'genbb_mgr'!" 
		<< std::endl; 
  
      bool debug = false;
      std::string input_file;

      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string arg = argv_[iarg];

	  if (arg[0] == '-')
	    {
	      if (arg == "-d" || arg == "--debug") debug = true;
	    }
	  else
	    {
	      if (input_file.empty ())
		{
		  input_file = arg;
		}
	    }
	  iarg++;
	}
      if (input_file.empty ())
	{
	  input_file = "resources/se82_0nubb_mn.genbb";
	}
      std::clog << "NOTICE: input_file = '" << input_file << "'" << std::endl;
      genbb::genbb_mgr mgr;
      
      if (boost::algorithm::iends_with (input_file, ".genbb")) 
	{
	  std::clog << "NOTICE: Use GENBB input file format." << std::endl;
	  mgr.set_format (genbb::genbb_mgr::FORMAT_GENBB);
	}
      
      mgr.set (input_file.c_str ()); 
      if (debug)  mgr.dump ();
      mgr.init ();
      mgr.dump ();

      double me = CLHEP::electron_mass_c2;
      genbb::primary_event pe;
      while (mgr.has_next ())
	{
	  mgr.load_next (pe);
	  if (debug) pe.dump ();
	  double e_sum = 0.0;
	  if (debug) std::clog << "debug: me=" 
			       << me / CLHEP::MeV 
			       << " MeV" << std::endl;
	  if (pe.get_particles ().size ()== 2)
	    {
	      genbb::primary_particle pp1 = pe.get_particles ().front ();
	      pe.get_particles ().pop_front ();
	      genbb::primary_particle pp2 = pe.get_particles ().front ();
	      pe.get_particles ().pop_front ();
	      if (! pp1.is_electron () && ! pp2.is_electron ())
		{
		  continue;
		}
	      if (debug) 
		{
		  std::clog << "p1=" << pp1.get_momentum () / CLHEP::MeV << std::endl;
		  std::clog << "p2=" << pp2.get_momentum () / CLHEP::MeV << std::endl;
		}
	      double p1 = pp1.get_momentum ().mag ();
	      double p2 = pp2.get_momentum ().mag ();
	      if (debug) 
		{
		  std::clog << "|p1|=" << p1 / CLHEP::MeV  << std::endl;
		  std::clog << "|p2|=" << p2 / CLHEP::MeV  << std::endl;
		}
	      double p1_dot_p2 = pp1.get_momentum () * pp2.get_momentum ();
	      double cos_theta = p1_dot_p2 / (p1 * p2);
	      std::cout << cos_theta << std::endl;
	    }
	}
      mgr.reset ();
      if (debug) mgr.dump ();
   
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of bb0nu_cos_theta.cxx
