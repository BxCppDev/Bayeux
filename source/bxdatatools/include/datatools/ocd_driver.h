/* -*- mode: c++; -*- */
/* ocd_driver.h */

#ifndef DATATOOLS_OCD_DRIVER_H_
#define DATATOOLS_OCD_DRIVER_H_

// Standard Library
#include <string>
#include <vector>
#include <iostream>

namespace datatools {

class properties;

/// \brief Parameters of the OCD manual object
struct ocd_driver_params
{
public:
  /// Constructor
  ocd_driver_params();

  /// Reset
  void reset();

  /// Smart print
  void print(std::ostream & = std::cout) const;

public:

  bool debug; /// Debug flag
  bool interactive; /// Interactive flag
  std::vector<std::string> dlls; /// List of DLLs to be loaded
  std::string dll_loader_config; /// Configuration file of the DLL loader
  std::string action; /// The name of the action to invoke
  std::string class_id; /// The class registration ID
  std::vector<std::string> action_options; /// The options associated to the action
  std::string input_path; /// Input path
  std::string output_path; /// Output path

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

  void _run_interactive();

  int _run_action();

  int _run_list(std::ostream & out_ = std::cout, std::ostream & log_ = std::clog);

  int _run_show(const std::string & class_id_, std::ostream & out_ = std::cout);

  int _run_generate_skeleton(const std::string & class_id_,
                             const std::string & path_ = "");

  int _run_validate(const std::string & class_id_,
                    const std::string & path_);
private:

  bool _initialized_;         /// Initialization flag
  ocd_driver_params _params_; /// Configuration parameters

};

} // end of namespace datatools

#endif // DATATOOLS_OCD_DRIVER_H_
