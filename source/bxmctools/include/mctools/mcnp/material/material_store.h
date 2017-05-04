//! \file  mctools/mcnp/material/material_store.h
//! \brief MNCP material store
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MCTOOLS_MCNP_MATERIAL_MATERIAL_STORE_H
#define MCTOOLS_MCNP_MATERIAL_MATERIAL_STORE_H

// Standard library:
#include <string>
#include <iostream>
#include <vector>

// Third party:
// - Boost:
#include <boost/shared_ptr.hpp>
// - Bayeux:
#include <materials/manager.h>

// This project:
#include <mctools/mcnp/material/material.h>

namespace mctools {

  namespace mcnp {

    namespace material {

      //! \brief Dictionary of materials
      typedef std::map<int, material_ptr_type> material_dict_type;

      //! \brief Store of materials
      class material_store
      {
      public:

        //! Default constructor
        material_store();

        //! Destructor
        ~material_store();

        //! Reset
        void reset();

        //! Check material by identifier
        bool has_material(int id_) const;

        //! Add a material with identifier
        material & add_material(int id_);

        //! Remove material with identifier
        void remove_material(int id_);

        //! Return the material pointer with identifier
        const material_ptr_type & get(int id_) const;

        //! Build from Bayeux/materials manager
        void build_from_bxmaterials(const ::materials::manager &);

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

        //! Build a material
        bool build_material_from_bxmaterials(const ::materials::material & bxmat_,
                                             material & mat_);

      private:

        material_dict_type _materials_; //!< Dictionary of materials

      };

    } // namespace material

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_MATERIAL_MATERIAL_STORE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
