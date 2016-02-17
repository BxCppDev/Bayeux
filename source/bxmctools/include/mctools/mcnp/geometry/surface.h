//! \file  mctools/mcnp/geometry/surface.h
//! \brief Description of MNCP surface
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

#ifndef MCTOOLS_MCNP_GEOMETRY_SURFACE_H
#define MCTOOLS_MCNP_GEOMETRY_SURFACE_H

// Standard library:
#include <string>
#include <iostream>

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

      class surface;

      //! Shared pointer on a surface
      typedef boost::shared_ptr<surface> surface_ptr_type;

      //! \brief MCNP surface
      class surface
      {
      public:

        //! Invalid surface identifier
        static const int INVALID_ID =     -1;
        static const int MIN_ID     =      1;
        static const int MAX_ID     =  99999;

        //! \brief Special surface traits
        enum trait_type {
          TRAIT_NONE           = 0,
          TRAIT_REFLECTIVE     = 1,
          TRAIT_WHITE_BOUNDARY = 2
        };

        //! Default constructor
        surface();

        //! Destructor
        ~surface();

        //! Set the name
        void set_name(const std::string &);

        //! Return the name
        const std::string & get_name() const;

        //! Set the identifier
        void set_id(int);

        //! Return the identifier
        int get_id() const;

        //! Set the trait
        void set_trait(trait_type t_) ;

        //! Return the trait
        trait_type get_trait() const;

        //! Check white boundary
        bool is_white_boundary() const;

        //! Set white boundary
        void set_white_boundary(bool);

        //! Check reflective
        bool is_reflective() const;

        //! Set reflective
        void set_reflective(bool);

        //! Return the type of the surface
        surface_type get_type() const;

        //! Set the type of the surface
        void set_type(surface_type);

        //! Add a parameter
        template <class ParamType>
        void add_parameter(const std::string & name_,
                           const ParamType & value_)
        {
          parameter_type param;
          param.name = name_;
          param.value = value_;
          _parameters_.push_back(param);
          return;
        }

        //! Reset
        void reset();

        //! Check if surface is valid
        bool is_valid() const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

      private:

        std::string  _name_;  //!< Name of the surface
        int          _id_;    //!< Identifier of the surface
        surface_type _type_;  //!< Type of the surface
        parameter_array_type _parameters_; //!< Array of parameters
        trait_type   _trait_; //!< Special surface trait

      };

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_GEOMETRY_SURFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
