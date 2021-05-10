// gnuplot_drawer.cc

// Ourselves:
#include <geomtools/gnuplot_drawer.h>

// Standard libraries:
#include <stdexcept>
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/temporary_files.h>

// This project:
#include <geomtools/model_factory.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/color.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/display_data.h>
#include <geomtools/geom_id.h>
#include <geomtools/geom_info.h>
#include <geomtools/mapping.h>
#include <geomtools/manager.h>

namespace geomtools {

  // static
  const int gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT;

  // gnuplot_drawer::visibility_rules::visibility_rules()
  // {
  //   active = false;
  //   show_volume = true;
  //   show_envelope = false;
  //   show_daughters = true;
  //   daughter_level = 1;
  //   color.clear();
  //   return;
  // }

  void gnuplot_drawer::visibility_rules::reset()
  {
    active = false;
    show_volume = true;
    show_envelope = false;
    show_daughters = true;
    color.clear();
  }

  /*
  gnuplot_drawer::visibility_rules & gnuplot_drawer::grab_vis_rules()
  {
    return _vis_rules_;
  }

  const gnuplot_drawer::visibility_rules & gnuplot_drawer::grab_vis_rules() const
  {
    return _vis_rules_;
  }
  */

  // static
  int gnuplot_drawer::display_level_no_limit()
  {
    return gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT;
  }

