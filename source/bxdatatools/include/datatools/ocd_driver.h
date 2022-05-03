/// \file datatools/ocd_driver.h

#ifndef DATATOOLS_OCD_DRIVER_H
#define DATATOOLS_OCD_DRIVER_H

// Standard Library:
#include <string>
#include <vector>
#include <iostream>
#include <memory>

// Third Party:
// #include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/logger.h>

namespace datatools {

  class properties;
  class library_loader;

  /// \brief Parameters of the OCD manual object
  struct ocd_driver_params
  {
  public:
    /// Constructor
    ocd_driver_params() = default;

    /// Reset
    void reset();

    /// Smart print
    void print(std::ostream & = std::cout) const;

  public:

    datatools::logger::priority logging = datatools::logger::PRIO_NOTICE; //!< Logging priority
    bool interactive = false; //!< Interactive flag
    std::vector<std::string> dlls; //!< List of DLLs to be loaded
    std::string dll_loader_config; //!< Configuration file of the DLL loader
    std::string action; //!< The name of the action to invoke
    std::string class_id; //!< The class registration ID
    std::vector<std::string> action_options; //!< The options associated to the action
    std::string input_path; //!< Input path
    std::string output_path; //!< Output path

  };


  /// \brief The OCD manual application
  class ocd_driver
  {
  public:

    /// Constructor
    ocd_driver();

    /// Destructor
    virtual ~ocd_driver();

    /// Check initialization status
    bool is_initialized() const;

    /// Initialization
    /* future: consider to use a 'datatools::properties' object */
    void initialize(const ocd_driver_params & params_);

    /// Run
    int run();

    /// Reset
    void reset();

  protected:

    /// Interactive session
    void _run_interactive();

    /// Run the requested action
    int _run_action();

    /// Run the 'list' action
    int _run_list(std::ostream & out_ = std::cout, std::ostream & log_ = std::clog);

    /// Run the 'show' action
    int _run_show(const std::string & class_id_, std::ostream & out_ = std::cout);

    /// Run the 'generate skeleton' action
    int _run_generate_skeleton(const std::string & class_id_,
                               const std::string & path_ = "");

    /// Run the 'validate' action
    int _run_validate(const std::string & class_id_,
                      const std::string & path_);
  private:

    bool _initialized_;         //!< Initialization flag
    ocd_driver_params _params_; //!< Configuration parameters
    std::unique_ptr<library_loader> _dll_loader_; //!< Handle to the library loader

  };

} // end of namespace datatools

#endif // DATATOOLS_OCD_DRIVER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
