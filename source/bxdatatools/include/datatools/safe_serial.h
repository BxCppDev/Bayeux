// -*- mode: c++; -*-
/* safe_serial.h */
/**
 *
 * safe_serial is a template class useful to record
 * a list of instances of some type in a safe way
 * for serialization. We can access one instance at a time
 * (the current instance) but previous instances are accumulated in
 * some internal container, being never erased.
 *
 * \code
 * datatools::serialization::safe_serial<data_t> safe;
 *
 * safe.make(); // create a new instance and make it the current one
 * safe.get();  // access to the current instance
 * \endcode
 */
#ifndef DATATOOLS_SAFE_SERIAL_H_
#define DATATOOLS_SAFE_SERIAL_H_

// Standard Library
#include <list>
#include <stdexcept>

// Third Party

// Datatools
#include <datatools/exception.h>

namespace datatools {

//! \brief Template container wrapper class for safe storage of object with regards to Boost/Serilization memory tracking feature.
template<class T>
class safe_serial {
 public:
  typedef T data_type;

 public:
  safe_serial() : data_() {
    last_ = data_.rend();
  }


  virtual ~safe_serial() {
    this->clear();
  }


  void clear() {
    data_.clear();
    last_ = data_.rend();
  }


  bool empty() const {
    return data_.size() == 0;
  }


  data_type& get() {
    DT_THROW_IF (this->empty(), std::logic_error, "No data!");
    return *(last_);
  }


  void make() {
    try {
      data_.push_back(data_type());
    }
    catch (...) {
      DT_THROW_IF(true, std::logic_error, "Cannot insert new data element!");
    }
    last_ = data_.rbegin();
  }

  void set(const data_type& data) {
    this->make();
    this->get() = data;
  }

 private:
  typename std::list<T>                   data_;
  typename std::list<T>::reverse_iterator last_;
};

} // end of namespace datatools

#endif // DATATOOLS_SAFE_SERIAL_H_

