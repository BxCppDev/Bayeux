/// \file cuts/cut_tools.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-03-22
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
 *   Typedefs for the cut factory/manager.
 *
 * History:
 *
 */

#ifndef CUTS_CUT_TOOLS_H
#define CUTS_CUT_TOOLS_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <exception>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>

namespace datatools {
  // Forward declarations:
  class properties;
  class service_manager;
}

namespace cuts {

  //! \brief Enumeration of cut selection return status
  enum selection_result_type {
    SELECTION_INAPPLICABLE = -1, //!< Returned by the selection method when applying some criteria makes no sense in the current context
    SELECTION_REJECTED     =  0, //!< Returned by the selection method when some criteria is not passed
    SELECTION_ACCEPTED     =  1  //!< Returned by the selection method when some criteria is passed
  };

  // Forward declarations:
  class i_cut;
  class cut_manager;

  //! Alias type for a cut handle
  typedef datatools::handle<i_cut> cut_handle_type;

  /** \brief Internal record that handles a dynamically allocated cut
   *
   *   It also contains additional setup/status informations.
   */
  class cut_entry_type : public datatools::i_tree_dumpable
  {
  public:

    /// Status of the cut entry
    enum status_type {
      STATUS_BLANK       = 0,
      STATUS_CREATED     = datatools::bit_mask::bit00,
      STATUS_INITIALIZED = datatools::bit_mask::bit01
    };

    /// Return non mutable cut configuration
    const datatools::properties & get_cut_config() const;

    /// Return mutable cut configuration
    datatools::properties & grab_cut_config();

    /// Set the cut configuration
    void set_cut_config(const datatools::properties &);

    /// Return the cut name
    const std::string & get_cut_name() const;

    /// Set the cut name
    void set_cut_name(const std::string &);

    /// Return the cut class identifier
    const std::string & get_cut_id() const;

    /// Set the address of the embedded
    void set_ptr(i_cut *);

    /// Set the cut class identifier
    void set_cut_id(const std::string &);

    /// Check the availability of a cut manager
    bool has_manager() const;

    /// Set the cut manager address
    void set_manager(cut_manager &);

    /// Set the blank flag
    void set_blank();

    /// Set the 'creation' flag
    void set_created();

    /// Set the 'initialization' flag
    void set_initialized();

    /// Unset the 'initialization' flag
    void set_uninitialized();

    /// Check the 'creation' flag
    bool is_created() const;

    /// Check the 'initialization' flag
    bool is_initialized() const;

    /// Constructor
    cut_entry_type();

    /// Check is the cut is referenced in the embedded handle
    bool has_cut() const;

    /// Return a non mutable reference to the cut
    const i_cut & get_cut() const;

    /// Return a mutable reference to the cut
    i_cut & grab_cut();

    /// Return a non mutable handle to the cut
    const cut_handle_type & get_cut_handle() const;

    /// Return a mutable handle to the cut
    cut_handle_type & grab_cut_handle();

    /// Return a mutable handle to the initialized cut
    cut_handle_type & grab_initialized_cut_handle();

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  private:

    std::string           _cut_name_;   //!< Cut name
    std::string           _cut_id_;     //!< Cut class registration ID
    datatools::properties _cut_config_; //!< The configuration of the cut
    uint32_t              _cut_status_; //!< The status of the cut
    cut_handle_type       _cut_handle_; //!< The handle for the allocated service
    cut_manager *         _manager_;    //!< Handle to the module manager

    friend class cut_manager;

  };

  //! Alias type of a dictionary of cut entry
  typedef std::map<std::string, cut_entry_type> cut_handle_dict_type;

  //! Alias type of an array of cut names
  typedef std::vector<std::string> ordered_cut_list_type;

} // end of cuts

#endif // CUTS_CUT_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
