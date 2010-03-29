// -*- mode: c++ ; -*- 
/* gnuplot_drawer.cc
 */

#include <geomtools/gnuplot_drawer.h>

namespace geomtools {

  using namespace std;
  
  const string gnuplot_drawer::VIEW_KEY   = "view";
  const string gnuplot_drawer::VIEW_2D_XY = "xy";
  const string gnuplot_drawer::VIEW_2D_XZ = "xz";
  const string gnuplot_drawer::VIEW_2D_YZ = "yz";
  const string gnuplot_drawer::VIEW_3D    = "xyz";
  const string gnuplot_drawer::DEFAULT_VIEW  = gnuplot_drawer::VIEW_3D;

  const int    gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT = 1000;

  const string gnuplot_drawer::MODE_WIRED = "wired";
  const string gnuplot_drawer::MODE_SOLID = "solid";
  const string gnuplot_drawer::DEFAULT_MODE = MODE_WIRED;

  bool gnuplot_drawer::g_devel = false;

  void gnuplot_drawer::wait_for_key ()
  {
    /*
      #if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
      clog << endl << "Press ENTER to continue..." << endl;
      cin.clear ();
      cin.ignore (cin.rdbuf ()->in_avail ());
      cin.get ();
      #endif
    */
    clog << endl << "Press [Enter] to continue..." << endl;
    string s;
    getline (cin, s);
    return;
  }
  
  void gnuplot_drawer::set_view (const string & view_)
  {
    __view = view_;
  }
 
  const string & gnuplot_drawer::get_view () const
  {
    return __mode;
  }
  
  void gnuplot_drawer::set_mode (const string & mode_)
  {
    __mode = mode_;
  }

  const string & gnuplot_drawer::get_mode () const
  {
    return __mode;
  }

  bool gnuplot_drawer::is_view_2d () const
  {
    return ! is_view_3d ();
  }

  bool gnuplot_drawer::is_view_3d () const
  {
    return __mode == VIEW_3D;
  }

  bool gnuplot_drawer::is_solid () const
  {
    return __mode == MODE_SOLID;
  }

  bool gnuplot_drawer::is_wired () const
  {
    return __mode == MODE_WIRED;
  }
 
  ostringstream & gnuplot_drawer::_get_stream (const string & section_)
  {
    bool devel = g_devel;
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::_get_stream: "
	     << "Entering with section '" << section_ << "'..."
	     << endl;
      }
    cstreams_col_t::iterator i = __cstreams.find (section_);
    if (i == __cstreams.end ())
      {
	if (devel)
	  {
	    cerr << "DEVEL: gnuplot_drawer::_get_stream: "
		 << "Section '" << section_ << "' does not exist ! Create it !"
		 << endl;
	  }
	cstream new_cs;
	new_cs.label = section_;
	new_cs.filename = "";
	new_cs.oss = new ostringstream;
	/*
        cerr << "DEVEL: gnuplot_drawer::_get_stream: "
	     << "new_cs.oss   = " << new_cs.oss << endl;
	*/
	new_cs.color = color::get_color (section_);
	/*
        cerr << "DEVEL: gnuplot_drawer::_get_stream: "
	     << "new_cs.color = " << new_cs.color << endl;
	*/
	__cstreams[section_] = new_cs;
	return *(new_cs.oss);
      }
    else
      {
	if (devel)
	  {
	    cerr << "DEVEL: gnuplot_drawer::_get_stream: "
		 << "Found section '" << section_ << "' !"
		 << endl;
	  }
      }
    cstream & cs = i->second;
    return *(cs.oss);
  }

  // ctor:
  gnuplot_drawer::gnuplot_drawer ()
  {
    __initialized = false;
    __view = gnuplot_drawer::DEFAULT_VIEW;
    __mode = gnuplot_drawer::DEFAULT_MODE;
    //init ();
  }
  
  gnuplot_drawer::~gnuplot_drawer ()
  {
    /*
      if (__initialized) 
      {
      reset ();
      }
    */
    reset ();
  }

  /*
    void gnuplot_drawer::init ()
    {
    if (__initialized) 
    {
    clog << "WARNING: gnuplot_drawer::init: Already initialized !" << endl;
    }
    __initialized = true;
    }
  */
  
  void gnuplot_drawer::reset_cstreams ()
  {
    //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Entering..." << endl;
    if (__cstreams.size () > 0)
      {
	for (cstreams_col_t::iterator i = __cstreams.begin ();
	     i != __cstreams.end ();
	     i++)
	  {
	    //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Label == " << i->first << endl;
	    cstream & cs = i->second;
	    if (cs.oss != 0)
	      {
		cs.oss->flush ();
		//clog << "DEVEL: gnuplot_drawer::reset_cstreams: oss to be deleted..." << cs.oss << endl;
		delete cs.oss;
		cs.oss = 0;
		//clog << "DEVEL: gnuplot_drawer::reset_cstreams: done." << endl;
	      }
	  }
	__cstreams.clear ();
      }
    //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Exiting." << endl;
    return;
  }
 
  void gnuplot_drawer::reset ()
  {
    /*
      if (! __initialized) 
      {
      return;
      }
    */
    reset_cstreams ();
    __view = gnuplot_drawer::DEFAULT_VIEW;
    __mode = gnuplot_drawer::DEFAULT_MODE;
    //__initialized = false;
  }
  
  /****************************************************/
   
  
  void gnuplot_drawer::__draw (const logical_volume & log_,
			       const placement & p_,
			       int max_display_level_)
  {
    bool devel = g_devel;
    //devel = true;
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::__draw: Entering... " << endl;
      }
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
	max_display_level = 0;
      } 
    const geomtools::logical_volume & log = log_;
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::__draw: log name = " << log.get_name () << endl;
      }
    try
      {
	int display_level = 0;
	
	datatools::utils::properties log_visu_config;
	visibility::extract (log.parameters (), log_visu_config);
	
	if (devel)
	  {
	    ostringstream toss;
	    toss << "Logical '" << log.get_name () << "' visibility properties:";
	    log_visu_config.tree_dump (cerr, toss.str (), " DEVEL: ");
	  }

	bool shown = true;
	bool shown_envelop = true; 
	if (visibility::is_shown (log_visu_config))
	  {
	    shown = true;
	  }
	if (visibility::is_hidden (log_visu_config))
	  {
	    shown = false;
	  }
	if (visibility::is_hidden_envelop (log_visu_config))
	  {
	    shown_envelop = false;
	  }
	if (devel)
	  {
	    cerr << "DEVEL: gnuplot_drawer::__draw: Show         = " << shown << endl;
	    cerr << "DEVEL: gnuplot_drawer::__draw: Show envelop = " << shown_envelop << endl;
	  }

	if (shown && shown_envelop)
	  {
	    if (is_wired ())
	      {
		string color_label = color::default_color;
		if (visibility::has_color (log_visu_config))
		  {
		    color_label = visibility::get_color (log_visu_config);
		    if (devel)
		      {
			cerr << "DEVEL: gnuplot_drawer::__draw: Found color '" << color_label 
			     << "' for logical '" << log.get_name () << "'..." << endl;
		      }
		  } 
		else
		  {
		    if (devel)
		      {
			cerr << "DEVEL: gnuplot_drawer::__draw: No color specification for logical '" << log.get_name () << "' !" << endl;
		      }		    
		  }
		if (devel)
		  {
		    cerr << "DEVEL: gnuplot_drawer::__draw: color_label  = '" << color_label << "'" << endl;
		  }
		
		ostringstream & colored_oss = _get_stream (color_label);
		if (devel)
		  {
		    cerr << "DEVEL: gnuplot_drawer::__draw: " 
			 << "gnuplot_draw::draw: &colored_oss = " << &colored_oss << endl;
		    cerr << "DEVEL: gnuplot_drawer::__draw: gnuplot_draw::draw..." << endl;
		    /*
		    // test:
		    colored_oss << "0 0 0" << endl;
		    colored_oss << "1 0 0" << endl << endl;
		    colored_oss << "0 0 0" << endl;
		    colored_oss << "0 1 0" << endl << endl;
		    colored_oss << "0 0 0" << endl;
		    colored_oss << "0 0 1" << endl << endl;
		    */
		  }
		
		unsigned long mode = gnuplot_draw::MODE_NULL;
		if (visibility::is_wired_cylinder (log_visu_config))
		  {
		    mode |= gnuplot_draw::MODE_WIRED_CYLINDER;
		  }

                gnuplot_draw::draw (colored_oss, 
				    p_,
				    log.get_shape (),
				    mode);
		
	      }
	    else
	      {
		if (devel)
		  {
		    cerr << "WARNING: gnuplot_drawer::__draw: " 
			 << "Display mode '" << __mode << "'is not implemented !"  
			 << endl;
		  }
	      }
	  }
	
	// draw children:
	bool draw_children = false;
	if (devel)
	  {
	    cerr << "DEVEL: gnuplot_drawer::__draw: " 
		 << "TEST XXX"  
		 << endl;
	  }

	// check the disply level of the geometry tree:
	if ((display_level < max_display_level) 
	    && (log.get_physicals ().size () > 0))
	  {
	    // default is 'drawing children':
	    draw_children = true;
	  }

	// test if it is forbidden by the visibility properties:
	if (! shown || visibility::is_daughters_hidden (log_visu_config))
	  {
	    draw_children = false;
	  }

	// do it:
	if (draw_children)
	  {
	    if (devel)
	      {
		cerr << "WARNING: gnuplot_drawer::__draw: " 
		     << "Drawing children..."  
		     << endl;
	      }
	    display_level++;
	    for (geomtools::logical_volume::physicals_col_t::const_iterator i 
		   = log.get_physicals ().begin (); 
		 i != log.get_physicals ().end (); i++)
	      {
		const physical_volume & phys = *(i->second);
		const geomtools::logical_volume & log_child = phys.get_logical ();
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
		    
		    // recursive invocation of the visualization data generation
		    // for daughter #i:
		    gnuplot_drawer::__draw (log_child,
					    pt,
					    max_display_level - 1);
		  }
	      }
	  }
      }
    catch (GnuplotException ge)
      {
	cerr << "ERROR: gnuplot_drawer::__draw: " << ge.what () << endl;
      }
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::__draw: Exiting." << endl;
      }
    return;
  }
  
  
  /****************************************************/

  void gnuplot_drawer::draw (const logical_volume & log_,
			     const placement & p_,
			     int max_display_level_,
			     const string & name_)
  {
    bool devel = g_devel;
    //devel = true;
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
	max_display_level = 0;
      } 
    const geomtools::logical_volume & log = log_;

    datatools::utils::properties visu_config;
    visibility::extract (log_.parameters (), visu_config);

    bool shown = true; 
    bool shown_envelop = true; 
    if (visibility::is_hidden (visu_config))
      {
	shown = false;
      }
    if (visibility::is_shown (visu_config))
      {
	shown = true;
      } 
    if (visibility::is_hidden_envelop (visu_config))
      {
	shown_envelop = false;
      }

    int color = 1;
    int former_color = color::get_color (color::default_color);
    if (visibility::has_color (visu_config))
      {
	color = color::get_color (visibility::get_color (visu_config));
	if (devel)
	  {
	    cerr << "DEVEL: **** Found color '" 
		 << visibility::get_color (visu_config)
		 << "' color is : " << color << endl;
	  }
      } 

    gnuplot_draw::g_current_color = color;
    gnuplot_draw::g_current_color = former_color;

    if (shown)
      {
	gnuplot_drawer::__draw (log_, p_, max_display_level);
      }

    for (cstreams_col_t::iterator i = __cstreams.begin ();
	 i != __cstreams.end ();
	 i++)
      {
	const string & label = i->first;
	cstream & cs = i->second;
	char tmp[256];
	ostringstream oss;
	oss << ".tmp_gnuplot_drawer_" << label <<"_XXXXXX";
	sprintf (tmp, "%s", oss.str ().c_str ());
	char * c = mktemp (tmp);
	string tmp_filename = tmp;
	cs.filename = tmp;
	ofstream tmp_file (cs.filename.c_str ());
	tmp_file << "# label='" << label << "'" << endl;
	tmp_file << cs.oss->str ();
	tmp_file << "# end of label='" << label << "'" << endl;
	tmp_file.flush ();
	tmp_file.close ();
      }
    usleep (200);
   
    string view = __view;

    Gnuplot g1 ("lines");	
    ostringstream title_oss;
    title_oss << "Logical '" << name_ << "'";
    g1.set_title (title_oss.str ());

    std::ostringstream cmdstr;

    int col1 = 1;
    int col2 = 2;
    int col3 = 3;
    size_t plot_dim = 2;

    if (view == gnuplot_drawer::VIEW_2D_XY)
      {
	g1.set_xlabel ("x").set_ylabel ("y");
	col1 = 1;
	col2 = 2;
      }

    if (view == gnuplot_drawer::VIEW_2D_XZ)
      {
	g1.set_xlabel ("x").set_ylabel ("z");
	col1 = 1;
	col2 = 3;
      }

    if (view == gnuplot_drawer::VIEW_2D_YZ)
      {
	g1.set_xlabel ("y").set_ylabel ("z");
	col1 = 2;
	col2 = 3;
      }

    if (view == gnuplot_drawer::VIEW_3D)
      {
	plot_dim = 3;
	g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
	col1 = 1;
	col2 = 2;
	col3 = 3;
      }

    if (__cstreams.size ())
      {
	if (plot_dim == 2)
	  {
	    g1.set_grid ();
	    g1.cmd ("set size ratio -1");
	    cmdstr << "plot ";
	  }
	else
	  {
	    cmdstr << "splot ";
	  }
	for (cstreams_col_t::const_iterator i = __cstreams.begin ();
	     i != __cstreams.end ();
	     i++)
	  {
	    const cstream & cs = i->second;
	    cmdstr << "\"" << cs.filename << "\" using " << col1 << ":" << col2;
	    int color = cs.color;
	    if (plot_dim == 3)
	      {
		cmdstr << ":" << col3;
	      }
	    cmdstr << " notitle";
	    cmdstr << " with lines lt " << color << ' ' ;
	    cmdstr << " lw " << 0.5 << ' ' ;

	    // if there is another one:
	    {
	      cstreams_col_t::const_iterator j = i;
	      j++;
	      if (j != __cstreams.end ())
		{
		  cmdstr << ", "; 
		}
	    }
	  }
      }
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::draw: GNUPLOT command is :" << endl
	     << cmdstr.str () << endl;
      }
    if (! cmdstr.str ().empty ())
      {
	g1.cmd (cmdstr.str ());
	g1.showonscreen (); // window output
	wait_for_key ();
	usleep (200);
      }
    else
      {
	cerr << "WARNING: gnuplot_drawer::draw: Nothing to display !" << endl;
      }

    // remove tmp files:
    for (cstreams_col_t::iterator i = __cstreams.begin ();
	 i !=  __cstreams.end ();
	 i++)
      {
	cstream & cs = i->second;
	unlink (cs.filename.c_str ());
	/*
	if (! devel)
	  {
	    unlink (cs.filename.c_str ());
	  }
	else
	  {
	    cerr << "WARNING: gnuplot_drawer::draw: Do not remove file '" 
		 << cs.filename << "' !" << endl;
	  }
	*/
      }
    if (devel)
      {
	cerr << "DEVEL: gnuplot_drawer::draw: " 
	     << "reset_cstreams..." << endl;
      }
    reset_cstreams ();
    return;
  }

  /****************************************************/
   
  void gnuplot_drawer::draw (const model_factory & mf_,
			     const string & name_,
			     const placement & p_,
			     int max_display_level_)
  {
    bool devel = g_devel;
    //devel = true;
    models_col_t::const_iterator found = mf_.get_models ().find (name_);
    if (found ==  mf_.get_models ().end ())
      {
	ostringstream message;
	message << "gnuplot_drawer::draw: "
		<< "Cannot find model with name '" << name_ << "' !"; 
	throw runtime_error (message.str ());	
      }
    const i_model & model = *(found->second);
    const geomtools::logical_volume & log = model.get_logical ();

    draw (log, p_, max_display_level_, log.get_name ());

    return;
  }
  
} // end of namespace geomtools

// end of gnuplot_drawer.cc
