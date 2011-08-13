// -*- mode: c++ ; -*- 
// cuts::test_manager.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <list>
#include <map>

#include <datatools/utils/utils.h>
#include <datatools/utils/multi_properties.h>

#include <cuts/manager.h>
#include <cuts_test_data.h>

// Additional registered test cuts :
#include <cuts_test_cuts.cc>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'cuts::manager' !" << endl; 
  
      bool debug = false;
      long seed = 314159;
      string cut_def_filename = "";
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
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
	      if (cut_def_filename.empty ())
		{ 
		  cut_def_filename = argument;
		}
	      else if (cut_name.empty ())
		{ 
		  cut_name = argument;
		}
	      else
		{ 
		  clog << "warning: ignoring argument '" << argument << "'!" << endl; 
		}
	    }
	  iarg++;
      }
    
      srand48 (seed);

      cuts::i_cut::g_debug = debug;
      cuts::manager my_manager;

      datatools::utils::multi_properties config ("name", "type");

      if (cut_def_filename.empty ())
	{ 
	  cut_def_filename = "${CUTS_ROOT}/tests/test_cuts_def.conf";
	}
      datatools::utils::fetch_path_with_env (cut_def_filename);
      config.read (cut_def_filename);
  
      if (cut_name.empty ())
	{ 
	  cut_name = "selector";
	}    

      my_manager.set_debug (debug);
      my_manager.initialize (config);
      cuts::i_cut * the_cut = 0;
      if (! my_manager.has_cut (cut_name))
	{
	  ostringstream message;
	  message << "No cut with name '" << cut_name << "' !";
	  throw runtime_error (message.str ());    
	}
      the_cut = my_manager.get_cut (cut_name);

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
      clog << "Selection: " << endl;
      for (list<cuts::test::data>::iterator i = points.begin ();
	   i != points.end ();
	   i++)
	{
	  the_cut->set_user_data (&*i);
	  if (the_cut->accept () == cuts::i_cut::ACCEPT)
	    {
	      ostringstream * pss = 0;
	      if (i->color == cuts::test::data::BLACK)
		{
		  if (ss.find ("black") == ss.end ()) 
		    {
		      ss["black"] = new ostringstream ();
		    }
		  pss = ss["black"];
		}
	      else if (i->color == cuts::test::data::RED)
		{
		  if (ss.find ("red") == ss.end ()) 
		    {
		      ss["red"] = new ostringstream ();
		    }
		  pss = ss["red"];
		}
	      else if (i->color == cuts::test::data::GREEN)
		{
		  if (ss.find ("green") == ss.end ()) 
		    {
		      ss["green"] = new ostringstream ();
		    }
		  pss = ss["green"];
		}
	      else if (i->color == cuts::test::data::BLUE)
		{
		  if (ss.find ("blue") == ss.end ()) 
		    {
		      ss["blue"] = new ostringstream ();
		    }
		  pss = ss["blue"];
		}
	      if (pss != 0) 
		{
		  i->print (*pss);
		}
	    }
	} // for
      my_manager.reset ();

      {
	// print selected points in colored streams:
	ofstream fselected ("test_manager.selected.data");
	for (map<string,ostringstream *>::const_iterator i = ss.begin ();
	     i != ss.end ();
	     i++)
	  {
	    fselected << "# " << i->first << endl;
	    fselected << i->second->str ();
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
