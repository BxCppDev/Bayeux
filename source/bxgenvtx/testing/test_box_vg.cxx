// -*- mode: c++ ; -*-
// test_box_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <genvtx/box_vg.h>
#include <mygsl/rng.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'box_vg'!" << endl;

      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token;
               if ((option == "-d") || (option == "--debug"))
                 {
                   debug = true;
                 }
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
               {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
      }

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);

      size_t nshoots = 1000;

      geomtools::box b (4., 2., 3.);

      {
        geomtools::vector_3d pos;
        geomtools::rotation_3d rot;
        geomtools::gnuplot_draw::draw_box (cout, pos, rot, b);
        cout << endl << endl;
      }

      genvtx::box_vg vg;
      vg.set_debug (debug);

      bool surface = true;
      if (surface)
      {
        vg.set_box (b);
        vg.set_mode (genvtx::box_vg::MODE_SURFACE);
        clog << "warning: mode = " <<  vg.get_mode() << "\n";
        int surface_mask = 0;
        //surface_mask |= geomtools::box::FACE_BACK ;
        //surface_mask |= geomtools::box::FACE_FRONT;
        //surface_mask |= geomtools::box::FACE_LEFT;
        surface_mask |= geomtools::box::FACE_RIGHT;
        surface_mask |= geomtools::box::FACE_BOTTOM;
        //surface_mask |= geomtools::box::FACE_TOP;
        vg.set_surface_mask (surface_mask);
        vg.set_skin_skip (0.40);
        vg.set_skin_thickness (0.20);
        vg.initialize_simple ();
        vg.tree_dump (clog, "Box vertex generator (bottom+right surface, skipped)");
        // size_t nshoots = 20;
        geomtools::vector_3d vertex;
        for (int i = 0; i < nshoots; i++)
          {
            vg.shoot_vertex (random, vertex);
            geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
          }
        cout << endl << endl;
      }

      vg.reset ();

      bool bulk = true;
      //bulk = false;
      if (bulk)
      {
        vg.set_box (b);
        vg.set_mode (genvtx::box_vg::MODE_BULK);
        clog << "warning: mode = " <<  vg.get_mode()<< "\n";
        vg.set_skin_skip (0.0);
        vg.set_skin_thickness (0.30);
        vg.initialize_simple ();
        vg.tree_dump (clog, "Box vertex generator (bulk)");
        //size_t nshoots = 50;
        geomtools::vector_3d vertex;
        for (int i = 0; i < nshoots; i++)
          {
            vg.shoot_vertex (random, vertex);
            geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
          }
        cout << endl << endl;
      }

      bool other = true;
      //other = false;
      if (other)
      {
        genvtx::i_vertex_generator * ivg = 0;
        datatools::properties config;
        config.store ("mode", "surface");
        config.store ("skin_skip",      0.05);
        config.store ("skin_thickness", 0.1);
        vector<string> surfaces;
        surfaces.push_back ("left");
        surfaces.push_back ("back");
        surfaces.push_back ("top");
        config.store ("surfaces", surfaces);
        {
          config.store ("box.x", b.get_x ());
          config.store ("box.y", b.get_y ());
          config.store ("box.z", b.get_z ());
          config.store ("length_unit", "mm");
        }
        config.tree_dump (clog, "Configuration for dynamic VG:");
        const genvtx::i_vertex_generator::factory_register_type & the_vg_factory
          = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (genvtx::i_vertex_generator);

        ivg = the_vg_factory.get ("genvtx::box_vg")(); // new genvtx::box_vg;
        ivg->initialize_standalone (config);
        //size_t nshoots = 10;
        geomtools::vector_3d vertex;
        for (int i = 0; i < nshoots; i++)
          {
            ivg->shoot_vertex (random, vertex);
            geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
          }
        cout << endl << endl;
        if (ivg != 0) delete ivg;
      }

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
  return (error_code);
}

// end of test_box_vg.cxx
