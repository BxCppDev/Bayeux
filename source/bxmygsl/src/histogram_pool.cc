// histogram_pool.cc

#include <mygsl/histogram_pool.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

namespace mygsl {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(histogram_pool::histogram_entry_type,"mygsl::histogram_pool::histogram_entry_type")

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(histogram_pool,"mygsl::histogram_pool")

  const std::string histogram_pool::_INITIALIZED_FLAG_KEY_ = "__initialized";

  histogram_pool::histogram_entry_type::histogram_entry_type()
  {
    dimension = HISTOGRAM_DIM_UNDEFINED;
    return;
  }

  histogram_pool::histogram_entry_type::~histogram_entry_type()
  {
    return;
  }

  void histogram_pool::histogram_entry_type::tree_dump(std::ostream& out_,
                                                       const std::string& title_,
                                                       const std::string& indent_,
                                                       bool inherit_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    if (!title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name  : '" << name << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Title  : '" << title << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Dimension  : " << dimension << std::endl;
    if (dimension == HISTOGRAM_DIM_1D) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Histogram 1D : " << &(hh1d.get());
      if (hh1d.get().is_initialized()) {
        out_ << " has " << hh1d.get().counts() << " counts within "
             << hh1d.get().bins() << " bins" << std::endl;;
        //hh1d.get().tree_dump(out_, "", indent);
      } else {
        out_ << " (not initialized)" << std::endl;;
      }
    }
    if (dimension == HISTOGRAM_DIM_2D) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Histogram 2D : " << &(hh2d.get());
      if (hh2d.get().is_initialized()) {
        out_ << " has " << hh2d.get().counts() << " counts within "
             << hh2d.get().xbins() << '*' <<  hh2d.get().ybins() << " bins" << std::endl;
        //hh2d.get().tree_dump(out_, "", indent);
      } else {
        out_ << " (not initialized)" << std::endl;
      }
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Group  : '" << group << "'" << std::endl;

    // out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
    //       << "Auxiliaries : ";
    // if (auxiliaries.empty())
    //   {
    //     out_ << "<empty>";
    //   }
    // out_ << endl;
    // {
    //   ostringstream indent_oss;
    //   indent_oss << indent;
    //   indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
    //   auxiliaries.tree_dump(out_, "", indent_oss.str());
    // }

    return;
  }

  /************************************************************************/

