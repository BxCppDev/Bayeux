// -*- mode: c++; -*- 
// test_cylinder.cxx
// gnuplot macro: ./tests/test_cylinder.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/geomtools_config.h>
#include <geomtools/cylinder.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#endif

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 314159;
      bool test1 = true;
      bool test2 = true;
      bool test3 = true;

      int iarg = 1;
      while (iarg < argc_) 
        {
          std::string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-t1" || arg == "--test1") test1 = false;
          if (arg == "-t2" || arg == "--test2") test2 = false;
          if (arg == "-t3" || arg == "--test3") test3 = false;

          iarg++;
        }
    
      srand48 (seed);
    
      // test 1:
      if (test1) 
        {
          geomtools::cylinder my_cylinder (1000 * CLHEP::mm,
                                           230 * CLHEP::mm);

          std::clog << "test 1: Cylinder #1 = " << my_cylinder << " " << std::endl;

          std::clog << "test 1: Volume   = " << my_cylinder.get_volume () / CLHEP::m3 << " m3" << std::endl;

          std::clog << "test 1: Side surface = " 
                    << my_cylinder.get_surface (geomtools::cylinder::FACE_SIDE) / CLHEP::m2 
                    << " m2" << std::endl;

          std::clog << "test 1: Top surface = " 
                    << my_cylinder.get_surface (geomtools::cylinder::FACE_TOP) / CLHEP::m2
                    << " m2" << std::endl;

          std::clog << "test 1: Bottom surface = "
                    << my_cylinder.get_surface (geomtools::cylinder::FACE_BOTTOM) / CLHEP::m2 
                    << " m2" << std::endl;

          std::clog << "test 1: Full surface = " 
                    << my_cylinder.get_surface (geomtools::cylinder::FACE_ALL) / CLHEP::m2 
                    << " m2" << std::endl;

          {
            bool test_in = false;
            if (test_in)
              {
                std::clog << "test 1: Enter a cylinder (example: '{cylinder 1000 1000}'): ";
                std::cin >> std::ws >> my_cylinder;
                if (std::cin) 
                  {
                    std::clog << "test 1: Cylinder = " << my_cylinder << " " << std::endl;
                  }
                else 
                  {
                    throw std::runtime_error ("Invalid input for cylinder!");
                  }
              }
          }
        }

      // test 2:
      if (test2) 
        {
          geomtools::cylinder my_cylinder (3.0 * CLHEP::mm, 
                                           10.0 * CLHEP::mm);
          std::clog << "test 2: Cylinder #2   = " << my_cylinder << " " << std::endl;
      
          geomtools::vector_3d pos (4.0 * CLHEP::mm, 
                                    0.0 * CLHEP::mm, 
                                    0.0 * CLHEP::mm);
          geomtools::vector_3d dir (-1, 0, 0);
          geomtools::intercept_t intercept;
          int intercept_face;
          if (my_cylinder.find_intercept (pos, dir, 
                                          intercept))
            {
              std::clog << "test 2: Intercept face=" << intercept.get_face ()
                        << " at impact=" << intercept.get_impact ()
                        << std::endl;
            }
          else
            {
              std::clog << "test 2: No intercept." << std::endl;
            }

          geomtools::vector_3d cylinder_pos;
          geomtools::rotation cylinder_rot;
          geomtools::create_rotation (cylinder_rot, 0.0, 0.0, 0.0);
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          geomtools::gnuplot_draw::draw_cylinder (std::cout, cylinder_pos, cylinder_rot, 
                                                  my_cylinder.get_r (), 
                                                  my_cylinder.get_z ());
          std::cout << std::endl << std::endl;

          geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
          std::cout << std::endl << std::endl;

          geomtools::gnuplot_draw::basic_draw_point (std::cout, 
                                                     intercept.get_impact ());
          std::cout << std::endl << std::endl;

          geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
          geomtools::gnuplot_draw::basic_draw_point (std::cout, pos + dir);
          std::cout << std::endl << std::endl;
#endif
        }
 
      // test 3:
      if (test3) 
        {
          geomtools::cylinder my_cylinder (2.0 * CLHEP::mm, 
                                           4.0 * CLHEP::mm);
          std::clog << "test 3: Cylinder #3   = " << my_cylinder 
                    << " " << std::endl;
          geomtools::vector_3d cylinder_pos;
          geomtools::rotation cylinder_rot;
          geomtools::create_rotation (cylinder_rot, 0.0, 0.0, 0.0);
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          geomtools::gnuplot_draw::draw_cylinder (std::cout, 
                                                  cylinder_pos, 
                                                  cylinder_rot, 
                                                  my_cylinder.get_r (), 
                                                  my_cylinder.get_z ());
          std::cout << std::endl << std::endl;
#endif  
          size_t nshoots = 100000;
          for (int i = 0; i < (int) nshoots; i++)
            {
              double dim = 4. * CLHEP::mm;
              //dim = 0. * CLHEP::mm;
              geomtools::vector_3d pos (dim * drand48 (),
                                        dim * drand48 (),
                                        dim * drand48 ());
              //pos.setZ (3. * CLHEP::mm);
              geomtools::vector_3d dir;
              geomtools::randomize_direction (drand48, dir);

              geomtools::intercept_t intercept;
              if (my_cylinder.find_intercept (pos, dir, 
                                              intercept))
                {
                  if (debug) std::clog << "test 3: Intercept face=" 
                                       << intercept.get_face ()
                                       << " at impact=" << intercept.get_impact ()
                                       << std::endl;
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
                  geomtools::gnuplot_draw::basic_draw_point (std::cout, 
                                                             intercept.get_impact ());
#endif
                }
              else
                {
                  if (debug) std::clog << "test 3: No intercept." << std::endl;
                }
            }
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

// end of test_cylinder.cxx
