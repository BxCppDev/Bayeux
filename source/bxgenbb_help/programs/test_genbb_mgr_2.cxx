// -*- mode: c++; -*- 
// test_genbb_mgr_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/genbb_mgr.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Hello, this is a sample program for class 'genbb_mgr'!" 
		<< std::endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;

	  iarg++;
	}
    
      genbb::genbb_mgr mgr;
      mgr.set ("${GENBB_HELP_ROOT}/resources/se82_0nubb_mn.genbb");
      if (debug) mgr.dump ();
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
	  while (pe.particles.size () > 0)
	    {
	      genbb::primary_particle pp = pe.particles.front ();
	      pe.particles.pop_front ();
	      if (pp.is_electron ())
		{
		  double p = pp.get_momentum ().mag ();
		  if (debug) std::clog << "debug: p=" 
				       << p / CLHEP::MeV 
				       << " MeV" << std::endl;
		  double w = pp.get_total_energy ();
		  if (debug) std::clog << "debug: w=" 
				       << w / CLHEP::MeV 
				       << " MeV" << std::endl;
		  double e = pp.get_kinetic_energy ();
		  if (debug) std::clog << "debug: e=" 
				       << e / CLHEP::MeV 
				       << " MeV" << std::endl;
		  e_sum += e;
		}
	    }
	  std::cout << e_sum << std::endl;
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

// end of test_genbb_mgr_2.cxx
