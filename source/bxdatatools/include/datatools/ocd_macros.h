/// \file datatools/ocd_macros.h
/*
 * Description :
 *
 *  Some useful macros related to OCD features.
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
#ifndef DATATOOLS_OCD_MACROS_H
#define DATATOOLS_OCD_MACROS_H

// This project:
#include <datatools/detail/ocd_utils.h>

/// Declare the OCD support for a given class
/**
 *  \arg ClassName the class full name (including namespaces)
 */
#define DOCD_CLASS_DECLARATION(ClassName)                               \
  namespace datatools {                                                 \
    namespace detail {                                                  \
      namespace ocd {                                                   \
        template<>                                                      \
        void implement_load< ClassName >(::datatools::object_configuration_description & ocd_); \
        template<> struct loader< ClassName > {                         \
          static const bool value = true;                               \
          struct load {                                                 \
            void operator()(::datatools::object_configuration_description & ocd_) const { \
              implement_load< ClassName >(ocd_);                        \
            }                                                           \
          };                                                            \
        };                                                              \
      }}}                                                               \
  /**/

/// Open the OCD implementation block
/**
 *  \arg ClassType the class type
 *  \arg OcdId the OCD data structure to be filled
 */
#define DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(ClassType,OcdId)                \
  namespace datatools {                                                 \
  namespace detail {                                                    \
  namespace ocd {                                                       \
  template<> void implement_load< ClassType >(::datatools::object_configuration_description & OcdId) { \
  /**/

/// Close the OCD implementation block
#define DOCD_CLASS_IMPLEMENT_LOAD_END()         \
  }}}}                                          \
  /**/

/// Instantiate a specialization of the OCD system factory register manager for a given class
/**
 *  \arg ClassType the class type
 *  \arg ClassId the class registration Id (unique string)
 */
#define DOCD_CLASS_SYSTEM_REGISTRATION(ClassType,ClassId)               \
  namespace datatools {                                                 \
    namespace detail {                                                  \
      namespace ocd {                                                   \
        template <>                                                     \
        struct _ocd_sfr< ClassType > {                                  \
          static boost::scoped_ptr<base_system_factory_registrar> reg_; \
        };                                                              \
        boost::scoped_ptr<base_system_factory_registrar>                \
        _ocd_sfr< ClassType >::reg_(new system_factory_registrar< ClassType >(ClassId)); \
      }}}                                                               \
  /**/

#define DOCD_GET_SYSTEM_REGISTRATION()                                  \
  ::datatools::detail::ocd::ocd_registration::get_system_registration() \
  /**/

#endif // DATATOOLS_OCD_MACROS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
