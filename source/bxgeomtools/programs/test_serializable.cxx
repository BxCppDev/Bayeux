// -*- mode: c++; -*- 
// test_serializable.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/geom_id.h>
#include <geomtools/geomtools_version.h>
#include <datatools/utils/properties.h>

#include <datatools/serialization/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/serialization/bio_guard.h>
#include <geomtools/serialization/bio_guard.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      geomtools::vector_2d pos2d;
      pos2d.set (1., 2.);
      std::clog << "Vector 2D: " << pos2d << std::endl;

      geomtools::vector_3d pos;
      pos.set (1., 2., 3.);
      std::clog << "Vector 3D: " << pos << std::endl;

      geomtools::rotation rot;
      geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
      geomtools::tree_dump (rot, std::clog, "Rotation matrix 3D:");
      clog << endl;

      {
	namespace ds = datatools::serialization;
	ds::data_writer writer;
	writer.init ("test_utils.txt", 
		     ds::using_multi_archives);
	datatools::utils::properties config;
	config.store_flag ("test");
	config.tree_dump (clog, "Config :");
	writer.store (config);
	for (int i = 0; i < 5; i++)
	  {
	    geomtools::vector_2d v (geomtools::random_tools::random_flat (),
				    geomtools::random_tools::random_flat ());
	    if (i == 3) geomtools::invalidate (v);
	    std::clog << "v[" << i << "] = " << v << std::endl;
	    writer.store (geomtools::io::VECTOR_2D_SERIAL_TAG, v);
	  }
	writer.reset ();
	clog << endl;
      }

      {
	namespace ds = datatools::serialization;
	ds::data_writer writer;
	writer.init ("test_utils_3d.txt", 
		     ds::using_multi_archives);
	
	datatools::utils::properties config;
	config.store_flag ("test");
	config.tree_dump (clog, "Config :");
	writer.store (config);
	for (int i = 0; i < 5; i++)
	  {
	    geomtools::vector_3d v (geomtools::random_tools::random_flat (),
				    geomtools::random_tools::random_flat (),
				    geomtools::random_tools::random_flat ());
	    if (i == 3) geomtools::invalidate (v);
	    std::clog << "v[" << i << "] = " << v << std::endl;
	    writer.store (geomtools::io::VECTOR_3D_SERIAL_TAG, v);
	  }
	writer.reset ();
	clog << endl;
      }

      {
	namespace ds = datatools::serialization;
	ds::data_writer writer;
	writer.init ("test_utils_gid.txt", 
		     ds::using_multi_archives);
	
	datatools::utils::properties config;
	config.store_flag ("test");
	config.store ("library", "geomtools");
	config.store ("version", GEOMTOOLS_LIB_VERSION);
	config.store ("executable", "test_serializable");
	config.tree_dump (clog, "Config :");
	writer.store (config);
	for (int i = 0; i < 5; i++)
	  {
	    geomtools::geom_id gid (666, i);
	    if (i == 3) gid.invalidate ();
	    std::clog << "gid[" << i << "] = " << gid << std::endl;
	    writer.store (geomtools::geom_id::SERIAL_TAG, gid);
	  }
	writer.reset ();
	clog << endl;
      }
        
      {
	namespace ds = datatools::serialization;
	ds::data_reader reader;
	reader.init ("test_utils_gid.txt", 
		     ds::using_multi_archives);
	datatools::utils::properties config;
	reader.load (config);
	config.tree_dump (clog, "Config (LOADED) :");
	int i = 0;
	while (reader.has_record_tag ())
	  {
	    if (reader.get_record_tag () == geomtools::geom_id::SERIAL_TAG)
	      {
		geomtools::geom_id gid;
		reader.load (gid);
		std::clog << "gid[" << i << "] (LOADED) = " << gid << std::endl;
	      }
	    i++;
	  }
	reader.reset ();
 	clog << endl;
      }
        
      {
	namespace ds = datatools::serialization;
	ds::data_reader reader;
	reader.init ("test_utils.txt", 
		     ds::using_multi_archives);
	datatools::utils::properties config;
	reader.load (config);
	config.tree_dump (clog, "Config (LOADED) :");
	int i = 0;
	while (reader.has_record_tag ())
	  {
	    if (reader.get_record_tag () == geomtools::io::VECTOR_2D_SERIAL_TAG)
	      {
		geomtools::vector_2d v;
		reader.load (geomtools::io::VECTOR_2D_SERIAL_TAG, v);
		std::clog << "v[" << i << "] (LOADED) = " << v << std::endl;
	      }
	    i++;
	  }
	reader.reset ();
 	clog << endl;
     }
       
      {
	namespace ds = datatools::serialization;
	ds::data_reader reader;
	reader.init ("test_utils_3d.txt", 
		     ds::using_multi_archives);
	datatools::utils::properties config;
	reader.load (config);
	config.tree_dump (clog, "Config (LOADED) :");
	int i = 0;
	while (reader.has_record_tag ())
	  {
	    if (reader.get_record_tag () == geomtools::io::VECTOR_3D_SERIAL_TAG)
	      {
		geomtools::vector_3d v;
		reader.load (geomtools::io::VECTOR_3D_SERIAL_TAG, v);
		std::clog << "v[" << i << "] (LOADED) = " << v << std::endl;
	      }
	    i++;
	  }
	reader.reset ();
	clog << endl;
      }

      {
	namespace ds = datatools::serialization;
	ds::data_writer writer;
	writer.init ("test_utils2.txt", 
		     ds::using_multi_archives);
	
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
	    std::clog << "r[" << i << "] = " << r << std::endl;
	    geomtools::tree_dump (r, 
				  std::clog, 
				  "Rotation matrix (saved):");
	    writer.store (geomtools::io::ROTATION_3D_SERIAL_TAG, r);
	  }
	writer.reset ();
      }

      {
	namespace ds = datatools::serialization;
	ds::data_reader reader;
	reader.init ("test_utils2.txt", 
		     ds::using_multi_archives);
	int i = 0;
	while (reader.has_record_tag ())
	  {
	    if (reader.get_record_tag () == geomtools::io::ROTATION_3D_SERIAL_TAG)
	      {
		geomtools::rotation r;
		reader.load (geomtools::io::ROTATION_3D_SERIAL_TAG, r);
		geomtools::tree_dump (r, 
				      std::clog, 
				      "Rotation matrix (loaded):");
		std::clog << std::endl;
	      }
	    i++;
	  }
	reader.reset ();
	clog << endl;
      }

      geomtools::create_rotation (rot, 
				  30.0 * CLHEP::degree, 
				  45. * CLHEP::degree,
				  0.0);
      geomtools::tree_dump (rot, std::clog, "Rotation matrix 3D (valid):"); 
      geomtools::invalidate (rot);
      geomtools::tree_dump (rot, std::clog, "Rotation matrix 3D (invalid):");
      clog << endl;
 
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_serializable.cxx
