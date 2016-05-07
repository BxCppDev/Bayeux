#ifndef DATATOOLS_EXAMPLE_VARIANTS_FOO_H
#define DATATOOLS_EXAMPLE_VARIANTS_FOO_H

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// Bayeux/datatools:
// #include <datatools/datatools_config.h>

namespace datatools {
  class properties;
  class multi_properties;
}

/// \brief The application
class application
{
public:

  /// \brief The algorithm component
  class algo
  {
  public:

    /// \brief Running modes of the algorithm
    enum algo_mode_type {
      AM_UNDEFINED  = 0,
      AM_FAST,
      AM_SLOW,
      AM_DEFAULT = AM_FAST
    };

    /// Default constructor
    algo();

    /// Initialization
    void initialize(const datatools::properties & config_);

    /// Run
    void run();

    /// Basic print
    void print() const;

  private:

    std::string    _logging_;     //!< The logging priority threshold
    double         _max_time_;    //!< The maximum computing time
    algo_mode_type _mode_;        //!< Mode
    int            _accuracy_;    //!< Accuracy
    int            _nevents_;     //!< Number of events

  };

  /// \brief The GUI component
  class gui
  {
  public:

    /// \brief
    enum bw_mode_type {
      BW_UNDEFINED  = 0,
      BW_BW,
      BW_GRAY_SCALE,
      BW_DEFAULT = BW_BW
    };

    /// Default constructor
    gui(algo & a_);

    /// Initialization
    void initialize(const datatools::properties & config_);

    /// Run
    void run();

    /// Basic print
    void print() const;

    /// Check color mode
    bool is_colored() const;

  private:

    /// Load the color palette
    void _load_color_palette_();

  private:

    algo * _algo_; //!< The handle to the algo
    std::string  _logging_;          //!< The logging priority threshold
    bool         _antialiasing_;     //!< The flag to activate antialiasing
    bool         _color_;            //!< The flag to activate colors
    int          _color_palette_;    //!< The color palette id
    std::string  _color_foreground_; //!< The foreground color
    std::string  _color_background_; //!< The background color
    bw_mode_type _bw_mode_;          //!< The B/W mode
    bool         _bw_reverse_;       //!< The flag to activate reverse video (B/W- > W/B)

  };

  /// \brief The GUI plugin component
  class plugin
  {
  public:

    /// Default constructor
    plugin(gui &);

    /// Initialization
    void initialize(const datatools::properties & config_);

    /// Run
    void run();

    /// Basic print
    void print() const;

  private:

    gui * _gui_;
    std::string _logging_; //!< The logging priority threshold
    int         _dummy_;   //!< Dummy

  };

  /// \bried The help mode
  enum help_mode_type {
    HELP_UNDEFINED     = 0,
    HELP_FULL,
    HELP_ONLY_TOOLTIPS,
    HELP_DEFAULT = HELP_ONLY_TOOLTIPS
  };

  /// Default constructor
  application();

  /// Initialization
  void initialize(const datatools::properties & config_);

  /// Initialization
  void initialize_m(const datatools::multi_properties & mconfig_);

  /// Run
  void run();

  /// Basic print
  void print() const;

private:

  std::string    _logging_;           //!< The logging priority threshold
  std::string    _operator_;          //!< The operator id
  help_mode_type _help_mode_;         //!< The help mode
  std::string    _full_help_manpage_; //!< The path to the manpage in full help mode
  std::string    _full_help_url_;     //!< The url to online help in full help mode
  bool           _use_gui_;           //!< The flag for GUI usage
  boost::scoped_ptr<gui> _gui_;       //!< The handle to the GUI component
  algo           _algo_;              //!< The embedded algorithm
  bool           _use_plugin_;        //!< The flag for plugin usage
  boost::scoped_ptr<plugin> _plugin_; //!< The handle to the plugin component

};

#endif // DATATOOLS_EXAMPLE_VARIANTS_FOO_H
