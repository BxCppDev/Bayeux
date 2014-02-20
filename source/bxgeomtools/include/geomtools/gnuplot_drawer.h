// -*- mode: c++ ; -*-
/** \file geomtools/gnuplot_drawer.h
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

#ifndef GEOMTOOLS_GNUPLOT_DRAWER_H_
#define GEOMTOOLS_GNUPLOT_DRAWER_H_ 1

// Standard library:
#include <iostream>
#include <string>
#include <sstream>

// Third party:
// -  Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <geomtools/placement.h>

namespace geomtools {

  class model_factory;
  class logical_volume;
  class display_data;
  class geom_id;
  class mapping;
  class manager;

  /// \brief An Gnuplot driver object responsible of the display of a virtual geometry
  class gnuplot_drawer
  {
  public:

    static const int DISPLAY_LEVEL_NO_LIMIT = 1000;

    static int           display_level_no_limit();
    static const std::string & view_key();
    static const std::string & view_2d_xy();
    static const std::string & view_2d_xz();
    static const std::string & view_2d_yz();
    static const std::string & view_3d();
    static const std::string & view_3d_free_scale();
    static const std::string & default_view();
    static const std::string & mode_wired();
    static const std::string & mode_solid();
    static const std::string & default_mode();
    static const std::string & force_show_property_name();
    static const std::string & force_show_envelope_property_name();
    static const std::string & force_show_children_property_name();
    static const std::string & world_name_key();

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

    class dd_entry
    {
    public:
      dd_entry();
      void reset();
      void set_placement(const placement &);
      void set_display_data(const display_data &);
      const placement & get_placement() const;
      const display_data & get_display_data() const;
      bool is_valid() const;
    private:
      placement            _pl_;
      const display_data * _dd_address_;
    };

    class has_dd_addr
    {
    public:
      has_dd_addr(const display_data &);
      bool operator()(const dd_entry & dde_) const;
    private:
      const display_data * _dd_address_;
    };

    typedef std::vector<dd_entry> dd_col_type;

    struct visibility_rules {
      visibility_rules();
      void reset();
    public:
      bool active;
      bool show_volume;
      bool show_envelope;
      bool show_daughters;
      int daughter_level;
      std::string color;
    };

  public:

    void set_output (const std::string & output_);

    void reset_output ();

    void set_terminal (const std::string & terminal_ = "",
                       const std::string & terminal_options_ = "");

    void reset_terminal ();

    int set_output_medium (const std::string & file_ = "",
                           const std::string & terminal_ = "",
                           const std::string & terminal_options_ = "");

    void set_view (const std::string & view_);

    const std::string & get_view () const;

    void set_mode (const std::string & mode_);

    const std::string & get_mode () const;

    void set_labels (bool labels_);

    bool use_labels () const;

    void set_using_title (bool);

    bool use_title () const;

    bool is_view_2d () const;

    bool is_view_3d () const;

    bool is_solid () const;

    bool is_wired () const;

    bool is_initialized () const;

    // visibility_rules & grab_vis_rules();

    // const visibility_rules & grab_vis_rules() const;

    datatools::properties & grab_properties ();

    const datatools::properties & get_properties () const;

  protected:

    std::ostringstream & _get_stream (const std::string & section_);

  public:

    gnuplot_drawer ();

    virtual ~gnuplot_drawer ();

    void reset ();

    void reset_cstreams ();

    void print (std::ostream & out_) const;

  private:

    void _draw_ (const logical_volume & log_,
                 const placement & p_,
                 int max_display_level_ = 0);

    /*
    // Future : enrich the interface of the '_draw_' method...
    void _draw_ (const logical_volume & log_,
                 const placement & p_,
                 const visibility_rules & vis_rules_);
    */

  public:

    /// Main display method
    int draw (const manager & mgr_,
              const std::string & what_ = "",
              int max_display_level_ = 0);

    void draw (const logical_volume & log_,
               const placement & p_,
               int max_display_level_,
               const std::string & title_);

    void draw (const model_factory & mf_,
               const std::string & model_name_,
               const placement & p_,
               int max_display_level_);

    void draw_from_gid (const model_factory & mf_,
                        const geom_id & gid_,
                        const mapping & mapping_,
                        int max_display_level_);

    void draw_logical (const model_factory & mf_,
                       const std::string & logical_name_,
                       const placement & p_,
                       int max_display_level_);

    /// Add a display data object in the scene to be drawn
    void add_display_data(const display_data & dd_);

    /// Add a display data object in the scene to be drawn with a dedicated placement
    void add_display_data(const display_data & dd_, const placement & pl_);

    /// Remove all display data formerly added to the scene
    void reset_display_data();

  protected:

    void _draw_display_data (const model_factory & mf_,
                             const placement & p_);

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

    bool                  _initialized_;
    cstreams_col_type     _cstreams_;
    datatools::properties _props_;
    std::string _view_;
    bool        _using_title_;
    bool        _labels_;
    std::string _mode_;
    range _xrange_;
    range _yrange_;
    range _zrange_;
    //visibility_rules _vis_rules_;
    dd_col_type _display_data_;
    std::string _terminal_;
    std::string _terminal_options_;
    std::string _output_;

  }; // class gnuplot_drawer

} // end of namespace snemo

#endif // GEOMTOOLS_GNUPLOT_DRAWER_H_

// end of gnuplot_drawer.h
