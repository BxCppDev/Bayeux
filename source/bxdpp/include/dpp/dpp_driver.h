/// \file dpp/dpp_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2016-02-10
 *
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   The dpp driver.
 *
 */

#ifndef DPP_DPP_DRIVER_H
#define DPP_DPP_DRIVER_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/library_loader.h>

namespace dpp {

  /// \brief The set of configuration parameters for the data processing pipeline driver
  struct dpp_driver_params
  {
    /// Default constructor
    dpp_driver_params();

    /// Reset
    void reset();

    /// Print
    void dump(std::ostream & out_ = std::clog) const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const;

    bool                     help;
    std::string              logging_label;
    bool                     break_on_error_as_fatal;
    int                      print_modulo;
    std::string              module_manager_config_file;
    std::vector<std::string> module_names;
    std::string              LL_config;
    std::vector<std::string> LL_dlls;
    std::vector<std::string> input_files;
    std::vector<std::string> output_files;
    bool   no_max_records;
    int    max_records;
    int    max_records_per_output_file;
    int    slice_start;
    int    slice_stop;
    int    slice_width;
    bool   slice_store_out;
    bool   save_stopped_data_records;
    bool   preserve_existing_files;

  };

  class module_manager;
  class base_module;
  class input_module;
  class output_module;

  /// \brief The data processing pipeline driver
  class dpp_driver
    : boost::noncopyable
  {
  public:

    /// Default constructor
    dpp_driver();

    /// Destrcutor
    ~dpp_driver();

    /// Check initialization flag
    bool is_initialized() const;

    /// Setup from configuration parameters
    void setup(const dpp_driver_params &);

    /// Initialization
    void initialize();

    /// Run
    void run();

    /// Reset
    void reset();

  private:

    // Management:
    bool                        _initialized_; ///< Initialization flag
    datatools::logger::priority _logging_;     ///< Logging priority threshold

    // Parameters:
    dpp_driver_params _params_; ///< Parameters
    bool _use_slice_; ///< Slice usage flag

    // Working data:
    boost::scoped_ptr<datatools::library_loader> _lib_loader_; ///< Library loader
    boost::scoped_ptr<dpp::module_manager> _module_mgr_; ///< Manager for data processing modules
    std::vector<dpp::base_module*> _modules_; ///< Array of  data processing module handles
    boost::scoped_ptr<dpp::output_module> _sink_; ///< Output module
    boost::scoped_ptr<dpp::input_module> _source_; ///< Input module

  };

}

#endif // DPP_DPP_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
