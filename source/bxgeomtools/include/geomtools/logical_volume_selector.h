/// \file geomtools/logical_volume_selector.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2020-07-18
 * Last modified: 2020-07-18
 *
 * License:
 *
 * Copyright 2020 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Logical volume selector
 *
 */

#ifndef GEOMTOOLS_LOGICAL_VOLUME_SELECTOR_H
#define GEOMTOOLS_LOGICAL_VOLUME_SELECTOR_H 1

// Standard library:
#include <string>
#include <set>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

namespace geomtools {

  class manager;

  /// \brief Select a set of logical volumes from a geometry manager given some inclusion/exclusion rules
  class logical_volume_selector
  {
  public:
    logical_volume_selector(const manager & mgr_);
    void set_clear_first(bool);
    void add_all();
    void add_by_name(const std::string & logical_volume_name_);
    void add_by_name_regex(const std::string & logical_volume_name_regex_);
    void add_by_material(const std::string & material_name_);
    void add_by_material_regex(const std::string & material_name_regex_);
    void exclude_by_name(const std::string & logical_volume_name_);
    void exclude_by_name_regex(const std::string & logical_volume_name_);
    void exclude_by_material(const std::string & material_name_);
    void exclude_by_material_regex(const std::string & material_name_regex_);
    void initialize(const datatools::properties & config_);
    void reset();
    void resolve(std::set<std::string> & logical_volumes_names_);

  private:

    const manager & _mgr_;
    bool _clear_first_ = false;
    std::set<std::string> _lv_names_;                     ///< Explicit list of logical volume names to be selected
    std::set<std::string> _lv_names_regex_;               ///< Regex on logical volume names to be selected 
    std::set<std::string> _lv_materials_;                 ///< Explicit list of material names for the logical volume names to be selected
    std::set<std::string> _lv_materials_regex_;           ///< Regex on material names for the logical volume names to be selected
    std::set<std::string> _lv_names_excluded_;            ///< Explicit list of logical volume names to be excluded from the selection
    std::set<std::string> _lv_names_regex_excluded_;      ///< Regex on logical volume names to be excluded from the selection
    std::set<std::string> _lv_materials_excluded_;        ///< Explicit list of material names for the logical volume names to be excludedselected
    std::set<std::string> _lv_materials_regex_excluded_;  ///< Regex on material names to be excluded from the selection
   
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_LOGICAL_VOLUME_SELECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
