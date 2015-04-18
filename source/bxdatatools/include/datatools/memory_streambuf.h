/// \file datatools/memory_streambuf.h
#ifndef DATATOOLS_MEMORY_STREAMBUF_H
#define DATATOOLS_MEMORY_STREAMBUF_H

// Standard Library:
#include <streambuf>
#include <iostream>
#include <vector>
#include <stdexcept>

// This project:
#include <datatools/exception.h>

namespace datatools {

  //! \brief Input stream buffer associated to a plain array of characters (char *)
  class array_input_buffer : public std::streambuf {
  public:
    typedef char byte_t;

  public:
    array_input_buffer(const byte_t* array, size_t array_size) {
      DT_THROW_IF (array == 0,std::runtime_error,"Null array pointer !");
      byte_t* base = const_cast<byte_t*>(array);
      byte_t* endp = base + array_size;
      this->setg(base, base, endp);
    }
  };


  //! \brief std::istream interface for a plain array of characters (char *)
  class iarraystream
    : private virtual array_input_buffer,
      public std::istream {
  public:
    iarraystream(const byte_t* array, size_t array_size)
      : array_input_buffer(array, array_size),
        std::istream(this) {}
  };


  //! \brief Input stream buffer associated to a std::vector of characters (std::vector<char>)
  class vector_input_buffer : public std::streambuf {
  public:
    vector_input_buffer(std::vector<char>& vec) {
      char* base = const_cast<char*>(&vec[0]);
      char* endp = base + vec.size();
      this->setg(base, base, endp);
    }
  };


  //! \brief std::istream interface for a std::vector of characters (std::vector<char>)
  class ivectorstream
    : private virtual vector_input_buffer,
      public std::istream {
  public:
    ivectorstream(std::vector<char>& vec)
      : vector_input_buffer(vec),
        std::istream (this) {}
  };


  //! \brief Output stream buffer associated to a std::vector of characters (std::vector<char>)
  class vector_output_buffer : public std::streambuf {
  public:
    static const size_t MIN_CAPACITY = 128;
    static const size_t MIN_INCREMENT = 128;
    static const size_t DEFAULT_CAPACITY = 1024;
    static const size_t DEFAULT_INCREMENT = 0;

  public:
    typedef std::vector<char>::const_iterator iterator;

  public:
    vector_output_buffer(size_t capacity = 1000,
                         size_t increment = 1000,
                         size_t max_capacity = 0) {
      max_capacity_ = buffer_.max_size();
      this->_init(capacity);
      this->set_increment(increment);
      this->set_max_capacity(max_capacity);
    }

    virtual ~vector_output_buffer() {}

    const std::vector<char>& buffer() const {
      return buffer_;
    }


    void set_max_capacity(size_t max_capacity) {
      if (max_capacity > 0) {
        size_t new_max_capacity = max_capacity;
        max_capacity_ = std::max(buffer_.capacity(), new_max_capacity);
      } else {
        max_capacity_ = buffer_.max_size();
      }
    }


    void reset(size_t capacity = 1000,
               size_t increment = 1000,
               size_t max_capacity = 0) {
      max_capacity_ = buffer_.max_size();
      buffer_.clear();
      this->_init(capacity);
      this->set_increment(increment);
      this->set_max_capacity(max_capacity);
    }


    size_t get_size() const {
      return buffer_.size();
    }


    const char* get_address() const {
      if (buffer_.size() > 0) {
        return const_cast<char*>(&buffer_[0]);
      }
      return 0;
    }


  protected:
    void init(size_t capacity) {
      size_t min_capacity = MIN_CAPACITY;
      size_t capacity = std::max(min_capacity, capacity);
      buffer_.reserve(capacity);
      char* base = const_cast<char*>(&buffer_[0]);
      char* endp = base + buffer_.size();
      *base = '\0';
      setp(base, endp);
    }

    void _set_increment(size_t increment) {
      if (increment > 0) {
        size_t min_increment = MIN_INCREMENT;
        increment_ = std::max(min_increment, increment);
      } else {
        increment_ = 0;
      }
    }


  public:
    void dump() {
      std::cerr << "DUMP: " << std::endl;
      std::cerr << "|-- Increment:       " << increment_ << std::endl;
      std::cerr << "|-- Max capacity:    " << max_capacity_ << std::endl;
      std::cerr << "|-- Buffer size:     " << buffer_.size() << std::endl;
      std::cerr << "|-- Buffer capacity: " << buffer_.capacity() << std::endl;
      std::cerr << "|-- Buffer contains: `";
      for (int i = 0; i < (int)buffer_.size(); i++) {
        std::cerr << buffer_[i];
      }
      std::cerr << "'" << std::endl;
      std::cerr << "|-- pptr ():         "
                << hex
                << reinterpret_cast<void*>(this->pptr())
                << dec
                << std::endl;

      std::cerr << "|-- epptr ():        "
                << hex
                << reinterpret_cast<void*>(this->epptr())
                << dec
                << std::endl;

      std::cerr << "`-- " << "end" << std::endl;
    }


    int overflow(int c) {
      const char conv = std::char_traits<char>::to_char_type(c);
      const bool __testeof = std::char_traits<char>::eq_int_type(c, std::char_traits<char>::eof());

      if (__builtin_expect(__testeof, false)) {
        return std::char_traits<char>::not_eof(c);
      }

      const size_t capacity = buffer_.capacity();
      const bool testput = this->pptr() < this->epptr();
      if (!testput) {
        if (buffer_.size() == buffer_.capacity()) {
          if (capacity == max_capacity_) {
            return std::char_traits<char>::eof();
          }

          size_t opt_len = buffer_.capacity();
          if (increment_ > 0) {
            opt_len += increment_;
          } else {
            opt_len *= 3;
          }

          const size_t len = std::min(opt_len, max_capacity_);
          buffer_.reserve(len);
        }

        buffer_.push_back(conv);
        char* base = const_cast<char*>(&buffer_[0]);
        char* endp = base + buffer_.size();
        setp(base + buffer_.size(), endp);
      } else {
        buffer_.push_back(conv);
        this->pbump(1);
      }

      return std::char_traits<char>::not_eof(c);
    }


  private:
    size_t              capacity_;
    size_t              max_capacity_;
    size_t              increment_;
    std::vector<char>   buffer_;
  };

  //! \brief std::ostream interface for a std::vector of characters (std::vector<char>)
  class ovectorstream
    : public vector_output_buffer,
      public std::ostream {
  public:
    ovectorstream(size_t capacity = DEFAULT_CAPACITY,
                  size_t increment = DEFAULT_INCREMENT,
                  size_t max_capacity = 0)
      : vector_output_buffer(capacity, increment, max_capacity),
        std::ostream(this) {}
  };

} // end of namespace datatools

#endif // DATATOOLS_MEMORY_STREAMBUF_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
