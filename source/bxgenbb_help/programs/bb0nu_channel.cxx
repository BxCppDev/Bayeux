// -*- mode: c++; -*- 
// bb0nu_channel.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include <mygsl/rng.h>
#include <genbb_help/genbb_mgr.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      bool debug = false;
      string input_file;
      string rng_id              = "mt19937";
      unsigned long int rng_seed = 12345;
      double energy_resolution = 0.0;
      double energy_min        = 0.0;
      double energy_sum_min    = 0.0;
      double energy_sum_max    = 10000.0 * CLHEP::keV;

      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];

	  if (arg[0] == '-')
	    {
	      if (arg == "-d" || arg == "--debug") 
		{
		  debug = true;
		}

	      if (arg == "-r" || arg == "--energy-resolution") 
		{
		  iarg++;
		  string token = argv_[iarg];
		  istringstream iss (token);
		  iss >> energy_resolution;
		}

	      if (arg == "-s" || arg == "--seed") 
		{
		  iarg++;
		  string token = argv_[iarg];
		  istringstream iss (token);
		  iss >> rng_seed;
		}

	      if (arg == "-m" || arg == "--energy-min") 
		{
		  iarg++;
		  string token = argv_[iarg];
		  istringstream iss (token);
		  iss >> energy_min;
		  energy_min *= CLHEP::keV;
		}

	      if (arg == "-sm" || arg == "--energy-sum-min") 
		{
		  iarg++;
		  string token = argv_[iarg];
		  istringstream iss (token);
		  iss >> energy_sum_min;
		  energy_sum_min *= CLHEP::keV;
		}

	      if (arg == "-sM" || arg == "--energy-sum-max") 
		{
		  iarg++;
		  string token = argv_[iarg];
		  istringstream iss (token);
		  iss >> energy_sum_max;
		  energy_sum_max *= CLHEP::keV;
		}

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
      clog << "NOTICE: input_file = '" << input_file << "'" << endl;

      clog << "NOTICE: energy_resolution = " << energy_resolution 
	   << "" << endl;
      clog << "NOTICE: energy_min = " << energy_min / CLHEP::keV 
	   << " keV" << endl;
      clog << "NOTICE: energy_sum_min = " << energy_sum_min / CLHEP::keV 
	   << " keV" << endl;
      clog << "NOTICE: energy_sum_max = " << energy_sum_max  / CLHEP::keV
	   << " keV" << endl;
      clog << "NOTICE: rng_seed = " << rng_seed << endl;

      mygsl::rng r;
      r.init (rng_id, rng_seed);

      genbb::genbb_mgr mgr;
      
      if (boost::algorithm::iends_with (input_file, ".genbb")) 
	{
	  clog << "NOTICE: Use GENBB input file format." << endl;
	  mgr.set_format (genbb::genbb_mgr::FORMAT_GENBB);
	}
      
      mgr.set (input_file.c_str ()); 
      if (debug) mgr.dump ();
      mgr.init ();
      mgr.dump ();

      double me = CLHEP::electron_mass_c2;
      genbb::primary_event pe;
      int count = 0;
      int selected_count = 0;
      double factor = 2. * sqrt (2. * log (2));
      clog << "NOTICE: factor=" << factor << endl;
      while (mgr.has_next ())
	{
	  mgr.load_next (pe);
	  if (debug) pe.dump ();
	  double e_sum = 0.0;
	  count++;
	  if (debug) clog << "debug: me=" 
			  << me / CLHEP::MeV 
			  << " MeV" << endl;
	  if (pe.get_particles ().size () == 2)
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
		  clog << "p1=" << pp1.get_momentum () / CLHEP::MeV << endl;
		  clog << "p2=" << pp2.get_momentum () / CLHEP::MeV << endl;
		}
	      double p1 = pp1.get_momentum ().mag ();
	      double p2 = pp2.get_momentum ().mag ();
	      if (debug) 
		{
		  clog << "|p1|=" << p1 / CLHEP::MeV  << endl;
		  clog << "|p2|=" << p2 / CLHEP::MeV  << endl;
		}
	      double p1_dot_p2 = pp1.get_momentum () * pp2.get_momentum ();
	      double cos_theta = p1_dot_p2 / (p1 * p2);
	      double e1 = pp1.get_kinetic_energy ();
	      double e2 = pp2.get_kinetic_energy ();

	      double fwhm1 = energy_resolution * sqrt (e1 / CLHEP::MeV);
	      double fwhm2 = energy_resolution * sqrt (e2 / CLHEP::MeV);

	      double sigma_e1 = fwhm1 / factor; 
	      double sigma_e2 = fwhm2 / factor; 

	      if (sigma_e1 > 0.0)
		{
		  e1 = r.gaussian (e1, sigma_e1);
		}
	      if (sigma_e2 > 0.0)
		{
		  e2 = r.gaussian (e2, sigma_e2);
		}
	      double esum = e1 + e2;
	      if ((e1 > energy_min)
		  && (e2 > energy_min)
		  && (esum > energy_sum_min)
		  && (esum < energy_sum_max))
		{
		  selected_count++;
		  cout << e1 / CLHEP::keV  << ' ' << e2 / CLHEP::keV << ' ' 
		       << esum / CLHEP::keV << ' ' << cos_theta << endl;
		}
	    }
	}
      mgr.reset ();
      double efficiency = selected_count * 1.0 / count;
      clog << "Efficiency=" << efficiency << endl;
      if (debug) mgr.dump ();
   
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of bb0nu_channel.cxx
