/* kernel.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-26
 * Last modified: 2013-09-26
 *
 * License:
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  The datatools kernel singleton with embedded resources and components.
 *
 */

#ifndef DATATOOLS_KERNEL_H_
#define DATATOOLS_KERNEL_H_

// Standard Library
#include <iostream>

// Third Party
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

// This project
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

namespace datatools {

  class library_info;

  /// \brief The datatools kernel singleton with embedded resources and components.
  class kernel :
    private boost::noncopyable,
    public ::datatools::i_tree_dumpable
  {

  public:

    /// \brief The set of configuration parameters for the datatools' kernel
    struct param_type {
      bool                     help;
      std::string              logging_label;
      std::string              library_info_logging_label;
      bool                     inhibit_library_info_register;
      std::vector<std::string> unrecognized_args;
      std::vector<std::string> resource_paths;
      bool                     splash;
      param_type();
      void reset();
    };

    /// Default constructor
    kernel();

    /// Constructor with on-the-fly initialization
    kernel(int argc_, char * argv_[]);

    /// Destruction with on-the-fly shutdown if needed
    ~kernel();

    /// Initialization from command line arguments
    void initialize(int argc_, char * argv_[]);

    /// Shutdown
    void shutdown();

    /// Check initialization status
    bool is_initialized() const;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Check if the library/component info register is available
    bool has_library_info_register() const;

    /// Return a mutable reference to the library/component info registe
    library_info & grab_library_info_register();

    /// Return a non mutable reference to the library/component info registe
    const library_info & get_library_info_register() const;

    /// Return the name of the current application
    const std::string & get_application_name() const;

    /// Check if the name of the current application is defined
    bool has_application_name() const;

    /// Check if the kernel singleton is available
    static bool is_instantiated();

    /// Return a mutable reference to the kernel singleton
    static kernel & instance();

    /// Instantiate and return a mutable reference to the kernel singleton
    static kernel & instantiate();

    /// Return a non mutable reference to the kernel singleton
    static const kernel & const_instance();

    static void build_opt_desc(boost::program_options::options_description &, param_type &);

    static void print_splash(std::ostream & = std::clog);

    static void print_opt_desc(const boost::program_options::options_description &,
                               std::ostream & = std::clog,
                               const std::string & indent_ = "");

    /// Smart print
    void tree_dump(std::ostream& out_ = std::clog,
                   const std::string& title_ = "",
                   const std::string& indent_ = "",
                   bool inherit_ = false) const;

  protected:

    void _initialize(const param_type & params_);

  private:

    /// Basic construction
    void _construct_();

    bool             _initialized_; /// Initialization flag
    logger::priority _logging_;     /// Logging priority
    bool             _activate_library_info_register_; /// Activation flag for Library/component information register
    boost::scoped_ptr<library_info> _library_info_register_; /// Library/component information register
    std::string     _application_name_; /// The name of the current application
    static kernel * _instance_; /// Singleton handle

  };

} // end of namespace datatools

#endif // DATATOOLS_KERNEL_H_
