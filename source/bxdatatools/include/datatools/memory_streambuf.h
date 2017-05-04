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

    /// Constructor
    array_input_buffer(const byte_t * array, size_t array_size);

  };


  //! \brief std::istream interface for a plain array of characters (char *)
  class iarraystream
    : private virtual array_input_buffer,
      public std::istream {
  public:
    /// Constructor
    iarraystream(const byte_t * array, size_t array_size);

  };


  //! \brief Input stream buffer associated to a std::vector of characters (std::vector<char>)
  class vector_input_buffer : public std::streambuf {
  public:
    /// Constructor
    vector_input_buffer(std::vector<char> & vec_);

  };


  //! \brief std::istream interface for a std::vector of characters (std::vector<char>)
  class ivectorstream
    : private virtual vector_input_buffer,
      public std::istream {
  public:
    /// Constructor
    ivectorstream(std::vector<char> &);

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
    /// Constructor
    vector_output_buffer(size_t capacity = 1000,
                         size_t increment = 1000,
                         size_t max_capacity = 0);

    virtual ~vector_output_buffer();

    const std::vector<char>& buffer() const;

    void set_max_capacity(size_t max_capacity);

    void reset(size_t capacity = 1000,
               size_t increment = 1000,
               size_t max_capacity = 0);

    size_t get_size() const;

    const char* get_address() const;

  protected:

    void _init(size_t capacity);

    void _set_increment(size_t increment);


  public:
    void dump() const;

    int overflow(int c);

  private:

    size_t              max_capacity_; //!< Maximum capacity
    size_t              increment_; //!< Increment
    std::vector<char>   buffer_; //!< Array of characters

  };

  //! \brief std::ostream interface for a std::vector of characters (std::vector<char>)
  class ovectorstream
    : public vector_output_buffer,
      public std::ostream {
  public:
    ovectorstream(size_t capacity = DEFAULT_CAPACITY,
                  size_t increment = DEFAULT_INCREMENT,
                  size_t max_capacity = 0);
    /*
      : vector_output_buffer(capacity, increment, max_capacity),
      std::ostream(this) {}
    */
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
