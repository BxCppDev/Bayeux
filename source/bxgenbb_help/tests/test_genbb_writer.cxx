// -*- mode: c++; -*- 
// test_genbb_writer.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <genbb_help/genbb_writer.h>

void usage(std::ostream & out_)
{
  out_ << "\ntest_genbb_writer usage:" << std::endl;
  out_ << "\n To generate a GENBB files with tuned properties:" << std::endl;
  out_ << "   shell>$ test_genbb_writer [options]" << std::endl;
  out_ << "\nExample: " << std::endl;
  out_ << "   shell>$ test_genbb_writer -d -N 100000 --type e-"
       << std::endl;
  out_ << "                 --energy-min 0. --energy-max 5."
       << std::endl;
  out_ << "                 --phi-min 0. --phi-max 90."
       << std::endl;
  out_ << "                 --seed 314159"
       << std::endl;
  out_ << "                 --outfile /scratch/test_genbb_writer.data"
       << std::endl;
  out_ << std::endl;
  out_ << std::endl;
}


int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  

      bool debug = false;

      int N = -1;
      int seed = 314159;
      std::string type = "";
      double Emin = -1., Emax = -1.;
      double phi_min = -1., phi_max = -1.;
      std::string file_out = "";

      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string arg = argv_[iarg];
	  if (arg[0] == '-')
	    {
	      if (arg == "-d" || arg == "--debug" )
		{
		  debug = true;
		}
	      else if (arg == "-h" || arg == "--help" )
		{
		  usage ( std::clog );
		  return 0;
		}
	      else if ( arg == "-s" || arg == "--seed" ) 
		{
		  ;
		  if ( ++iarg < argc_ )
		    seed = atoi ( argv_[iarg] );
		}
	      else if ( arg == "-N") 
		{
		  ;
		  if ( ++iarg < argc_ )
		    N = atoi ( argv_[iarg] );
		}
	      else if ( arg == "-t" || arg == "--type" ) 
		{
		  ;
		  if (++iarg < argc_)
		    type = argv_[iarg];
		}
	      else if ( arg == "-Emin" || arg == "--energy-min") 
		{
		  ;
		  if ( ++iarg < argc_ )
		    Emin = atof ( argv_[iarg] );
		}
	      else if ( arg == "-Emax" || arg == "--energy-max") 
		{
		  ;
		  if ( ++iarg < argc_ )
		    Emax = atof ( argv_[iarg] );
		}
	      else if ( arg == "-phi_min" || arg == "--phi-min") 
		{
		  ;
		  if ( ++iarg < argc_ )
		    phi_min = atof ( argv_[iarg] );
		}
	      else if ( arg == "-phi_max" || arg == "--phi-max") 
		{
		  ;
		  if ( ++iarg < argc_ )
		    phi_max = atof ( argv_[iarg] );
		}
	      else if (arg == "-o" || arg == "--outfile") 
		{
		  ;
		  if (++iarg < argc_)
		    file_out = argv_[iarg];
		}
	    }
	  iarg++;
	}

    if ( N < 0 || Emin < 0 || Emax < 0 || 
	 phi_min < 0 || phi_max > 180. )
      {
	usage ( std::clog );
	return 1;
      }
      genbb::genbb_writer writer;

      writer.set_npart        ( N );
      writer.set_part_per_evt ( 1 );
      writer.set_seed         ( seed );
      writer.set_Emin         ( Emin );
      writer.set_Emax         ( Emax );
      writer.set_phi_min      ( phi_min * M_PI / 180. );
      writer.set_phi_max      ( phi_max * M_PI / 180. );
      writer.set_outfile      ( file_out );

      if ( type == "electron" || type == "e-" )
	writer.set_type ( 3 );
      else if ( type == "positron" || type == "e+" )
	writer.set_type ( 2 );
      else if ( type == "gamma" || type == "g" )
	writer.set_type ( 1 );
      else if ( type == "alpha" || type == "a" )
	writer.set_type ( 47 );
      else
	writer.set_type ( -1 );

      if ( debug )
	writer.dump( std::clog );

      writer.run();

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

// end of test_genbb_writer.cxx
