// -*- mode: c++; -*-
// test_serializable.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/geom_id.h>
#include <geomtools/version.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>

#include <datatools/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <geomtools/bio_guard.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  cerr << "Program starts..." << endl;
  int error_code = EXIT_SUCCESS;
  try
    {

      geomtools::vector_2d pos2d;
      pos2d.set (1., 2.);
      cerr << "Vector 2D: " << pos2d << endl;

      geomtools::vector_3d pos;
      pos.set (1., 2., 3.);
      cerr << "Vector 3D: " << pos << endl;

      geomtools::rotation rot;
      geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
      geomtools::tree_dump (rot, cerr, "Rotation matrix 3D:");
      cerr << endl;

      {
        datatools::data_writer writer;
        writer.init ("test_utils.txt",
                     datatools::using_multi_archives);
        datatools::properties config;
        config.store_flag ("test");
        config.tree_dump (cerr, "Config :");
        writer.store (config);
        for (int i = 0; i < 5; i++)
          {
            geomtools::vector_2d v (geomtools::random_tools::random_flat (),
                                    geomtools::random_tools::random_flat ());
            if (i == 3) geomtools::invalidate (v);
            cerr << "v[" << i << "] = " << v << endl;
            writer.store (geomtools::io::vector_2d_serial_tag(), v);
          }
        /*
        datatools::multi_properties mconfig;
        writer.store (mconfig);
        writer.reset ();
        */
        cerr << endl;
      }

      {
        datatools::data_writer writer;
        writer.init ("test_utils_3d.txt",
                     datatools::using_multi_archives);

        datatools::properties config;
        config.store_flag ("test");
        config.tree_dump (cerr, "Config :");
        writer.store (config);
        for (int i = 0; i < 5; i++)
          {
            geomtools::vector_3d v (geomtools::random_tools::random_flat (),
                                    geomtools::random_tools::random_flat (),
                                    geomtools::random_tools::random_flat ());
            if (i == 3) geomtools::invalidate (v);
            cerr << "v[" << i << "] = " << v << endl;
            writer.store (geomtools::io::vector_3d_serial_tag(), v);
          }
        writer.reset ();
        cerr << endl;
      }

      {
        datatools::data_writer writer;
        writer.init ("test_utils_gid.txt",
                     datatools::using_multi_archives);

        datatools::properties config;
        config.store_flag ("test");
        config.store ("library", "geomtools");
        config.store ("version", GEOMTOOLS_LIB_VERSION);
        config.store ("executable", "test_serializable");
        config.tree_dump (cerr, "Config :");
        writer.store (config);
        for (int i = 0; i < 5; i++)
          {
            geomtools::geom_id gid (666, i);
            if (i == 3) gid.invalidate ();
            cerr << "gid[" << i << "] = " << gid << endl;
            writer.store (geomtools::geom_id::serial_tag(), gid);
          }
        writer.reset ();
        cerr << endl;
      }

      {
        datatools::data_reader reader;
        reader.init ("test_utils_gid.txt",
                     datatools::using_multi_archives);
        datatools::properties config;
        reader.load (config);
        config.tree_dump (cerr, "Config (LOADED) :");
        int i = 0;
        while (reader.has_record_tag ())
          {
            if (reader.get_record_tag () == geomtools::geom_id::SERIAL_TAG)
              {
                geomtools::geom_id gid;
                reader.load (gid);
                cerr << "gid[" << i << "] (LOADED) = " << gid << endl;
              }
            i++;
          }
        reader.reset ();
        cerr << endl;
      }

      {
        datatools::data_reader reader;
        reader.init ("test_utils.txt",
                     datatools::using_multi_archives);
        datatools::properties config;
        reader.load (config);
        config.tree_dump (cerr, "Config (LOADED) :");
        int i = 0;
        while (reader.has_record_tag ())
          {
            if (reader.get_record_tag () == geomtools::io::vector_2d_serial_tag())
              {
                geomtools::vector_2d v;
                reader.load (geomtools::io::vector_2d_serial_tag(), v);
                cerr << "v[" << i << "] (LOADED) = " << v << endl;
              }
            else
              {
                break;
              }
            i++;
          }
        /*
        datatools::multi_properties mconfig;
        reader.load (mconfig);
        reader.reset ();
        */
        cerr << endl;
     }

      {
        datatools::data_reader reader;
        reader.init ("test_utils_3d.txt",
                     datatools::using_multi_archives);
        datatools::properties config;
        reader.load (config);
        config.tree_dump (cerr, "Config (LOADED) :");
        int i = 0;
        while (reader.has_record_tag ())
          {
            if (reader.get_record_tag () == geomtools::io::vector_3d_serial_tag())
              {
                geomtools::vector_3d v;
                reader.load (geomtools::io::vector_3d_serial_tag(), v);
                cerr << "v[" << i << "] (LOADED) = " << v << endl;
              }
            i++;
          }
        reader.reset ();
        cerr << endl;
      }

      {
        datatools::data_writer writer;
        writer.init ("test_utils2.txt",
                     datatools::using_multi_archives);

        for (int i = 0; i < 4; i++)
          {
            geomtools::rotation r;
            if (i == 3)
              {
                geomtools::invalidate (r);
              }
            else
              {
                geomtools::create_rotation (r,
                                            2. * M_PI * geomtools::random_tools::random_flat (),
                                            0.25 * M_PI * geomtools::random_tools::random_flat (),
                                            0.0);
              }
            cerr << "r[" << i << "] = " << r << endl;
            geomtools::tree_dump (r,
                                  cerr,
                                  "Rotation matrix (saved):");
            writer.store (geomtools::io::rotation_3d_serial_tag(), r);
          }
        writer.reset ();
      }

      {
        datatools::data_reader reader;
        reader.init ("test_utils2.txt",
                     datatools::using_multi_archives);
        int i = 0;
        while (reader.has_record_tag ())
          {
            if (reader.get_record_tag () == geomtools::io::rotation_3d_serial_tag())
              {
                geomtools::rotation r;
                reader.load (geomtools::io::rotation_3d_serial_tag(), r);
                geomtools::tree_dump (r,
                                      cerr,
                                      "Rotation matrix (loaded):");
                cerr << endl;
              }
            i++;
          }
        reader.reset ();
        cerr << endl;
      }

      geomtools::create_rotation (rot,
                                  30.0 * CLHEP::degree,
                                  45. * CLHEP::degree,
                                  0.0);
      geomtools::tree_dump (rot, cerr, "Rotation matrix 3D (valid):");
      geomtools::invalidate (rot);
      geomtools::tree_dump (rot, cerr, "Rotation matrix 3D (invalid):");
      cerr << endl;

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_serializable.cxx
