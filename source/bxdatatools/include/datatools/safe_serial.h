/// \file datatools/safe_serial.h
/**
 * datatools::safe_serial is a template class useful to record
 * a list of instances of some type in a safe way
 * for serialization. We can access one instance at a time
 * (the current instance) but previous instances are accumulated in
 * some internal container, being never erased. This behaviour is
 * fundamental when using the memory tracking features of the Boost/serialization
 * library.
 *
 * \code
 * datatools::safe_serial<data_t> safe;
 *
 * safe.make(); // create a new instance and make it the current one
 * safe.get();  // access to the current instance
 * \endcode
 */

#ifndef DATATOOLS_SAFE_SERIAL_H
#define DATATOOLS_SAFE_SERIAL_H

// Standard Library:
#include <list>
#include <stdexcept>

// This project:
#include <datatools/exception.h>

namespace datatools {

  //! \brief Template container wrapper class for safe storage of object with regards to Boost/Serilization memory tracking feature.
  template<class T>
  class safe_serial
  {
  public:
    typedef T data_type;

    /// Default constructor
    safe_serial() : data_()
    {
      last_ = data_.rend();
    }

    /// Destructor
    virtual ~safe_serial()
    {
      this->clear();
    }

    /// Clear the container
    void clear()
    {
      data_.clear();
      last_ = data_.rend();
    }

    /// Check if the container is empty
    bool empty() const
    {
      return data_.size() == 0;
    }

    /// Return a const reference to the current object
    const data_type & get() const
    {
      DT_THROW_IF (this->empty(), std::logic_error, "No data!");
      return *(last_);
    }

    // /// \deprecated Return a mutable reference to the current object
    // data_type& get() {
    //   DT_THROW_IF (this->empty(), std::logic_error, "No data!");
    //   return *(last_);
    // }

    /// Return a mutable reference to the current object
    data_type & grab()
    {
      DT_THROW_IF (this->empty(), std::logic_error, "No data!");
      return *(last_);
    }

    /// Insert a new object at the end
    void make()
    {
      try {
        data_.push_back(data_type());
      }
      catch (...) {
        DT_THROW_IF(true, std::logic_error, "Cannot insert new data element!");
      }
      last_ = data_.rbegin();
    }

    /// Set the current object
    void set(const data_type & data)
    {
      this->make();
      this->get() = data;
    }

  private:

    typename std::list<T>                   data_; /// The list of objects
    typename std::list<T>::reverse_iterator last_; /// The current position of the last object

  };

} // end of namespace datatools

#endif // DATATOOLS_SAFE_SERIAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
