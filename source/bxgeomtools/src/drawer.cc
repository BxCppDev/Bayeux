// -*- mode: c++ ; -*- 
/* drawer.cc
 */

#include <geomtools/drawer.h>

namespace geomtools {

  using namespace std;

  void drawer::wait_for_key ()
  {
    /*
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    clog << endl << "Press ENTER to continue..." << endl;
    cin.clear ();
    cin.ignore (cin.rdbuf ()->in_avail ());
    cin.get ();
#endif
    */
    clog << endl << "Press ENTER to continue..." << endl;
    string s;
    getline (cin, s);
    return;
  }
   
  void drawer::draw (const logical_volume & log_,
		     const placement & p_,
		     int max_display_level_,
		     const string & name_)
  {
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
	max_display_level = 0;
      } 
    const geomtools::logical_volume & log = log_;
    char tmp[256];
    sprintf (tmp, "%s",".tmp_drawer_XXXXXX");
    mktemp (tmp);
    string tmp_filename = tmp;
    ofstream tmp_file (tmp_filename.c_str ());

    drawer::__draw (tmp_file, log_, p_, max_display_level);

    Gnuplot g1 ("lines");	
    ostringstream title_oss;
    title_oss << "Logical '" << name_ << "'";
    g1.set_title (title_oss.str ());
    g1.set_grid ();
    g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
    g1.plotfile_xyz (tmp_filename, 1, 2, 3, "3D view");
    g1.showonscreen (); // window output
    wait_for_key ();
    // remove tmp file
    unlink (tmp_filename.c_str ());
    return;
  }
   
  void drawer::__draw (ostream & out_,
		       const logical_volume & log_,
		       const placement & p_,
		       int max_display_level_)
  {
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
	max_display_level = 0;
      } 
    const geomtools::logical_volume & log = log_;
 	
    try
      {
	int display_level = 0;

	gnuplot_draw::draw (out_, 
			    p_,
			    log.get_shape ());
	
	// draw children:
	if ((display_level < max_display_level) && log.get_physicals ().size ())
	  {
	    display_level++;
	    for (geomtools::logical_volume::physicals_col_t::const_iterator i 
		   = log.get_physicals ().begin (); 
		 i != log.get_physicals ().end (); i++)
	      {
		const physical_volume & phys = *(i->second);
		const geomtools::logical_volume & log_child 
		  = phys.get_logical ();
		if (log_child.parameters ().has_key ("visibility"))
		  {
		    if (log_child.parameters ().fetch_string ("visibility")
			== "invisible")
		      {
			continue;
		      }
		  }
		const geomtools::i_placement * pp = &(phys.get_placement ());
		for (int i = 0; i < pp->get_number_of_items (); i++)
		  {
		    clog << "DEVEL: item #" << i << endl;
		    
		    geomtools::placement p;
		    pp->get_placement (i, p);
		    pp->tree_dump (clog, "item placement (child)", "    ");
		    geomtools::placement pt = p;
		    p_.child_to_mother (p, pt);
		    pt.tree_dump (clog, "item placement (mother)", "    ");

		    /*
		    gnuplot_draw::draw (out_, 
					pt,
					log_child.get_shape ());
		    */
		    drawer::__draw (out_,
				    log_child,
				    pt,
				    max_display_level - 1);
		  }
	      }
	  }

    }
    catch (GnuplotException ge)
    {
      cerr << "ERROR: drawer::draw: " << ge.what () << endl;
    }
    return;
  }
   
  void drawer::draw (const model_factory & mf_,
		     const string & name_,
		     const placement & p_,
		     int max_display_level_,
		     const datatools::utils::properties & config_)
  {
    models_col_t::const_iterator found = mf_.get_models ().find (name_);
    if (found ==  mf_.get_models ().end ())
      {
	ostringstream message;
	message << "drawer::draw: "
		<< "Cannot find model with name '" << name_ << "' !"; 
	throw runtime_error (message.str ());	
      }
    const i_model & model = *(found->second);
    const geomtools::logical_volume & log = model.get_logical ();

    draw (log, p_, max_display_level_, log.get_name ());

    return;
  }
  
} // end of namespace geomtools

// end of drawer.cc
