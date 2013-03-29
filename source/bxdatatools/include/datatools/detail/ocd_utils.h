/* -*- mode: c++; -*- */
/* datatools::detail::ocd_utils.h */
/* 
 * Description :
 *
 *  Some useful classes related to OCD features.
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */ 
#ifndef DATATOOLS_DETAIL_OCD_UTILS_H_
#define DATATOOLS_DETAIL_OCD_UTILS_H_

// Standard Library
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>

#include <boost/scoped_ptr.hpp>

#include <datatools/object_configuration_description.h>

namespace datatools {                                                   
  namespace detail {                                                   
    namespace ocd {                                                   
      template <typename T>
      struct loader {};
      template <typename T>
      void implement_load(::datatools::object_configuration_description & ocd_);
    } // namespace ocd
  } // namespace detail
} // namespace datatools

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/has_xxx.hpp>

/// Template support for configurable type 
BOOST_MPL_HAS_XXX_TRAIT_DEF(load);

namespace datatools {                                                   

  template <typename ConfigurableType>
  bool load_ocd(::datatools::object_configuration_description & ocd_,
                typename boost::disable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* dummy = 0)
  {
    return false;
  }


  template <typename ConfigurableType>
  bool load_ocd(::datatools::object_configuration_description & ocd_,
                typename boost::enable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* dummy = 0)
  {
    typename ::datatools::detail::ocd::loader<ConfigurableType>::load l;
    l(ocd_);
    return true;
  }

} // namespace datatools

#include <boost/shared_ptr.hpp>

namespace datatools {                                                   
  namespace detail {                                                   
    namespace ocd {                                                   

      /// \brief OCD registration container
      class ocd_registration
      {
      public:

        typedef boost::shared_ptr<object_configuration_description> ocd_handle_type;

        struct entry_type {
          ocd_handle_type handle;
        };

        typedef std::map<std::string, entry_type> ocd_dict_type; 

        ocd_registration(bool preload_system_registration_ = false);

        ~ocd_registration();

        bool has_id(const std::string & class_id_) const;

        const object_configuration_description & 
        get(const std::string & class_id_) const;

        template<class T>
        void registration(const std::string & class_name_, const ocd_handle_type & handle_);

        void smart_dump(std::ostream & out_ = std::clog,
                        const std::string & title_ = "",
                        const std::string & indent_ = "") const;
                 
        // Access to a static singleton (mutable version) :
        static ocd_registration & grab_system_registration(); 

        // Access to a static singleton (non-mutable version) :
        static const ocd_registration & get_system_registration(); 

      protected:

        void _preload_system_registration();

      private:

        ocd_dict_type _dict_; 

      };


      template<class T>
      void ocd_registration::registration(const std::string & class_id_,
                                          const ocd_handle_type & handle_) {
        if (has_id(class_id_)) {
          std::ostringstream message;
          message << "datatools::detail::ocd::ocd_registration::registration: "
                  << "Class ID '" << class_id_ << "' is already registered !";
          throw std::logic_error(message.str());
        }
        {
          entry_type dummy;
          _dict_[class_id_] = dummy;
        }
        entry_type & e = _dict_[class_id_];
        e.handle = handle_;
      }

      struct base_system_factory_registrar
      {
      };

      template <class T>
      class system_factory_registrar : public base_system_factory_registrar
      {
      public:
        system_factory_registrar(const std::string & class_id_) {
          ocd_registration & ocd_reg = ocd_registration::grab_system_registration();
          ocd_registration::ocd_handle_type h;
          h.reset(new object_configuration_description);
          object_configuration_description & the_ocd = *h.get();
          bool support = load_ocd<T>(the_ocd);
          if (support) {
            ocd_reg.registration<T>(class_id_, h); 
          }
        }
      };
 
      template <typename ClassType>                                        
      struct _ocd_sfr {                                                 
      };                                                              
 
    } // namespace ocd
  } // namespace detail
} // namespace datatools


#endif // DATATOOLS_DETAIL_OCD_UTILS_H_

