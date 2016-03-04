/// \file dpp/dump_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2015-05-13
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A dump data processing module.
 *
 * History:
 *
 */

#ifndef DPP_DUMP_MODULE_H
#define DPP_DUMP_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module to dump data records
  class dump_module : public base_module
  {
  public:

    enum output_type
    {
      OUTPUT_INVALID = 0,
      OUTPUT_COUT,
      OUTPUT_CLOG,
      OUTPUT_FILE,
      OUTPUT_DEFAULT = OUTPUT_COUT
    };

  public:

    /// Set the title:
    void set_title(const std::string & a_title);

    /// Set the indenting string:
    void set_indent(const std::string & a_indent);

    // Set the output:
    void set_output(const std::string & a_output,
                    const std::string & a_file = "");

    /// Constructor
    dump_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~dump_module();

    /// Initialization
    virtual void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */);

    /// Reset
    virtual void reset();

    /// Data record processing
    virtual process_status process(::datatools::things & /* data_ */);

    /// Smart print :
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const;

  protected:

    /// Set default values before explicit settings and initialization
    void _set_defaults ();

    /// Get a reference to the output stream
    std::ostream & _grab_output ();

  private:

    std::string    _indent_; //!< Indent string
    std::string    _title_; //!< Title string
    int            _output_;          //!< The output stream
    std::string    _output_filename_; //!< The name of the output file
    boost::scoped_ptr<std::ofstream > _fout_; //!< Scoped pointer for output file
    std::ostream * _out_;             //<! Output stream handle

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(dump_module)

  };

}  // end of namespace dpp

#endif // DPP_DUMP_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
