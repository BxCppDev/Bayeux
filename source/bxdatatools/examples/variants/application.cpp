// Ourselves:
#include "application.hpp"

// Standard library:
#include <iostream>

// Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>

application::algo::algo()
{
  _logging_   = "fatal";            // default
  _max_time_ = 1.0 * CLHEP::second; // default
  _mode_     = AM_UNDEFINED;        // unset
  _accuracy_ = -1;                  // unset
  _nevents_  = -1;                  // unset
  return;
}

void application::algo::initialize(const datatools::properties & config_)
{
  // Parse configuration properties:

  if (config_.has_key("logging")) {
    std::string logging_label = config_.fetch_string("logging");
    _logging_ = logging_label;
  }

  if (config_.has_key("max_time")) {
    _max_time_ = config_.fetch_real("max_time");
    if (!config_.has_explicit_unit("max_time")) {
      _max_time_ *= CLHEP::second;
    }
  }

  if (config_.has_key("mode")) {
    std::string mode_label = config_.fetch_string("mode");
    if (mode_label == "fast") {
      _mode_ = AM_FAST;
    } else if (mode_label == "slow") {
      _mode_ = AM_SLOW;
    } else {
      DT_THROW(std::logic_error, "Invalid mode (" << mode_label << ")!");
    }
    if (_mode_ == AM_UNDEFINED) {
      _mode_ == AM_DEFAULT;
    }
  }

  if (config_.has_key("accuracy")) {
    int acc = config_.fetch_integer("accuracy");
    DT_THROW_IF(acc < 1 || acc > 6, std::range_error, "Invalid accuracy order (" << acc << ")!");
    _accuracy_ = acc;
  }

  if (config_.has_key("nevents")) {
    int nevents = config_.fetch_integer("nevents");
    DT_THROW_IF(nevents < 1 || nevents > 100000,
                std::range_error, "Invalid number of events (" << nevents << ")!");
    _nevents_ = nevents;
  }

  // Initialization

  return;
}

void application::algo::run()
{
  std::clog << "Application algorithm is running..." << std::endl;
  return;
}

void application::algo::print() const
{
  std::clog << "application::algo: \n";
  std::clog << "  logging  = '" << _logging_  << "'" << std::endl;
  std::clog << "  max_time = " << _max_time_ / CLHEP::second << " s" << std::endl;
  std::clog << "  mode     = " << _mode_ << std::endl;
  std::clog << "  accuracy = " << _accuracy_  << std::endl;
  std::clog << "  nevents  = " << _nevents_ << std::endl;
  return;
}

application::gui::gui()
{
  _logging_          = "fatal";      // default
  _antialiasing_     = true;         // default
  _color_            = false;        // default
  _color_palette_    = -1;           // unset
  _color_foreground_ = "black";      // default
  _color_background_ = "white";      // default
  _bw_mode_          = BW_UNDEFINED; // unset
  _bw_reverse_       = false;        // default
  return;
}

void application::gui::initialize(const datatools::properties & config_)
{

  // Parse configuration properties:

  if (config_.has_key("logging")) {
    const std::string & logging_label = config_.fetch_string("logging");
    _logging_ = logging_label;
  }

  if (config_.has_key("antialiasing")) {
    _antialiasing_ = config_.fetch_boolean("antialiasing");
  }

  if (config_.has_flag("color")) {
    _color_ = true;
  }

  if (_color_) {

    // Set the palette id:
    {
      DT_THROW_IF (!config_.has_key("color.palette"), std::logic_error,
                   "Missing 'color.palette' configuration property!");
      int palette_id = config_.fetch_integer("color.palette");
      DT_THROW_IF(palette_id < 0 || palette_id > 7, std::domain_error,
                  "Invalid palette id (" << palette_id << ")!");
      _color_palette_ = palette_id;
    }

    // Set the foreground color:
    if (config_.has_key("color.foreground")) {
      _color_foreground_ = config_.fetch_string("color.foreground");
    }

    // Set the background color:
    if (config_.has_key("color.background")) {
      _color_background_ = config_.fetch_string("color.background");
    }

  } else {

    // Set the B/W mode:
    DT_THROW_IF (!config_.has_key("bw.mode"), std::logic_error,
                   "Missing 'bw.mode' configuration property!");
    std::string bw_mode_label = config_.fetch_string("bw.mode");
    if (bw_mode_label == "B/W") {
      _bw_mode_ == BW_BW;
    } else if (bw_mode_label == "GrayScale") {
      _bw_mode_ == BW_GRAY_SCALE;
    } else {
      DT_THROW(std::logic_error, "Invalid B/W mode (" << bw_mode_label << ")!");
    }
    if (_bw_mode_ == BW_UNDEFINED) {
      _bw_mode_ == BW_DEFAULT;
    }

    // Set the reverse video mode:
    if (config_.has_key("bw.reverse")) {
      _bw_reverse_ = true;
    }

  }

  // Initialization:
  if (_color_) {
    _load_color_palette_();
  }

  return;
}

