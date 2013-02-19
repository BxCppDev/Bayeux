/* utils_cut.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-18
 * Last modified : 2013-02-18
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
 *
 *
 * Description:
 *
 *   Utility cut.
 *
 * History:
 *
 */

#ifndef DPP_UTILS_CUT_H_
#define DPP_UTILS_CUT_H_ 1

#include <string>
#include <vector>

#include <dpp/base_cut.h>

namespace datatools {
#if DPP_DATATOOLS_LEGACY == 1
  namespace utils {
#endif
    class properties;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif
#if DPP_DATATOOLS_LEGACY == 1
  namespace service {
#endif
    class service_manager;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif
}

namespace dpp {

  class utils_cut : public base_cut
  {

  public:

    enum mode_type
      {
        MODE_UNDEFINED = 0,
        MODE_HAS_BANK  = 1,
      };

    void set_mode (int mode_);

    int get_mode () const;

    // mode HAS_BANK:
    bool is_mode_has_bank () const;

    void set_has_bank_name (const std::string & bank_name_);

    const std::string & get_has_bank_name () const;

    void set_has_bank_type (const std::string & bank_type_);

    const std::string & get_has_bank_type () const;

    void set_has_bank (const std::string & bank_name_,
                       const std::string & bank_type_ = "");

    CUT_INTERFACE_CTOR_DTOR (utils_cut);

    // ctor:
    utils_cut (const std::string & cut_name_,
               const std::string & cut_description_ = "",
               const std::string & cut_version_     = "",
               int debug_level_ = 0);

    /*
      virtual void tree_dump (ostream & a_out         = clog,
      const std::string & a_title  = "",
      const std::string & a_indent = "",
      bool a_inherit          = false) const;
    */

  private:

    int _mode_;                            //!< Mode of the cut

    std::vector<std::string> _bank_names_; //!< Labels of the event record bank to be checked
    std::vector<std::string> _bank_types_; //!< Types of the event record bank to be checked (optionnal)

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(utils_cut);

  };

}  // end of namespace dpp

#endif //DPP_UTILS_CUT_H_

// end of utils_cut.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
