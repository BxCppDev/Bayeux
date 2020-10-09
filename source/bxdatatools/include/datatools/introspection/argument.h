//! \file  datatools/introspection/argument.h
//! \brief Description of a method argument
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

#ifndef DATATOOLS_INTROSPECTION_ARGUMENT_H
#define DATATOOLS_INTROSPECTION_ARGUMENT_H

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/optional.hpp>
#include <boost/any.hpp>
#include <boost/program_options.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <datatools/introspection/access_type.h>
#include <datatools/introspection/data_description.h>

namespace datatools {

  namespace introspection {

    class method;

    //! \brief Description of a method argument
    class argument
      : public datatools::i_tree_dumpable
    {
    public:

      //! Default constructor
      argument();

      //! Destructor
      ~argument() override;

      //! Check if the method of which the argument belong to the list of arguments is set
      bool has_method() const;

      //! Set the method of which the argument belong to the list of arguments
      void set_method(const method &);

      //! Reset the method of which the argument belong to the list of arguments
      void reset_method();

      //! Return the method of which the argument belong to the list of arguments
      const method & get_method() const;

      //! Check if the argument has an explicit rank in the list of arguments it belongs to
      bool has_rank() const;

      //! Return the argument's explicit rank in the list of arguments it belongs to
      std::size_t get_rank() const;

      //! Set the argument's explicit rank in the list of arguments it belongs to
      void set_rank(std::size_t);

      //! Check if the argument has an explicit name in the list of arguments it belongs to
      bool has_name() const;

      //! Return the argument's explicit name in the list of arguments it belongs to
      const std::string & get_name() const;

      //! Set the argument's explicit name in the list of arguments it belongs to
      void set_name(const std::string &);

      //! Check if the argument has an access mode
      bool has_access() const;

      //! Return the argument's access mode
      access_type get_access() const;

      //! Set the argument's access mode
      void set_access(access_type);

      //! Check if the argument has a description
      bool has_description() const;

      //! Return a non mutable reference of the argument's description
      const std::string & get_description() const;

      //! Set the argument's description
      void set_description(const std::string &);

      //! Check if the argument is an input access argument
      bool is_input() const;

      //! Check if the argument is an output access argument
      bool is_output() const;

      //! Check if the argument has a data description
      bool has_data_description() const;

      //! Return a mutable reference of the argument's data description
      data_description & grab_data_description();

      //! Return a non mutable reference of the argument's data description
      const data_description & get_data_description() const;

      //! Set the argument's data description
      void set_data_description(const data_description &);

      //! Check if the argument has a default value
      bool has_default_value() const;

      //! Check default value type
      template <class T>
      bool is_default_value_a() const
      {
        return _default_value_.type() == typeid(T);
      }

      //! Return the argument's default value
      template <class T>
      const T & get_default_value_as() const
      {
        DT_THROW_IF(!has_default_value(), std::logic_error,
                  "No default value is set!");
        DT_THROW_IF(typeid(T) != get_data_description().native_type(),
                    std::logic_error,
                    "Unmatching type info!");
        return boost::any_cast<T>(_default_value_);
      }

      //! Set the argument's default_value
      template <class T>
      void set_default_value_as(const T & value_)
      {
        DT_THROW_IF(typeid(T) != get_data_description().native_type(),
                    std::logic_error,
                    "Unmatching type info!");
        _default_value_ = value_;
        return;
      }

      //! Return the argument's default_value
      const boost::any & get_default_value() const;

      //! Set the argument's default_value
      void set_default_value(const boost::any & any_value_);

      //! Check if the argument has a default value string representation
      bool has_default_value_str() const;

      //! Return the default value string representation
      const std::string & get_default_value_str() const;

      //! Set the default value string representation
      void set_default_value_str(const std::string & v_);

      //! Check validity
      bool is_valid() const;

      //! Initialization
      void initialize();

      //! Initialization
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! \brief Flags for export to a container of properties
      enum arg_xc_flags {
        ARG_XC_CLEAR              = bit_mask::bit00,
        ARG_XC_ACCESS             = bit_mask::bit01,
        ARG_XC_DATA_DESCRIPTION   = bit_mask::bit02,
        ARG_XC_RANK               = bit_mask::bit03,
        ARG_XC_NAME               = bit_mask::bit04,
        ARG_XC_DESCRIPTION        = bit_mask::bit05,
        ARG_XC_DEFAULT_VALUE_STR  = bit_mask::bit06,
        ARG_XC_DEFAULT = (ARG_XC_CLEAR
                         | ARG_XC_ACCESS
                         | ARG_XC_DATA_DESCRIPTION
                         | ARG_XC_RANK
                         | ARG_XC_NAME
                         | ARG_XC_DESCRIPTION
                         | ARG_XC_DEFAULT_VALUE_STR),
        ARG_XC_LAST    = ARG_XC_DEFAULT_VALUE_STR
      };

      //! Export to a container of properties
      void export_to_config(datatools::properties & config_,
                            uint32_t flags_ = ARG_XC_DEFAULT,
                            const std::string & prefix_ = "") const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    private:

      access_type      _access_;           //!< Access type of the argument (input/output)
      data_description _data_description_; //!< Description of the data
      const method   * _method_;           //!< Handle to the method (optional)
      boost::optional<std::size_t> _rank_; //!< Rank of the argument in the argument list of a method (optional)
      boost::optional<std::string> _name_; //!< Name of the argument in the unordered argument list of a method (optional)
      boost::optional<std::string> _description_; //!< Description of the argument (optional)
      boost::any                   _default_value_; //!< Default value
      boost::optional<std::string> _default_value_str_; //!< Default value (optional)

    };

  } // namespace introspection

} // namespace datatools

#endif // DATATOOLS_INTROSPECTION_ARGUMENT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
