// -*- mode: c++ ; -*- 
/* drawer.cc
 */

#include <geomtools/drawer.h>

namespace geomtools {

  using namespace std;

  void drawer::wait_for_key ()
  {
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    clog << endl << "Press ENTER to continue..." << endl;
    cin.clear ();
    cin.ignore (cin.rdbuf ()->in_avail ());
    cin.get ();
#endif
    return;
  }
   
  void drawer::draw (const model_factory & mf_,
		     const string & name_,
		     const placement & p_,
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

    try
      {
	Gnuplot g1 ("lines");
	
	ostringstream title_oss;
	title_oss << "Logical '" << name_ << "'";
	g1.set_title (title_oss.str ());
	
	string tmp_filename = "/tmp/tmp.data";
	ofstream tmp_file (tmp_filename.c_str ());

	const geomtools::logical_volume & log = model.get_logical ();
	gnuplot_draw::draw (tmp_file, 
			    p_,
			    log.get_shape ());

	// draw children:
	if (log.get_physicals ().size ())
	  {
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
		    geomtools::placement pt;
		    p_.child_to_mother (p, pt);
		    pt.tree_dump (clog, "item placement (mother)", "    ");
		    gnuplot_draw::draw (tmp_file, 
					pt,
					log_child.get_shape ());
		  }
	      }
	  }

        g1.set_grid ();
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        g1.plotfile_xyz (tmp_filename, 1, 2, 3, "3D view");

	g1.showonscreen (); // window output
        wait_for_key ();

    }
    catch (GnuplotException ge)
    {
      cerr << "ERROR: drawer::draw: " << ge.what () << endl;
    }


    return;
  }
  
} // end of namespace geomtools

// end of drawer.cc
