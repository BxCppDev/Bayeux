// memory_streambuf.cc
//
// Copyright (c) 2015 by F.Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/memory_streambuf.h>

namespace datatools {

  array_input_buffer::array_input_buffer(const byte_t * array_, size_t array_size_)
  {
    DT_THROW_IF(array_ == 0, std::logic_error, "Null array pointer !");
    byte_t * base = const_cast<byte_t*>(array_);
    byte_t * endp = base + array_size_;
    this->setg(base, base, endp);
    return;
  }

  iarraystream::iarraystream(const byte_t * array_, size_t array_size_)
    : array_input_buffer(array_, array_size_)
    , std::istream(this)
  {
    return;
  }

  vector_input_buffer::vector_input_buffer(std::vector<char> & vec_)
  {
    char * base = const_cast<char*>(&vec_[0]);
    char * endp = base + vec_.size();
    this->setg(base, base, endp);
    return;
  }

  ivectorstream::ivectorstream(std::vector<char> & vec_)
    : vector_input_buffer(vec_),
      std::istream(this)
  {
    return;
  }

  // static
  const size_t vector_output_buffer::MIN_CAPACITY;
  const size_t vector_output_buffer::MIN_INCREMENT;
  const size_t vector_output_buffer::DEFAULT_CAPACITY;
  const size_t vector_output_buffer::DEFAULT_INCREMENT;

  vector_output_buffer::vector_output_buffer(size_t capacity_,
                                             size_t increment_,
                                             size_t max_capacity_)
  {
    _max_capacity_ = _buffer_.max_size();
    this->_init(capacity_);
    this->_set_increment(increment_);
    this->set_max_capacity(max_capacity_);
    return;
  }

  vector_output_buffer::~vector_output_buffer()
  {
    return;
  }

  const std::vector<char> & vector_output_buffer::buffer() const
  {
    return _buffer_;
  }

  void vector_output_buffer::set_max_capacity(size_t max_capacity_)
  {
    if (max_capacity_ > 0) {
      size_t new_max_capacity = max_capacity_;
      _max_capacity_ = std::max(_buffer_.capacity(), new_max_capacity);
    } else {
      _max_capacity_ = _buffer_.max_size();
    }
    return;
  }

  void vector_output_buffer::reset(size_t capacity_,
                                   size_t increment_,
                                   size_t max_capacity_)
  {
    _max_capacity_ = _buffer_.max_size();
    _buffer_.clear();
    this->_init(capacity_);
    this->_set_increment(increment_);
    this->set_max_capacity(max_capacity_);
    return;
  }

  size_t vector_output_buffer::get_size() const
  {
    return _buffer_.size();
  }


  const char * vector_output_buffer::get_address() const
  {
    if (_buffer_.size() > 0) {
      return const_cast<char*>(&_buffer_[0]);
    }
    return 0;
  }

  void vector_output_buffer::_init(size_t new_capacity_)
  {
    size_t min_capacity = MIN_CAPACITY;
    size_t capacity = std::max(min_capacity, new_capacity_);
    _buffer_.reserve(capacity);
    char* base = const_cast<char*>(&_buffer_[0]);
    char* endp = base + _buffer_.size();
    *base = '\0';
    setp(base, endp);
    return;
  }

  void vector_output_buffer::_set_increment(size_t increment_)
  {
    if (increment_ > 0) {
      size_t min_increment = MIN_INCREMENT;
      _increment_ = std::max(min_increment, increment_);
    } else {
      _increment_ = 0;
    }
    return;
  }

  void vector_output_buffer::dump() const
  {
    std::cerr << "DUMP: " << std::endl;
    std::cerr << "|-- Increment:       " << _increment_ << std::endl;
    std::cerr << "|-- Max capacity:    " << _max_capacity_ << std::endl;
    std::cerr << "|-- Buffer size:     " << _buffer_.size() << std::endl;
    std::cerr << "|-- Buffer capacity: " << _buffer_.capacity() << std::endl;
    std::cerr << "|-- Buffer contains: `";
    for (int i = 0; i < (int)_buffer_.size(); i++) {
      std::cerr << _buffer_[i];
    }
    std::cerr << "'" << std::endl;
    std::cerr << "|-- pptr ():         "
              << std::hex
              << reinterpret_cast<void*>(this->pptr())
              << std::dec
              << std::endl;

    std::cerr << "|-- epptr ():        "
              << std::hex
              << reinterpret_cast<void*>(this->epptr())
              << std::dec
              << std::endl;

    std::cerr << "`-- " << "end" << std::endl;
    return;
  }

  int vector_output_buffer::overflow(int c_)
  {
    const char conv = std::char_traits<char>::to_char_type(c_);
    const bool testeof = std::char_traits<char>::eq_int_type(c_, std::char_traits<char>::eof());

    if (__builtin_expect(testeof, false)) {
      return std::char_traits<char>::not_eof(c_);
    }

    const size_t capacity = _buffer_.capacity();
    const bool testput = this->pptr() < this->epptr();
    if (!testput) {
      if (_buffer_.size() == _buffer_.capacity()) {
        if (capacity == _max_capacity_) {
          return std::char_traits<char>::eof();
        }

        size_t opt_len = _buffer_.capacity();
        if (_increment_ > 0) {
          opt_len += _increment_;
        } else {
          opt_len *= 3;
        }

        const size_t len = std::min(opt_len, _max_capacity_);
        _buffer_.reserve(len);
      }

      _buffer_.push_back(conv);
      char* base = const_cast<char*>(&_buffer_[0]);
      char* endp = base + _buffer_.size();
      setp(base + _buffer_.size(), endp);
    } else {
      _buffer_.push_back(conv);
      this->pbump(1);
    }

    return std::char_traits<char>::not_eof(c_);
  }

  ovectorstream::ovectorstream(size_t capacity_,
                               size_t increment_,
                               size_t max_capacity_)
    : vector_output_buffer(capacity_, increment_, max_capacity_)
    , std::ostream(this)
  {
    return;
  }

} // end of namespace datatools
