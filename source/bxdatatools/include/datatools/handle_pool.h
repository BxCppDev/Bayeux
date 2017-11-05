/// \file datatools/handle_pool.h
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

  /// \brief A pool of handles on a given class
  template <class T>
  class handle_pool
  {
  public:

    typedef T element_type;
    typedef datatools::handle<element_type> handle_type;

    /// Construct an empty pool
    handle_pool();

    /// Construct a pool with a default size
    explicit handle_pool(size_t size_);

    /// Destructor
    virtual ~handle_pool();

    /// Empty the pool
    void clear();

    /// Return the number of handles the pool can contain
    size_t get_capacity() const;

    /// Return the number of handles currently in use
    size_t get_number_of_used_item() const;

    /// Set the number of used handles in the pool to zero
    void reset();

    /// Change number of handles pool can contain
    void resize(size_t size_);

    /// Return a const reference to a new handle
    const handle_type& create();

    /// Input pool capacity and used handles to an output stream
    void dump(std::ostream & out_ = std::clog,
              const std::string & title_ = "",
              const std::string & indent_ = "",
              bool abridged_ = true) const;

    /// Implementation for public resize method
    void resize_impl(size_t size_);

  protected:

    std::vector<handle_type> _buffer_; //<! Collection of handles held in the pool
    size_t _number_of_used_item_; //<! Number of pool handles currently in use

  };

  //----------------------------------------------------------------------
  // Implementation of handle_pool methods

  template <typename T>
  handle_pool<T>::handle_pool()
  {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif
    _number_of_used_item_ = 0;
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
  }


  template <typename T>
  handle_pool<T>::handle_pool(size_t size_)
  {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif
    _number_of_used_item_ = 0;
    this->resize_impl(size_);
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
  }


  template <typename T>
  handle_pool<T>::~handle_pool()
  {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "buffer size is " << _buffer_.size());
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "number of used element " << _number_of_used_item);
#endif
    _number_of_used_item_ = 0;
    _buffer_.clear();
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
    return;
  }


  template <typename T>
  void handle_pool<T>::clear()
  {
    _buffer_.clear();
    return;
  }


  template <typename T>
  size_t handle_pool<T>::get_capacity() const
  {
    return _buffer_.size();
  }


  template <typename T>
  size_t handle_pool<T>::get_number_of_used_item() const
  {
    return _number_of_used_item_;
  }


  template <typename T>
  void handle_pool<T>::reset()
  {
    _number_of_used_item_ = 0;
    return;
  }


  template <typename T>
  void handle_pool<T>::resize(size_t size_)
  {
    DT_THROW_IF (_number_of_used_item_ > 0,
                 std::logic_error,
                 "Cannot resize pool with items in use");
    this->resize_impl(size_);
    return;
  }


  template <typename T>
  const typename handle_pool<T>::handle_type& handle_pool<T>::create()
  {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "buffer size is " << _buffer_.size());
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "number of used element " << _number_of_used_item_);
#endif

    if (_number_of_used_item_ < _buffer_.size()) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Use a pre-allocated element @ position "
                   << _number_of_used_item_);
      DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
      return _buffer_[_number_of_used_item_++];
    }

    element_type *ptr = new element_type;
    _buffer_.push_back(handle_type(ptr));
    _number_of_used_item_++;
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Use a newly allocated element @ position "
                 << (_number_of_used_item_ - 1));
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif
    return _buffer_.back();
  }


  template <typename T>
  void handle_pool<T>::dump(std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool abridged_) const {
    if (!title_.empty()) {
      out_ << indent_ << title_ << " : " << std::endl;
    }
    out_ << indent_ << "|-- " << "Capacity     : " << this->get_capacity() << std::endl;
    out_ << indent_ << "|-- " << "# Used items : " << _number_of_used_item_ << std::endl;
    out_ << indent_ << "`-- " << "Items @ " << std::endl;
    for (size_t i = 0; i < _buffer_.size(); ++i) {
      if (abridged_) {
        if (i >= 10) continue;
      }
      out_ << indent_ << "    ";
      const handle_type& h = _buffer_[i];
      if (i < _buffer_.size() - 1) {
        out_ << "|-- ";
      } else {
        out_ << "`-- ";
      }
      out_ << "index " << i << " : ";
      if (!h) {
        out_ << "No element!" << std::endl;
      } else {
        out_ << "@ " << &h.get() << " [unique=" << h.unique() << "]"<< std::endl;
      }
    }
    return;
  }


  template <typename T>
  void handle_pool<T>::resize_impl(size_t size_) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_ENTERING(datatools::logger::PRIO_ALWAYS);
#endif

    size_t current_size = _buffer_.size();

#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS,  "current_size = " << current_size);
#endif

    if (size_ <= current_size) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Capacity is enough for " << size_);
#endif
      return;
    }

    _buffer_.reserve(size_);
    for (size_t i = current_size; i < size_; ++i) {
#ifdef DATATOOLS_HANDLE_POOL_DEVEL
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS,  "Add a handle with a new element...");
#endif
      _buffer_.push_back(handle_type(new element_type));
    }

    _number_of_used_item_ = 0;

#ifdef DATATOOLS_HANDLE_POOL_DEVEL
    DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
#endif

    return;
  }

} // namespace datatools

#endif // DATATOOLS_HANDLE_POOL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
