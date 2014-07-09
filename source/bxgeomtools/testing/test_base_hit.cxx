// test_base_hit.cxx

// Ourselves:
#include <geomtools/base_hit.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>
#include <datatools/io_factory.h>

// This project:
#include <geomtools/base_hit.ipp>

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::base_hit'!" << std::endl;

    // bool debug = false;

    // int iarg = 1;
    // while (iarg < argc_)
    //   {
    //     std::string token = argv_[iarg];

    //     if (token[0] == '-')
    //       {
    //          std::string option = token;
    //          if ((option == "-d") || (option == "--debug"))
    //            {
    //              debug = true;
    //            }
    //          else
    //            {
    //               std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
    //            }
    //       }
    //     else
    //       {
    //         std::string argument = token;
    //         {
    //           std::clog << "warning: ignoring argument '"
    //                << argument << "'!" << std::endl;
    //         }
    //       }
    //     iarg++;
    // }

    std::clog << "Step 0:" << std::endl;
    {
      geomtools::base_hit my_base_hit;
      my_base_hit.set_hit_id (245);
      geomtools::geom_id gid (10, 1, 1, 4);
      my_base_hit.set_geom_id (gid);
      my_base_hit.grab_auxiliaries ().store ("time", 896.0 * CLHEP::picosecond);
      my_base_hit.grab_auxiliaries ().store ("event_id", "666_34");
      my_base_hit.grab_auxiliaries ().store_flag ("suspicous");
      my_base_hit.tree_dump (std::clog, "Base hit");
    }

    typedef std::vector<geomtools::base_hit> base_hit_col_type;

    std::clog << "Step 1:" << std::endl;
    {
      // Serialization:
      datatools::data_writer DW ("test_base_hit.xml",
                                 datatools::using_multi_archives);

      // create a list of random base hits:
      srand48 (314159);
      srandom (314159);
      base_hit_col_type list_of_base_hits;
      int nb_hits = 10;
      //double cell_radius = 15.0 * CLHEP::mm;
      for (int i = 0; i < (int) nb_hits; i++) {
        // append a new empty hit in the list:
        list_of_base_hits.push_back (geomtools::base_hit ());
        // get a reference to the newly pushed hit:
        geomtools::base_hit & my_base_hit = list_of_base_hits.back ();
        my_base_hit.set_hit_id (5715 + i);
        geomtools::geom_id gid (10, // module
                                1,  // half chamber
                                (random () % 10), // layer
                                4 + (random () % 3)); // cell
        my_base_hit.set_geom_id (gid);
        my_base_hit.grab_auxiliaries ().store ("strange", "");
        double t = drand48 () * 5000. * CLHEP::picosecond;
        my_base_hit.grab_auxiliaries ().store ("drift_time", t);
        if (drand48 () < 0.3) {
          my_base_hit.grab_auxiliaries ().store_flag ("noisy");
        }

        // a trick for test:
        if (i == (nb_hits / 2)) {
          // Do not store auxiliaries properties for this hit...
          my_base_hit.invalidate_auxiliaries ();
        }
        if (i == (nb_hits / 3)) {
          // Do not store geometry ID for this hit...
          my_base_hit.invalidate_geom_id ();
        }
        DW.store (my_base_hit);
      }
      std::clog << "The list of base hits has been stored in a Boost serialization XML file." << std::endl;

      {
        geomtools::base_hit::has_hit_id_predicate hit_id_pred (5717);
        base_hit_col_type::const_iterator found
          = std::find_if (list_of_base_hits.begin (),
                          list_of_base_hits.end (),
                          hit_id_pred);
        if (found != list_of_base_hits.end ()) {
          found->tree_dump (std::clog, "Found hit #5717 :", "*** ");
        }
      }
      {
        geomtools::base_hit::has_hit_id_predicate hit_id_pred (666);
        base_hit_col_type::const_iterator found
          = std::find_if (list_of_base_hits.begin (),
                          list_of_base_hits.end (),
                          hit_id_pred);
        if (found == list_of_base_hits.end ()) {
          std::clog << "Hit #666 not found...\n";
        }
      }

    }

    std::clog << "Step 2:" << std::endl;
    {
      // Serialization:
      datatools::data_reader DR ("test_base_hit.xml",
                                 datatools::using_multi_archives);

      // Load the list of base hits from Boost serialization XML file:
      base_hit_col_type list_of_base_hits;
      while (DR.has_record_tag ()) {
        if (DR.record_tag_is (geomtools::base_hit::SERIAL_TAG)) {
          {
            geomtools::base_hit BH;
            list_of_base_hits.push_back (BH);
          }
          geomtools::base_hit & base_hit = list_of_base_hits.back ();
          DR.load (base_hit);
          base_hit.tree_dump (std::clog, "A new base hit loaded from the reader:");
        } else {
          std::string bad_tag = DR.get_record_tag ();
          std::clog << "ERROR: unknown data tag '"
                    << bad_tag << "'!" << std::endl;
          break;
        }
      }
      std::clog << "The list of base hits has been loaded from a Boost serialization XML file." << std::endl;


      typedef datatools::smart_ref<geomtools::base_hit> hit_smart_ref_t;
      std::clog << "Step 2': search for hits with min anode drift time" << std::endl;
      // search for hits with min/max anode  drift radius:
      hit_smart_ref_t tmin_base_ref;
      for (base_hit_col_type::const_iterator i = list_of_base_hits.begin ();
           i != list_of_base_hits.end ();
           i++) {
        const geomtools::base_hit & base_hit = *i;
        if (base_hit.get_auxiliaries ().has_key ("drift_time")) {
          // only consider hits with a stored 'time' auxiliary property:
          if (! tmin_base_ref.is_valid ()) {
            tmin_base_ref.set (base_hit);
          } else if (base_hit.get_auxiliaries ().fetch_real ("drift_time")
                     < tmin_base_ref.get ().get_auxiliaries ().fetch_real ("drift_time")) {
            tmin_base_ref.set (base_hit);
          }
        }
      }
      tmin_base_ref.get ().tree_dump (std::clog, "Base hit with min drift time:");
    }

    unlink ("test_base_hit.xml");

    std::clog << "DEVEL: The End." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
