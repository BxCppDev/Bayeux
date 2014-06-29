// -*- mode: c++ ; -*-
/// \file datatools/smart_ref.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2010-03-16
 *
 * License:
 *
 * Description:
 *   Smart reference to an existing instance
 *   The referenced type must have a default constructor.
 *
 * History:
 *
 */

#ifndef DATATOOLS_SMART_REF_H
#define DATATOOLS_SMART_REF_H

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <stdexcept>
#include <sstream>

// Third Party:
// - Boost:
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// This Project:
#include <datatools/properties.h>

namespace datatools {

//! \brief A template class that stores a reference to an object and additional associated meta-data
template <class T>
class smart_ref {
 public:
  typedef T                        instance_type;
  typedef instance_type&           reference_type;
  typedef const instance_type&     const_reference_type;
  typedef instance_type*           pointer_type;
  typedef const instance_type*     const_pointer_type;
  typedef std::list<instance_type> col_type;
  typedef smart_ref<instance_type> smart_ref_type;

 public:

  /// Default constructor
  smart_ref() : ref_(0), properties_() {}

  /// Constructor
  smart_ref(const_reference_type obj) {
    this->set(obj);
  }

  /// Destructor
  virtual ~smart_ref() {}

  /// Set the internal reference to the target object
  void set(const_reference_type obj) {
    ref_ = const_cast<pointer_type>(&obj);
  }

  /// Return the reference to the target object
  const_reference_type get() const {
    return *ref_;
  }

  /// Return a non mutable reference to the container of properties
  const datatools::properties& get_properties() const {
    return properties_;
  }

  /// Return a mutable reference to the container of properties
  datatools::properties& grab_properties() {
    return properties_;
  }

  /// \deprecated Return a mutable reference to the container of properties
  datatools::properties& get_properties() {
    return properties_;
  }

  /// Set the container of properties
  void set_properties(const datatools::properties& props) {
    properties_ = props;
  }

  /// Reset the internal reference
  void reset() {
    ref_ = 0;
  }

  /// Check the validity of the smart reference
  bool is_valid() const {
    return ref_ != 0;
  }

  //! \brief Predicate used by the smart_ref template class
  class has_flag : public std::unary_function<smart_ref_type, bool> {
   public:
    explicit has_flag(const std::string& flag) : flag_(flag) {}

    bool operator()(const smart_ref_type& s) const {
      if (!s.is_valid()) return false;
      return (s.get_properties().has_flag(flag_));
    }
   private:
    std::string flag_;
  };

 private:

  pointer_type          ref_; /// Handle to the target object
  datatools::properties properties_; /// Container of auxiliary properties

  friend class boost::serialization::access;

  /// Boost serialization method
  template<class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/) {
    ar & boost::serialization::make_nvp("properties", properties_);
    ar & boost::serialization::make_nvp("ref", ref_);
  }

};

} // end of namespace datatools

#endif // DATATOOLS_SMART_REF_H
