// -*- mode: c++ ; -*- 
/* gnuplot_drawer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *  Algorithm to recursively draw the geometry using gnuplot.
 *
 * History: 
 * 
 */

#ifndef __geomtools__gnuplot_drawer_h
#define __geomtools__gnuplot_drawer_h 1

#include <iostream>
#include <string>
#include <sstream>

#include <datatools/utils/properties.h>

namespace geomtools {
  
  using namespace std;  
  
  class placement;
  class model_factory;
  class logical_volume;

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

    static const string FORCE_SHOW_PROPERTY_NAME;
    static const string FORCE_SHOW_ENVELOP_PROPERTY_NAME;
    static const string FORCE_SHOW_CHILDREN_PROPERTY_NAME;

    static void wait_for_key ();

    struct cstream
    {
      string label;
      string filename;
      ostringstream * oss;
      int color;

      cstream ()
      {
	oss = 0;
	return;
      }
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

    datatools::utils::properties & get_properties ();

    const datatools::utils::properties & get_properties () const;

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
	       const string & model_name_,
	       const placement & p_,
	       int max_display_level_);

    void draw_logical (const model_factory & mf_,
		       const string & model_name_,
		       const placement & p_,
		       int max_display_level_);

  }; // class gnuplot_drawer
  
} // end of namespace snemo

#endif // __geomtools__gnuplot_drawer_h

// end of gnuplot_drawer.h
