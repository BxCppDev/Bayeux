/// \file genbb_help/genbb_mgr.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2013-02-25
 *
 * License:
 * Copyright 2007-2013 F. Mauger
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
 *   A GENBB input data file manager class using the i_genbb interface
 *
 * History:
 *
 */

#ifndef GENBB_HELP_GENBB_MGR_H
#define GENBB_HELP_GENBB_MGR_H 1

// Standard library:
#include <string>
#include <list>
#include <iostream>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/properties.h>

// This project:
#include <genbb_help/i_genbb.h>
#include <genbb_help/primary_event.h>

// Implementation of serialization method for the 'primary_event'
// class, implies also <genbb_help/primary_particle.ipp> :
//#include <genbb_help/primary_event.ipp>

/// \brief Top-level namespace of the Bayeux/genbb_help module library
namespace genbb {

  //! \brief Legacy bridge manager that reads Genbb files
  class genbb_mgr : public i_genbb
  {
  public:

    enum format_type {
      FORMAT_GENBB = 0,
      FORMAT_BOOST = 1
    };

    static const std::string & format_genbb_label();
    static const std::string & format_boost_label();

    bool is_debug () const;

    void set_debug (bool d_);

    int get_format () const;

    void set_format (int format_ = FORMAT_GENBB);

    void set_format (const std::string & format_ = "");

    bool is_format_genbb () const;

    bool is_format_boost () const;

    /// Constructor
    genbb_mgr (int format_ = FORMAT_GENBB);

    /// Destructor
    ~genbb_mgr () override;

    void dump (std::ostream & out_ = std::clog) const;

    void set (const std::string & filename_);

    bool is_initialized () const override;

    void initialize (const datatools::properties & config_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_) override;

    void reset () override;

    bool has_next () override;

  protected:

    void _load_next (primary_event & event_,
                             bool compute_classification_ = true) override;

  private:

    void _at_init_ ();

    void _at_reset_ ();

    void _load_next_ ();

    void _load_next_genbb_ ();

    void _load_next_boost_ ();

  private:

    bool                   _debug_;        //!< Debug flag
    bool                   _initialized_;  //!< Initialization flag
    std::list<std::string> _filenames_;    //!< List of input files' name
    std::string            _current_filename_; //!< Current file's name
    int                    _format_;       //!< Format of the input file
    std::istream *         _in_;           //!< Handle to the current input stream
    std::ifstream          _fin_;          //!< Current input file stream
    datatools::data_reader _reader_;       //!< GENBB event reader
    primary_event          _current_;      //!< Current primary event
    double                 _genbb_weight_; //!< GENBB event weight (for DBD energy range)

    GENBB_PG_REGISTRATION_INTERFACE(genbb_mgr)

  };

} // end of namespace genbb

#endif // GENBB_HELP_GENBB_MGR_H

// Local Variables: --
// mode: c++ --
// End: --
