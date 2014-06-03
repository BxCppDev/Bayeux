// -*- mode: c++ ; -*-
// test_base_hit.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <geomtools/base_hit.h>

#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/smart_ref.h>

#include <datatools/io_factory.h>
#include <geomtools/base_hit.ipp>

using namespace std;

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'geomtools::base_hit'!" << endl;

      // bool debug = false;

      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];

      //     if (token[0] == '-')
      //       {
      //          string option = token;
      //          if ((option == "-d") || (option == "--debug"))
      //            {
      //              debug = true;
      //            }
      //          else
      //            {
      //               clog << "warning: ignoring option '" << option << "'!" << endl;
      //            }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         {
      //           clog << "warning: ignoring argument '"
      //                << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      // }

      clog << "Step 0:" << endl;
      {
        geomtools::base_hit my_base_hit;
        my_base_hit.set_hit_id (245);
        geomtools::geom_id gid (10, 1, 1, 4);
        my_base_hit.set_geom_id (gid);
        my_base_hit.grab_auxiliaries ().store ("time", 896.0 * CLHEP::picosecond);
        my_base_hit.grab_auxiliaries ().store ("event_id", "666_34");
        my_base_hit.grab_auxiliaries ().store_flag ("suspicous");
        my_base_hit.tree_dump (clog, "Base hit");
      }

      typedef vector<geomtools::base_hit> base_hit_col_type;

      clog << "Step 1:" << endl;
      {
        // Serialization:
        datatools::data_writer DW ("test_base_hit.xml",
                                   datatools::using_multi_archives);

        // create a list of random base hits:
        srand48 (314159);
        srandom (314159);
        base_hit_col_type list_of_base_hits;
        size_t nb_hits = 10;
        //double cell_radius = 15.0 * CLHEP::mm;
        for (int i = 0; i < (int) nb_hits; i++)
          {
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
            if (drand48 () < 0.3)
              {
                my_base_hit.grab_auxiliaries ().store_flag ("noisy");
              }

            // a trick for test:
            if (i == (nb_hits / 2))
              {
                // Do not store auxiliaries properties for this hit...
                my_base_hit.invalidate_auxiliaries ();
              }
            if (i == (nb_hits / 3))
              {
                // Do not store geometry ID for this hit...
                my_base_hit.invalidate_geom_id ();
              }
            DW.store (my_base_hit);
          }
        clog << "The list of base hits has been stored in a Boost serialization XML file." << endl;

        {
          geomtools::base_hit::has_hit_id_predicate hit_id_pred (5717);
          base_hit_col_type::const_iterator found
            = std::find_if (list_of_base_hits.begin (),
                            list_of_base_hits.end (),
                            hit_id_pred);
          if (found != list_of_base_hits.end ())
            {
              found->tree_dump (std::clog, "Found hit #5717 :", "*** ");
            }
        }
        {
          geomtools::base_hit::has_hit_id_predicate hit_id_pred (666);
          base_hit_col_type::const_iterator found
            = std::find_if (list_of_base_hits.begin (),
                            list_of_base_hits.end (),
                            hit_id_pred);
          if (found == list_of_base_hits.end ())
            {
              std::clog << "Hit #666 not found...\n";
            }
        }

      }

      clog << "Step 2:" << endl;
      {
        // Serialization:
        datatools::data_reader DR ("test_base_hit.xml",
                                   datatools::using_multi_archives);

        // Load the list of base hits from Boost serialization XML file:
        base_hit_col_type list_of_base_hits;
        while (DR.has_record_tag ())
          {
            if (DR.record_tag_is (geomtools::base_hit::SERIAL_TAG))
              {
                {
                  geomtools::base_hit BH;
                  list_of_base_hits.push_back (BH);
                }
                geomtools::base_hit & base_hit = list_of_base_hits.back ();
                DR.load (base_hit);
                base_hit.tree_dump (clog, "A new base hit loaded from the reader:");
              }
            else
              {
                string bad_tag = DR.get_record_tag ();
                clog << "ERROR: unknown data tag '"
                     << bad_tag << "'!" << endl;
                break;
              }
          }
        clog << "The list of base hits has been loaded from a Boost serialization XML file." << endl;


        typedef datatools::smart_ref<geomtools::base_hit> hit_smart_ref_t;
        clog << "Step 2': search for hits with min anode drift time" << endl;
        // search for hits with min/max anode  drift radius:
        hit_smart_ref_t tmin_base_ref;
        for (base_hit_col_type::const_iterator i = list_of_base_hits.begin ();
             i != list_of_base_hits.end ();
             i++)
          {
            const geomtools::base_hit & base_hit = *i;
            if (base_hit.get_auxiliaries ().has_key ("drift_time"))
              {
                // only consider hits with a stored 'time' auxiliary property:
                if (! tmin_base_ref.is_valid ())
                  {
                    tmin_base_ref.set (base_hit);
                  }
                else if (base_hit.get_auxiliaries ().fetch_real ("drift_time")
                         < tmin_base_ref.get ().get_auxiliaries ().fetch_real ("drift_time"))
                  {
                    tmin_base_ref.set (base_hit);
                  }
            }
          }
        tmin_base_ref.get ().tree_dump (clog, "Base hit with min drift time:");
      }

      clog << "DEVEL: The End." << endl;
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

// end of test_base_hit.cxx
