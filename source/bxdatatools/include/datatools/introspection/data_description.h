//! \file  datatools/introspection/data_description.h
//! \brief Data description
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_INTROSPECTION_DATA_DESCRIPTION_H
#define DATATOOLS_INTROSPECTION_DATA_DESCRIPTION_H

// Third party:
// - Boost:
#include <boost/optional.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <datatools/introspection/data_type.h>
#include <datatools/introspection/data_layout.h>

namespace datatools {

  namespace introspection {

    //! \brief Data description
    class data_description : public datatools::i_tree_dumpable
    {
    public:

      //! Default constructor
      data_description();

      //! Destructor
      ~data_description();

      //! Check if data type is defined
      bool has_type() const;

      //! Return the data type
      data_type get_type() const;

      //! Set the data type
      void set_type(data_type);

      //! Return the native type
      const std::type_info & native_type() const;

      //! Check if data layout is defined
      bool has_layout() const;

      //! Return the data layout
      data_layout get_layout() const;

      //! Return the labale associated to the data layout
      std::string get_layout_label() const;

      //! Set the data layout
      void set_layout(data_layout);

      //! Check if vector fixed size is defined
      bool has_vector_fixed_size() const;

      //! Return the vector fixed size
      std::size_t get_vector_fixed_size() const;

      //! Set the vector fixed size
      void set_vector_fixed_size(std::size_t);

      //! Check if the (real) data is stored using an implicit unit (ex: "volt", "ns", "mA"...)
      bool has_implicit_unit() const;

      //! Return the real data implicit unit symbol
      const std::string & get_implicit_unit_symbol() const;

      //! Set the real data implicit unit symbol
      void set_implicit_unit_symbol(const std::string &);

      //! Check if the (real) data is stored using an explicit unit of known dimension (ex: "length"/"time"/"mass"...)
      bool has_explicit_unit_dimension() const;

      //! Return the real data explicit unit dimension label ("length"/"time"/"mass"...)
      const std::string & get_explicit_unit_dimension_label() const;

      //! Set the real data explicit unit dimension label ("length"/"time"/"mass"...)
      void set_explicit_unit_dimension_label(const std::string &);

      //! Check if the (real) data has a preferred unit (ex: "volt", "ns", "mA"...)
      bool has_explicit_preferred_unit() const;

      //! Return the (real) data preferred unit (ex: "volt", "ns", "mA"...)
      const std::string & get_explicit_preferred_unit_symbol() const;

      //! Set the (real) data preferred unit (ex: "volt", "ns", "mA"...)
      void set_explicit_preferred_unit_symbol(const std::string &);

      //! Check if the data has a type identifier (for enumeration and class)
      bool has_type_id() const;

      //! Return the type identifier (for enumeration and class)
      const std::string & get_type_id() const;

      //! Set the type identifier (for enumeration and class)
      void set_type_id(const std::string &);

      //! Check validity
      bool is_valid() const;

      //! Initialization
      void initialize();

      //! Initialization
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      //! Build a scalar data
      void make_scalar(data_type, const std::string & info_ = "");

      //! Build a vector data with fixed size
      void make_fixed_size_vector(data_type, std::size_t size_, const std::string & info_ = "");

      //! Build a vector data with free size
      void make_free_size_vector(data_type, const std::string & info_ = "");

    private:

      //! Set default values to attributes
      void _set_defaults();

    private:

      data_type   _type_;   //!< Type of the data
      data_layout _layout_; //!< Layout of the data
      boost::optional<std::size_t> _vector_fixed_size_; //!< Fixed size for a vector
      boost::optional<std::string> _implicit_unit_symbol_; //!< Implicit unit symbol for real data
      boost::optional<std::string> _explicit_unit_dimension_label_; //!< Explicit unit dimension label for real data
      boost::optional<std::string> _explicit_preferred_unit_symbol_; //!< Explicit preferred unit symbol for real data
      boost::optional<std::string> _type_id_; //!< Explicit user/enum type id (meta class name)
      // type restriction

    };

  } // namespace introspection

} // namespace datatools

#endif // DATATOOLS_INTROSPECTION_DATA_DESCRIPTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