  // static
  const std::string & gnuplot_drawer::view_key()
  {
    static std::string token = "view";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::view_2d_xy()
  {
    static std::string token = "xy";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::view_2d_xz()
  {
    static std::string token = "xz";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::view_2d_yz()
  {
    static std::string token = "yz";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::view_3d()
  {
    static std::string token = "xyz";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::view_3d_free_scale()
  {
    static std::string token = "xyz_free";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::default_view()
  {
    static std::string token = gnuplot_drawer::view_3d();
    return token;
  }

  // static
  bool gnuplot_drawer::check_view(const std::string & view_)
  {
    if (view_ == gnuplot_drawer::view_2d_xy()) return true;
    if (view_ == gnuplot_drawer::view_2d_xz()) return true;
    if (view_ == gnuplot_drawer::view_2d_yz()) return true;
    if (view_ == gnuplot_drawer::view_3d()) return true;
    if (view_ == gnuplot_drawer::view_3d_free_scale()) return true;
    return false;
  }

  // static
  const std::string & gnuplot_drawer::mode_wired()
  {
    static std::string token = "wired";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::mode_solid()
  {
    static std::string token = "solid";
    return token;
  }

  // static
  const std::string & gnuplot_drawer::default_mode()
  {
    static std::string token = gnuplot_drawer::mode_wired();
    return token;
  }

  const std::string & gnuplot_drawer::force_show_property_name()
  {
    static std::string token = "visibility.force_show";
    return token;
  }

  const std::string & gnuplot_drawer::force_show_envelope_property_name()
  {
    static std::string token = "visibility.force_show_envelope";
    return token;
  }

  const std::string & gnuplot_drawer::force_show_children_property_name()
  {
    static std::string token = "visibility.force_show_children";
    return token;
  }

  const std::string & gnuplot_drawer::force_hide_envelope_property_name()
  {
    static std::string token = "visibility.force_hide_envelope";
    return token;
  }

  const std::string & gnuplot_drawer::force_hide_children_property_name()
  {
    static std::string token = "visibility.force_hide_children";
    return token;
  }

  const std::string & gnuplot_drawer::world_name_key()
  {
    static std::string token = "world_name";
    return token;
  }

  gnuplot_drawer::cstream::cstream ()
  {
    oss = 0;
    return;
  }

  /**********************************************************************/

  bool gnuplot_drawer::dd_entry::is_valid() const
  {
    return _dd_address_ != nullptr && _pl_.is_valid ();
  }

  void gnuplot_drawer::dd_entry::reset()
  {
    _dd_address_ = nullptr;
    _pl_.reset();
    return;
  }

  void gnuplot_drawer::dd_entry::set_placement(const placement & pl_)
  {
    _pl_ = pl_;
    return;
  }

  void gnuplot_drawer::dd_entry::set_display_data(const display_data & dd_)
  {
    _dd_address_ = &dd_;
    return;
  }

  const placement & gnuplot_drawer::dd_entry::get_placement() const
  {
    return _pl_;
  }

  const display_data & gnuplot_drawer::dd_entry::get_display_data() const
  {
    return *_dd_address_;
  }

  gnuplot_drawer::has_dd_addr::has_dd_addr(const display_data& dd_) : _dd_address_(&dd_)
  {
    return;
  }

  bool gnuplot_drawer::has_dd_addr::operator()(const dd_entry & dde_) const
  {
    DT_THROW_IF (! dde_.is_valid(), std::logic_error, "Invalid dd_entry instance !");
    return _dd_address_ == &dde_.get_display_data();
  }

  /**********************************************************************/

  gnuplot_drawer::range::range (char axis_)
  {
    axis = 0;
    set_axis (axis_);
    min = std::numeric_limits<double>::quiet_NaN ();
    max = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  void gnuplot_drawer::range::set_axis (char axis_)
  {
    axis = axis_;
    return;
  }

  void gnuplot_drawer::range::reset ()
  {
    min = std::numeric_limits<double>::quiet_NaN ();
    max = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  void gnuplot_drawer::range::print (std::ostream & out_) const
  {
    out_ << "set " << axis << "range [" << min << ':' << max << "]";
    return;
  }

  void gnuplot_drawer::wait_for_key ()
  {
    std::clog << std::endl << "Press [Enter] to continue..." << std::endl;
    std::string s;
    std::getline(std::cin, s);
    return;
  }

  void gnuplot_drawer::set_drawing_display_data(bool ddd_)
  {
    _drawing_display_data_ = ddd_;
    return;
  }

  bool gnuplot_drawer::is_drawing_display_data() const
  {
    return _drawing_display_data_;
  }

  void gnuplot_drawer::set_view(const std::string & view_)
  {
    DT_THROW_IF(!check_view(view_), std::logic_error, "Invalid display view '" << view_ << "'!");
    _view_ = view_;
    return;
  }

  const std::string & gnuplot_drawer::get_view() const
  {
    return _view_;
  }

  void gnuplot_drawer::set_mode(const std::string & mode_)
  {
    _mode_ = mode_;
    return;
  }

  const std::string & gnuplot_drawer::get_mode() const
  {
    return _mode_;
  }

  datatools::properties & gnuplot_drawer::grab_properties()
  {
    return _props_;
  }

  const datatools::properties & gnuplot_drawer::get_properties() const
  {
    return _props_;
  }

  bool gnuplot_drawer::is_view_2d() const
  {
    return ! is_view_3d ();
  }

  bool gnuplot_drawer::is_view_3d() const
  {
    return _view_ == view_3d();
  }

  bool gnuplot_drawer::is_solid() const
  {
    return _mode_ == mode_solid();
  }

  bool gnuplot_drawer::is_wired() const
  {
    return _mode_ == mode_wired();
  }

  std::ostringstream & gnuplot_drawer::_get_stream (const std::string & section_)
  {
    cstreams_col_type::iterator i = _cstreams_.find (section_);
    if (i == _cstreams_.end ())
      {
        cstream new_cs;
        new_cs.label = section_;
        new_cs.filename = "";
        new_cs.oss = new std::ostringstream;
        new_cs.color = color::get_color(section_);
        _cstreams_[section_] = new_cs;
        return *(new_cs.oss);
      }
    cstream & cs = i->second;
    return *(cs.oss);
  }

  void gnuplot_drawer::set_labels (bool labels_)
  {
    _labels_ = labels_;
    return;
  }

  bool gnuplot_drawer::use_labels () const
  {
    return _labels_;
  }

  void gnuplot_drawer::set_using_title (bool u_)
  {
    _using_title_ = u_;
    return;
  }

  bool gnuplot_drawer::use_title () const
  {
    return _using_title_;
  }

  void gnuplot_drawer::add_display_data(const display_data & dd_)
  {
    const placement pl;
    add_display_data(dd_,pl);
  }

  void gnuplot_drawer::add_display_data(const display_data & dd_, const placement & pl_)
  {
    {
      dd_entry dummy;
      _display_data_.push_back(dummy);
    }
    dd_entry & dde = _display_data_.back();
    dde.set_placement(pl_);
    dde.set_display_data(dd_);
    return;
  }

  void gnuplot_drawer::reset_display_data()
  {
    _display_data_.clear();
    return;
  }

  void gnuplot_drawer::set_rendering_options_current(uint32_t flags_)
  {
    _rendering_options_current_ = flags_;
    return;
  }

  void gnuplot_drawer::reset_rendering_options_current()
  {
    _rendering_options_current_ = 0;
    return;
  }

  void gnuplot_drawer::set_rendering_options_depth(int32_t depth_)
  {
    if (depth_ <= 0) {
      _rendering_options_depth_ = 0;
    } else {
      _rendering_options_depth_ = depth_;
    }
    return;
  }

  void gnuplot_drawer::reset_rendering_options_depth()
  {
    _rendering_options_depth_ = 0;
    return;
  }

  gnuplot_drawer::gnuplot_drawer ()
  {
    _initialized_ = false;
    _view_ = gnuplot_drawer::default_view();
    _mode_ = gnuplot_drawer::default_mode();
    _using_title_ = true;
    _labels_ = true;
    _xrange_.set_axis ('x');
    _yrange_.set_axis ('y');
    _zrange_.set_axis ('z');
    _drawing_display_data_ = true;
    reset_output();
    reset_terminal();
    _rendering_options_current_ = 0;
    _rendering_options_depth_ = 0;
    return;
  }

  gnuplot_drawer::~gnuplot_drawer ()
  {
    reset_cstreams ();
    reset_terminal ();
    _display_data_.clear();
    return;
  }

  void gnuplot_drawer::reset_cstreams ()
  {
    if (_cstreams_.size () > 0) {
      for (cstreams_col_type::iterator i = _cstreams_.begin ();
           i != _cstreams_.end ();
           i++) {
        cstream & cs = i->second;
        if (cs.oss != nullptr) {
          cs.oss->flush ();
          delete cs.oss;
          cs.oss = nullptr;
        }
      }
      _cstreams_.clear ();
    }
    return;
  }

  void gnuplot_drawer::reset ()
  {
    _initialized_ = false;
    reset_cstreams ();
    reset_terminal ();
    reset_rendering_options_current();
    reset_rendering_options_depth();
    _view_ = gnuplot_drawer::default_view();
    _labels_ = true;
    _using_title_ = true;
    _mode_ = gnuplot_drawer::default_mode();
    _display_data_.clear();
    return;
  }

  void gnuplot_drawer::print (std::ostream & out_) const
  {
    out_ << "Gnuplot drawer : " << std::endl;
    out_ << "|-- " << "Initialized : " << _initialized_ << std::endl;
    out_ << "|-- " << "Streams     : " << _cstreams_.size() << std::endl;
    for (cstreams_col_type::const_iterator i = _cstreams_.begin();
         i != _cstreams_.end();
         i++) {
      cstreams_col_type::const_iterator j = i;
      j++;
      out_ << "|   ";
      if (j ==  _cstreams_.end()) out_ << "`-- ";
      else out_ << "|-- ";
      out_ << "Color : '" << i->first << "'" << std::endl;
    }
    out_ << "|-- " << "Properties  : " << _props_.size() << std::endl;
    out_ << "|-- " << "View        : " << _view_ << std::endl;
    out_ << "|-- " << "Labels      : " << _labels_ << std::endl;
    out_ << "|-- " << "Mode        : " << _mode_ << std::endl;
    out_ << "|-- " << "X-range     : ";
    _xrange_.print(out_);
    out_<< std::endl;
    out_ << "|-- " << "Y-range     : ";
    _yrange_.print(out_);
    out_<< std::endl;
    out_ << "|-- " << "Z-range     : ";
    _zrange_.print(out_);
    out_<< std::endl;
    out_ << "|-- " << "Terminal         : " << _terminal_ << std::endl;
    out_ << "|-- " << "Terminal options : " << _terminal_options_ << std::endl;
    out_ << "`-- " << "Output           : " << _output_ << std::endl;
    return;
  }

  void gnuplot_drawer::_draw_(const logical_volume & log_,
                              const placement & p_,
                              int max_display_level_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    char * envStr = getenv("GNUPLOT_DRAWER_DEBUG");
    if (envStr != nullptr) {
      local_priority = datatools::logger::PRIO_DEBUG;
    }
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0) {
      max_display_level = 0;
    }
    DT_LOG_DEBUG(local_priority, "max_display_level = " << max_display_level);
    const geomtools::logical_volume & log = log_;
    try {
      int display_level = 0;

      datatools::properties log_visu_config;
      visibility::extract(log.get_parameters (), log_visu_config);
      DT_LOG_DEBUG(local_priority, "Logical volume '" << log.get_name() << "' visibility properties:");
      if (datatools::logger::is_debug(local_priority)) {
        log_visu_config.tree_dump(std::cerr);
      }

      bool shown = true;
      bool shown_envelope = true;

      // Really useful ?
      if (visibility::is_shown(log_visu_config)) {
        DT_LOG_DEBUG(local_priority, "found shown directive in log visu config");
        shown = true;
      }
      if (visibility::is_hidden(log_visu_config)) {
        DT_LOG_DEBUG(local_priority, "found hidden directive in log visu config");
        shown = false;
      }
      if (get_properties().has_key(force_show_property_name())) {
        DT_LOG_DEBUG(local_priority, "found '" << force_show_property_name() << "' directive in log visu config");
        shown = get_properties().fetch_boolean(force_show_property_name());
      }

      // Add 2021-05-05 (FM)
      if (!shown) {
        DT_LOG_DEBUG(local_priority, "force show flag for logical volume " << log_.get_name());
        shown = true;
      }

      // Default envelope visibility for the logical volume:
      if (visibility::is_hidden_envelope(log_visu_config)) {
        DT_LOG_DEBUG(local_priority, "found hidden envelope directive in log visu config");
        shown_envelope = false;
      }
      // Force envelope visibility from the Gnuplot drawer:
      if (get_properties().has_key(force_show_envelope_property_name())) {
        shown_envelope = get_properties().fetch_boolean(force_show_envelope_property_name());
      }

      DT_LOG_DEBUG (local_priority, "Show         = " << shown);
      DT_LOG_DEBUG (local_priority, "Show envelope = " << shown_envelope);

      // Draw the envelope volume :
      if (shown && shown_envelope){
        DT_LOG_DEBUG (local_priority, "Wired         = " << is_wired());
        if (is_wired()) {
          std::string color_label = color::default_color();
          if (visibility::has_envelope_color(log_visu_config)) {
            // First search for a forced envelope color :
            color_label = visibility::get_envelope_color(log_visu_config);
          } else if (visibility::has_color(log_visu_config)) {
            // Then search for color :
            color_label = visibility::get_color(log_visu_config);
            DT_LOG_DEBUG(local_priority, "Found color '" << color_label
                          << "' for logical '" << log.get_name() << "'...");
          }
          if (color_label != color::transparent()) {
            std::ostringstream & colored_oss = _get_stream (color_label);
            // unsigned long mode = gnuplot_draw::MODE_NULL;
            // if (visibility::is_wired_cylinder(log_visu_config)) {
            //   mode |= gnuplot_draw::MODE_WIRED_CYLINDER;
            // }
            // Default rendering options:
            uint32_t options = 0;
            // std::cerr << "DEVEL: **** gnuplot_drawer::_draw_: "
            //           << "log = '" << log.get_name() << "'" << std::endl;
            // std::cerr << "DEVEL: **** gnuplot_drawer::_draw_: "
            //           << "_rendering_options_depth_ = "
            //           << _rendering_options_depth_ << std::endl;
            if ((_rendering_options_depth_ + 1) > 0) {
              options = _rendering_options_current_;
            }
            gnuplot_draw::draw(colored_oss,
                               p_,
                               log.get_shape(),
                               options);
          }
        } // if (is_wired())
        
      } else {
        DT_LOG_DEBUG (local_priority, "Do not draw the envelope volume");
      }

      // Draw the children volume :
      bool draw_children = true;

      // Check the display level of the geometry tree:
      if ((display_level < max_display_level)
          && (log.get_physicals().size() > 0)) {
        // default is 'drawing children':
        draw_children = true;
      } else {
        draw_children = false;
      }

      // test if it is forbidden by the visibility properties:
      if (! shown) {
        draw_children = false;
      } else if (visibility::is_daughters_hidden(log_visu_config)) {
        // Logical volume tells us not to draw its daughters by default:
        draw_children = false;
      }
      // Gnuplot drawer can force daughters' visibility:
      if (get_properties().has_key(force_show_children_property_name())) {
        draw_children = get_properties().fetch_boolean(force_show_children_property_name());
      }

      DT_LOG_DEBUG(local_priority, "Drawing children (" << log.get_physicals().size() << " items)");
      display_level++;
      for (geomtools::logical_volume::physicals_col_type::const_iterator iter
             = log.get_physicals().begin();
           iter != log.get_physicals().end();
           ++iter) {
        const physical_volume & phys = *(iter->second);
        DT_LOG_DEBUG(local_priority, "Processing child '" <<  phys.get_name() << "'...");
        const geomtools::logical_volume & log_child = phys.get_logical();
        // Default behaviour:
        bool draw_it = true;
        if (! draw_children) {
          draw_it = false;
        }
        DT_LOG_DEBUG(local_priority, "Drawing child log : " << draw_it);
         /*
        // 2016-05-04 FM: removed
        if (visibility::has_flag(log_child.get_parameters(),visibility::constants::instance().VISIBILITY_HIDDEN_FLAG)) {
          draw_it = false;
        }
        */
        std::string phys_label = i_model::extract_label_from_physical_volume_name(iter->first);
        DT_LOG_DEBUG(local_priority, "phys_label = '" << phys_label << "'");
        // Individual hidden child volume:
        int explicit_vis_key = 0;
        std::ostringstream vis_child_name_hidden_key;
        vis_child_name_hidden_key << "visibility.daughters." << phys_label << ".hidden";
        {
          DT_LOG_DEBUG(local_priority, "vis key = '" << vis_child_name_hidden_key.str() << "'");
          if (log_visu_config.has_flag(vis_child_name_hidden_key.str())) {
            DT_LOG_DEBUG(local_priority, "has vis hidden flag");
            bool hidden = log_visu_config.fetch_boolean(vis_child_name_hidden_key.str());
            if (hidden) {
              explicit_vis_key++;
              DT_LOG_DEBUG(local_priority, "vis hidden");
              draw_it = false;
            }
          }
        }
        // Individual shown child volume:
        std::ostringstream vis_child_name_shown_key;
        vis_child_name_shown_key << "visibility.daughters." << phys_label << ".shown";
        {
          DT_LOG_DEBUG(local_priority, "vis key = '" << vis_child_name_shown_key.str() << "'");
          if (log_visu_config.has_flag(vis_child_name_shown_key.str())) {
            DT_LOG_DEBUG(local_priority, "has vis shown key");
            bool local_shown = true; // log_visu_config.fetch_boolean(vis_child_name_shown_key.str());
            if (local_shown) {
              if (explicit_vis_key > 0) {
                DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                               "Logical '" << log.get_name() << ", daughter physical '" << phys_label << "' : "
                               << "Property '" << vis_child_name_shown_key.str()
                               << "' conflicts with '" << vis_child_name_hidden_key.str() << "'! Ignoring...");
                draw_it = false;
                explicit_vis_key++;
              } else {
                DT_LOG_DEBUG(local_priority, "vis shown");
                draw_it = true;
              }
            }
          }
        }
        if (! draw_it) {
          DT_LOG_DEBUG(local_priority, "Not drawn");
          continue;
        }
        const geomtools::i_placement * pp = &(phys.get_placement());
        size_t npp = pp->get_number_of_items();
        for (size_t i = 0; i < npp; i++) {
          geomtools::placement p;
          // Get placement from the daughter physical #i:
          pp->get_placement(i, p);
          if (datatools::logger::is_debug(local_priority)) {
            pp->tree_dump(std::clog, "item placement (child)", "[debug] ");
          }
          geomtools::placement pt = p;
          // compute the placement relative to the mother volume:
          p_.child_to_mother(p, pt);
          if (datatools::logger::is_debug(local_priority)) {
            pt.tree_dump(std::clog, "item placement (mother)", "[debug] ");
          }
          // Recursive invocation of the visualization data generation
          // for daughter #i:
          _rendering_options_depth_--;
          gnuplot_drawer::_draw_(log_child,
                                 pt,
                                 max_display_level - 1);
          _rendering_options_depth_++;
        }
      }
    } catch (GnuplotException& ge) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Logical '" << log.get_name() << "' : " << ge.what());
    } catch (std::exception & x) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Logical '" << log.get_name() << "' : " << x.what());
    } catch (...) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Logical '" << log.get_name() << "' : " << "Unexpected error !");
    }
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void gnuplot_drawer::draw_logical(const logical_volume & log_,
                                    const placement & p_,
                                    int max_display_level_,
                                    const std::string & title_,
                                    bool drawing_display_data_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    char * envStr = getenv("GNUPLOT_DRAWER_DEBUG");
    if (envStr != nullptr) {
      local_priority = datatools::logger::PRIO_DEBUG;
    }
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0) {
      max_display_level = 0;
    }
    DT_LOG_DEBUG(local_priority, "max_display_level = " << max_display_level);
    if (drawing_display_data_) {
      DT_LOG_DEBUG(local_priority, "drawing display data...");
      _draw_display_data(p_);
    }

    datatools::properties visu_config;
    visibility::extract (log_.get_parameters (), visu_config);
    DT_LOG_DEBUG(local_priority, "visu_config:");
    if (datatools::logger::is_debug(local_priority)) {
      visu_config.tree_dump(std::cerr, "", "[debug] ");
    }

    bool shown = true;
    //bool shown_envelope = true;
    if (visibility::is_hidden (visu_config)) {
      DT_LOG_DEBUG(local_priority, "found hidden directive in visu config");
      shown = false;
    }
    if (visibility::is_shown (visu_config)) {
      DT_LOG_DEBUG(local_priority, "found shown directive in visu config");
      shown = true;
    }
    if (visibility::is_hidden_envelope (visu_config)) {
      DT_LOG_DEBUG(local_priority, "found hidden envelope directive in visu config (unused)");
      //shown_envelope = false;
    }
    if (!shown) {
      DT_LOG_DEBUG(local_priority, "force show flag for logical volume " << log_.get_name());
      shown = true;
    }
    color::code_type color = color::COLOR_DEFAULT;
    color::code_type former_color = color::get_color(color::default_color());
    if (visibility::has_color (visu_config)) {
      color = color::get_color (visibility::get_color(visu_config));
      DT_LOG_DEBUG(local_priority, "Found color '" << visibility::get_color (visu_config)
                   << "' color is : " << color);
    }

    // Activate a drawing bounding box computer :
    gnuplot_draw::xyz_range & BB = gnuplot_draw::bounding_box(gnuplot_draw::BB_ACTION_ACTIVATE);
    // Reset it from scratch :
    BB.reset_ranges();
    gnuplot_draw::color_context().set_color_code(former_color);

    if (shown) {
      DT_LOG_DEBUG(local_priority, "shown!");
      gnuplot_drawer::_draw_(log_, p_, max_display_level);
    }

    try {
      _xrange_.min = BB.get_x_range().get_min ();
      _xrange_.max = BB.get_x_range().get_max ();
      _yrange_.min = BB.get_y_range().get_min ();
      _yrange_.max = BB.get_y_range().get_max ();
      _zrange_.min = BB.get_z_range().get_min ();
      _zrange_.max = BB.get_z_range().get_max ();
    } catch (std::exception & error) {
      DT_THROW(std::logic_error, error.what());
    }
    double dx = _xrange_.max - _xrange_.min;
    double dy = _yrange_.max - _yrange_.min;
    double dz = _zrange_.max - _zrange_.min;
    double amax = std::max (dx, dy);
    amax = std::max (amax, dz);
    amax *= 1.1;
    _xrange_.min = BB.get_x_range().get_median () - 0.5 * amax;
    _xrange_.max = BB.get_x_range().get_median () + 0.5 * amax;
    _yrange_.min = BB.get_y_range().get_median () - 0.5 * amax;
    _yrange_.max = BB.get_y_range().get_median () + 0.5 * amax;
    _zrange_.min = BB.get_z_range().get_median () - 0.5 * amax;
    _zrange_.max = BB.get_z_range().get_median () + 0.5 * amax;

    // Deactivate the drawing bounding box :
    BB.deactivate();

    for (cstreams_col_type::iterator i = _cstreams_.begin ();
         i != _cstreams_.end ();
         i++) {
      const std::string & label = i->first;
      cstream & cs = i->second;
      {
        // New method:
        datatools::temp_file tmp_file;
        tmp_file.set_remove_at_destroy(false);
        std::ostringstream oss2;
        oss2 << ".tmp_gnuplot_drawer_" << label << "_";
        tmp_file.create(".", oss2.str());
        cs.filename = tmp_file.get_filename();
        tmp_file.out() << "# Temporary file generated by the 'geomtools::gnuplot_drawer'" << std::endl;
        tmp_file.out() << "# class for visualization." << std::endl;
        tmp_file.out() << "# label='" << label << "'" << std::endl;
        tmp_file.out() << cs.oss->str ();
        tmp_file.out() << "# end of label='" << label << "'" << std::endl;
        tmp_file.out().flush();
      }
    }

    usleep(200);

    std::string view = _view_;

    Gnuplot g1 ("lines");
    std::string set_term_args = "set terminal ";
    set_term_args += _terminal_;
    if (! _terminal_options_.empty()) {
      set_term_args += " " + _terminal_options_;
    }
    // std::cerr << "DEVEL: gnuplot_drawer::draw: set_term_args = '" << set_term_args << "'" << std::endl;
    g1.cmd (set_term_args);

    if (! _output_.empty()) {
      std::string output = _output_;
      // std::cerr << "DEVEL: gnuplot_drawer::draw: output = '" << output << "'" << std::endl;
      datatools::fetch_path_with_env(output);
      std::string set_output_args = "set output ";
      set_output_args += "'" + output + "'";
      g1.cmd(set_output_args);
    }

    g1.cmd("set size ratio -1");
    if (use_title()) {
      std::ostringstream title_oss;
      if (! title_.empty()) {
        title_oss << title_;
      } else {
        title_oss << "Instance of logical volume '" << log_.get_name() << "'";
      }
      if (!title_oss.str().empty()) {
        g1.set_title(title_oss.str ());
      }
    }

    std::ostringstream cmdstr;
    int col1 = 1;
    int col2 = 2;
    int col3 = 3;
    size_t plot_dim = 2;
    bool labels = _labels_;
    if (! labels) {
      g1.cmd ("unset border");
      g1.cmd ("unset xtics");
      g1.cmd ("unset ytics");
      g1.cmd ("unset ztics");
    }
    if (view == gnuplot_drawer::view_2d_xy()) {
      if (labels) {
        g1.set_xlabel ("x").set_ylabel ("y");
        g1.cmd ("set xtics");
        g1.cmd ("set ytics");
      }
      col1 = 1;
      col2 = 2;
    }

    if (view == gnuplot_drawer::view_2d_xz()) {
      if (labels) {
        g1.set_xlabel ("x").set_ylabel ("z");
        g1.cmd ("set xtics");
        g1.cmd ("set ztics");
      }
      col1 = 1;
      col2 = 3;
    }

    if (view == gnuplot_drawer::view_2d_yz()) {
      if (labels) {
        g1.set_xlabel ("y").set_ylabel ("z");
        g1.cmd ("set ytics");
        g1.cmd ("set ztics");
      }
      col1 = 2;
      col2 = 3;
    }

    if (view == gnuplot_drawer::view_3d()) {
      plot_dim = 3;
      g1.cmd ("set view equal xyz");
      {
        std::ostringstream xr_oss;
        _xrange_.print (xr_oss);
        g1.cmd (xr_oss.str ());
      }
      {
        std::ostringstream yr_oss;
        _yrange_.print (yr_oss);
        g1.cmd (yr_oss.str ());
      }
      {
        std::ostringstream zr_oss;
        _zrange_.print (zr_oss);
        g1.cmd (zr_oss.str ());
      }
      //g1.cmd ("set iso 100, 100");
      //g1.cmd ("set view 30,60,1.");
      std::ostringstream cmd_oss;
      cmd_oss << "set xyplane at " << _zrange_.min;
      g1.cmd (cmd_oss.str ());
      if (labels) {
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        g1.cmd ("set xtics");
        g1.cmd ("set ytics");
        g1.cmd ("set ztics");
      }
      col1 = 1;
      col2 = 2;
      col3 = 3;
    }

    if (view == gnuplot_drawer::view_3d_free_scale()) {
      plot_dim = 3;
      if (labels) {
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        g1.cmd ("set xtics");
        g1.cmd ("set ytics");
        g1.cmd ("set ztics");
      }
      col1 = 1;
      col2 = 2;
      col3 = 3;
    }

    if (_cstreams_.size ()) {
      if (plot_dim == 2) {
        g1.set_grid ();
        g1.cmd ("set size ratio -1");
        cmdstr << "plot ";
      } else {
        cmdstr << "splot ";
      }
      for (cstreams_col_type::const_iterator i = _cstreams_.begin ();
           i != _cstreams_.end ();
           i++) {
        const cstream & cs = i->second;
        cmdstr << "\"" << cs.filename << "\" using " << col1 << ":" << col2;
        int local_color = cs.color;
        if (plot_dim == 3) {
          cmdstr << ":" << col3;
        }
        cmdstr << " notitle";
        bool color_rgb = false;
        std::string color_repr;
        switch (local_color) {
        case color::COLOR_BLACK:
          color_rgb = true;
          color_repr = "black";
          break;
        case color::COLOR_GREY:
          color_rgb = true;
          color_repr = "grey";
          // color_repr = "0";
          break;
        case color::COLOR_RED:
          color_rgb = true;
          color_repr = "red";
          break;
        case color::COLOR_GREEN:
          color_rgb = true;
          color_repr = "green";
          break;
        case color::COLOR_BLUE:
          color_rgb = true;
          color_repr = "blue";
          break;
        case color::COLOR_MAGENTA:
          color_rgb = true;
          color_repr = "magenta";
          break;
        case color::COLOR_CYAN:
          color_rgb = true;
          color_repr = "cyan";
          break;
        case color::COLOR_YELLOW:
          color_rgb = true;
          color_repr = "yellow";
          break;
        case color::COLOR_BROWN:
          color_rgb = true;
          color_repr = "brown";
          break;
        case color::COLOR_ORANGE:
          color_rgb = true;
          color_repr = "orange";
          break;
        default:
          color_repr = "0";
        }
        cmdstr << " with lines lt ";
        if (color_rgb) {
          cmdstr << "rgb \"";
        }
        cmdstr << color_repr;
        if (color_rgb) {
          cmdstr << '\"';
        }
        cmdstr << ' ';
        cmdstr << "lw " << 0.5 << ' ' ;

        // if there is another one:
        {
          cstreams_col_type::const_iterator j = i;
          j++;
          if (j != _cstreams_.end ()) {
            cmdstr << ", ";
          }
        }
      }
    }
    DT_LOG_TRACE (local_priority, "GNUPLOT command is :" << cmdstr.str());
    //std::cerr << "DEVEL: " << "GNUPLOT command is :" << cmdstr.str() << std::endl;
    if (! cmdstr.str().empty()) {
      g1.cmd (cmdstr.str());
      if (! _output_.empty()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "File '" << _output_ << "' was generated.");
      }
      g1.showonscreen(); // window output
      wait_for_key();
      usleep(200);
    } else {
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Nothing to display !");
    }

    // remove tmp files:
    for (cstreams_col_type::iterator i = _cstreams_.begin();
         i != _cstreams_.end();
         i++) {
      cstream & cs = i->second;
      if (boost::filesystem::exists(cs.filename)) {
        // unlink(cs.filename.c_str());
        boost::filesystem::remove(cs.filename);
        //std::cerr << "DEVEL: " << "file " << cs.filename << " was deleted !" << std::endl;
      }
    }
    DT_LOG_TRACE(local_priority, "reset_cstreams...");
    reset_cstreams();
    return;
  }

  /****************************************************/

  void gnuplot_drawer::draw_logical(const model_factory & mf_,
                                    const std::string & logical_name_,
                                    const placement & p_,
                                    int max_display_level_)
  {
    geomtools::logical_volume::dict_type::const_iterator found;
    found = mf_.get_logicals ().find (logical_name_);
    DT_THROW_IF (found ==  mf_.get_logicals ().end (),
                 std::logic_error,
                 "Cannot find logical volume with name '" << logical_name_ << "' !");
    const geomtools::logical_volume & log = *(found->second);
    std::ostringstream title_oss;
    title_oss << "Logical volume '" << log.get_name () << "'";
    draw_logical(log, p_, max_display_level_, title_oss.str(), false);
    return;
  }

  int gnuplot_drawer::draw(const manager & geo_mgr_,
                           const std::string & what_,
                           int max_display_level_)
  {
    std::string what = what_;
    if (what.empty()) {
      what = geo_mgr_.get_world_name();
    }
    DT_THROW_IF(what.empty(), std::logic_error,
                "Missing object label to be displayed !");
    std::istringstream visu_gid_iss(what);
    geomtools::geom_id visu_gid;
    visu_gid_iss >> visu_gid;
    if (visu_gid_iss) {
      if (! geo_mgr_.get_mapping().validate_id(visu_gid)) {
        std::ostringstream message;
        message << "geomtools::gnuplot_drawer::draw: "
                << "Cannot find geometry volume with GID='" << visu_gid << "' in mapping dictionnary !";
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot find geometry volume with GID='" << visu_gid << "' in mapping dictionnary !");
        return EXIT_FAILURE;
      }
      draw_physical_from_gid(geo_mgr_.get_factory(),
                             visu_gid,
                             geo_mgr_.get_mapping(),
                             max_display_level_);
    } else {
      //DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG,"Label '" << what << "' is not a GID !");
      // Try to draw logical:
      const logical_volume::dict_type & logs = geo_mgr_.get_factory().get_logicals();
      const std::string & visu_log_name = what;
      logical_volume::dict_type::const_iterator log_found = logs.find(visu_log_name);
      if (log_found != logs.end()) {
        geomtools::placement visu_placement;
        draw_logical(*log_found->second,
                     visu_placement,
                     max_display_level_,
                     "",
                     false);
      } else {
        // Try to draw model:
        geomtools::placement visu_placement;
        const std::string & visu_model_name = what;
        models_col_type::const_iterator found = geo_mgr_.get_factory().get_models().find(visu_model_name);
        if (found == geo_mgr_.get_factory().get_models().end()) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Cannot find geometry model with name='" << visu_model_name
                       << "' in the geometry model factory !");
          return EXIT_FAILURE;
        }
        draw_model(geo_mgr_.get_factory(),
                   visu_model_name,
                   visu_placement,
                   max_display_level_);
      }
    }
    return EXIT_SUCCESS;
  }

  void gnuplot_drawer::draw_physical_from_gid(const model_factory & mf_,
                                              const geom_id & gid_,
                                              const mapping & mapping_,
                                              int max_display_level_)
  {
    DT_THROW_IF(! mapping_.validate_id(gid_),
                 std::logic_error,
                 "Cannot find object with GID='" << gid_ << "' in the mapping dictionnary !");
    const geom_info & ginfo = mapping_.get_geom_info(gid_);
    const placement & wpl = ginfo.get_world_placement();
    const logical_volume & log = ginfo.get_logical();
    if (_drawing_display_data_) {
      _draw_display_data(mf_, wpl);
    }
    std::ostringstream title_oss;
    title_oss << "Volume " << gid_ << " (instance of logical volume '"
              << log.get_name() << "')";
    draw_logical(log, wpl, max_display_level_, title_oss.str(), false);
    return;
  }

  void gnuplot_drawer::draw_model(const model_factory & mf_,
                                  const std::string & name_,
                                  const placement & p_,
                                  int max_display_level_)
  {
    models_col_type::const_iterator found = mf_.get_models().find(name_);
    DT_THROW_IF(found ==  mf_.get_models().end(),
                 std::logic_error,
                 "Cannot find geometry model with name '" << name_ << "' !");
    const i_model & model = *(found->second);
    const geomtools::logical_volume & log = model.get_logical();
    std::string world_name;
    if (get_properties().has_key(gnuplot_drawer::world_name_key())
        && get_properties().is_string(gnuplot_drawer::world_name_key())) {
      world_name = get_properties().fetch_string(gnuplot_drawer::world_name_key());
    }
    if (world_name.empty()) {
      world_name = geomtools::model_factory::default_world_label();
    }
    if (_drawing_display_data_ && (name_ == world_name)) {
      _draw_display_data(mf_, p_);
    }
    draw_logical(log, p_, max_display_level_, log.get_name(), false);
    return;
  }

  void gnuplot_drawer::_draw_display_data(const model_factory & /*mf_*/,
                                          const placement & /*p_*/)
  {
    const datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    bool draw_static = true;
    int draw_frame_index = -1;
    std::string draw_group;
    std::string draw_name;
    for (size_t idd = 0; idd < _display_data_.size(); idd++) {
      const display_data & dd = _display_data_[idd].get_display_data();
      const placement & dd_pl = _display_data_[idd].get_placement();

      DT_LOG_TRACE (local_priority, "Display data:");
      if (local_priority >= datatools::logger::PRIO_TRACE) dd.tree_dump(std::cerr);
      for (size_t icolor = 0; icolor < dd.get_colors().size(); icolor++) {
        const std::string & draw_color = dd.get_colors()[icolor];
        DT_LOG_TRACE (local_priority, "Color = " << draw_color);
        std::ostringstream & colored_out = _get_stream(draw_color);
        geomtools::gnuplot_draw::draw_display_data(colored_out,
                                                   dd_pl.get_translation(),
                                                   dd_pl.get_rotation(),
                                                   dd,
                                                   draw_static,
                                                   draw_frame_index,
                                                   draw_color,
                                                   draw_group,
                                                   draw_name);
      }
    }
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void gnuplot_drawer::_draw_display_data(const placement & p_)
  {
    const datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    bool draw_static = true;
    int draw_frame_index = -1;
    std::string draw_group;
    std::string draw_name;
    for (size_t idd = 0; idd < _display_data_.size(); idd++) {
      const display_data & dd = _display_data_[idd].get_display_data();
      const placement & dd_pl = _display_data_[idd].get_placement();
      placement eff_pl;
      p_.child_to_mother(dd_pl, eff_pl);

      DT_LOG_TRACE (local_priority, "Display data:");
      if (local_priority >= datatools::logger::PRIO_TRACE) dd.tree_dump(std::cerr);
      for (size_t icolor = 0; icolor < dd.get_colors().size(); icolor++) {
        const std::string & draw_color = dd.get_colors()[icolor];
        DT_LOG_TRACE (local_priority, "Color = " << draw_color);
        std::ostringstream & colored_out = _get_stream(draw_color);
        geomtools::gnuplot_draw::draw_display_data(colored_out,
                                                   eff_pl.get_translation(),
                                                   eff_pl.get_rotation(),
                                                   dd,
                                                   draw_static,
                                                   draw_frame_index,
                                                   draw_color,
                                                   draw_group,
                                                   draw_name);
      }
    }
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void gnuplot_drawer::set_output (const std::string & output_)
  {
    _output_ = output_;
    return;
  }

  void gnuplot_drawer::reset_output ()
  {
    _output_.clear();
    return;
  }

  void gnuplot_drawer::set_terminal (const std::string & terminal_,
                                     const std::string & terminal_options_)
  {
    // std::cerr << "DEVEL: gnuplot_drawer::set_terminal: terminal = '" << terminal_ << "'" << std::endl;
    // std::cerr << "DEVEL: gnuplot_drawer::set_terminal: terminal options = '" << terminal_options_ << "'" << std::endl;
    _terminal_ = terminal_;
    _terminal_options_ = terminal_options_;
    return;
  }

  void gnuplot_drawer::reset_terminal ()
  {
    reset_output();
    _terminal_ = Gnuplot::terminal_std();
    _terminal_options_.clear();
    // if (_terminal_ == "x11") _terminal_options_ = "noenhanced";
    // _terminal_options_ = "persist noenhanced size 500,500";
    return;
  }

  int gnuplot_drawer::set_output_medium(const std::string & file_,
                                        const std::string & terminal_,
                                        const std::string & terminal_options_)
  {
    std::string requested_terminal = terminal_;
    std::string requested_terminal_options = terminal_options_;
    std::string output;
    std::string terminal;
    std::string terminal_options;
    if (! file_.empty()) {
      std::string file = file_;
      datatools::fetch_path_with_env(file);
      output = file;
      boost::filesystem::path output_pth = file;
      boost::filesystem::path output_ext = output_pth.extension();
      if (output_ext.empty()) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot extract terminal type from file '" << file_ << "' !");
        return 1;
      }
      const std::string ext = output_ext.string();
      if (ext == ".jpeg" || ext == ".jpg") {
        terminal = "jpeg";
        terminal_options = "noenhanced nointerlace large size 500,500 background rgb \"#FFFFFF\"";
      } else if (ext == ".png") {
        terminal = "png";
        terminal_options = "noenhanced notransparent nointerlace medium background rgb \"#FFFFFF\"";
      } else if (ext == ".eps") {
        terminal = "postscript";
        terminal_options = "eps noenhanced color background rgb \"#FFFFFF\" size 15cm,15cm";
      } else if (ext == ".fig") {
        terminal = "fig";
        terminal_options = "color landscape size 20 20 metric pointsmax 1000 solid textspecial depth 50";
      } else if (ext == ".svg") {
        terminal = "svg";
        terminal_options = "size 20,20 dynamic noenhanced solid background rgb \"#FFFFFF\"";
      } else {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot extract terminal type from file '" << file_ << "' !");
        return 1;
      }
    }
    // std::cerr << "DEVEL:  gnuplot_drawer::set_output_medium: terminal = '" << terminal << "'" << std::endl;
    if (! terminal_.empty()) {
      if (terminal != terminal_) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Guessed terminal '" << terminal << "' does not match "
                     << "requested terminal '" << terminal_ << "' !");
        return 1;
      }
    }

    if (! terminal_options_.empty()) {
      terminal_options = terminal_options_;
    }
    if (terminal == "x11") {
      reset_terminal();
      reset_output();
    } else {
      set_output(output);
      set_terminal(terminal, terminal_options);
    }
    return 0;
  }

} // end of namespace geomtools
