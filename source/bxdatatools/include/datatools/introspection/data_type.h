//! \file  datatools/introspection/data_type.h
//! \brief Data type information
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

#ifndef DATATOOLS_INTROSPECTION_DATA_TYPE_H
#define DATATOOLS_INTROSPECTION_DATA_TYPE_H

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/any.hpp>
#include <boost/cstdint.hpp>

#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  namespace introspection {

    //! \brief Supported data types
    enum data_type {
      DATA_TYPE_INVALID =  -1, //!< Invalid data type
      DATA_TYPE_VOID    =   0, //!< No type
      DATA_TYPE_BOOLEAN =   1, //!< bool
      DATA_TYPE_INT8    =   2, //!< int8_t
      DATA_TYPE_UINT8   =   3, //!< uint8_t
      DATA_TYPE_INT16   =   4, //!< int16_t
      DATA_TYPE_UINT16  =   5, //!< uint16_t
      DATA_TYPE_INT32   =   6, //!< int32_t
      DATA_TYPE_UINT32  =   7, //!< uint32_t
      DATA_TYPE_INT64   =   8, //!< int64_t
      DATA_TYPE_UINT64  =   9, //!< uint64_t
      DATA_TYPE_FLOAT   =  10, //!< float value
      DATA_TYPE_DOUBLE  =  11, //!< double value
      DATA_TYPE_STRING  =  12, //!< character string
      DATA_TYPE_PATH    =  13, //!< filesystem path as a character string
      DATA_TYPE_ENUM    = 100, //!< enumeration type
      DATA_TYPE_USER    = 200  //!< user data type
    };

    //! Return the type info associated to a given data type with specific layout
    //!
    //! Supported layout labels are:
    //!  - "scalar" for int value;
    //!  - "vector" for std::vector<int> values;
    //!  - "list"   for std::list<int> values;
    //!  - "map"    for std::map<std::string, int> values;
    const std::type_info & get_type_info(data_type dt_, const std::string & layout_label_ = "");

    //! Make value as a boost::any object
    void make_value(boost::any &, data_type dt_, const std::string & layout_label_ = "");

    //! Convert a data type to a string
    const std::string & to_string(data_type);

    //! Convert a string to a data type
    bool from_string(const std::string & label_, data_type & layout_);

    //! Convert a string to a data type
    data_type from_label_to_data_type(const std::string & label_);

    //! Check the validity of a data type
    bool is_valid(data_type);

    //! Check if a data type is a void
    bool is_void(data_type);

    //! Check if a data type is a boolean
    bool is_boolean(data_type);

    //! Check if a data type is an integer
    bool is_integer(data_type);

    //! Check if a data type is a real
    bool is_real(data_type);

    //! Check if a data type is a string
    bool is_string(data_type);

    //! Check if a data type is a path (string)
    bool is_path(data_type);

    //! Check if a data type is an enumeration
    bool is_enum(data_type);

    //! Check if a data type is a user type
    bool is_user(data_type);

  } // end of namespace introspection

} // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::introspection::data_type)
#endif // Q_MOC_RUN

#endif // DATATOOLS_INTROSPECTION_DATA_TYPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
