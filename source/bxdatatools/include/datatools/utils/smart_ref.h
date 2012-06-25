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
#ifndef DATATOOLS_UTILS_SMART_REF_H_
#define DATATOOLS_UTILS_SMART_REF_H_
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
#include <datatools/utils/properties.h>

namespace datatools {
namespace utils {

template <class T>
class smart_ref {
 public:
  typedef T            instance_t;
  typedef T&           reference_t;
  typedef const T&     const_reference_t;
  typedef T*           pointer_t;
  typedef const T*     const_pointer_t;
  typedef std::list<T> col_t;
  typedef smart_ref<T> smart_ref_t;

 public: 
  // ctor:
  smart_ref() : ref_(0), properties_() {}

  // ctor:
  smart_ref(const_reference_t obj) {
    this->set(obj);
  }

  // dtor:
  virtual ~smart_ref() {}


  void set(const_reference_t obj) {
    ref_ = const_cast<pointer_t>(&obj);
  }

  const_reference_t get() const {
    return *ref_;
  }


  const datatools::utils::properties& get_properties() const {
    return properties_;
  }

  datatools::utils::properties& get_properties() {
    return properties_;
  }

  void set_properties(const datatools::utils::properties& props) {
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
  pointer_t                    ref_;
  datatools::utils::properties properties_;


//----------------------------------------------------------------------
// has_flag predicate (inner class)
 public:
  class has_flag : public std::unary_function<smart_ref_t, bool> {
   public:
    explicit has_flag(const std::string& flag) : flag_(flag) {}

    bool operator()(const smart_ref_t& s) const {
      if (!s.is_valid()) return false;
      return (s.get_properties().has_flag(flag_));
    }
   private:
    std::string flag_;
  };
};

} // end of namespace utils 
} // end of namespace datatools

#endif // DATATOOLS_UTILS_SMART_REF_H_

