/// \file datatools/detail/ocd_utils.h
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

#ifndef DATATOOLS_DETAIL_OCD_UTILS_H
#define DATATOOLS_DETAIL_OCD_UTILS_H

// Standard Library:
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>

namespace datatools {

  /// Nested private namespace of the Bayeux/datatools module library (detail)
  namespace detail {

    /// Nested private namespace of the Bayeux/datatools module library (OCD)
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
BOOST_MPL_HAS_XXX_TRAIT_DEF(load)

namespace datatools {

  template <typename ConfigurableType>
  bool load_ocd(::datatools::object_configuration_description & /*ocd_*/,
                typename boost::disable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* /*dummy*/ = 0)
  {
    return false;
  }


  template <typename ConfigurableType>
  bool load_ocd(::datatools::object_configuration_description & ocd_,
                typename boost::enable_if< has_load< ::datatools::detail::ocd::loader<ConfigurableType> > >::type* dummy = 0)
  {
    (void)dummy;
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

        /// Handle to the OCD data associated to a class
        // The choice for a shared_ptr is questionable for we should not need
        // to share this data structure (a scoped_ptr should be a better choice
        // at the price of changing the registration interface)
        typedef boost::shared_ptr<object_configuration_description> ocd_handle_type;

        /// \brief Internal entry to handle the OCD data associated to a class
        struct entry_type {
          ocd_handle_type handle;
          const object_configuration_description * link;
          entry_type();
          ~entry_type();
          bool has_ocd() const;
          const object_configuration_description & get() const;
        };

        /// The dictionary of OCD data handle
        typedef std::map<std::string, entry_type> ocd_dict_type;

        /// Check if the current instance is the system singleton OCD registration
        bool is_system() const;

        /// Constructor
        ocd_registration(bool preload_system_registration_ = false);

        /// Destructor
        ~ocd_registration();

        /// Check if a class Id is registered in the registration container
        bool has_id(const std::string & class_id_) const;

        /// Fetch the OCD data associated to a given registered class Id
        const object_configuration_description &
        get(const std::string & class_id_) const;

        // Does it make sense to make it a template ?
        template<class T>
        void registration(const std::string & class_name_, const ocd_handle_type & handle_);

        // Does it make sense to make it a template ?
        template<class T>
        void unregistration(const std::string & class_name_);

        /// Smart print
        void smart_dump(std::ostream & out_ = std::clog,
                        const std::string & title_ = "",
                        const std::string & indent_ = "") const;

        /// Compute the list of registered class Ids
        void compute_ids(std::vector<std::string> &) const;

        // Access to a static singleton (mutable version) :
        static ocd_registration & grab_system_registration();

        // Access to a static singleton (non-mutable version) :
        static const ocd_registration & get_system_registration();


      protected:

        void _preload_system_registration();

      private:

        logger::priority _logging_;
        ocd_dict_type _dict_;

      };


      template<class T>
      void ocd_registration::registration(const std::string & class_id_,
                                          const ocd_handle_type & handle_) {
        DT_LOG_TRACE(_logging_, "Attempt to register class ID '" << class_id_ << "' !");
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
        DT_LOG_TRACE(_logging_, "Registration of class ID '" << class_id_ << "' was successful!");
        return;
      }

      template<class T>
      void ocd_registration::unregistration(const std::string & class_id_) {
        DT_LOG_TRACE(_logging_, "Attempt to unregister class ID '" << class_id_ << "' !");
        ocd_dict_type::iterator found = _dict_.find(class_id_);
        if (found == _dict_.end()) {
          DT_LOG_ERROR(_logging_,  "Class ID '" << class_id_ << "' is not registered !");
          return;
          // throw std::logic_error(message.str());
        }
        if (! found->second.handle.unique()) {
          DT_LOG_FATAL(_logging_,
                       "OCD registration entry for class ID '" << class_id_ << "' is not uniquely referenced !"
                       << " TO BE FIXED!");
        }
        _dict_.erase(found);
        DT_LOG_TRACE(_logging_, "Unregistration of class ID '" << class_id_ << "' was successful!");
        return;
      }

      /// \brief Base class for all OCD registrar classes
      struct base_system_factory_registrar
      {
        base_system_factory_registrar(){}
        virtual ~base_system_factory_registrar(){}
      };

      /// \brief Templatized OCD registrar class
      template <class T>
      class system_factory_registrar : public base_system_factory_registrar
      {
      public:
        system_factory_registrar(const std::string & class_id_) {
          _logging_ = ::datatools::logger::PRIO_FATAL;
          char * docd_logging = getenv("DATATOOLS_OCD_DEVEL_LOGGING");
          if (docd_logging) {
            std::string ocd_logging_label = docd_logging;
            _logging_ = ::datatools::logger::get_priority(ocd_logging_label);
            if (_logging_ == ::datatools::logger::PRIO_UNDEFINED) {
              _logging_ = ::datatools::logger::PRIO_FATAL;
            }
          }
          DT_LOG_TRACE(_logging_, "Creation of OCD system_factory_registrar for class '" << class_id_ << "'...");
          DT_THROW_IF(class_id_.empty(), std::logic_error, "Class ID is empty!");
          ocd_registration & ocd_reg = ocd_registration::grab_system_registration();
          ocd_registration::ocd_handle_type h;
          h.reset(new object_configuration_description);
          object_configuration_description & the_ocd = *h.get();
          bool support = load_ocd<T>(the_ocd);
          if (support) {
            ocd_reg.registration<T>(class_id_, h);
            _class_id_ = class_id_;
          }
        }
        ~system_factory_registrar() override {
          DT_LOG_TRACE(_logging_, "Destruction of OCD system_factory_registrar for registered class '" << _class_id_ << "'...");
          ocd_registration & ocd_reg = ocd_registration::grab_system_registration();
          if (! _class_id_.empty() && ocd_reg.has_id(_class_id_)) {
            ocd_reg.unregistration<T>(_class_id_);
          }
        }
      private:

        logger::priority _logging_;
        std::string _class_id_;
      };

      /// \brief Default templatized OCD system factory register manager
      template <typename ClassType>
      struct _ocd_sfr {
      };

    } // namespace ocd
  } // namespace detail
} // namespace datatools

#endif // DATATOOLS_DETAIL_OCD_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
