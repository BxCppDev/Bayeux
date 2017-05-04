// -*- mode: c++ ; -*- 
// domestic_3.cxx

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

      if (the_gid_manager.has_category_info ("room"))
       { 
	 // Construction of the GID of a 'room' object 
	 // using the corresponding 'category_info' stored 
	 // in the lookup table.
	 const geomtools::id_mgr::category_info & category_room_info
	    = the_gid_manager.get_category_info ("room");

	 // Get the type associated to the "room" category :
	 int room_type = category_room_info.get_type ();

	 // Get the indexes of various subaddresses that
	 // compose the addresses path :
	 int house_number_index = 
           category_room_info.get_subaddress_index ("house_number");
	 int floor_number_index = 
           category_room_info.get_subaddress_index ("floor_number");
	 int room_number_index = 
           category_room_info.get_subaddress_index ("room_number");

	 // Massive generation of GIDs of the "room" category 
	 // for 3 houses, each with 2 floors with 4 rooms per floor :
	 for (int house_number = 666; house_number < 669; house_number++)
	   {
	     for (int floor_number = 0; floor_number < 2; floor_number++)
	       {
		 for (int room_number = 0; room_number < 4; room_number++)
		   {
		     // Instantiate a GID :
		     geomtools::geom_id the_room_gid;

		     // Set the type of the GID :
		     the_room_gid.set_type (room_type);

		     // Allocate the addresses path values at all depths :
		     the_room_gid.set (house_number_index, house_number);
		     the_room_gid.set (floor_number_index, floor_number);
		     the_room_gid.set (room_number_index, room_number);
		     
		     std::clog << "Room has GID = " << the_room_gid  
			       << std::endl;
		   }
	       }
	   }
      }

    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of domestic_3.cxx
