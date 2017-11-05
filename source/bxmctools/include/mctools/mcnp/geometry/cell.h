//! \file  mctools/mcnp/geometry/cell.h
//! \brief Description of MNCP cell
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

#ifndef MCTOOLS_MCNP_GEOMETRY_CELL_H
#define MCTOOLS_MCNP_GEOMETRY_CELL_H

// Standard library:
#include <string>
#include <iostream>
#include <list>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

// This project:
#include <mctools/mcnp/geometry/surface_type.h>
#include <mctools/mcnp/utils.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      class cell;

      //! Shared pointer on a cell
      typedef boost::shared_ptr<cell> cell_ptr_type;

      //! \brief MCNP cell
      class cell
      {
      public:

        //! Invalid cell identifier
        static const int INVALID_ID =     -1;
        static const int MIN_ID     =      1;
        static const int MAX_ID     =  99999;
        static const int MATERIAL_VOID =   0;

        enum surface_operator_type {
          SURF_OP_INVALID      = -1, ///< Invalid surface operator
          SURF_OP_INTERSECTION =  0, ///< Surface intersection operator
          SURF_OP_UNION        =  1, ///< Surface union operator
          SURF_OP_COMPLEMENT   =  2  ///< Surface complement operator
        };

        enum surface_int_ext_type {
          SURF_IE_INVALID  = -1, ///< Invalid surface interior/exterior
          SURF_IE_INTERIOR =  0, ///< Surface interior
          SURF_IE_EXTERIOR =  1  ///< Surface exterior
        };

        //! \brief Surface node
        struct surface_node_type
        {
        public:

          //! Default constructor
          surface_node_type();

          //! Make the node a leaf
          void set_leaf(int surface_id_, surface_int_ext_type sie_);

          //! Make the node an operator branch
          void set_branch(surface_operator_type surface_op_);

          //! Check if the node is valid
          bool is_valid() const;

          //! Check if the node is a surface leaf
          bool is_leaf() const;

          //! Check if the node is a surface operator branch
          bool is_branch() const;

          //! Reset
          void reset();

          //! Add a surface leaf
          surface_node_type & add_leaf(int surface_id_, surface_int_ext_type sie_);

          //! Add a surface operator branch
          surface_node_type & add_branch(surface_operator_type);

          //! Smart tree print
          void tree_dump(std::ostream & out_, int indent_level_ = 0) const;

          //! Print MCNP card
          void to_card(std::ostream & out_) const;

        public:

          const surface_node_type * parent;
          // Leaf
          int                       surface_id;
          surface_int_ext_type      surface_ie;
          // Branch
          surface_operator_type     op;
          std::list<surface_node_type> children;

        };

        //! Default constructor
        cell();

        //! Destructor
        ~cell();

        //! Set the name
        void set_name(const std::string &);

        //! Return the name
        const std::string & get_name() const;

        //! Set the identifier
        void set_id(int);

        //! Return the identifier
        int get_id() const;

        //! Check if the cell is 'like'
        bool is_like() const;

        //! Set the cell identifier in like mode
        void set_like_id(int);

        //! Return the cell identifier in like mode
        int get_like_id() const;

        //! Set the material identifier
        void set_material_id(int);

        //! Return the material identifier
        int get_material_id() const;

        //! Set the material density
        void set_material_density(double);

        //! Return the identifier
        double get_material_density() const;

        //! Check if the cell is 'void'
        bool is_void() const;

        //! Return a non mutable reference to the top surface node
        const surface_node_type & get_surface_node() const;

        //! Return a mutable reference to the top surface node
        surface_node_type & grab_surface_node();

        //! Reset
        void reset();

        //! Check if cell is valid
        bool is_valid() const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

      protected:

        //! Set default values
        void _set_defaults();

      private:

        std::string  _name_;   //!< Name of the cell
        int          _id_;     //!< Identifier of the cell
        int          _like_id_; //!< Identifier of the cell model
        //        int          _like_but_it_

        // Material:
        int          _material_id_; //!< Identifier of the material
        double       _material_density_; //!< Material density

        // Surfaces:
        surface_node_type _surface_node_;

      };

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_GEOMETRY_CELL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
