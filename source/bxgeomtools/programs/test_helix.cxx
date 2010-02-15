// -*- mode: c++; -*- 
// test_helix.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/helix_3d.h>
#include <geomtools/gnuplot_i.h>


void
wait_for_key ()
{
  std::cout << std::endl
	    << "Press ENTER to continue..."
	    << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
  return;
}


int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug   = false;
      bool gnuplot = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug")
	    debug = true;
	  if (arg == "-g" || arg == "--gnuplot")
	    gnuplot = true;

	  iarg++;
	}
    
      geomtools::helix_3d my_helix_1;
      geomtools::helix_3d my_helix_2;

      geomtools::vector_3d helix_center_1 ( 0., 0., 0. );
       
      my_helix_1.set_radius ( 1. * CLHEP::m  );
      my_helix_1.set_center ( helix_center_1 );
      my_helix_1.set_step   ( 1. * CLHEP::m  );
      my_helix_1.set_t1     ( 0.0 );
      my_helix_1.set_t2     ( 2.0 );

      geomtools::vector_3d helix_center_2 ( 0., 0., 0. );
       
      my_helix_2.set_radius ( 0.5 * CLHEP::m );
      my_helix_2.set_center ( helix_center_2 );
      my_helix_2.set_step   ( -1. * CLHEP::m );
      my_helix_2.set_t1     ( 0.0 );
      my_helix_2.set_t2     ( 2.0 );

      my_helix_1.tree_dump  ( std::cerr, "helix #1:" );
      std::cerr << std::endl;

      my_helix_2.tree_dump  ( std::cerr, "helix #2:" );
      std::cerr << std::endl;

      if ( gnuplot )
	{
	  std::vector<double> x1, y1, z1;
	  for ( double t = my_helix_1.get_t1();
		t <= my_helix_1.get_t2(); 
		t += ( ( my_helix_1.get_t2() - my_helix_1.get_t1() ) / 1000. ) )
	    {
	      x1.push_back ( my_helix_1.get_point ( t ).x() );
	      y1.push_back ( my_helix_1.get_point ( t ).y() );
	      z1.push_back ( my_helix_1.get_point ( t ).z() );
	    }
	  
	  std::vector<double> x2, y2, z2;
	  for ( double t = my_helix_2.get_t1();
		t <= my_helix_2.get_t2(); 
		t += ( ( my_helix_2.get_t2() - my_helix_2.get_t1() ) / 1000. ) )
	    {
	      x2.push_back ( my_helix_2.get_point ( t ).x() );
	      y2.push_back ( my_helix_2.get_point ( t ).y() );
	      z2.push_back ( my_helix_2.get_point ( t ).z() );
	    }
	  
	  Gnuplot g1("lines");
	  g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
	  g1.set_grid();
	  g1.plot_xyz ( x1, y1, z1, "Helix1" );
	  g1.plot_xyz ( x2, y2, z2, "Helix2" );
	  wait_for_key();
	}
      else
	{
	  my_helix_1.print_xyz  ( std::cout, my_helix_1, 0.1, 0 );
	  
	  std::cout << std::endl;
	  std::cout << std::endl;
	  
	  my_helix_2.print_xyz  ( std::cout, my_helix_2, 0.1, 0 );
	}

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

// end of test_helix.cxx
