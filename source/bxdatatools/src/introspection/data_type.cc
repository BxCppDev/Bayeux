// data_type.cc
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

// Ourselves:
#include <datatools/introspection/data_type.h>

// Standard library:
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

// Third party:
// - Camp:
#include <camp/enum.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace datatools {

  namespace introspection {

    const std::type_info & get_type_info(data_type dt_, const std::string & layout_label_)
    {
      if (layout_label_.empty() || layout_label_ == "scalar") {
        switch (dt_) {
        case DATA_TYPE_VOID    : return typeid(void);
        case DATA_TYPE_BOOLEAN : return typeid(bool);
        case DATA_TYPE_INT8    : return typeid(int8_t);
        case DATA_TYPE_UINT8   : return typeid(uint8_t);
        case DATA_TYPE_INT16   : return typeid(int16_t);
        case DATA_TYPE_UINT16  : return typeid(uint16_t);
        case DATA_TYPE_INT32   : return typeid(int32_t);
        case DATA_TYPE_UINT32  : return typeid(uint32_t);
        case DATA_TYPE_INT64   : return typeid(int64_t);
        case DATA_TYPE_UINT64  : return typeid(uint64_t);
        case DATA_TYPE_FLOAT   : return typeid(float);
        case DATA_TYPE_DOUBLE  : return typeid(double);
        case DATA_TYPE_STRING  : return typeid(std::string);
        case DATA_TYPE_PATH    : return typeid(std::string);
        case DATA_TYPE_ENUM    : return typeid(std::string);
        default :
          DT_THROW(std::logic_error, "Unsupported scalar data type '" << to_string(dt_) << "'!");
        }
      } else if (layout_label_ == "vector") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : return typeid(std::vector<bool>);
        case DATA_TYPE_INT8    : return typeid(std::vector<int8_t>);
        case DATA_TYPE_UINT8   : return typeid(std::vector<uint8_t>);
        case DATA_TYPE_INT16   : return typeid(std::vector<int16_t>);
        case DATA_TYPE_UINT16  : return typeid(std::vector<uint16_t>);
        case DATA_TYPE_INT32   : return typeid(std::vector<int32_t>);
        case DATA_TYPE_UINT32  : return typeid(std::vector<uint32_t>);
        case DATA_TYPE_INT64   : return typeid(std::vector<int64_t>);
        case DATA_TYPE_UINT64  : return typeid(std::vector<uint64_t>);
        case DATA_TYPE_FLOAT   : return typeid(std::vector<float>);
        case DATA_TYPE_DOUBLE  : return typeid(std::vector<double>);
        case DATA_TYPE_STRING  : return typeid(std::vector<std::string>);
        case DATA_TYPE_PATH    : return typeid(std::vector<std::string>);
        case DATA_TYPE_ENUM    : return typeid(std::vector<std::string>);
        default :
          DT_THROW(std::logic_error, "Unsupported vector data type '" << to_string(dt_) << "'!");
        }
      } else if (layout_label_ == "list") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : return typeid(std::list<bool>);
        case DATA_TYPE_INT8    : return typeid(std::list<int8_t>);
        case DATA_TYPE_UINT8   : return typeid(std::list<uint8_t>);
        case DATA_TYPE_INT16   : return typeid(std::list<int16_t>);
        case DATA_TYPE_UINT16  : return typeid(std::list<uint16_t>);
        case DATA_TYPE_INT32   : return typeid(std::list<int32_t>);
        case DATA_TYPE_UINT32  : return typeid(std::list<uint32_t>);
        case DATA_TYPE_INT64   : return typeid(std::list<int64_t>);
        case DATA_TYPE_UINT64  : return typeid(std::list<uint64_t>);
        case DATA_TYPE_FLOAT   : return typeid(std::list<float>);
        case DATA_TYPE_DOUBLE   : return typeid(std::list<double>);
        case DATA_TYPE_STRING :
        case DATA_TYPE_PATH : return typeid(std::list<std::string>);
        case DATA_TYPE_ENUM : return typeid(std::list<std::string>);
        default :
          DT_THROW(std::logic_error, "Unsupported list data type '" << to_string(dt_) << "'!");
        }
      } else if (layout_label_ == "map") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : return typeid(std::map<std::string,bool>);
        case DATA_TYPE_INT8    : return typeid(std::map<std::string,int8_t>);
        case DATA_TYPE_UINT8   : return typeid(std::map<std::string,uint8_t>);
        case DATA_TYPE_INT16   : return typeid(std::map<std::string,int16_t>);
        case DATA_TYPE_UINT16  : return typeid(std::map<std::string,uint16_t>);
        case DATA_TYPE_INT32   : return typeid(std::map<std::string,int32_t>);
        case DATA_TYPE_UINT32  : return typeid(std::map<std::string,uint32_t>);
        case DATA_TYPE_INT64   : return typeid(std::map<std::string,int64_t>);
        case DATA_TYPE_UINT64  : return typeid(std::map<std::string,uint64_t>);
        case DATA_TYPE_FLOAT   : return typeid(std::map<std::string,float>);
        case DATA_TYPE_DOUBLE  : return typeid(std::map<std::string,double>);
        case DATA_TYPE_STRING :
        case DATA_TYPE_PATH   : return typeid(std::map<std::string,std::string>);
        case DATA_TYPE_ENUM   : return typeid(std::map<std::string,std::string>);
        default :
          DT_THROW(std::logic_error, "Unsupported map data type '" << to_string(dt_) << "'!");
        }
      } else {
        DT_THROW(std::logic_error, "Unsupported layout label '" << layout_label_ << "'!");
      }
    }

    void make_value(boost::any & value_, data_type dt_, const std::string & layout_label_)
    {
      if (layout_label_.empty() || layout_label_ == "scalar") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : value_ = false; return;
        case DATA_TYPE_INT8    : value_ = int8_t(0); return;
        case DATA_TYPE_UINT8   : value_ = uint8_t(0); return;
        case DATA_TYPE_INT16   : value_ = int16_t(0); return;
        case DATA_TYPE_UINT16  : value_ = uint16_t(0); return;
        case DATA_TYPE_INT32   : value_ = int32_t(0); return;
        case DATA_TYPE_UINT32  : value_ = uint32_t(0);  return;
        case DATA_TYPE_INT64   : value_ = int64_t(0);  return;
        case DATA_TYPE_UINT64  : value_ = uint64_t(0); return;
        case DATA_TYPE_FLOAT   : value_ = std::numeric_limits<float>::quiet_NaN(); return;
        case DATA_TYPE_DOUBLE  : value_ = std::numeric_limits<double>::quiet_NaN(); return;
        case DATA_TYPE_STRING  : value_ = std::string(); return;
        case DATA_TYPE_PATH    : value_ = std::string(); return;
        case DATA_TYPE_ENUM    : value_ = std::string(); return;
        case DATA_TYPE_VOID :
        default : return;
        }
      } else if (layout_label_ == "vector") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : value_ = std::vector<bool>(); return;
        case DATA_TYPE_INT8    : value_ = std::vector<int8_t>(); return;
        case DATA_TYPE_UINT8   : value_ = std::vector<uint8_t>(); return;
        case DATA_TYPE_INT16   : value_ = std::vector<int16_t>(); return;
        case DATA_TYPE_UINT16  : value_ = std::vector<uint16_t>(); return;
        case DATA_TYPE_INT32   : value_ = std::vector<int32_t>(); return;
        case DATA_TYPE_UINT32  : value_ = std::vector<uint32_t>(); return;
        case DATA_TYPE_INT64   : value_ = std::vector<int64_t>(); return;
        case DATA_TYPE_UINT64  : value_ = std::vector<uint64_t>(); return;
        case DATA_TYPE_FLOAT   : value_ = std::vector<float>(); return;
        case DATA_TYPE_DOUBLE  : value_ = std::vector<double>(); return;
        case DATA_TYPE_STRING  : value_ = std::vector<std::string>(); return;
        case DATA_TYPE_PATH    : value_ = std::vector<std::string>(); return;
        case DATA_TYPE_ENUM    : value_ = std::vector<std::string>(); return;
        default : return;
        }
      } else if (layout_label_ == "list") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : value_ = std::list<bool>(); return;
        case DATA_TYPE_INT8    : value_ = std::list<int8_t>(); return;
        case DATA_TYPE_UINT8   : value_ = std::list<uint8_t>(); return;
        case DATA_TYPE_INT16   : value_ = std::list<int16_t>(); return;
        case DATA_TYPE_UINT16  : value_ = std::list<uint16_t>(); return;
        case DATA_TYPE_INT32   : value_ = std::list<int32_t>(); return;
        case DATA_TYPE_UINT32  : value_ = std::list<uint32_t>(); return;
        case DATA_TYPE_INT64   : value_ = std::list<int64_t>(); return;
        case DATA_TYPE_UINT64  : value_ = std::list<uint64_t>(); return;
        case DATA_TYPE_FLOAT   : value_ = std::list<float>(); return;
        case DATA_TYPE_DOUBLE  : value_ = std::list<double>(); return;
        case DATA_TYPE_STRING : value_ = std::list<std::string>(); return;
        case DATA_TYPE_PATH : value_ = std::list<std::string>(); return;
        case DATA_TYPE_ENUM : value_ = std::list<std::string>(); return;
        default : return;
        }
       } else if (layout_label_ == "map") {
        switch (dt_) {
        case DATA_TYPE_BOOLEAN : value_ = std::map<std::string,bool>(); return;
        case DATA_TYPE_INT8    : value_ = std::map<std::string,int8_t>(); return;
        case DATA_TYPE_UINT8   : value_ = std::map<std::string,uint8_t>(); return;
        case DATA_TYPE_INT16   : value_ = std::map<std::string,int16_t>(); return;
        case DATA_TYPE_UINT16  : value_ = std::map<std::string,uint16_t>(); return;
        case DATA_TYPE_INT32   : value_ = std::map<std::string,int32_t>(); return;
        case DATA_TYPE_UINT32  : value_ = std::map<std::string,uint32_t>(); return;
        case DATA_TYPE_INT64   : value_ = std::map<std::string,int64_t>(); return;
        case DATA_TYPE_UINT64  : value_ = std::map<std::string,uint64_t>(); return;
        case DATA_TYPE_FLOAT   :  value_ = std::map<std::string,float>(); return;
        case DATA_TYPE_DOUBLE  :  value_ = std::map<std::string,double>(); return;
        case DATA_TYPE_STRING : value_ = std::map<std::string,std::string>(); return;
        case DATA_TYPE_PATH : value_ = std::map<std::string,std::string>(); return;
        case DATA_TYPE_ENUM : value_ = std::map<std::string,std::string>(); return;
        default : return;
        }
      }
      DT_THROW(std::logic_error, "Unsupported '" << layout_label_ << "' data type '" << to_string(dt_) << "'!");

      return;
    }

    const std::string & to_string(data_type t_)
    {
      const camp::Enum & metaDataTypeEnum = camp::enumByType<data_type>();
      return metaDataTypeEnum.name(t_);
    }

    data_type from_label_to_data_type(const std::string & label_)
    {
      data_type ret;
      DT_THROW_IF(!from_string(label_, ret),
                  std::logic_error,
                  "Invalid data type label '" << label_ << "'!");
      return ret;
    }

    bool from_string(const std::string & label_, data_type & type_)
    {
      type_ = DATA_TYPE_INVALID;
      const camp::Enum & metaDataTypeEnum = camp::enumByType<data_type>();
      if (metaDataTypeEnum.hasName(label_)) {
        type_= static_cast<data_type>(metaDataTypeEnum.value(label_));
        return true;
      }
      return false;
    }

    bool is_valid(data_type t_)
    {
      return t_ != DATA_TYPE_INVALID;
    }

    bool is_void(data_type t_)
    {
      return t_ == DATA_TYPE_VOID;
    }

    bool is_boolean(data_type t_)
    {
      return t_ == DATA_TYPE_BOOLEAN;
    }

    bool is_integer(data_type t_)
    {
      return t_ == DATA_TYPE_INT8
        || t_ == DATA_TYPE_UINT8
        || t_ == DATA_TYPE_INT16
        || t_ == DATA_TYPE_UINT16
        || t_ == DATA_TYPE_INT32
        || t_ == DATA_TYPE_UINT32
        || t_ == DATA_TYPE_INT64
        || t_ == DATA_TYPE_UINT64
        ;
    }

    bool is_real(data_type t_)
    {
      return t_ == DATA_TYPE_FLOAT
        || t_ == DATA_TYPE_DOUBLE;
    }

    bool is_string(data_type t_)
    {
      return t_ == DATA_TYPE_STRING || t_ == DATA_TYPE_PATH;
    }

    bool is_path(data_type t_)
    {
      return t_ == DATA_TYPE_PATH;
    }

    bool is_enum(data_type t_)
    {
      return t_ == DATA_TYPE_ENUM;
    }

    bool is_user(data_type t_)
    {
      return t_ == DATA_TYPE_USER;
    }

  } // namespace introspection

} // namespace datatools
