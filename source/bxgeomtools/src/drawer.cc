// -*- mode: c++ ; -*- 
/* drawer.cc
 */

#include <geomtools/drawer.h>

namespace geomtools {

  using namespace std;
  
  const string drawer::VIEW_KEY   = "view";
  const string drawer::VIEW_2D_XY = "xy";
  const string drawer::VIEW_2D_XZ = "xz";
  const string drawer::VIEW_2D_YZ = "yz";
  const string drawer::VIEW_3D    = "xyz";
  const int    drawer::DISPLAY_LEVEL_NO_LIMIT = 1000;

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
		     const string & name_,
		     const datatools::utils::properties & config_)
  {
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
	max_display_level = 0;
      } 
    const geomtools::logical_volume & log = log_;
    char tmp[256];
    sprintf (tmp, "%s",".tmp_drawer_XXXXXX");
    char * c = mktemp (tmp);
    string tmp_filename = tmp;
    ofstream tmp_file (tmp_filename.c_str ());

    datatools::utils::properties visu_config;
    visibility::extract (log_.parameters (), visu_config);

    bool shown = true;
    if (visibility::is_hidden (visu_config))
      {
	shown = false;
      }
    if (visibility::is_shown (visu_config))
      {
	shown = true;
      } 
    if (shown)
      {
	drawer::__draw (tmp_file, log_, p_, max_display_level);
	tmp_file.flush ();
      }

    tmp_file.close ();
    usleep (200);
   
    string view = drawer::VIEW_3D;
    if (config_.has_key ("view"))
      {
	view = config_.fetch_string (drawer::VIEW_KEY);
      }

    Gnuplot g1 ("lines");	
    ostringstream title_oss;
    title_oss << "Logical '" << name_ << "'";
    g1.set_title (title_oss.str ());

    if (view == drawer::VIEW_2D_XY)
      {
	g1.set_grid ();
	g1.cmd ("set size ratio -1");
	g1.set_xlabel ("x").set_ylabel ("y");
	g1.plotfile_xy (tmp_filename, 1, 2, "XY view");
      }

    if (view == drawer::VIEW_2D_XZ)
      {
	g1.set_grid ();
	g1.cmd ("set size ratio -1");
	g1.set_xlabel ("x").set_ylabel ("z");
	g1.plotfile_xy (tmp_filename, 1, 3, "XZ view");
      }

    if (view == drawer::VIEW_2D_YZ)
      {
	g1.set_grid ();
	g1.cmd ("set size ratio -1");
	g1.set_xlabel ("y").set_ylabel ("z");
	g1.plotfile_xy (tmp_filename, 2, 3, "YZ view");
      }

    if (view == drawer::VIEW_3D)
      {
	g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
	g1.plotfile_xyz (tmp_filename, 1, 2, 3, "3D view");
      }

    g1.showonscreen (); // window output
    wait_for_key ();
    usleep (200);


    // remove tmp file:
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

	datatools::utils::properties log_visu_config;
	visibility::extract (log_.parameters (), log_visu_config);
	
	bool shown = true;
	if (visibility::is_shown (log_visu_config))
	  {
	    shown = true;
	  }
	if (visibility::is_hidden (log_visu_config))
	  {
	    shown = false;
	  }

	if (shown)
	  {
	    gnuplot_draw::draw (out_, 
				p_,
				log.get_shape ());
	  }
	
	bool draw_children = false;
	// draw children:
	if ((display_level < max_display_level) && log.get_physicals ().size ())
	  {
	    draw_children = true;
	  }

	if (visibility::is_daughters_hidden (log_visu_config))
	  {
	    draw_children = false;
	  }

	if (draw_children)
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
		    geomtools::placement p;
		    // get placement from the daughter physical #i: 
		    pp->get_placement (i, p);
		    //pp->tree_dump (clog, "item placement (child)", "    ");
		    geomtools::placement pt = p;
		    // compute the placement relative to the mother volume:
		    p_.child_to_mother (p, pt);
		    //pt.tree_dump (clog, "item placement (mother)", "    ");

		    // invoke rendering for daughter #i:
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

    draw (log, p_, max_display_level_, log.get_name (), config_);

    return;
  }
  
} // end of namespace geomtools

// end of drawer.cc
