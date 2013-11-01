/* output_module.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2013-08-16
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
 *
 * Description:
 *
 *   An output data processing module.
 *
 * History:
 *
 */

#ifndef DPP_OUTPUT_MODULE_H_
#define DPP_OUTPUT_MODULE_H_ 1

#include <boost/scoped_ptr.hpp>

#include <dpp/base_module.h>    // data processing module abstract base class

#include <datatools/smart_filename.h>

namespace dpp {

  class i_data_sink;
  class io_common;

  /// \brief A output data processing module for automated I/O operations
  DPP_MODULE_CLASS_DECLARE(output_module)
  {
  public:

    DPP_MODULE_INTERFACE_CTOR_DTOR(output_module);

    void set_limits(int max_record_total_,
                    int max_record_per_file_ = 0,
                    int max_files_ = -1);

    void set_single_output_file(const std::string & filepath_);

    void set_list_of_output_files(const std::vector<std::string> & filepaths_,
                                  bool allow_duplicate_ = false);

    void set_incremental_output_files(const std::string & path_,
                                      const std::string & prefix_,
                                      const std::string & extension_,
                                      unsigned int stop_,
                                      unsigned int start_ = 0,
                                      int increment_ = 1);

    void set_preserve_existing_output (bool a_preserve_existing_output);

    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    bool is_terminated () const;

    const io_common & get_common() const;

  protected:

    int _store (const datatools::things & a_event_record);

    void _set_defaults ();

    io_common & _grab_common();

  private:

    bool                         _preserve_existing_output_; //!< Flag to preserve existing output files
    boost::scoped_ptr<io_common> _common_; //!< Common data structure
    i_data_sink                * _sink_;   //!< Abstract data writer

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(output_module);

  };

} // namespace dpp

#endif // DPP_OUTPUT_MODULE_H_

// end of output_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
