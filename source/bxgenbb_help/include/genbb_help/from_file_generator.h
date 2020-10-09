/// \file genbb_help/from_file_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-18
 * Last modified: 2014-06-18
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
 *   A generator that read primary events from some input files
 *   using an embedded dpp::input_module. The file format is the
 *   one produced by the genbb::save_to_file_wrapper class.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_FROM_FILE_GENERATOR_H
#define GENBB_HELP_FROM_FILE_GENERATOR_H 1

// This project:
#include <genbb_help/i_genbb.h>

namespace genbb {

  /// \brief A generator that loads primary events from a file
  class from_file_generator : public i_genbb
  {
  public:

    // Forward PIMPL declaration
    struct pimpl_imp;

    /// Set the label of the primary event bank
    void set_primary_event_bank_label(const std::string &);

    /// Default constructor
    from_file_generator();

    /// Destructor
    ~from_file_generator() override;

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

    void _load_next(primary_event & event_,
                            bool compute_classification_ = true) override;

  private:

    bool        _initialized_; //!< Initialization flag
    std::string _PE_label_;    //!< Label of the "primary event" bank
    boost::scoped_ptr<pimpl_imp> _pimpl_; //!< PIMPL smart pointer

    GENBB_PG_REGISTRATION_INTERFACE(from_file_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::from_file_generator)

#endif // GENBB_HELP_FROM_FILE_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --
