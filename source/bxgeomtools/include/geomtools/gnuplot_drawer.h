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
  
  class placement;
  class model_factory;
  class logical_volume;

  class gnuplot_drawer 
  {
  public:

    static const std::string VIEW_KEY;
    static const std::string VIEW_2D_XY;
    static const std::string VIEW_2D_XZ;
    static const std::string VIEW_2D_YZ;
    static const std::string VIEW_3D;
    static const std::string VIEW_3D_FREE_SCALE;
    static const std::string DEFAULT_VIEW;

    static const int    DISPLAY_LEVEL_NO_LIMIT;
  
    static const std::string MODE_WIRED;
    static const std::string MODE_SOLID;
    static const std::string DEFAULT_MODE;

    static const std::string FORCE_SHOW_PROPERTY_NAME;
    static const std::string FORCE_SHOW_ENVELOP_PROPERTY_NAME;
    static const std::string FORCE_SHOW_CHILDREN_PROPERTY_NAME;

    static void wait_for_key ();

    struct cstream
    {
      std::string label;
      std::string filename;
      std::ostringstream * oss; 
      int color;

      cstream ();
    };

    typedef std::map<std::string, cstream> cstreams_col_type;

  public:

    static bool g_devel;

  public:

    void set_view (const std::string & view_);

    const std::string & get_view () const;

    void set_mode (const std::string & mode_);

    const std::string & get_mode () const;

    void set_labels (bool labels_);

    bool use_labels () const;

    bool is_view_2d () const;

    bool is_view_3d () const;

    bool is_solid () const;

    bool is_wired () const;

    bool is_initialized () const;

    datatools::utils::properties & get_properties ();

    const datatools::utils::properties & get_properties () const;

  protected:

    std::ostringstream & _get_stream (const std::string & section_);

  public:

    gnuplot_drawer ();

    virtual ~gnuplot_drawer ();

    void reset ();

    void reset_cstreams ();

    void dump ();
 
  private:
    
    void _draw_ (const logical_volume & log_,
                 const placement & p_,
                 int max_display_level_ = 0);
    
  public:

    void draw (const logical_volume & log_,
               const placement & p_,
               int max_display_level_,
               const std::string & name_);

    void draw (const model_factory & mf_,
               const std::string & model_name_,
               const placement & p_,
               int max_display_level_);

    void draw_logical (const model_factory & mf_,
                       const std::string & model_name_,
                       const placement & p_,
                       int max_display_level_);

  public:

    struct range
    {
      char axis;
      double min;
      double max;
      range (char axis_ = 0);
      void reset ();
      void set_axis (char axis_);
      void print (std::ostream & out_) const;
    };

  private:

    bool             _initialized_;
    cstreams_col_type   _cstreams_;
    datatools::utils::properties _props_;
    std::string _view_;
    bool        _labels_;
    std::string _mode_;
    range _xrange_;
    range _yrange_;
    range _zrange_;

  }; // class gnuplot_drawer
  
} // end of namespace snemo

#endif // __geomtools__gnuplot_drawer_h

// end of gnuplot_drawer.h