void application::gui::_load_color_palette_()
{
  DT_THROW_IF(_color_palette_ < 0, std::logic_error,
              "Invalid color palette id!");
  DT_LOG_INFORMATION(datatools::logger::PRIO_INFORMATION,
                     "Loading color palette...");
  return;
}

void application::gui::run()
{
  std::clog << "Application GUI is running..." << std::endl;
  return;
}

void application::gui::print() const
{
  std::clog << "application::gui: \n";
  std::clog << "  logging = '" << _logging_  << "'" << std::endl;
  std::clog << "  color = " << _color_ << std::endl;
  std::clog << "  antialiasing = " << _antialiasing_ << std::endl;
  std::clog << "  color_palette = " << _color_palette_ << std::endl;
  std::clog << "  color_foreground = '" << _color_foreground_ << "'"  << std::endl;
  std::clog << "  color_background = '" << _color_background_ << "'"  << std::endl;
  std::clog << "  bw_mode = '" << _bw_mode_ << "'"  << std::endl;
  std::clog << "  bw_reverse = " << _bw_reverse_ << std::endl;
  return;
}

application::application()
{
  _logging_           = "warning";      // default
  _operator_          = "";             // unset
  _help_mode_         = HELP_DEFAULT;   // default
  _full_help_url_     = "";             // unset
  _full_help_manpage_ = "";             // unset
  return;
}

void application::initialize(const datatools::properties & config_)
{
  bool use_gui = false;

  // Parse configuration properties:

  if (config_.has_flag("gui")) {
    use_gui = true;
  }

  if (config_.has_key("help_mode")) {
    const std::string & help_mode = config_.fetch_string("help_mode");
    if (help_mode == "OnlyTooltips") {
      _help_mode_ == HELP_ONLY_TOOLTIPS;
    } else if (help_mode == "FullHelp") {
      _help_mode_ == HELP_FULL;
    } else {
      DT_THROW(std::logic_error, "Invalid help mode (" << help_mode << ")!");
    }
    if (_help_mode_ == HELP_FULL) {
      if (config_.has_key("full_help.manpage")) {
        _full_help_manpage_ = config_.fetch_string("help");
      }
      if (config_.has_key("full_help.url")) {
        _full_help_url_ = config_.fetch_string("url");
      }
    }
  }

  if (config_.has_key("logging")) {
    std::string logging_label = config_.fetch_string("logging");
    _logging_ = logging_label;
  }

  if (config_.has_key("operator")) {
    std::string operator_id = config_.fetch_string("operator");
    _operator_ = operator_id;
  }

  // Initialization:

  if (use_gui) {
    _gui_.reset(new gui);
    datatools::properties gui_config;
    // Extract configuration parameters dedicated to the GUI:
    config_.export_and_rename_starting_with(gui_config, "gui.", "");
    _gui_->initialize(gui_config);
  }

  {
    datatools::properties algo_config;
    // Extract configuration parameters dedicated to the algorithm:
    config_.export_and_rename_starting_with(algo_config, "algo.", "");
    _algo_.initialize(algo_config);
  }

  return;
}

void application::run()
{
  std::clog << "Application is running..." << std::endl;
  if (_gui_) {
    _gui_->run();
  }
  _algo_.run();
  return;
}

void application::print() const
{
  std::clog << "application: \n";
  std::clog << "  logging   = '" << _logging_ << "'" << std::endl;
  std::clog << "  operator  = '" << _operator_ << "'"  << std::endl;
  std::clog << "  help_mode = " << _help_mode_ << std::endl;
  if (_help_mode_ == HELP_FULL) {
    std::clog << "    full_help_url     = '" << _full_help_url_ << "'"  << std::endl;
    std::clog << "    full_help_manpage = '" << _full_help_manpage_ << "'"  << std::endl;
  }
  std::clog << "  gui       = " << (_gui_ ? "yes" : "no") << std::endl;
  if (_gui_) {
    _gui_->print();
  }
  _algo_.print();
  return;
}
