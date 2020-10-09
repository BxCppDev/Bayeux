//! \file  datatools/introspection/unit_support.h
//! \brief Unit support utilities
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_INTROSPECTION_UNIT_SUPPORT_H
#define DATATOOLS_INTROSPECTION_UNIT_SUPPORT_H

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  namespace introspection {

    //! \brief Unit support type for real parameters
    enum unit_support_type {
      UNIT_SUPPORT_INVALID            = -1, //!< Invalid unit support
      UNIT_SUPPORT_NONE               =  0, //!< No unit management (plain float/double)
      UNIT_SUPPORT_IMPLICIT_UNIT      =  1, //!< Implicit unit associated to the parameter
      UNIT_SUPPORT_EXPLICIT_DIMENSION =  2  //!< Explicit unit dimension associated to the parameter
    };

    //! Convert an unit support type to a string
    const std::string & to_string(unit_support_type);

    //! Convert a string to an unit support type
    bool from_string(const std::string & label_, unit_support_type & us_);

    //! Convert a string to an unit support type
    unit_support_type from_label_to_unit_support(const std::string & label_);

    //! Check the validity of an unit support type
    bool is_valid(unit_support_type);

    //! Check if an unit support type is none
    bool is_no_unit_support(unit_support_type);

    //! Check if an unit support type is implicit unit
    bool is_implicit_unit(unit_support_type);

    //! Check if an unit support type is explicit dimension
    bool is_explicit_unit_dimension(unit_support_type);

    //! \brief Unit information associated to a parameter
    class unit_info : public datatools::i_tree_dumpable
    {
    public:

      //! Default constructor
      unit_info();

      //! Constructor
      unit_info(unit_support_type, const std::string & unit_token_, const std::string & unit_token2_ = "");

      //! Destructor
      ~unit_info() override;

      //! Check validity
      bool is_valid() const;

      //! Check the implicit unit trait
      bool has_implicit_unit() const;

      //! Check the explicit unit dimension trait
      bool has_explicit_unit_dimension() const;

      //! Set the unit support
      void set_unit_support(unit_support_type);

      //! Return the unit support
      unit_support_type get_unit_support() const;

      //! Set the implicit unit symbol
      void set_implicit_unit_symbol(const std::string &);

      //! Return the implicit unit symbol
      const std::string & get_implicit_unit_symbol() const;

      //! Set the explicit unit dimension label
      void set_explicit_unit_dimension_label(const std::string &);

      //! Return the explicit unit dimension
      const std::string & get_explicit_unit_dimension_label() const;

      //! Check if preferred unit symbol is set
      bool has_preferred_unit_symbol() const;

      //! Set the preferred unit symbol
      void set_preferred_unit_symbol(const std::string &);

      //! Return the preferred unit symbol
      const std::string & get_preferred_unit_symbol() const;

      //! Make the unit info with no unit support
      void make_none();

      //! Make the unit info with implicit unit
      void make_implicit_unit(const std::string & unit_symbol_,
                              const std::string & preferred_unit_symbol_ = "");


      //! Make the unit info with explicit unit dimension
      void make_explicit_unit_dimension(const std::string & unit_dimension_label_,
                                        const std::string & preferred_unit_symbol_ = "");

      //! Initialize from a set of properties
      //!
      //! Examples:
      //! a) No unit support:
      //! \code
      //! support : string = "none"
      //! \endcode
      //!
      //! b) Implicit unit support:
      //! \code
      //! support       : string = "implicit_unit"
      //! implicit_unit : string = "second"
      //! \endcode
      //!
      //! c) Explicit unit dimension support:
      //! \code
      //! support                 : string = "explicit_unit_dimension"
      //! explicit_unit_dimension : string = "time"
      //! preferred_unit          : string = "ms"
      //! \endcode
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! \brief Flags for export to a container of properties
      enum unit_info_xc_flags {
        UI_XC_CLEAR              = bit_mask::bit00,
        UI_XC_UNIT_SUPPORT       = bit_mask::bit01,
        UI_XC_PREFERRED_UNIT     = bit_mask::bit02,
        UI_XC_DEFAULT = (UI_XC_CLEAR
                         | UI_XC_UNIT_SUPPORT
                         | UI_XC_PREFERRED_UNIT),
        DD_XC_LAST    = UI_XC_PREFERRED_UNIT
      };

      //! Export to a container of properties
      void export_to_config(datatools::properties & config_,
                            uint32_t flags_ = UI_XC_DEFAULT,
                            const std::string & prefix_ = "") const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      //! Make the class "equality comparable" (needed by Boost/optional for some reason...)
      //! Do not understand why (see also the "data_description::has_unit_info()" method)
      //! In order to use the "boost::none" semantic, I had to add
      //! the "unit_info::operator==(...)" operator despite what is written at:
      //!  http://www.boost.org/doc/libs/1_59_0/libs/optional/doc/html/boost_optional/tutorial/relational_operators.html
      //! \remark
      //! In case where optional<T> is compared to none, it is not required that T be EqualityComparable.
      //!
      //! That's fine but is not true with Boost 1.55.
      //!
      //! The use of "boost::none" semantic was introduced to prepare the support for recent C++
      //! compilers (with C++11 support) that seems to not support anymore the following semantics:
      //! \code
      //! boost::optional<unit_info> opt;
      //! ...
      //! if (opt) { /* do something */ }
      //! \endcode
      //! but require:
      //! \code
      //! boost::optional<unit_info> opt;
      //! ...
      //! if (opt != boost::none) { /* do something */ }
      //! \endcode
      //! However, for builtin types (int, float...), it works.
      //! This must be clarified in the future.
      bool operator==(const unit_info & ui_) const;

    private:

      unit_support_type _us_;                      //!< Type of unit support
      std::string _implicit_unit_symbol_;          //!< Implicit unit symbol for real data
      std::string _explicit_unit_dimension_label_; //!< Explicit unit dimension label for real data
      std::string _preferred_unit_symbol_;         //!< Preferred unit symbol for real data

    };

  } // end of namespace introspection

} // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::introspection::unit_support_type)
#endif // Q_MOC_RUN

#endif // DATATOOLS_INTROSPECTION_UNIT_SUPPORT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
