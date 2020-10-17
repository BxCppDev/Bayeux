/// \file dpp/utils_cut.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   Utility cut.
 *
 */

#ifndef DPP_UTILS_CUT_H
#define DPP_UTILS_CUT_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Bayeux/:
#include <cuts/i_cut.h>

namespace datatools {
    class properties;
    class service_manager;
}

namespace dpp {

  class utils_cut
    : public cuts::i_cut
  {
  public:

    enum mode_type {
      MODE_UNDEFINED = -1,
      MODE_HAS_BANK  =  1
    };

    void set_mode (int mode_);

    void reset_mode ();

    int get_mode () const;

    bool is_mode_has_bank () const;

    void set_has_bank_name (const std::string & bank_name_);

    const std::string & get_has_bank_name () const;

    void set_has_bank_type (const std::string & bank_type_);

    const std::string & get_has_bank_type () const;

    void set_has_bank (const std::string & bank_name_,
                       const std::string & bank_type_ = "");

    utils_cut(datatools::logger::priority a_logging_priority =
              datatools::logger::PRIO_FATAL);
    ~utils_cut() override;

    /// Initialization
    void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &) override;

    /// Reset
    void reset() override;


    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  protected:
    int _accept() override;

  private:

    int _mode_;                            //!< Mode of the cut
    std::vector<std::string> _bank_names_; //!< Labels of the event record bank to be checked
    std::vector<std::string> _bank_types_; //!< Types of the event record bank to be checked (optionnal)

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(utils_cut)

  };

}  // end of namespace dpp

#endif //DPP_UTILS_CUT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// End: --
