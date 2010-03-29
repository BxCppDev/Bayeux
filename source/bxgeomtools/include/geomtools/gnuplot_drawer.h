// -*- mode: c++ ; -*- 
/* gnuplot_drawer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *   
 * 
 * History: 
 * 
 */

#ifndef __geomtools__gnuplot_drawer_h
#define __geomtools__gnuplot_drawer_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/model_factory.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>

#include <geomtools/color.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>

namespace geomtools {
  
  using namespace std;  
  
  class gnuplot_drawer 
  {
  public:

    static const string VIEW_KEY;
    static const string VIEW_2D_XY;
    static const string VIEW_2D_XZ;
    static const string VIEW_2D_YZ;
    static const string VIEW_3D;
    static const string DEFAULT_VIEW;

    static const int    DISPLAY_LEVEL_NO_LIMIT;
  
    static const string MODE_WIRED;
    static const string MODE_SOLID;
    static const string DEFAULT_MODE;

    static void wait_for_key ();

    struct cstream
    {
      string label;
      string filename;
      ostringstream * oss;
      int color;

      cstream ()
      {
	//clog << "DEVEL: cstream::CTOR: Entering..." << endl;
	oss = 0;
	//clog << "DEVEL: cstream::CTOR: Exiting." << endl;
      }
      /*
      // Bug:
      ~cstream ()
      {
	clog << "DEVEL: cstream::DTOR: Entering..." << endl;
	if (oss)
	  {
	    delete oss;
	    oss = 0;
	  }
	clog << "DEVEL: cstream::DTOR: Exiting." << endl;
      }
      */
    };

    typedef map<string, cstream> cstreams_col_t;

  public:

    static bool g_devel;

  private:
    bool             __initialized;
    cstreams_col_t   __cstreams;
    datatools::utils::properties __props;
    string __view;
    string __mode;


  public:

    void set_view (const string & view_);

    const string & get_view () const;

    void set_mode (const string & mode_);

    const string & get_mode () const;

    bool is_view_2d () const;

    bool is_view_3d () const;

    bool is_solid () const;

    bool is_wired () const;

    bool is_initialized () const;

    //ostringstream & get_stream (const string & section_);

 protected:

    ostringstream & _get_stream (const string & section_);

  public:

    gnuplot_drawer ();

    virtual ~gnuplot_drawer ();

    //void init ();

    void reset ();

    void reset_cstreams ();

    void dump ();
 
  private:
    
    void __draw (const logical_volume & log_,
		 const placement & p_,
		 int max_display_level_ = 0);
    

  public:

    void draw (const logical_volume & log_,
	       const placement & p_,
	       int max_display_level_,
	       const string & name_);

    void draw (const model_factory & mf_,
	       const string & name_,
	       const placement & p_,
	       int max_display_level_);
  
  public:
    


  }; // class gnuplot_drawer
  
} // end of namespace snemo

#endif // __geomtools__gnuplot_drawer_h

// end of gnuplot_drawer.h
