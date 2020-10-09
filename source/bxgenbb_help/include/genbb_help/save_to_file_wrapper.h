/// \file genbb_help/save_to_file_wrapper.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-05
 * Last modified: 2014-06-05
 *
 * License:
 * Copyright 2014 F. Mauger
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
 *   A generator wrapper that use another generator to
 *   shoot primary events but save them on the fly in some
 *   files using an embedded dpp::output_module.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_SAVE_TO_FILE_WRAPPER_H
#define GENBB_HELP_SAVE_TO_FILE_WRAPPER_H 1

// This project:
#include <genbb_help/i_genbb.h>

namespace genbb {

  /// \brief Wrapper for another generator
  class save_to_file_wrapper : public i_genbb
  {
  public:

    // Forward PIMPL declaration
    struct pimpl_imp;

    /// Return the default label for the "primary event" bank
    static const std::string & default_pe_label();

    /// Set the label of the primary event bank
    void set_primary_event_bank_label(const std::string &);

    /// Set the particle generator
    void set_generator(i_genbb &);

    /// Default constructor
    save_to_file_wrapper();

    /// Destructor
    ~save_to_file_wrapper() override;

    /// Main initialization interface method
    void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check if the generator has a next event
    bool has_next() override;

    /// Check initialization status
    bool is_initialized() const override;

  protected:

    /// Load next primary event from the generator algorithm
    void _load_next(primary_event & event_,
                            bool compute_classification_ = true) override;

  private:

    bool        _initialized_; //!< Initialization flag
    i_genbb *   _pg_;          //!< Handle to a particle generator
    std::string _PE_label_;    //!< Label of the "primary event" bank
    boost::scoped_ptr<pimpl_imp> _pimpl_; //!< PIMPL smart pointer

    GENBB_PG_REGISTRATION_INTERFACE(save_to_file_wrapper)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::save_to_file_wrapper)

#endif // GENBB_HELP_SAVE_TO_FILE_WRAPPER_H

// Local Variables: --
// mode: c++ --
// End: --
