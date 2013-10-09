/* dpp_driver.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2013-10-08
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DPP_DPP_DRIVER_H_
#define DPP_DPP_DRIVER_H_ 1

// Standard library
#include <iostream>
#include <string>
#include <vector>

// Third party
// - Boost
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

// - datatools
#include <datatools/logger.h>

namespace dpp {

  struct dpp_driver_params
  {
    dpp_driver_params();
    void reset();
    void dump(std::ostream & out_ = std::clog);

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
    bool   save_stopped_data_records;
    bool   preserve_existing_files;


    static void build_opts(boost::program_options::options_description &,
                           dpp_driver_params &);

  };

  class module_manager;
  class base_module;
  class input_module;
  class output_module;

  class dpp_driver {
  public:
    dpp_driver();
    ~dpp_driver();
    bool is_initialized() const;
    void setup(const dpp_driver_params &);
    void initialize();
    void run();
    void reset();
  private:
    bool _initialized_;
    datatools::logger::priority _logging_;
    dpp_driver_params _params_;
    boost::scoped_ptr<dpp::module_manager> _module_mgr_;
    std::vector<dpp::base_module*> _modules_;
    boost::scoped_ptr<dpp::output_module> _sink_;
    boost::scoped_ptr<dpp::input_module> _source_;

  };

}

#endif // DPP_DPP_DRIVER_H_
