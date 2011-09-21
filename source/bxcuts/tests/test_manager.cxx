// -*- mode: c++ ; -*- 
// cuts::test_manager.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <list>
#include <map>

#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h> 
 
#include <cuts/cut_manager.h>
#include <cuts_test_data.h>

// Additional registered test cuts :
#include <cuts_test_range_cut.cc>
#include <cuts_test_sphere_cut.cc>
#include <cuts_test_color_cut.cc>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'cuts::cut_manager' !" << endl; 
  
      bool debug = false;
      long seed = 314159;
      string config_filename = "";
      string cut_name = "";

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
                  clog << "warning: ignoring option '" 
                       << option << "'!" << endl; 
                }
            }
          else
            {
              string argument = token; 
              if (config_filename.empty ())
                { 
                  config_filename = argument;
                }
              else if (cut_name.empty ())
                { 
                  cut_name = argument;
                }
              else
                { 
                  clog << "warning: ignoring argument '" 
                       << argument << "'!" << endl; 
                }
            }
          iarg++;
        }
    
      srand48 (seed);

      cuts::cut_manager my_cut_manager;

      datatools::utils::properties cut_manager_config;

      if (config_filename.empty ())
        { 
          config_filename = "${CUTS_ROOT}/tests/config/test_cut_manager.conf";
        }
      datatools::utils::fetch_path_with_env (config_filename);
      datatools::utils::properties::read_config (config_filename,
                                                 cut_manager_config);

      if (cut_name.empty ())
        { 
          cut_name = "selector";
        }    
      clog << "NOTICE: Cut name is '" << cut_name << "'" << endl; 

      my_cut_manager.set_debug (debug);
      my_cut_manager.initialize (cut_manager_config);
      cuts::i_cut * the_cut = 0;
      if (! my_cut_manager.has (cut_name))
        {
          ostringstream message;
          message << "No cut with name '" << cut_name << "' !";
          throw logic_error (message.str ());    
        }
      the_cut = &my_cut_manager.get (cut_name);

      // build a collection of points:
      list<cuts::test::data> points;
      map<string,ostringstream *> ss0;
      for (int i = 0; i < 100000; i++)
        {
          cuts::test::data dummy;
          points.push_back (dummy);
          cuts::test::data & d = points.back ();
          d.x = -1.0 + 2.0 * drand48 ();
          d.y = -1.0 + 2.0 * drand48 ();
          d.z = -1.0 + 2.0 * drand48 ();
          d.color = (int) (3.5 * drand48 ());
          ostringstream * pss = 0;
          if (d.color == cuts::test::data::BLACK)
            {
              if (ss0.find ("black") == ss0.end ()) 
                {
                  ss0["black"] = new ostringstream ();
                }
              pss = ss0["black"];
            }
          else if (d.color == cuts::test::data::RED)
            {
              if (ss0.find ("red") == ss0.end ()) 
                {
                  ss0["red"] = new ostringstream ();
                }
              pss = ss0["red"];
            }
          else if (d.color == cuts::test::data::GREEN)
            {
              if (ss0.find ("green") == ss0.end ()) 
                {
                  ss0["green"] = new ostringstream ();
                }
              pss = ss0["green"];
            }
          else if (d.color == cuts::test::data::BLUE)
            {
              if (ss0.find ("blue") == ss0.end ()) 
                {
                  ss0["blue"] = new ostringstream ();
                }
              pss = ss0["blue"];
            }
          if (pss != 0) 
            {
              d.print (*pss);
            }
        }
      
      {
        ofstream forig ("test_manager.orig.data");
        // print original points in colored streams:
        for (map<string,ostringstream *>::const_iterator i = ss0.begin ();
             i != ss0.end ();
             i++)
          {
            forig << "# " << i->first << endl;
            forig << i->second->str ();
            forig << endl;
            forig << endl;
            delete i->second;
          }
      }

      // apply the selection:
      map<string,ostringstream *> ss;
      ss["black"] = new ostringstream ();
      ss["red"]   = new ostringstream ();
      ss["green"] = new ostringstream ();
      ss["blue"]  = new ostringstream ();
      bool has_black = false;
      bool has_red = false;
      bool has_green = false;
      bool has_blue = false;
      clog << "NOTICE: Selection... " << endl;
      for (list<cuts::test::data>::iterator i = points.begin ();
           i != points.end ();
           i++)
        {
          the_cut->set_user_data (&*i);
          int status = the_cut->process ();
          if (status == cuts::i_cut::ACCEPTED)
            {
              ostringstream * pss = 0;
              if (i->color == cuts::test::data::BLACK)
                {
                  if (ss.find ("black") == ss.end ()) 
                    {
                      ss["black"] = new ostringstream ();
                    }
                  pss = ss["black"];
                  has_black = true;
                }
              else if (i->color == cuts::test::data::RED)
                {
                  if (ss.find ("red") == ss.end ()) 
                    {
                      ss["red"] = new ostringstream ();
                    }
                  pss = ss["red"];
                  has_red = true;
                }
              else if (i->color == cuts::test::data::GREEN)
                {
                  if (ss.find ("green") == ss.end ()) 
                    {
                      ss["green"] = new ostringstream ();
                    }
                  pss = ss["green"];
                  has_green = true;
                }
              else if (i->color == cuts::test::data::BLUE)
                {
                  if (ss.find ("blue") == ss.end ()) 
                    {
                      ss["blue"] = new ostringstream ();
                    }
                  pss = ss["blue"];
                  has_blue = true;
                }
              if (pss != 0) 
                {
                  i->print (*pss);
                }
            }
        } // for
      my_cut_manager.reset ();

      {
        // print selected points in colored streams:
        ofstream fselected ("test_manager.selected.data");
        for (map<string,ostringstream *>::const_iterator i = ss.begin ();
             i != ss.end ();
             i++)
          {
            fselected << "# " << i->first << endl;
            fselected << i->second->str ();
            if (i->first == "black" && ! has_black) 
              {
                fselected << "1000 1000 1000" << endl;
              }
            if (i->first == "red" && ! has_red) 
              {
                fselected << "1000 1000 1000" << endl;
              }
            if (i->first == "blue" && ! has_blue) 
              {
                fselected << "1000 1000 1000" << endl;
              }
            if (i->first == "green" && ! has_green) 
              {
                fselected << "1000 1000 1000" << endl;
              }
            fselected << endl;
            fselected << endl;
            delete i->second;
          }
      }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of cuts::test_manager.cxx
