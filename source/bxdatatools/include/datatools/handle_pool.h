/// \file datatools/handle.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-05-10
 * Last modified : 2013-04-22
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *  A pool of handles.
 *
 */
#ifndef DATATOOLS_HANDLE_POOL_H
#define DATATOOLS_HANDLE_POOL_H

// Standard Library:
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// This Project:
#include <datatools/handle.h>

namespace datatools {

  /*! \brief A pool of handles on a given class
   */
  template <class T>
  class handle_pool  {
  public:
    typedef T element_type;
    typedef datatools::handle<element_type> handle_type;

  public:
    /// Construct an empty pool
    handle_pool();

    /// Construct a pool with a default size
    explicit handle_pool(size_t size);

    /// Destructor
    virtual ~handle_pool();

  public:
    /// Empty the pool
    void clear();

    /// Return the number of handles the pool can contain
    size_t get_capacity() const;

    /// Return the number of handles currently in use
    size_t get_number_of_used_item() const;

    /// Set the number of used handles in the pool to zero
    void reset();

    /// Change number of handles pool can contain
    void resize(size_t size);

    /// Return a const reference to a new handle
    const handle_type& create();

    /// Input pool capacity and used handles to an output stream
    void dump(std::ostream& out = std::clog,
              const std::string& title = "",
              const std::string& indent = "",
              bool abridged = true) const;

  protected:
    /// Implementation for public resize method
    void resize_impl(size_t size);

  protected:
    std::vector<handle_type> buffer_; //<! Collection of handles held in the pool
    size_t number_of_used_item_; //<! Number of pool handles currently in use
  };


  //----------------------------------------------------------------------
  // Implementation of handle_pool methods

  template <typename T>
  handle_pool<T>::handle_pool() {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif
    number_of_used_item_ = 0;
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
  }


  template <typename T>
  handle_pool<T>::handle_pool(size_t size) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif
    number_of_used_item_ = 0;
    this->resize_impl(size);
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
  }


  template <typename T>
  handle_pool<T>::~handle_pool() {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "buffer size is " << buffer.size());
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "number of used element " << _number_of_used_item);
#endif
    number_of_used_item_ = 0;
    buffer_.clear();
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
  }


  template <typename T>
  void handle_pool<T>::clear() {
    buffer_.clear();
  }


  template <typename T>
  size_t handle_pool<T>::get_capacity() const {
    return buffer_.size();
  }


  template <typename T>
  size_t handle_pool<T>::get_number_of_used_item() const {
    return number_of_used_item_;
  }


  template <typename T>
  void handle_pool<T>::reset() {
    number_of_used_item_ = 0;
  }


  template <typename T>
  void handle_pool<T>::resize(size_t size) {
    DT_THROW_IF (number_of_used_item_ > 0,
                 std::logic_error,
                 "Cannot resize pool with items in use");
    this->resize_impl(size);
  }


  template <typename T>
  const typename handle_pool<T>::handle_type& handle_pool<T>::create() {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "buffer size is " << buffer.size());
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "number of used element " << _number_of_used_item);
#endif

    if (number_of_used_item_ < buffer_.size()) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Use a pre-allocated element @ position "
                   << number_of_used_item_);
      DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
      return buffer_[number_of_used_item_++];
    }

    element_type *ptr = new element_type;
    buffer_.push_back(handle_type(ptr));
    number_of_used_item_++;
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Use a newly allocated element @ position "
                 << (number_of_used_item_ - 1));
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
    return buffer_.back();
  }


  template <typename T>
  void handle_pool<T>::dump(std::ostream& out,
                            const std::string& title,
                            const std::string& indent,
                            bool abridged) const {
    if (!title.empty()) {
      out << indent << title << " : " << std::endl;
    }
    out << indent << "|-- " << "Capacity     : " << this->get_capacity() << std::endl;
    out << indent << "|-- " << "# Used items : " << number_of_used_item_ << std::endl;
    out << indent << "`-- " << "Items @ " << std::endl;
    for (size_t i = 0; i < buffer_.size(); ++i) {
      if (abridged) {
        if (i >= 10) continue;
      }
      out << indent << "    ";
      const handle_type& h = buffer_[i];
      if (i < buffer_.size() - 1) {
        out << "|-- ";
      } else {
        out << "`-- ";
      }
      out << "index " << i << " : ";
      if (!h) {
        out << "No element!" << std::endl;
      }
      else {
        out << "@ " << &h.get() << " [unique=" << h.unique() << "]"<< std::endl;
      }
    }
  }


  template <typename T>
  void handle_pool<T>::resize_impl(size_t size) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif

    size_t current_size = buffer_.size();

#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS,  "current_size = " << current_size);
#endif

    if (size <= current_size) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Capacity is enough for " << size);
#endif
      return;
    }

    buffer_.reserve(size);
    for (size_t i = current_size; i < size; ++i) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS,  "Add a handle with a new element...");
#endif
      buffer_.push_back(handle_type(new element_type));
    }

    number_of_used_item_ = 0;

#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif

    return;
  }

} // namespace datatools

#endif // DATATOOLS_HANDLE_POOL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
