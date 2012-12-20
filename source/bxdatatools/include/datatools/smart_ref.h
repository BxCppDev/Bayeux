// -*- mode: c++ ; -*- 
/* smart_ref.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_SMART_REF_H_
#define DATATOOLS_SMART_REF_H_
// Standard Library
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <stdexcept>
#include <sstream>

// Third Party
// - A
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// This Project
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
  // ctor:
  smart_ref() : ref_(0), properties_() {}

  // ctor:
  smart_ref(const_reference_type obj) {
    this->set(obj);
  }

  // dtor:
  virtual ~smart_ref() {}


  void set(const_reference_type obj) {
    ref_ = const_cast<pointer_type>(&obj);
  }

  const_reference_type get() const {
    return *ref_;
  }


  const datatools::properties& get_properties() const {
    return properties_;
  }

  datatools::properties& get_properties() {
    return properties_;
  }

  void set_properties(const datatools::properties& props) {
    properties_ = props;
  }

  void reset() {
    ref_ = 0;
  }

  bool is_valid() const {
    return ref_ != 0;
  }


 private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & boost::serialization::make_nvp("properties", properties_);
    ar & boost::serialization::make_nvp("ref", ref_);
  }

 private: 
  pointer_type          ref_;
  datatools::properties properties_;


//----------------------------------------------------------------------
// has_flag predicate (inner class)
 public:
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
};

} // end of namespace datatools

#endif // DATATOOLS_SMART_REF_H_

