/// \file geomtools/gnuplot_drawer.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2014-07-16
 *
 * Description:
 *
 *  Algorithm to recursively draw the geometry using gnuplot.
 *
 */

#ifndef GEOMTOOLS_GNUPLOT_DRAWER_H
#define GEOMTOOLS_GNUPLOT_DRAWER_H 1

// Standard library:
#include <iostream>
#include <string>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <geomtools/placement.h>

namespace geomtools {

  // Forward declarations:
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

    static int display_level_no_limit();
    static const std::string & view_key();
    static const std::string & view_2d_xy();
    static const std::string & view_2d_xz();
    static const std::string & view_2d_yz();
    static const std::string & view_3d();
    static const std::string & view_3d_free_scale();
    static const std::string & default_view();
    static bool check_view(const std::string & view_label_);
    static const std::string & mode_wired();
    static const std::string & mode_solid();
    static const std::string & default_mode();
    static const std::string & force_show_property_name();
    static const std::string & force_show_envelope_property_name();
    static const std::string & force_show_children_property_name();
    static const std::string & force_hide_envelope_property_name();
    static const std::string & force_hide_children_property_name();
    static const std::string & world_name_key();

    /// Wait for key stroke in interactive Gnuplot session
    static void wait_for_key ();

    /// \brief Colored stream handle
    struct cstream
    {
      std::string label;
      std::string filename;
      std::ostringstream * oss = nullptr;
      int color;

      cstream ();
    };

    /// \brief Dictionary of colored stream handle
    typedef std::map<std::string, cstream> cstreams_col_type;

    /// \brief Storage entry for embedded display data
    class dd_entry
    {
    public:
      dd_entry() = default;
      void reset();
      void set_placement(const placement &);
      void set_display_data(const display_data &);
      const placement & get_placement() const;
      const display_data & get_display_data() const;
      bool is_valid() const;
    private:
      placement            _pl_;         //!< The placement of the display data
      const display_data * _dd_address_ = nullptr; //!< The handle of the display data
    };

    /// \brief Predicate that tests the address of a display data
    class has_dd_addr
    {
    public:
      has_dd_addr(const display_data &);
      bool operator()(const dd_entry & dde_) const;
    private:
      const display_data * _dd_address_ = nullptr; //!< The address of the display data to be checked
    };

    /// Collection of display data handle
    typedef std::vector<dd_entry> dd_col_type;

    /// \brief Visibility rules for 3D volume rendering
    struct visibility_rules {
      visibility_rules() = default;
      void reset();
    public:
      bool active = false;
      bool show_volume = true;
      bool show_envelope = false;
      bool show_daughters = true;
      int  daughter_level = 1;
      std::string color;
    };

  public:

    // Set the Gnuplot output
    void set_output (const std::string & output_);

    // Reset the Gnuplot output
    void reset_output ();

    /// Set Gnuplot terminal and options
    void set_terminal (const std::string & terminal_ = "",
                       const std::string & terminal_options_ = "");

    /// Reset Gnuplot terminal and options
    void reset_terminal ();

    // Set the Gnuplot output medium
    int set_output_medium (const std::string & file_ = "",
                           const std::string & terminal_ = "",
                           const std::string & terminal_options_ = "");

    /// Set the display view
    void set_view (const std::string & view_);

    /// Return the display view
    const std::string & get_view () const;

    /// Set the display mode
    void set_mode (const std::string & mode_);

    /// Return the display mode
    const std::string & get_mode () const;

    /// Set the 'labels' flag
    void set_labels (bool labels_);

    /// Check 'labels' flag
    bool use_labels () const;

    /// Set the 'title' flag
    void set_using_title (bool);

    /// Check 'title' flag
    bool use_title () const;

    /// Check '2D display' flag
    bool is_view_2d () const;

    /// Check '3D display' flag
    bool is_view_3d () const;

    /// Check 'solid display' flag
    bool is_solid () const;

    /// Check 'wired display' flag
    bool is_wired () const;

    /// Check intialization flag
    bool is_initialized () const;

    // visibility_rules & grab_vis_rules();

    // const visibility_rules & grab_vis_rules() const;

    /// Return a mutable reference to the embedded auxiliary properties
    datatools::properties & grab_properties ();

    /// Return a non mutable reference to the embedded auxiliary properties
    const datatools::properties & get_properties () const;

    /// Default constructor
    gnuplot_drawer ();

    /// Destructor
    virtual ~gnuplot_drawer ();

    /// Reset
    void reset ();

    /// Reset colored streams
    void reset_cstreams ();

    /// Basic print
    void print (std::ostream & out_) const;

    /// Main display method
    int draw (const manager & mgr_,
              const std::string & what_ = "",
              int max_display_level_ = 0);

    /// Draw a logical volume in its own reference frame
    void draw_logical (const logical_volume & log_,
                       const placement & p_,
                       int max_display_level_,
                       const std::string & title_,
                       bool drawing_display_data_ = false);

    /// Draw a geometry model in its own reference frame
    void draw_model (const model_factory & mf_,
                     const std::string & model_name_,
                     const placement & p_,
                     int max_display_level_);

    /// Draw a physical volume given its GID its the world reference frame
    void draw_physical_from_gid (const model_factory & mf_,
                                 const geom_id & gid_,
                                 const mapping & mapping_,
                                 int max_display_level_);

    /// Draw a logical volume with given name from a model factory
    void draw_logical (const model_factory & mf_,
                       const std::string & logical_name_,
                       const placement & p_,
                       int max_display_level_);

    /// Set the flag to draw embedded display data
    void set_drawing_display_data(bool);

    /// Return the flag to draw embedded display data
    bool is_drawing_display_data() const;

    /// Add a display data object in the scene to be drawn
    void add_display_data(const display_data & dd_);

    /// Add a display data object in the scene to be drawn with a dedicated placement
    void add_display_data(const display_data & dd_, const placement & pl_);

    /// Remove all display data formerly added to the scene
    void reset_display_data();

    /// Set shape rendering options
    void set_rendering_options_current(uint32_t flags_);

    /// Reset shape rendering options
    void reset_rendering_options_current();

    /// Set shape rendering options depth
    void set_rendering_options_depth(int32_t depth_);

    /// Reset shape rendering options depth
    void reset_rendering_options_depth();

  protected:

    void _draw_display_data(const model_factory & mf_,
                            const placement & p_);

    void _draw_display_data(const placement & p_);

    std::ostringstream & _get_stream(const std::string & section_);

  private:

    void _draw_(const logical_volume & log_,
                const placement & p_,
                int max_display_level_ = 0);

    /*
    // Future : enrich the interface of the '_draw_' method...
    void _draw_ (const logical_volume & log_,
                 const placement & p_,
                 const visibility_rules & vis_rules_);
    */

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

    bool                  _initialized_ = false;
    cstreams_col_type     _cstreams_;
    datatools::properties _props_;
    std::string _view_;
    bool        _using_title_ = true;
    bool        _labels_ = true;
    std::string _mode_;
    range _xrange_;
    range _yrange_;
    range _zrange_;
    // visibility_rules _vis_rules_;
    bool        _drawing_display_data_ = true;
    dd_col_type _display_data_;
    std::string _terminal_;
    std::string _terminal_options_;
    std::string _output_;
    // int         _max_display_level_; 
    uint32_t    _rendering_options_current_ = 0;
    int32_t     _rendering_options_depth_ = 0;

  }; // class gnuplot_drawer

} // end of namespace snemo

#endif // GEOMTOOLS_GNUPLOT_DRAWER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
