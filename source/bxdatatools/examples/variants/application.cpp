// Ourselves:
#include "application.hpp"

// Standard library:
#include <iostream>
#include <cstdlib>
#include <cmath>

// Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/clhep_units.h>

application::plugin::plugin(gui & g_)
{
  _gui_     = &g_;
  DT_THROW_IF(!_gui_->is_colored(), std::logic_error, "GUI does not run in no-color mode!");
  _logging_ = "fatal"; // default
  _dummy_   = -1;      // unset
  return;
}

void application::plugin::initialize(const datatools::properties & config_)
{
  std::clog << "Initializing plugin..." << std::endl;
  // Parse configuration properties:

  if (config_.has_key("logging")) {
    std::string logging_label = config_.fetch_string("logging");
    _logging_ = logging_label;
  }

  if (config_.has_key("dummy")) {
    int dummy = config_.fetch_integer("dummy");
    DT_THROW_IF(dummy < 0, std::range_error, "Invalid dummy (" << dummy << ")!");
    _dummy_ = dummy;
  }

  return;
}

void application::plugin::run()
{
  std::clog << "Application plugin is running with dummy=" << _dummy_ << "..." << std::endl;
  return;
}

void application::plugin::print(const std::string & indent_) const
{
  std::clog << indent_ << "application::plugin: \n";
  std::clog << indent_ <<"  logging  = '" << _logging_  << "'" << std::endl;
  std::clog << indent_ <<"  dummy    = " << _dummy_ << std::endl;
  return;
}

application::algo::algo()
{
  _logging_  = "fatal";             // default
  _max_time_ = 1.0 * CLHEP::second; // default
  _mode_     = AM_UNDEFINED;        // unset
  _accuracy_ = -1;                  // unset
  _nevents_  = -1;                  // unset
  return;
}

void application::algo::initialize(const datatools::properties & config_)
{
  std::clog << "Initializing algo..." << std::endl;
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
    DT_THROW_IF(nevents < 1 || nevents > 20,
                std::range_error, "Invalid number of events (" << nevents << ")!");
    _nevents_ = nevents;
  }

  // Initialization

  return;
}

void application::algo::run()
{
  std::clog << "Application algorithm is running..." << std::endl;
  double tau = 0.25 * CLHEP::second;
  if (_mode_ == AM_SLOW) {
    tau = 0.4 * CLHEP::second;
  }
  tau *= _accuracy_;
  for (int ievent = 0; ievent < _nevents_; ievent++) {
    std::clog << "\tProcessing event #" << ievent << "... ";
    double duration = -tau * std::log(drand48());
    if (duration > _max_time_) {
      std::clog << "(maximum time was reached)";
    } else {
      std::clog << "ok";
    }
    std::clog << std::endl;
  }
  return;
}

void application::algo::print(const std::string & indent_) const
{
  std::clog << indent_ << "application::algo: \n";
  std::clog << indent_ << "  logging  = '" << _logging_  << "'" << std::endl;
  std::clog << indent_ << "  max_time = " << _max_time_ / CLHEP::second << " s" << std::endl;
  std::clog << indent_ << "  mode     = " << _mode_ << std::endl;
  std::clog << indent_ << "  accuracy = " << _accuracy_  << std::endl;
  std::clog << indent_ << "  nevents  = " << _nevents_ << std::endl;
  return;
}

