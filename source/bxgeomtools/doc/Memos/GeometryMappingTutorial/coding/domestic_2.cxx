// -*- mode: c++ ; -*- 
// domestic_2.cxx

#include <cstdlib>
#include <iostream>
#include <exception>

#include <geomtools/id_mgr.h>

int main (void)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      geomtools::id_mgr the_gid_manager;
      the_gid_manager.load ("domestic_categories.lis");

      // Construction of the GID of a 'room' object :
      geomtools::geom_id the_room_gid;
      if (the_gid_manager.has_category_info ("room"))
       { 
	 // Create a 'room' object from scratch :
	 the_gid_manager.make_id ("room", the_room_gid);
	 // Allocate the addresses path values at all depths :
	 the_gid_manager.set (the_room_gid, "house_number", 666);
	 the_gid_manager.set (the_room_gid, "floor_number", 0);
	 the_gid_manager.set (the_room_gid, "room_number", 1);
      }

      // Extraction of the GID of the 'floor' mother object : 
      geomtools::geom_id the_floor_gid;
      if (the_gid_manager.inherits (the_room_gid, "floor"))
	{
	  // Create a mother object from scratch :
	  the_gid_manager.make_id ("floor", the_floor_gid);
	  // Extract the addresses path of the mother object,
	  // removing the trailing "room_number" value:
	  the_gid_manager.extract (the_room_gid, the_floor_gid);
	}


      // Construction of the GID of a 'chair' daughter object : 
      geomtools::geom_id the_chair_gid;
      the_gid_manager.make_id ("chair", the_chair_gid);
      if (the_gid_manager.inherits (the_chair_gid, "room"))
	{
	  // Inherits the addresses path of the mother :
	  the_chair_gid.inherits_from (the_room_gid);
	  // Appends an additionnal "chair_number" value 
	  // to the addresses path :
	  the_gid_manager.set (the_chair_gid, "chair_number", 6);
	}

      std::clog << "Reference GID = " << the_room_gid   
		<< " in category '" 
		<< the_gid_manager.get_category (the_room_gid) 
		<< "'" << std::endl;
      std::clog << "Mother GID    = " << the_floor_gid   
		<< " in category '" 
		<< the_gid_manager.get_category (the_floor_gid) 
		<< "'" << std::endl;
      std::clog << "Daughter GID  = " << the_chair_gid   
		<< " in category '" 
		<< the_gid_manager.get_category (the_chair_gid) 
		<< "'" << std::endl;	  

    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of domestic_2.cxx