  void histogram_pool::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
  }

  datatools::logger::priority histogram_pool::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool histogram_pool::is_initialized() const
  {
    return _auxiliaries_.has_flag(_INITIALIZED_FLAG_KEY_);
  }

  const std::string & histogram_pool::get_description() const
  {
    return _description_;
  }

  void histogram_pool::set_description(const std::string & desc_)
  {
    _description_ = desc_;
    return;
  }

  const datatools::properties & histogram_pool::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & histogram_pool::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  histogram_pool::histogram_pool()
  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    return;
  }

  histogram_pool::histogram_pool(const std::string & desc_)
  {
    _logging_priority_ =  datatools::logger::PRIO_WARNING;
    set_description(desc_);
    return;
  }

  histogram_pool::~histogram_pool()
  {
    reset();
    return;
  }

  void histogram_pool::load(const std::string & histo_setups_filename_)
  {
    datatools::multi_properties histo_setups;
    std::string hsf = histo_setups_filename_;
    datatools::fetch_path_with_env(hsf);
    datatools::read_config(hsf, histo_setups);
    load(histo_setups);
    return;
  }

  void histogram_pool::load(const datatools::multi_properties & histo_setups_)
  {
    const datatools::multi_properties::entries_ordered_col_type& hentries = histo_setups_.ordered_entries();
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator
           iter = hentries.begin(); iter != hentries.end(); iter++) {
      const datatools::multi_properties::entry & hentry = *(*iter);
      std::string histo_name = hentry.get_key();
      std::string histo_type = hentry.get_meta();
      DT_LOG_NOTICE(get_logging_priority(), "Loading histogram '"
                    << histo_name << "' of type '"
                    << histo_type << "'");
      const datatools::properties& histo_params = hentry.get_properties();
      std::string histo_title;
      std::string histo_group;
      if (histo_params.has_key("group")) {
        histo_group = histo_params.fetch_string("group");
      }
      if (histo_params.has_key("title")) {
        histo_title = histo_params.fetch_string("title");
      }
      if (histo_type == "mygsl::histogram_1d") {
        mygsl::histogram_1d & h_1d = this->add_1d(histo_name);
        init_histo_1d(h_1d, histo_params, this);
        if (! histo_title.empty()) {
          h_1d.grab_auxiliaries().update("display.title", histo_title);
        }
      } else if (histo_type == "mygsl::histogram_2d") {
        mygsl::histogram_2d & h_2d = this->add_2d(histo_name);
        init_histo_2d(h_2d, histo_params, this);
        if (! histo_title.empty()) {
          h_2d.grab_auxiliaries().update("display.title", histo_title);
        }
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid histogram class name '" << histo_type << "' !");
      }
      // Add traits :
      this->set_title(histo_name, histo_title);
      this->set_group(histo_name, histo_group);
    }
    return;
  }

  // static
  void histogram_pool::init_histo_1d(histogram_1d & h1_,
                                     const datatools::properties& h1_setup_,
                                     const histogram_pool* histo_pool_)
  {
    if (h1_.is_initialized()) {
      h1_.reset();
    }
    std::string mode;
    if (h1_setup_.has_key("mode")) {
      mode = h1_setup_.fetch_string("mode");
      DT_THROW_IF(mode != "regular" && mode != "table" && mode != "mimic",
                  std::logic_error,
                  "Invalid 1D-histogram build mode '" << mode << "' !");
    } else {
      DT_THROW_IF(true, std::logic_error, "Missing 1D-histogram build mode property !");
    }

    std::string xunit_type;  // The physical dimension of the X axis binning
    std::string xunit_str;   // The unit symbol of the X axis
    double      xunit = 1.0; // The unit intrinsic value of the X axis

    // Store all display properties :
    h1_setup_.export_starting_with(h1_.grab_auxiliaries(), "display.");

    if (histo_pool_->_auxiliaries_.has_key("histo.export_prefixes")) {
      std::vector<std::string> export_prefixes;
      histo_pool_->_auxiliaries_.fetch("histo.export_prefixes", export_prefixes);
      for (size_t i = 0; i < export_prefixes.size(); i++) {
        const std::string & aux_prefix = export_prefixes[i];
        h1_setup_.export_starting_with(h1_.grab_auxiliaries(), aux_prefix);
      }
    }

    // Extract the unit used for X axis bins :
    if (h1_setup_.has_key("unit")) {
      xunit_str = h1_setup_.fetch_string("unit");
      DT_THROW_IF(! datatools::units::find_unit(xunit_str, xunit, xunit_type),
                  std::logic_error,
                  "Invalid X binning unit ('" << xunit_str << "') !");
    }

    if (mode == "regular" || mode == "table") {
      // Extract the physical dimension for the X axis :
      if (h1_setup_.has_key("unit.type")) {
        std::string xunit_type2 = h1_setup_.fetch_string("unit.type");
        DT_THROW_IF(! datatools::units::is_unit_label_valid(xunit_type2),
                    std::logic_error,
                    "Invalid X binning unit type ('" << xunit_type << "') !");
        DT_THROW_IF(! xunit_type.empty() && xunit_type2 != xunit_type,
                    std::logic_error,
                    "Incompatible unit type ('" << xunit_type2
                    << "') with bins unit ('" << xunit_type << "') !");
      }

      // Extract the optional display unit for X axis and check it :
      if (h1_setup_.has_key("display.xaxis.unit")) {
        std::string display_xunit_str = h1_setup_.fetch_string("display.xaxis.unit");
        std::string display_xunit_type;
        double display_xunit = 1.0;
        if (datatools::units::find_unit(display_xunit_str, display_xunit, display_xunit_type)) {
          if (xunit_type.empty()) {
            xunit_type = display_xunit_type;
          } else {
            DT_THROW_IF(display_xunit_type != xunit_type,
                        std::logic_error,
                        "X axis display unit type ('" << display_xunit_type << "') does not match the X axis unit !");
          }
        }
      } else {
        // Default display unit for X axis :
        if (! xunit_str.empty()) {
          h1_.grab_auxiliaries().update("display.xaxis.unit", xunit_str);
        }
      }
    }

    if (! xunit_type.empty()) {
      h1_.grab_auxiliaries().store("unit.type", xunit_type);
    }

    if (mode == "regular") {
      double xmin, xmax;
      int nxbins = -1;
      int xbinmode = BIN_MODE_INVALID;
      datatools::invalidate(xmin);
      datatools::invalidate(xmax);

      if (h1_setup_.has_flag("linear")) {
        xbinmode = BIN_MODE_LINEAR;
      }

      if (h1_setup_.has_flag("logarithmic")) {
        xbinmode = BIN_MODE_LOG;
      }

      if (xbinmode == BIN_MODE_INVALID) {
        xbinmode = BIN_MODE_LINEAR;
      }

      if (h1_setup_.has_key("min")) {
        xmin = h1_setup_.fetch_real("min");
      }

      if (h1_setup_.has_key("max")) {
        xmax = h1_setup_.fetch_real("max");
      }

      if (! datatools::is_valid(xmin)) {
        if (xmax > 0.0) {
          xmin = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 1D-histogram X range [" << xmin << ":" << xmax << "[ !");
        }
      }

      if (! datatools::is_valid(xmax)) {
        if (xmin < 0.0) {
          xmax = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 1D-histogram X range [" << xmin << ":" << xmax << "[ !");
        }
      }
      if (! h1_setup_.has_explicit_unit("min")) {
        xmin *= xunit;
      }
      if (! h1_setup_.has_explicit_unit("max")) {
        xmax *= xunit;
      }

      if (h1_setup_.has_key("number_of_bins")) {
        nxbins = h1_setup_.fetch_integer("number_of_bins");
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing number of bins in 1D-histogram !");
      }
      DT_THROW_IF(nxbins < 1, std::logic_error,"Invalid number of bins ('" << nxbins << "') in 1D-histogram !");
      h1_.initialize(nxbins, xmin, xmax, xbinmode);
    } else if (mode == "table") {
      std::vector<double> xranges;
      if (h1_setup_.has_key("bounds")) {
        h1_setup_.fetch("bounds", xranges);
        if (! h1_setup_.has_explicit_unit("bounds")) {
          for (size_t i = 0; i < xranges.size(); i++) {
            xranges[i] *= xunit;
          }
        }
      } else if (h1_setup_.has_key("bounds.file")) {
        std::string bounds_file;
        bounds_file = h1_setup_.fetch_string("bounds.file");

        std::string bounds_file2 = bounds_file;
        datatools::fetch_path_with_env(bounds_file2);
        std::ifstream bounds_ifs(bounds_file2.c_str());
        DT_THROW_IF(! bounds_ifs,
                    std::logic_error,
                    "Cannot open file '" << bounds_file2 << "' !");
        while (bounds_ifs) {
          double bound;
          bounds_ifs >> bound;
          DT_THROW_IF(! bounds_ifs,
                      std::logic_error,
                      "Invalid format in file '" << bounds_file << "' !");
          xranges.push_back(bound * xunit);
          bounds_ifs >> std::ws;
          if (bounds_ifs.eof()) {
            break;
          }
        }
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "No rule was given to setup the bins (bounds, bounds.file) !");
      }
      DT_THROW_IF(xranges.size() < 2,
                  std::logic_error,
                  "Invalid number of bins ('" << xranges.size() << "') in 1D-histogram !");
      h1_.initialize(xranges);
    } else if (mode == "mimic") {
      // extract information about another histogram :
      // 1D clone
      // 2D use X or Y
      DT_THROW_IF(histo_pool_ == 0,
                  std::logic_error,
                  "Missing pool of histograms !");
      if (h1_setup_.has_key("mimic.histogram_1d")) {
        // Copy the structure of another 1D histogram :
        std::string h1d_name = h1_setup_.fetch_string("mimic.histogram_1d");

        DT_THROW_IF(! histo_pool_->has(h1d_name, histogram_pool::HISTOGRAM_DIM_1D),
                    std::logic_error,
                    "No 1D-histogram named '" << h1d_name << "' does exist !");
        const histogram & mimic_h1 = histo_pool_->get_1d(h1d_name);
        std::vector<std::string> import_prop_prefixes;
        if (h1_setup_.has_flag("mimic.histogram_1d.import_aux")) {
          h1_setup_.fetch("mimic.histogram_1d.import_aux", import_prop_prefixes);
        }
        h1_.initialize(mimic_h1, import_prop_prefixes);
      } else if (h1_setup_.has_key("mimic.histogram_2d")) {
        // Copy the structure of one of the axis of a 2D histogram :
        std::string h2d_name = h1_setup_.fetch_string("mimic.histogram_2d");
        DT_THROW_IF(! histo_pool_->has(h2d_name, histogram_pool::HISTOGRAM_DIM_2D),
                    std::logic_error,
                    "No 2D-histogram named '" << h2d_name << "' does exist !");
        const histogram_2d & mimic_h2 = histo_pool_->get_2d(h2d_name);
        int bin_axis = BIN_AXIS_INVALID;
        // Extract the axis of the 2D histogram to copy :
        if (h1_setup_.has_key("mimic.histogram_2d.axis")) {
          std::string axis_label = h1_setup_.fetch_string("mimic.histogram_2d.axis");
          if (axis_label == "x") {
            bin_axis = BIN_AXIS_X;
          } else if (axis_label == "y") {
            bin_axis = BIN_AXIS_Y;
          } else {
            DT_THROW_IF(true, std::logic_error, "Invalid bin axis label '" << axis_label << "' !");
          }
        } else {
          DT_THROW_IF(true, std::logic_error, "Missing bin axis property '" << "mimic.histogram_2d.axis" << "' !");
        }
        std::vector<std::string> import_aux_prefixes;
        if (h1_setup_.has_flag("mimic.histogram_2d.import_aux")) {
          h1_setup_.fetch("mimic.histogram_2d.import_aux", import_aux_prefixes);
        }
        h1_.initialize(mimic_h2, bin_axis, import_aux_prefixes);
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing mimic mode !");
      }
    }
    return;
  }

  // static
  void histogram_pool::init_histo_2d(histogram_2d & h2_,
                                     const datatools::properties& h2_setup_,
                                     const histogram_pool* histo_pool_ )
  {
    if (h2_.is_initialized()) {
      h2_.reset();
    }

    std::string mode;
    if (h2_setup_.has_key("mode")) {
      mode = h2_setup_.fetch_string("mode");
      DT_THROW_IF(mode != "regular" && mode != "table" && mode != "mimic",
                  std::logic_error,
                  "Invalid 2D-histogram build mode '" << mode << "' !");
    } else {
      DT_THROW_IF(true, std::logic_error, "Missing 2D-histogram build mode !");
    }

    std::string xunit_type;  // The physical dimension of the X axis binning
    std::string xunit_str;   // The unit symbol of the X axis
    double      xunit = 1.0; // The unit intrinsic value of the X axis

    std::string yunit_type;  // The physical dimension of the Y axis binning
    std::string yunit_str;   // The unit symbol of the Y axis
    double      yunit = 1.0; // The unit intrinsic value of the Y axis

    // Store all display properties :
    h2_setup_.export_starting_with(h2_.grab_auxiliaries(), "display.");

    if (histo_pool_->_auxiliaries_.has_key("histo.export_prefixes")) {
      std::vector<std::string> export_prefixes;
      histo_pool_->_auxiliaries_.fetch("histo.export_prefixes", export_prefixes);
      for (size_t i = 0; i < export_prefixes.size(); i++) {
        const std::string & aux_prefix = export_prefixes[i];
        h2_setup_.export_starting_with(h2_.grab_auxiliaries(), aux_prefix);
      }
    }

    // Extract the unit used for X axis bins :
    if (h2_setup_.has_key("x.unit")) {
      xunit_str = h2_setup_.fetch_string("x.unit");
      DT_THROW_IF(! datatools::units::find_unit(xunit_str, xunit, xunit_type),
                  std::logic_error,
                  "Invalid X binning unit ('" << xunit_str << "') !");
    }
    // Extract the unit used for Y axis bins :
    if (h2_setup_.has_key("y.unit")) {
      yunit_str = h2_setup_.fetch_string("y.unit");
      DT_THROW_IF(! datatools::units::find_unit(yunit_str, yunit, yunit_type),
                  std::logic_error,
                  "Invalid Y binning unit ('" << yunit_str << "') !");
    }

    if (mode == "regular" || mode == "table") {
      // Extract the physical dimension for the X axis :
      if (h2_setup_.has_key("x.unit.type")) {
        std::string xunit_type2 = h2_setup_.fetch_string("x.unit.type");
        DT_THROW_IF(! datatools::units::is_unit_label_valid(xunit_type2),
                    std::logic_error,
                    "Invalid X binning unit type ('" << xunit_type << "') !");
        DT_THROW_IF(! xunit_type.empty() && xunit_type2 != xunit_type,
                    std::logic_error,
                    "Incompatible X unit type ('" << xunit_type2
                    << "') with X bins unit ('" << xunit_type << "') !");
      }

      // Extract the physical dimension for the Y axis :
      if (h2_setup_.has_key("y.unit.type")) {
        std::string yunit_type2 = h2_setup_.fetch_string("y.unit.type");
        DT_THROW_IF(! datatools::units::is_unit_label_valid(yunit_type2),
                    std::logic_error,
                    "Invalid Y binning unit type ('" << yunit_type << "') !");
        DT_THROW_IF(! yunit_type.empty() && yunit_type2 != yunit_type,
                    std::logic_error,
                    "Incompatible Y unit type ('" << yunit_type2
                    << "') with Y bins unit ('" << yunit_type << "') !");
      }

      // Extract the optional display unit for X axis and check it :
      if (h2_setup_.has_key("display.xaxis.unit")) {
        std::string display_xunit_str = h2_setup_.fetch_string("display.xaxis.unit");
        std::string display_xunit_type;
        double display_xunit = 1.0;
        if (datatools::units::find_unit(display_xunit_str, display_xunit, display_xunit_type)) {
          if (xunit_type.empty()) {
            xunit_type = display_xunit_type;
          } else {
            DT_THROW_IF(display_xunit_type != xunit_type,
                        std::logic_error,
                        "X axis display unit type ('" << display_xunit_type << "') does not match the X axis unit !");
          }
        }
      } else {
        // Default display unit for X axis :
        if (! xunit_str.empty()) {
          h2_.grab_auxiliaries().update("display.xaxis.unit", xunit_str);
        }
      }

      // Extract the optional display unit for Y axis and check it :
      if (h2_setup_.has_key("display.yaxis.unit")) {
        std::string display_yunit_str = h2_setup_.fetch_string("display.yaxis.unit");
        std::string display_yunit_type;
        double display_yunit = 1.0;
        if (datatools::units::find_unit(display_yunit_str, display_yunit, display_yunit_type)) {
          if (yunit_type.empty()) {
            yunit_type = display_yunit_type;
          } else {
            DT_THROW_IF(display_yunit_type != yunit_type,
                        std::logic_error,
                        "Y axis display unit type ('" << display_yunit_type << "') does not match the Y axis unit !");
          }
        }
      } else {
        // Default display unit for Y axis :
        if (! yunit_str.empty()) {
          h2_.grab_auxiliaries().update("display.yaxis.unit", yunit_str);
        }
      }
    }

    if (! xunit_type.empty()) {
      h2_.grab_auxiliaries().store("x.unit.type", xunit_type);
    }

    if (! yunit_type.empty()) {
      h2_.grab_auxiliaries().store("y.unit.type", yunit_type);
    }

    if (mode == "regular") {
      double xmin, xmax;
      int nxbins = -1;
      int xbinmode = BIN_MODE_INVALID;
      datatools::invalidate(xmin);
      datatools::invalidate(xmax);
      double ymin, ymax;
      int nybins = -1;
      int ybinmode = BIN_MODE_INVALID;
      datatools::invalidate(ymin);
      datatools::invalidate(ymax);

      if (h2_setup_.has_flag("x.linear")) {
        xbinmode = BIN_MODE_LINEAR;
      }
      if (h2_setup_.has_flag("x.logarithmic")) {
        xbinmode = BIN_MODE_LOG;
      }
      if (xbinmode == BIN_MODE_INVALID) {
        xbinmode = BIN_MODE_LINEAR;
      }

      if (h2_setup_.has_flag("y.linear")) {
        ybinmode = BIN_MODE_LINEAR;
      }
      if (h2_setup_.has_flag("y.logarithmic")) {
        ybinmode = BIN_MODE_LOG;
      }
      if (ybinmode == BIN_MODE_INVALID) {
        ybinmode = BIN_MODE_LINEAR;
      }

      if (h2_setup_.has_key("x.min")) {
        xmin = h2_setup_.fetch_real("x.min");
      }

      if (h2_setup_.has_key("x.max")) {
        xmax = h2_setup_.fetch_real("x.max");
      }

      if (! datatools::is_valid(xmin)) {
        if (xmax > 0.0) {
          xmin = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 2D-histogram X range [" << xmin << ":" << xmax << "[ !");
        }
      }

      if (! datatools::is_valid(xmax)) {
        if (xmin < 0.0) {
          xmax = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 2D-histogram X range [" << xmin << ":" << xmax << "[ !");
        }
      }

      if (h2_setup_.has_key("y.min")) {
        ymin = h2_setup_.fetch_real("y.min");
      }

      if (h2_setup_.has_key("y.max")) {
        ymax = h2_setup_.fetch_real("y.max");
      }

      if (! datatools::is_valid(ymin)) {
        if (ymax > 0.0) {
          ymin = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 2D-histogram Y range [" << ymin << ":" << ymax << "[ !");
        }
      }

      if (! datatools::is_valid(ymax)) {
        if (ymin < 0.0) {
          ymax = 0.0;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid 2D-histogram Y range [" << ymin << ":" << ymax << "[ !");
        }
      }

      if (!h2_setup_.has_explicit_unit("x.max")) {
        xmax *= xunit;
      }
      if (!h2_setup_.has_explicit_unit("x.min")) {
        xmin *= xunit;
      }
      if (!h2_setup_.has_explicit_unit("y.min")) {
        ymin *= yunit;
      }
      if (!h2_setup_.has_explicit_unit("y.max")) {
        ymax *= yunit;
      }

      if (h2_setup_.has_key("x.number_of_bins")) {
        nxbins = h2_setup_.fetch_integer("x.number_of_bins");
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing number of X bins in 2D-histogram !");
      }
      DT_THROW_IF(nxbins < 1, std::logic_error, "Invalid number of X bins ('" << nxbins << "') in 2D-histogram !");
      if (h2_setup_.has_key("y.number_of_bins")) {
        nybins = h2_setup_.fetch_integer("y.number_of_bins");
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing number of Y bins in 2D-histogram !");
      }
      DT_THROW_IF(nybins < 1, std::logic_error,"Invalid number of Y bins ('" << nybins << "') in 2D-histogram !)");
      h2_.initialize(nxbins, xmin, xmax,
                     nybins, ymin, ymax,
                     xbinmode, ybinmode);
    } else if (mode == "table") {
      std::vector<double> xranges;
      std::vector<double> yranges;
      if (h2_setup_.has_key("x.bounds")) {
        h2_setup_.fetch("x.bounds", xranges);
        if (! h2_setup_.has_explicit_unit("x.bounds")) {
          for (size_t i = 0; i < xranges.size(); i++) {
            xranges[i] *= xunit;
          }
        }
      }
      if (h2_setup_.has_key("y.bounds")) {
        h2_setup_.fetch("y.bounds", yranges);
        if (! h2_setup_.has_explicit_unit("y.bounds")) {
          for (size_t i = 0; i < yranges.size(); i++) {
            yranges[i] *= yunit;
          }
        }
      }
      if (xranges.size() == 0 && h2_setup_.has_key("x.bounds.file")) {
        std::string xbounds_file;
        xbounds_file = h2_setup_.fetch_string("x.bounds.file");

        std::string xbounds_file2 = xbounds_file;
        datatools::fetch_path_with_env(xbounds_file2);
        std::ifstream xbounds_ifs(xbounds_file2.c_str());
        while (xbounds_ifs) {
          double xbound;
          xbounds_ifs >> xbound;
          DT_THROW_IF(! xbounds_ifs, std::logic_error, "Invalid format in file  '" << xbounds_file << "' !");
          xranges.push_back(xbound * xunit);
          xbounds_ifs >> std::ws;
          if (xbounds_ifs.eof()) {
            break;
          }
        }
      }
      if (yranges.size() == 0 && h2_setup_.has_key("y.bounds.file")) {
        std::string ybounds_file;
        ybounds_file = h2_setup_.fetch_string("y.bounds.file");

        std::string ybounds_file2 = ybounds_file;
        datatools::fetch_path_with_env(ybounds_file2);
        std::ifstream ybounds_ifs(ybounds_file2.c_str());
        while (ybounds_ifs) {
          double ybound;
          ybounds_ifs >> ybound;
          DT_THROW_IF(! ybounds_ifs, std::logic_error, "Invalid format in file  '" << ybounds_file << "' !");
          yranges.push_back(ybound * yunit);
          ybounds_ifs >> std::ws;
          if (ybounds_ifs.eof()) {
            break;
          }
        }
      }
      DT_THROW_IF(xranges.size() == 0, std::logic_error,"No rule was given to setup the X bins (xbounds, xbounds.file) !");
      DT_THROW_IF(yranges.size() == 0, std::logic_error,"No rule was given to setup the Y bins (ybounds, ybounds.file) !");
      DT_THROW_IF(xranges.size() < 2, std::logic_error, "Invalid number of X bins ('" << xranges.size() << "') in 2D-histogram !");
      DT_THROW_IF(yranges.size() < 2, std::logic_error, "Invalid number of Y bins ('" << yranges.size() << "') in 2D-histogram !");
      h2_.initialize(xranges, yranges);
    } else if (mode == "mimic") {
      // Extract information about other histograms :
      // 1D clone x 1D clone
      // 2D clone
      DT_THROW_IF(histo_pool_ == 0, std::logic_error, "Missing pool of histograms !");
      if (h2_setup_.has_key("mimic.x.histogram_1d")
          && h2_setup_.has_key("mimic.y.histogram_1d")) {
        // Copy the structure of another 1D histogram :
        std::string h2d_x_name = h2_setup_.fetch_string("mimic.x.histogram_1d");
        std::string h2d_y_name = h2_setup_.fetch_string("mimic.y.histogram_1d");
        {
          bool check = ! histo_pool_->has(h2d_x_name, histogram_pool::HISTOGRAM_DIM_1D);
          DT_THROW_IF(check,
                      std::logic_error,
                      "No 1D-histogram named '" << h2d_x_name << "' does exist !");
        }
        {
          bool check = ! histo_pool_->has(h2d_x_name, histogram_pool::HISTOGRAM_DIM_1D);
          DT_THROW_IF(check,
                      std::logic_error,
                      "No 1D-histogram named '" << h2d_y_name << "' does exist !");
        }
        const histogram & mimic_x_h1 = histo_pool_->get_1d(h2d_x_name);
        const histogram & mimic_y_h1 = histo_pool_->get_1d(h2d_y_name);
        std::vector<std::string> import_prop_prefixes;
        if (h2_setup_.has_flag("mimic.histogram_2d.import_aux")) {
          h2_setup_.fetch("mimic.histogram_2d.import_aux", import_prop_prefixes);
        }
        h2_.initialize(mimic_x_h1, mimic_y_h1, import_prop_prefixes);
      } else if (h2_setup_.has_key("mimic.histogram_2d")) {
        // Copy the structure of another 2D histogram :
        std::string h2d_name = h2_setup_.fetch_string("mimic.histogram_2d");

        DT_THROW_IF(! histo_pool_->has(h2d_name, histogram_pool::HISTOGRAM_DIM_2D),
                    std::logic_error,
                    "No 2D-histogram named '" << h2d_name << "' does exist !");
        const histogram_2d & mimic_h2 = histo_pool_->get_2d(h2d_name);
        std::vector<std::string> import_prop_prefixes;
        if (h2_setup_.has_flag("mimic.histogram_2d.import_aux")) {
          h2_setup_.fetch("mimic.histogram_2d.import_aux", import_prop_prefixes);
        }
        h2_.initialize(mimic_h2, import_prop_prefixes);
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing mimic mode !");
      }
    }

    return;
  }

  void histogram_pool::initialize(const datatools::properties & setup_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,"Pool is already initialized !");

    if (_description_.empty()) {
      if (setup_.has_key("description")) {
        set_description(setup_.fetch_string("description"));
      }
    }

    if (setup_.has_key("histo.export_prefixes")) {
      std::vector<std::string> export_prefixes;
      setup_.fetch("histo.export_prefixes", export_prefixes);
      _auxiliaries_.store("histo.export_prefixes", export_prefixes);
    }

    std::vector<std::string> histo_setup_filenames;
    if (setup_.has_key("histo.setups")) {
      setup_.fetch("histo.setups", histo_setup_filenames);
    }

    for (size_t i = 0; i < histo_setup_filenames.size(); i++) {
      load(histo_setup_filenames[i]);
    }

    _auxiliaries_.set_flag(_INITIALIZED_FLAG_KEY_);

    /*
      _auxiliaries_.tree_dump(std::cerr,
      "********** _auxiliaries_: ",
      "DEVEL: histogram_pool:");
    */
    return;
  }

  void histogram_pool::reset()
  {
    if (is_initialized()) {
      _description_.clear();
      remove_all();
      clear_auxiliaries();
    }
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    return;
  }

  void histogram_pool::remove_all()
  {
    _dict_.clear();
    return;
  }

  void histogram_pool::clear_auxiliaries()
  {
    _auxiliaries_.clear();
    return;
  }

  void histogram_pool::remove(const std::string & name_)
  {
    dict_type::iterator found = _dict_.find(name_);
    if (found == _dict_.end()) {
      return;
    }
    _dict_.erase(found);
    return;
  }

  void histogram_pool::rename(const std::string & name_, const std::string & new_name_)
  {
    dict_type::iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    dict_type::iterator found2 = _dict_.find(new_name_);
    DT_THROW_IF(found2 != _dict_.end(),
                std::logic_error,
                "An histogram named '" << name_ << "' already exists !");
    histogram_entry_type hentry = found->second;
    _dict_.erase(found);
    hentry.name = new_name_;
    _dict_[new_name_] = hentry;
    return;
  }

  const std::string & histogram_pool::get_group(const std::string & name_) const
  {
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    return found->second.group;
  }

  const std::string & histogram_pool::get_title(const std::string & name_) const
  {
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    return found->second.title;
  }

  void histogram_pool::set_title(const std::string & name_, const std::string & title_)
  {
    dict_type::iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    found->second.title = title_;
    return;
  }

  void histogram_pool::set_group(const std::string & name_, const std::string & group_)
  {
    dict_type::iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    found->second.group = group_;
    return;
  }

  bool histogram_pool::has_1d(const std::string & name_) const
  {
    return has(name_, HISTOGRAM_DIM_1D);
  }

  bool histogram_pool::has_2d(const std::string & name_) const
  {
    return has(name_, HISTOGRAM_DIM_2D);
  }

  bool histogram_pool::has(const std::string & name_, int dim_) const
  {
    dict_type::const_iterator found = _dict_.find(name_);
    if (found == _dict_.end()) {
      return false;
    }
    if (dim_ == HISTOGRAM_DIM_UNDEFINED) {
      return true;
    }
    const histogram_entry_type & hentry = found->second;
    if (hentry.dimension == dim_) {
      return true;
    }
    return false;
  }

  histogram & histogram_pool::grab_1d(const std::string & name_)
  {
    dict_type::iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    histogram_entry_type & hentry = found->second;
    DT_THROW_IF(hentry.dimension != HISTOGRAM_DIM_1D,
                std::logic_error,
                "Histogram '" << name_ << "' is not a 1D histogram !");
    return hentry.hh1d.grab();
  }

  const histogram & histogram_pool::get_1d(const std::string & name_) const
  {
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    const histogram_entry_type & hentry = found->second;
    DT_THROW_IF(hentry.dimension != HISTOGRAM_DIM_1D,
                std::logic_error,
                "Histogram '" << name_ << "' is not a 1D histogram !");
    return hentry.hh1d.get();
  }


  histogram_2d & histogram_pool::grab_2d(const std::string & name_)
  {
    dict_type::iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    histogram_entry_type & hentry = found->second;
    DT_THROW_IF(hentry.dimension != HISTOGRAM_DIM_2D,
                std::logic_error,
                "Histogram '" << name_ << "' is not a 2D histogram !");
    return hentry.hh2d.grab();
  }

  const histogram_2d & histogram_pool::get_2d(const std::string & name_) const
  {
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found == _dict_.end(),
                std::logic_error,
                "No histogram named '" << name_ << "' exists !");
    const histogram_entry_type & hentry = found->second;
    DT_THROW_IF(hentry.dimension != HISTOGRAM_DIM_2D,
                std::logic_error,
                "Histogram '" << name_ << "' is not a 2D histogram !");
    return hentry.hh2d.get();
  }

  histogram & histogram_pool::add_1d(const std::string & name_,
                                     const std::string & title_,
                                     const std::string & group_)
  {
    // std::cerr << "DEVEL: histogram_pool::add_1d: name='" << name_<< "'" << std::endl;
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found != _dict_.end(), std::logic_error, "Histogram named '" << name_ << "' already exists !");
    {
      histogram_entry_type dummy;
      _dict_[name_] = dummy;
    }
    histogram_entry_type & he = _dict_[name_];
    he.name = name_;
    he.title = title_;
    he.group = group_;
    he.dimension = HISTOGRAM_DIM_1D;
    he.hh1d.reset(new histogram);
    return he.hh1d.grab();
  }

  histogram_2d & histogram_pool::add_2d(const std::string & name_,
                                        const std::string & title_,
                                        const std::string & group_)
  {
    dict_type::const_iterator found = _dict_.find(name_);
    DT_THROW_IF(found != _dict_.end(), std::logic_error, "Histogram named '" << name_ << "' already exists !");
    histogram_entry_type & he = _dict_[name_];
    he.name = name_;
    he.title = title_;
    he.group = group_;
    he.dimension = HISTOGRAM_DIM_2D;
    he.hh2d.reset(new histogram_2d);
    return he.hh2d.grab();
  }

  bool histogram_pool::empty() const
  {
    return _dict_.size() == 0;
  }

  size_t histogram_pool::size() const
  {
    return _dict_.size();
  }

  void histogram_pool::names(std::vector<std::string> &names_, const std::string & filter_, bool reverse_) const
  {
    if (names_.size() == 0) names_.reserve(_dict_.size());
    for (dict_type::const_iterator i = _dict_.begin();
         i != _dict_.end();
         i++) {
      const std::string& key = i->first;
      bool select = false;

      // here, all histogram are selected, we make no use of the 'filter_' argument :
      const histogram_entry_type& he = i->second;
      if (filter_ == "" || filter_ == "*") {
        select = true;
      } else if (filter_ == "dim=1") {
        if (he.dimension == HISTOGRAM_DIM_1D) {
          select = true;
        }
      } else if (filter_ == "dim=2") {
        if (he.dimension == HISTOGRAM_DIM_2D) {
          select = true;
        }
      } else if (filter_.substr(0,6) == "group=") {
        std::string group_name = filter_.substr(6);
        if (he.group == group_name) {
          select = true;
        }
      } else if (filter_.substr(0,5) == "flag=") {
        std::string flag_name = filter_.substr(5);

        if (he.dimension == HISTOGRAM_DIM_1D) {
          if (he.hh1d.get().get_auxiliaries().has_flag(flag_name)) {
            select = true;
          }
        }
        if (he.dimension == HISTOGRAM_DIM_2D) {
          if (he.hh2d.get().get_auxiliaries().has_flag(flag_name)) {
            select = true;
          }
        }
      }

      bool push_it = false;
      if (select) {
        if (! reverse_) push_it = true;
      } else {
        if (reverse_) push_it = true;
      }
      if (push_it) {
        if (std::find(names_.begin(), names_.end(), key) == names_.end()) {
          names_.push_back(key);
        }
      }
    }
    return;
  }

  void histogram_pool::tree_dump(std::ostream& out_,
                                 const std::string& title_,
                                 const std::string& indent_,
                                 bool inherit_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    if (!title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Description : '" <<  this->get_description() << "'" << std::endl;


    out_ << indent << datatools::i_tree_dumpable::tag
         << "Histograms  : ";
    if (_dict_.empty()) {
      out_ << "<empty>";
    }
    out_ << std::endl;
    for (dict_type::const_iterator i = _dict_.begin();
         i != _dict_.end();
         i++) {
      const std::string& key = i->first;
      const histogram_entry_type& he = i->second;
      out_ << indent << datatools::i_tree_dumpable::skip_tag;
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      dict_type::const_iterator j = i;
      j++;
      if (j == _dict_.end()) {
        out_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Name : " << '"' << key << '"' << std::endl;
      he.tree_dump(out_, "", indent_oss.str());
    }


    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Auxiliaries : ";
    if (_auxiliaries_.empty()) {
      out_ << "<empty>";
    }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }

    return;
  }

} // end of namespace mygsl

  /***************
   * OCD support *
   ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::mygsl::histogram_pool' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mygsl::histogram_pool,ocd_)
{
  ocd_.set_class_name("mygsl::histogram_pool");
  ocd_.set_class_description("A pool of histograms");
  ocd_.set_class_library("mygsl");
  ocd_.set_class_documentation("A container for 1D and/or 2D histogram objects");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("description")
      .set_terse_description("The embeded description string")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a previous call of :                          \n"
                            "  ``mygsl::histogram_pool::set_description(...)``           \n"
                            "                                                            \n"
                            "Example::                                                   \n"
                            "                                                            \n"
                            "    description : string = \"Histograms for data analysis\" \n"
                            "                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("histo.export_prefixes")
      .set_terse_description("A list of character strings used as the prefixes of auxiliary properties exported in histograms")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("                                           \n"
                            "Example::                                  \n"
                            "                                           \n"
                            "    histo.export_prefixes : string[1] = \\ \n"
                            "      \"value.\"                           \n"
                            "                                           \n"
                            );
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("histo.setups")
      .set_terse_description("A list of configuration file names for embeded histograms")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description("A list of filenames from where the histogram pool                  \n"
                            "loads the directives to dynamically instantiate                    \n"
                            "new embeded histogram objects. The filenames may contain           \n"
                            "some environment variables.                                        \n"
                            "                                                                   \n"
                            "Example::                                                          \n"
                            "                                                                   \n"
                            "    histo.setups : string[1] as path = \\                          \n"
                            "      \"${CONFIG_REPOSITORY_DIR}/histo_1d.conf\"                   \n"
                            "                                                                   \n"
                            "The target files must use the format of the                        \n"
                            "``datatools::multi_properties`` class.                             \n"
                            "The loading order of the files is critical                         \n"
                            "because some histograms may depend on other ones                   \n"
                            "which should thus be defined **before** their                      \n"
                            "dependers.                                                         \n"
                            "Extends the instantiation of histograms triggered by               \n"
                            "previous calls to the ``mygsl::histogram_pool::load(...)`` method. \n"
                            "See OCD support for ``mygsl::histogram_1d`` and                    \n"
                            "``mygsl::histogram_2d```classes.                                   \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("The histogram pool uses a ``datatools::properties`` object       \n"
                               "to initialize its behaviour and contents.                        \n"
                               "                                                                 \n"
                               "Example of configuration for the histogram pool::                \n"
                               "                                                                 \n"
                               "    description  : string = \"The data analysis histogram pool\" \n"
                               "    histo.setups : string[1] as path = \"histos.conf\"           \n"
                               "    histo.export_prefixes : string[1] as path = \"value.\"       \n"
                               "                                                                 \n"
                               "                                                                 \n"
                               "Example of associated file ``histos.conf`` for automated         \n"
                               "construction of four histograms::                                \n"
                               "                                                                 \n"
                               "    #@description A sample list of histograms build rules        \n"
                               "    #@key_label   \"name\"                                       \n"
                               "    #@meta_label  \"type\"                                       \n"
                               "                                                                 \n"
                               "    [name=\"E1\" type=\"mygsl::histogram_1d\"]                   \n"
                               "    mode   : string = \"regular\"                                \n"
                               "    unit   : string = \"MeV\"                                    \n"
                               "    linear : boolean = 1                                         \n"
                               "    min    : real = 0.0                                          \n"
                               "    max    : real = 3.0                                          \n"
                               "    number_of_bins : integer = 120                               \n"
                               "    unit.type : string = \"energy\"                              \n"
                               "    display.xaxis.unit  : string = \"keV\"                       \n"
                               "    display.xaxis.label : string = \"E1\"                        \n"
                               "    value.1d.accessor   : string = \"e1\"                        \n"
                               "                                                                 \n"
                               "    [name=\"E2\" type=\"mygsl::histogram_1d\"]                   \n"
                               "    mode : string = \"mimic\"                                    \n"
                               "    display.xaxis.unit  : string = \"keV\"                       \n"
                               "    display.xaxis.label : string = \"E2\"                        \n"
                               "    mimic.histogram_1d  : string = \"E1\"                        \n"
                               "    value.1d.accessor   : string = \"e2\"                        \n"
                               "                                                                 \n"
                               "    [name=\"E1+E2\" type=\"mygsl::histogram_1d\"]                \n"
                               "    mode : string = \"mimic\"                                    \n"
                               "    unit : string = \"keV\"                                      \n"
                               "    unit.type : string = \"energy\"                              \n"
                               "    display.xaxis.unit : string = \"MeV\"                        \n"
                               "    mimic.histogram_1d : string = \"E1\"                         \n"
                               "    value.1d.accessor  : string = \"e1+e2\"                      \n"
                               "                                                                 \n"
                               "    [name=\"E1xE2\" type=\"mygsl::histogram_2d\"]                \n"
                               "    mode : string = \"regular\"                                  \n"
                               "    x.unit : string = \"keV\"                                    \n"
                               "    x.unit.type : string = \"energy\"                            \n"
                               "    x.linear : boolean = 1                                       \n"
                               "    x.min : real = 0.0                                           \n"
                               "    x.max : real = 3.0                                           \n"
                               "    x.number_of_bins : integer = 120                             \n"
                               "    display.xaxis.unit  : string = \"keV\"                       \n"
                               "    display.xaxis.label : string = \"E1\"                        \n"
                               "    y.unit : string = \"keV\"                                    \n"
                               "    y.unit.type : string = \"energy\"                            \n"
                               "    y.linear : boolean = 1                                       \n"
                               "    y.min : real = 0.0                                           \n"
                               "    y.max : real = 3.0                                           \n"
                               "    y.number_of_bins : integer = 120                             \n"
                               "    display.yaxis.unit  : string = \"keV\"                       \n"
                               "    display.yaxis.label : string = \"E2\"                        \n"
                               "    value.2d.accessors  : string[2] = \"e1\" \"e2\"              \n"
                               "                                                                 \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()


// OCD support for class '::mygsl::histogram' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mygsl::histogram,ocd_)
{
  ocd_.set_class_name("mygsl::histogram_1d");
  ocd_.set_class_description("One dimension histogram as built by the ``mygsl::histogram_pool`` class");
  ocd_.set_class_library("mygsl");
  ocd_.set_class_documentation("A one dimensional histogram class.");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("title")
      .set_terse_description("The histogram title mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("A human friendly title for the histogram(handled by the histogram pool). \n"
                            "Default value is empty.                                                  \n"
                            "                                                                         \n"
                            "Example::                                                                \n"
                            "                                                                         \n"
                            "  title : string = \"The energy distribution\"                           \n"
                            "                                                                         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("group")
      .set_terse_description("The name of the group the histogram is assigned to by the histogram pool")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The group may be used for selection operations on histograms. \n"
                            "Default value is empty.                                       \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  group : string = \"energy\"                                 \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The histogram build mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The mode can take the following values :                                 \n"
                            "                                                                         \n"
                            " - ``regular`` : histogram has a regular binning (linear or logarithmic) \n"
                            " - ``table`` : histogram binning is tabulated                            \n"
                            " - ``mimic`` : histogram binning mimic some other histogram(s) binning   \n"
                            "                                                                         \n"
                            "Example::                                                                \n"
                            "                                                                         \n"
                            "    mode : string = \"regular\"                                          \n"
                            "                                                                         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("unit")
      .set_terse_description("The histogram unit symbol on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The unit symbol on X-axis should be known from \n"
                            "the ``datatools::units`` module.               \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "    unit : string = \"keV\"                    \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("unit.type")
      .set_terse_description("The histogram physical dimension on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      //.set_complex_triggering_conditions(true)
      .set_long_description("The unit type should be known from the ``datatools::units`` module. \n"
                            "The ``unit.type`` property should be compatible with the ``unit``   \n"
                            "property if given previously.                                       \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    unit.type : string = \"energy\"                                 \n"
                            "                                                                    \n"
                            "This property is only defined for modes ``regular`` and ``table``.  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("display.xaxis.unit")
      .set_terse_description("The histogram display physical unit on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      .set_long_description("The display unit type should be known from the ``datatools::units`` \n"
                            "module and of the same physical dimension the unit.                 \n"
                            "The ``display.xaxis.unit`` property should be compatible with the   \n"
                            "``unit`` property if given previously.                              \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    display.xaxis.unit : string = \"MeV\"                           \n"
                            "                                                                    \n"
                            "This property is only defined for modes ``regular`` and ``table``.  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("linear")
      .set_terse_description("The flag for linear regular binning")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``linear`` flag setups the linear regular binning of the histogram. \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "    mode : string = \"regular\"                                         \n"
                            "    linear : boolean = 1                                                \n"
                            "                                                                        \n"
                            "This property is only defined for mode ``regular``.                     \n"
                            "This property is not compatible with the ``logarithmic`` flag.          \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("logarithmic")
      .set_terse_description("The flag for logarithmic regular binning")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``logarithmic`` property sets the logarithmic regular binning \n"
                            " of the histogram.                                                \n"
                            "                                                                  \n"
                            "Example::                                                         \n"
                            "                                                                  \n"
                            "    mode : string = \"regular\"                                   \n"
                            "    logarithmic : boolean = 1                                     \n"
                            "                                                                  \n"
                            "This property is only defined for mode ``regular``.               \n"
                            "This property is not compatible with the ``linear`` flag.         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("min")
      .set_terse_description("The minimal bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``min`` property sets the lower bound of the regular histogram. \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    min : real = 0.0                                                \n"
                            "                                                                    \n"
                            "This property should be lower than the ``max`` value.               \n"
                            "This property is only defined for mode ``regular``.                 \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("max")
      .set_terse_description("The maximal bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``max`` property sets the upper bound of the regular histogram. \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    max : real = 10.0                                               \n"
                            "                                                                    \n"
                            "This property should be greater than the ``min`` value.             \n"
                            "This property is only defined for mode ``regular``.                 \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("number_of_bins")
      .set_terse_description("The number of bins of the histogram")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``number_of_bins`` property sets the total number of bins. \n"
                            "                                                               \n"
                            "Example::                                                      \n"
                            "                                                               \n"
                            "    mode : string = \"regular\"                                \n"
                            "    number_of_bins : integer = 20                              \n"
                            "                                                               \n"
                            "This property is only defined for mode ``regular``.            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("bounds")
      .set_terse_description("The bounds of tabulated bins")
      .set_traits(datatools::TYPE_REAL,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The ``bounds`` of the histogram's bins.             \n"
                            "                                                    \n"
                            "Example::                                           \n"
                            "                                                    \n"
                            "    mode : string = \"table\"                       \n"
                            "    bounds : real[6] = 0.0 1.0 3.0 6.0 10.0 15.0    \n"
                            "                                                    \n"
                            "This property is only defined for mode ``table``.   \n"
                            "This property is incompatible with ``bounds.file``. \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("bounds.file")
      .set_terse_description("The name of a file that stores the bounds of tabulated bins")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The name of a file where the bins' bounds are stored. \n"
                            "                                                      \n"
                            "Example::                                             \n"
                            "                                                      \n"
                            "    mode : string = \"table\"                         \n"
                            "    bounds.file : string as path = \"bins.data\"      \n"
                            "                                                      \n"
                            "This property is only defined for mode ``table``.     \n"
                            "This property is incompatible with ``bounds``.        \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_1d")
      .set_terse_description("The name of a 1D-histogram to be cloned")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("The binning of the histogram is cloned from the 1D-histogram of    \n"
                            "which the name is given. The 1D-histogram to be cloned is searched \n"
                            "for a dictionnary of histograms provided by the histogram pool.    \n"
                            "                                                                   \n"
                            "Example::                                                          \n"
                            "                                                                   \n"
                            "    mode : string = \"mimic\"                                      \n"
                            "    mimic.histogram_1d : string = \"hx\"                           \n"
                            "                                                                   \n"
                            "This property is only defined for mode ``mimic``.                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_1d.import_aux")
      .set_terse_description("The list of prefixes for auxiliary properties to be imported from the cloned 1D-histogram")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("When we mimic the binning of a 1D-histogram, we can also import \n"
                            "all its properties of which the name starts with some specific  \n"
                            "prefixes (display and unit properties).                         \n"
                            "                                                                \n"
                            "Example::                                                       \n"
                            "                                                                \n"
                            "    mode : string = \"mimic\"                                   \n"
                            "    mimic.histogram_1d : string = \"hx\"                        \n"
                            "    mimic.histogram_1d.import_aux : string[1] = \"user.\"       \n"
                            "                                                                \n"
                            "This property is only defined for mode ``mimic``.               \n"
                            "It is used with the ``mimic.histogram_1d`` property.            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_2d")
      .set_terse_description("The name of a 2D-histogram from which one of the axis is to be cloned")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("The binning of the histogram is cloned from the 2D-histogram of     \n"
                            "which the name is given. The histogram to be cloned is searched for \n"
                            "a dictionnary of histograms provided by the histogram pool.         \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"mimic\"                                       \n"
                            "    mimic.histogram_2d : string = \"hxy\"                           \n"
                            "                                                                    \n"
                            "This property is only defined for mode ``mimic``.                   \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_2d.axis")
      .set_terse_description("Select the axis of the original 2D-histogram to be used")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("When we mimic the binning of a 2D-histogram, we must specify \n"
                            "the histogram axis of which the binning must be cloned.      \n"
                            "Possible values are : ``x`` and ``y``.                       \n"
                            "                                                             \n"
                            "Example::                                                    \n"
                            "                                                             \n"
                            "    mode : string = \"mimic\"                                \n"
                            "    mimic.histogram_2d : string = \"hxy\"                    \n"
                            "    mimic.histogram_2d.axis : string = \"x\"                 \n"
                            "                                                             \n"
                            "This property is only defined for mode ``mimic``.            \n"
                            "It is used with the ``mimic.histogram_2d`` property.         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_2d.import_aux")
      .set_terse_description("The list of prefixes for auxiliary properties to be imported from the cloned 2D-histogram")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("When we mimic the binning of a 2D-histogram, we can also import \n"
                            "all its properties of which the name starts with some specific  \n"
                            "prefixes (display and unit properties).                         \n"
                            "                                                                \n"
                            "Example::                                                       \n"
                            "                                                                \n"
                            "    mode : string = \"mimic\"                                   \n"
                            "    mimic.histogram_2d : string = \"hxy\"                       \n"
                            "    mimic.histogram_2d.import_aux : string[1] = \"display.\"    \n"
                            "                                                                \n"
                            "This property is only defined for mode ``mimic``.               \n"
                            "It is used with the ``mimic.histogram_2d`` property.            \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("Display properties:                                             \n"
                               "all properties the name of which starts with the prefix         \n"
                               "``display.`` are preserved and passed to the histogram object   \n"
                               "in the hope they will be useful for another application.        \n"
                               "                                                                \n"
                               "Here are typical examples from a ``datatools::multi_properties``\n"
                               "ASCII file that must begin with::                               \n"
                               "                                                                \n"
                               "    #@decription  Some 1D-histograms                            \n"
                               "    #@key_label  \"name\"                                       \n"
                               "    #@meta_label \"type\"                                       \n"
                               "                                                                \n"
                               "* A 1D-histogram with linear regular bins::                     \n"
                               "                                                                \n"
                               "    [name=\"E1\" type=\"mygsl::histogram_1d\"]                  \n"
                               "    mode                : string  = \"regular\"                 \n"
                               "    linear              : boolean = 1                           \n"
                               "    unit                : string  = \"keV\"                     \n"
                               "    min                 : real    = 0.0                         \n"
                               "    max                 : real    = 3000.0                      \n"
                               "    number_of_bins      : integer = 120                         \n"
                               "    unit.type           : string = \"energy\"                   \n"
                               "    display.xaxis.unit  : string = \"MeV\"                      \n"
                               "    display.xaxis.label : string = \"Energy\"                   \n"
                               "    display.color       : string = \"red\"                      \n"
                               "    value.1d.accessor   : string = \"event.E1\"                 \n"
                               "                                                                \n"
                               "                                                                \n"
                               "* A 1D-histogram build from explicit binning::                  \n"
                               "                                                                \n"
                               "    [name=\"E2\" type=\"mygsl::histogram_1d\"]                  \n"
                               "    mode               : string  = \"table\"                    \n"
                               "    bounds             : real[5] = 0.0 1.0 2.5 4.5 7.0          \n"
                               "    unit.type          : string  = \"energy\"                   \n"
                               "    display.xaxis.unit : string = \"MeV\"                       \n"
                               "    display.yaxis.unit : string = \"Counts\"                    \n"
                               "    value.1d.accessor  : string = \"event.E2\"                  \n"
                               "    test.label         : string = \"test\"                      \n"
                               "    test.index         : integer = 8                            \n"
                               "                                                                \n"
                               "                                                                \n"
                               "* A 1D-histogram build from a 1D histogram::                    \n"
                               "                                                                \n"
                               "    [name=\"E3\" type=\"mygsl::histogram_1d\"]                  \n"
                               "    mode               : string = \"mimic\"                     \n"
                               "    mimic.histogram_1d : string = \"E1\"                        \n"
                               "    mimic.histogram_1d.import_aux : string[1] = \"test.\"       \n"
                               "    value.1d.accessor : string = \"event.E3\"                   \n"
                               "                                                                \n"
                               "                                                                \n"
                               "* A 1D-histogram build from the Y-layout of a 2D histogram::    \n"
                               "                                                                \n"
                               "    [name=\"E3\" type=\"mygsl::histogram_1d\"]                  \n"
                               "    mode                    : string = \"mimic\"                \n"
                               "    mimic.histogram_2d      : string = \"E1xE2\"                \n"
                               "    mimic.histogram_2d.axis : string = \"y\"                    \n"
                               "    value.1d.accessor : string = \"event.E3\"                   \n"
                               "                                                                \n"
                               );
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()


// OCD support for class '::mygsl::histogram_2d' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mygsl::histogram_2d,ocd_)
{
  ocd_.set_class_name("mygsl::histogram_2d");
  ocd_.set_class_description("Two dimension histogram as built by the 'mygsl::histogram_pool' class");
  ocd_.set_class_library("mygsl");
  ocd_.set_class_documentation("A two dimensional histogram class.");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("title")
      .set_terse_description("The histogram title mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("A human friendly title for the histogram (handled by the histogram pool). \n"
                            "Default value is empty.                                                   \n"
                            "                                                                          \n"
                            "Example::                                                                 \n"
                            "                                                                          \n"
                            "  title : string = \"The energy vs time distribution\"                    \n"
                            "                                                                          \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("group")
      .set_terse_description("The name of the group the histogram is assigned to by the histogram pool")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The group may be used for selection operations on histograms. \n"
                            "Default value is empty.                                       \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  group : string = \"my_biplots\"                             \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The histogram build mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The mode can take the following values :                                                      \n"
                            "                                                                                              \n"
                            " - ``regular`` : histogram has a regular binning on both X and Y axis (linear or logarithmic) \n"
                            " - ``table`` : histogram binning is tabulated on both X and Y axis                            \n"
                            " - ``mimic`` : histogram X and Y binning mimic some other histogram(s) binning                \n"
                            "                                                                                              \n"
                            "Example::                                                                                     \n"
                            "                                                                                              \n"
                            "    mode : string = \"regular\"                                                               \n"
                            "                                                                                              \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.unit")
      .set_terse_description("The histogram unit symbol on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The unit symbol on X-axis should be known from \n"
                            "the ``datatools::units`` module.               \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "    x.unit : string = \"keV\"                  \n"
                            "                                               \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.unit")
      .set_terse_description("The histogram unit symbol on the Y-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The unit symbol on Y-axis should be known from \n"
                            "the ``datatools::units`` module.               \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "    y.unit : string = \"keV\"                  \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.unit.type")
      .set_terse_description("The histogram physical dimension on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      //.set_complex_triggering_conditions(true)
      .set_long_description("The unit type should be known from the ``datatools::units`` module.   \n"
                            "The ``x.unit.type`` property should be compatible with the ``x.unit`` \n"
                            "property if given previously.                                         \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "    mode : string = \"regular\"                                       \n"
                            "    x.unit : string = \"keV\"                                         \n"
                            "    x.unit.type : string = \"energy\"                                 \n"
                            "                                                                      \n"
                            "This property is only defined for modes ``regular`` and ``table``.    \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.unit.type")
      .set_terse_description("The histogram physical dimension on the Y-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      //.set_complex_triggering_conditions(true)
      .set_long_description("The unit type should be known from the ``datatools::units`` module.   \n"
                            "The ``y.unit.type`` property should be compatible with the ``y.unit`` \n"
                            "property if given previously.                                         \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "    mode : string = \"regular\"                                       \n"
                            "    y.unit : string = \"keV\"                                         \n"
                            "    y.unit.type : string = \"energy\"                                 \n"
                            "                                                                      \n"
                            "This property is only defined for modes ``regular`` and ``table``.    \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("display.xaxis.unit")
      .set_terse_description("The histogram display physical unit on the X-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      .set_long_description("The display unit type should be known from the ``datatools::units`` \n"
                            "module and of the same physical dimension the unit.                 \n"
                            "The ``display.xaxis.unit`` property should be compatible with the   \n"
                            "``x.unit`` property if given previously.                            \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    x.unit : string = \"keV\"                                       \n"
                            "    display.xaxis.unit : string = \"keV\"                           \n"
                            "                                                                    \n"
                            "This property is only defined for modes ``regular`` and ``table``.  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("display.yaxis.unit")
      .set_terse_description("The histogram display physical unit on the Y-axis")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular;table")
      .set_long_description("The display unit type should be known from the ``datatools::units`` \n"
                            "module and of the same physical dimension the unit.                 \n"
                            "The ``display.xaxis.unit`` property should be compatible with the   \n"
                            "``y.unit`` property if given previously.                            \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    y.unit : string = \"keV\"                                       \n"
                            "    display.yaxis.unit : string = \"keV\"                           \n"
                            "                                                                    \n"
                            "This property is only defined for modes ``regular`` and ``table``.  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.linear")
      .set_terse_description("The flag for linear regular binning on X axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``x.linear`` flag setups the linear regular X binning of the histogram. \n"
                            "                                                                            \n"
                            "Example::                                                                   \n"
                            "                                                                            \n"
                            "    mode : string = \"regular\"                                             \n"
                            "    x.linear : boolean = 1                                                  \n"
                            "                                                                            \n"
                            "This property is only defined for mode ``regular``.                         \n"
                            "This property is not compatible with the ``x.logarithmic`` flag.            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.linear")
      .set_terse_description("The flag for linear regular binning on Y axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``y.linear`` flag setups the linear regular Y binning of the histogram. \n"
                            "                                                                            \n"
                            "Example::                                                                   \n"
                            "                                                                            \n"
                            "    mode : string = \"regular\"                                             \n"
                            "    y.linear : boolean = 1                                                  \n"
                            "                                                                            \n"
                            "This property is only defined for mode ``regular``.                         \n"
                            "This property is not compatible with the ``y.logarithmic`` flag.            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.logarithmic")
      .set_terse_description("The flag for logarithmic regular binning on X axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``logarithmic`` property sets the logarithmic regular X binning \n"
                            "of the histogram.                                                   \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    x.logarithmic : boolean = 1                                     \n"
                            "                                                                    \n"
                            "This property is only defined for mode ``regular``.                 \n"
                            "This property is not compatible with the ``x.linear`` flag.         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.logarithmic")
      .set_terse_description("The flag for logarithmic regular binning on Y axis")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``logarithmic`` property sets the logarithmic regular Y binning \n"
                            "of the histogram.                                                   \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "    mode : string = \"regular\"                                     \n"
                            "    y.logarithmic : boolean = 1                                     \n"
                            "                                                                    \n"
                            "This property is only defined for mode ``regular``.                 \n"
                            "This property is not compatible with the ``y.linear`` flag.         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.min")
      .set_terse_description("The minimal X bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``x.min`` property sets the lower X bound of the regular histogram. \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "    mode : string = \"regular\"                                         \n"
                            "    x.min : real = 0.0                                                  \n"
                            "                                                                        \n"
                            "This property should be lower than the ``x.max`` value.                 \n"
                            "This property is only defined for mode ``regular``.                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.max")
      .set_terse_description("The maximal X bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``x.max`` property sets the upper X bound of the regular histogram. \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "    mode : string = \"regular\"                                         \n"
                            "    x.max : real = 10.0                                                 \n"
                            "                                                                        \n"
                            "This property should be greater than the ``x.min`` value.               \n"
                            "This property is only defined for mode ``regular``.                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.min")
      .set_terse_description("The minimal Y bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``y.min`` property sets the lower Y bound of the regular histogram. \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "    mode : string = \"regular\"                                         \n"
                            "    y.min : real = 0.0                                                  \n"
                            "                                                                        \n"
                            "This property should be lower than the ``y.max`` value.                 \n"
                            "This property is only defined for mode ``regular``.                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.max")
      .set_terse_description("The maximal Y bound of the histogram")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``y.max`` property sets the upper Y bound of the regular histogram. \n"
                            "                                                                        \n"
                            "Example::                                                               \n"
                            "                                                                        \n"
                            "    mode : string = \"regular\"                                         \n"
                            "    y.max : real = 10.0                                                 \n"
                            "                                                                        \n"
                            "This property should be greater than the ``y.min`` value.               \n"
                            "This property is only defined for mode ``regular``.                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.number_of_bins")
      .set_terse_description("The number of X bins of the histogram")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``x.number_of_bins`` property sets the total number of X bins. \n"
                            "                                                                   \n"
                            "Example::                                                          \n"
                            "                                                                   \n"
                            "    mode : string = \"regular\"                                    \n"
                            "    x.number_of_bins : integer = 20                                \n"
                            "                                                                   \n"
                            "This property is only defined for mode ``regular``.                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.number_of_bins")
      .set_terse_description("The number of Y bins of the histogram")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_triggered_by_label("mode", "regular")
      .set_long_description("The ``y.number_of_bins`` property sets the total number of Y bins. \n"
                            "                                                                   \n"
                            "Example::                                                          \n"
                            "                                                                   \n"
                            "    mode : string = \"regular\"                                    \n"
                            "    y.number_of_bins : integer = 20                                \n"
                            "                                                                   \n"
                            "This property is only defined for mode ``regular``.                \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.bounds")
      .set_terse_description("The bounds of tabulated X bins")
      .set_traits(datatools::TYPE_REAL,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The ``x.bounds`` of the histogram's bins.                 \n"
                            "                                                          \n"
                            "Example::                                                 \n"
                            "                                                          \n"
                            "    mode : string = \"table\"                             \n"
                            "    x.bounds : real[6] in keV = 0.0 1.0 3.0 6.0 10.0 15.0 \n"
                            "                                                          \n"
                            "This property is only defined for mode ``table``.         \n"
                            "This property is incompatible with ``x.bounds.file``.     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.bounds")
      .set_terse_description("The bounds of tabulated Y bins")
      .set_traits(datatools::TYPE_REAL,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The ``y.bounds`` of the histogram's bins.             \n"
                            "                                                      \n"
                            "Example::                                             \n"
                            "                                                      \n"
                            "    mode : string = \"table\"                         \n"
                            "    y.bounds : real[4] in mm = 0.0 5.0 20.0 40.0      \n"
                            "                                                      \n"
                            "This property is only defined for mode ``table``.     \n"
                            "This property is incompatible with ``y.bounds.file``. \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("x.bounds.file")
      .set_terse_description("The name of a file that stores the bounds of tabulated X bins")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The name of a file where the X bins' bounds are stored. \n"
                            "                                                        \n"
                            "Example::                                               \n"
                            "                                                        \n"
                            "    mode : string = \"table\"                           \n"
                            "    x.bounds.file : string as path = \"bins.data\"      \n"
                            "                                                        \n"
                            "This property is only defined for mode ``table``.       \n"
                            "This property is incompatible with ``x.bounds``.        \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("y.bounds.file")
      .set_terse_description("The name of a file that stores the bounds of tabulated Y bins")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "table")
      .set_long_description("The name of a file where the Y bins' bounds are stored. \n"
                            "                                                        \n"
                            "Example::                                               \n"
                            "                                                        \n"
                            "    mode : string = \"table\"                           \n"
                            "    y.bounds.file : string as path = \"bins.data\"      \n"
                            "                                                        \n"
                            "This property is only defined for mode ``table``.       \n"
                            "This property is incompatible with ``y.bounds``.        \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.x.histogram_1d")
      .set_terse_description("The name of a 1D-histogram of which the binning must be cloned for the X axis binning")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("The X binning of the 2D-histogram is cloned from the 1D-histogram of \n"
                            "which the name is given. The 1D-histogram to be cloned is searched   \n"
                            "for a dictionnary of histograms provided by the histogram pool.      \n"
                            "                                                                     \n"
                            "Example::                                                            \n"
                            "                                                                     \n"
                            "    mode : string = \"mimic\"                                        \n"
                            "    mimic.x.histogram_1d : string = \"hx\"                           \n"
                            "                                                                     \n"
                            "This property is only defined for mode ``mimic``.                    \n"
                            "The ``mimic.y.histogram_1d`` must also be provided for the Y axis.   \n"
                            "This property is not compatible with the ``mimic.histogram_2d``      \n"
                            "property.                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.y.histogram_1d")
      .set_terse_description("The name of a 1D-histogram of which the binning must be cloned for the X axis binning")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("The Y binning of the 2D-histogram is cloned from the 1D-histogram of \n"
                            "which the name is given. The 1D-histogram to be cloned is searched   \n"
                            "for a dictionnary of histograms provided by the histogram pool.      \n"
                            "                                                                     \n"
                            "Example::                                                            \n"
                            "                                                                     \n"
                            "    mode : string = \"mimic\"                                        \n"
                            "    mimic.y.histogram_1d : string = \"hb\"                           \n"
                            "                                                                     \n"
                            "This property is only defined for mode ``mimic``.                    \n"
                            "The ``mimic.x.histogram_1d`` must also be provided for the Y axis.   \n"
                            "This property is not compatible with the ``mimic.histogram_2d``      \n"
                            "property.                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_2d")
      .set_terse_description("The name of a 1D-histogram of which the binning must be cloned for the X axis binning")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("The X and Y binnings of the 2D-histogram are cloned from the 2D-histogram of \n"
                            "which the name is given. The 2D-histogram to be cloned is searched           \n"
                            "for a dictionnary of histograms provided by the histogram pool.              \n"
                            "                                                                             \n"
                            "Example::                                                                    \n"
                            "                                                                             \n"
                            "    mode : string = \"mimic\"                                                \n"
                            "    mimic.histogram_2d : string = \"hxy\"                                    \n"
                            "                                                                             \n"
                            "This property is only defined for mode ``mimic``.                            \n"
                            "This property is not compatible with the ``mimic.*.histogram_1d``            \n"
                            "properties.                                                                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("mimic.histogram_2d.import_aux")
      .set_terse_description("The list of prefixes for auxiliary properties to be imported from the cloned histogram")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "mimic")
      .set_long_description("When we mimic the binning of a 2D-histogram, we can also import \n"
                            "all its properties of which the name starts with some specific  \n"
                            "prefixes (display and unit properties).                         \n"
                            "                                                                \n"
                            "Example::                                                       \n"
                            "                                                                \n"
                            "    mode : string = \"mimic\"                                   \n"
                            "    mimic.histogram_2d : string = \"hxy\"                       \n"
                            "    mimic.histogram_2d.import_aux : string[1] = \"user.\"       \n"
                            "                                                                \n"
                            "This property is only defined for mode ``mimic``.               \n"
                            "It is used with the ``mimic.histogram_2d`` property.            \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("Display properties:                                                   \n"
                               "all properties the name of which starts with the prefix               \n"
                               "``display.`` are preserved and passed to the histogram object         \n"
                               "in the hope they will be useful for another application.              \n"
                               "                                                                      \n"
                               "Here are typical examples from a ``datatools::multi_properties``      \n"
                               "ASCII file that must begin with::                                     \n"
                               "                                                                      \n"
                               "    #@decription  Some 2D-histograms                                  \n"
                               "    #@key_label  \"name\"                                             \n"
                               "    #@meta_label \"type\"                                             \n"
                               "                                                                      \n"
                               "* A 2D-histogram with tabulated bins::                                \n"
                               "                                                                      \n"
                               "    [name=\"Ngg.vs.Ncalo\" type=\"mygsl::histogram_2d\"]              \n"
                               "    title : string = \"Number of Geiger hits vs number of calo hits\" \n"
                               "    group : string = \"multiplicities\"                               \n"
                               "    mode : string = \"table\"                                         \n"
                               "    x.bounds : real [4] = 0.0  24.0  36  100.0                        \n"
                               "    y.bounds : real [5] = 0.0  1.0  2.0  3.0  10.0                    \n"
                               "    display.xaxis.label : string = \"Ngg\"                            \n"
                               "    display.yaxis.label : string = \"Ncalo\"                          \n"
                               "    display.zaxis.label : string = \"Counts\"                         \n"
                               "    display.color       : string = \"red\"                            \n"
                               "    value.2d.accessor   : string[2] = \"event.Ngg\" \"event.Ncalo\"   \n"
                               "                                                                      \n"
                               "* Another 2D-histogram with tabulated bins and  units::               \n"
                               "                                                                      \n"
                               "    [name=\"E1.vs.E2\" type=\"mygsl::histogram_2d\"]                  \n"
                               "    title : string = \"E1 vs E2\"                                     \n"
                               "    group : string = \"calorimetry\"                                  \n"
                               "    mode : string = \"table\"                                         \n"
                               "    x.unit : string = \"MeV\"                                         \n"
                               "    x.bounds : real [41] = 0.0  0.1  0.2  0.3  0.4 \\                 \n"
                               "                           0.5  0.6  0.7  0.8  0.9 \\                 \n"
                               "                           1.0  1.1  1.2  1.3  1.4 \\                 \n"
                               "                           1.5  1.6  1.7  1.8  1.9 \\                 \n"
                               "                           2.0  2.1  2.2  2.3  2.4 \\                 \n"
                               "                           2.5  2.6  2.7  2.8  2.9 \\                 \n"
                               "                           3.0  3.1  3.2  3.3  3.4 \\                 \n"
                               "                           3.5  4.0  4.5  5.0  6.0 \\                 \n"
                               "                           10.0                                       \n"
                               "    y.bounds : real [41] in MeV = \\                                  \n"
                               "                              0  100  200  300  400 \\                \n"
                               "                            500  600  700  800  900 \\                \n"
                               "                           1000 1100 1200 1300 1400 \\                \n"
                               "                           1500 1600 1700 1800 1900 \\                \n"
                               "                           2000 2100 2200 2300 2400 \\                \n"
                               "                           2500 2600 2700 2800 2900 \\                \n"
                               "                           3000 3100 3200 3300 3400 \\                \n"
                               "                           3500 4000 4500 5000 6000 \\                \n"
                               "                           10000                                      \n"
                               "    display.xaxis.label : string = \"E1\"                             \n"
                               "    display.xaxis.unit  : string = \"keV\"                            \n"
                               "    display.yaxis.label : string = \"E2\"                             \n"
                               "    display.yaxis.unit  : string = \"keV\"                            \n"
                               "    display.zaxis.label : string = \"Counts\"                         \n"
                               "    display.style       : string = \"lego\"                           \n"
                               "    value.2d.accessor   : string[2] = \"event.E1\" \"event.E2\"       \n"
                               "                                                                      \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(::mygsl::histogram_pool,"mygsl::histogram_pool")
DOCD_CLASS_SYSTEM_REGISTRATION(::mygsl::histogram_1d,  "mygsl::histogram_1d")
DOCD_CLASS_SYSTEM_REGISTRATION(::mygsl::histogram_2d,  "mygsl::histogram_2d")

// end of histogram_pool.cc