application::gui::gui(algo & a_)
{
  _algo_             = &a_;          // set
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
  std::clog << "Initializing gui..." << std::endl;

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

bool application::gui::is_colored() const
{
  return _color_;
}

void application::gui::print(const std::string & indent_) const
{
  std::clog << indent_ << "application::gui: \n";
  std::clog << indent_ << "  logging = '" << _logging_  << "'" << std::endl;
  std::clog << indent_ << "  color = " << _color_ << std::endl;
  std::clog << indent_ << "  antialiasing = " << _antialiasing_ << std::endl;
  std::clog << indent_ << "  color_palette = " << _color_palette_ << std::endl;
  std::clog << indent_ << "  color_foreground = '" << _color_foreground_ << "'"  << std::endl;
  std::clog << indent_ << "  color_background = '" << _color_background_ << "'"  << std::endl;
  std::clog << indent_ << "  bw_mode = '" << _bw_mode_ << "'"  << std::endl;
  std::clog << indent_ << "  bw_reverse = " << _bw_reverse_ << std::endl;
  return;
}

application::application()
{
  _logging_           = "warning";    // default
  _operator_          = "";           // unset
  _help_mode_         = HELP_DEFAULT; // default
  _full_help_url_     = "";           // unset
  _full_help_manpage_ = "";           // unset
  _use_gui_           = false;        // default
  _use_plugin_        = false;        // default
  return;
}

void application::initialize_m(const datatools::multi_properties & mconfig_)
{
  std::clog << "Initializing core (m)..." << std::endl;
  // mconfig_.tree_dump(std::clog, "Core multi-config: ", "DEBUG: ");

  if (mconfig_.has_section("core")) {
    const datatools::properties & core_config = mconfig_.get_section("core");

    if (core_config.has_key("help_mode")) {
      const std::string & help_mode = core_config.fetch_string("help_mode");
      if (help_mode == "OnlyTooltips") {
        _help_mode_ == HELP_ONLY_TOOLTIPS;
      } else if (help_mode == "FullHelp") {
        _help_mode_ == HELP_FULL;
      } else {
        DT_THROW(std::logic_error, "Invalid help mode (" << help_mode << ")!");
      }
      if (_help_mode_ == HELP_FULL) {
        if (core_config.has_key("full_help.manpage")) {
          _full_help_manpage_ = core_config.fetch_string("help");
        }
        if (core_config.has_key("full_help.url")) {
          _full_help_url_ = core_config.fetch_string("url");
        }
      }
    }

    if (core_config.has_key("logging")) {
      std::string logging_label = core_config.fetch_string("logging");
      _logging_ = logging_label;
    }

    if (core_config.has_key("operator")) {
      std::string operator_id = core_config.fetch_string("operator");
      _operator_ = operator_id;
    }

    if (core_config.has_flag("gui")) {
      _use_gui_ = true;
      if (core_config.has_flag("plugin")) {
      _use_plugin_ = true;
      }
    }

  }

  // Initialization:

  {
    const datatools::properties & algo_config = mconfig_.get_section("algo");
    _algo_.initialize(algo_config);
  }

  if (_use_gui_) {
    _gui_.reset(new gui(_algo_));
    const datatools::properties & gui_config = mconfig_.get_section("gui");
    _gui_->initialize(gui_config);

    if (_use_plugin_) {
      _plugin_.reset(new plugin(*_gui_));
      const datatools::properties & plugin_config = mconfig_.get_section("plugin");
      _plugin_->initialize(plugin_config);
    }
  }

  return;
}

void application::initialize(const datatools::properties & config_)
{
  std::clog << "Initializing core..." << std::endl;
  config_.tree_dump(std::clog, "Core config: ", "DEBUG: ");

  // Parse core configuration properties:

  if (config_.has_key("test.A.level")) {
    std::clog << "INFO: Found 'test.A.level' parameter..." << std::endl;
  }

  if (config_.has_key("test.A.logging")) {
    std::clog << "INFO: Found 'test.A.logging' parameter..." << std::endl;
  }

  if (config_.has_key("test.X.threshold")) {
    std::clog << "INFO: Found 'test.X.threshold' parameter..." << std::endl;
  }

  if (config_.has_key("test.X.time_gate")) {
    std::clog << "INFO: Found 'test.X.time_gate' parameter..." << std::endl;
  }

  if (config_.has_key("test.B.dummy")) {
    std::clog << "INFO: Found 'test.B.dummy' parameter..." << std::endl;
  }

  if (config_.has_key("test.C.foo")) {
    std::clog << "INFO: Found 'test.C.foo' parameter..." << std::endl;
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

  if (config_.has_flag("gui")) {
    _use_gui_ = true;
    if (config_.has_flag("plugin")) {
      _use_plugin_ = true;
    }
  }

  // Initialization:

  {
    datatools::properties algo_config;
    // Extract configuration parameters dedicated to the algorithm:
    config_.export_and_rename_starting_with(algo_config, "algo.", "");
    _algo_.initialize(algo_config);
  }

  if (_use_gui_) {
    _gui_.reset(new gui(_algo_));
    datatools::properties gui_config;
    // Extract configuration parameters dedicated to the GUI:
    config_.export_and_rename_starting_with(gui_config, "gui.", "");
    _gui_->initialize(gui_config);

    if (_use_plugin_) {
      _plugin_.reset(new plugin(*_gui_));
      datatools::properties plugin_config;
      // Extract configuration parameters dedicated to the plugin:
      config_.export_and_rename_starting_with(plugin_config, "plugin.", "");
      _plugin_->initialize(plugin_config);
    }

  }

  return;
}

void application::run()
{
  std::clog << "Application is running..." << std::endl;
  if (_gui_) {
    _gui_->run();
    if (_plugin_) {
      _plugin_->run();
    }
  }
  _algo_.run();
  return;
}

void application::print(const std::string & indent_) const
{
  std::clog << indent_ << "application: \n";
  std::clog << indent_ << "  logging   = '" << _logging_ << "'" << std::endl;
  std::clog << indent_ << "  operator  = '" << _operator_ << "'"  << std::endl;
  std::clog << indent_ << "  help_mode = " << _help_mode_ << std::endl;
  if (_help_mode_ == HELP_FULL) {
    std::clog << indent_ << "    full_help_url     = '" << _full_help_url_ << "'"  << std::endl;
    std::clog << indent_ << "    full_help_manpage = '" << _full_help_manpage_ << "'"  << std::endl;
  }
  _algo_.print(indent_ + "  ");
  std::clog << indent_ << "  gui       = " << (_gui_ ? "yes" : "no") << std::endl;
  if (_gui_) {
    _gui_->print(indent_ + "  ");
  }
  std::clog << indent_ << "  plugin    = " << (_plugin_ ? "yes" : "no") << std::endl;
  if (_plugin_) {
    _plugin_->print(indent_ + "  ");
  }
  return;
}
