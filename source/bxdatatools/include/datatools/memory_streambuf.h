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
  class array_input_buffer
    : public std::streambuf
  {
  public:

    typedef char byte_t;

    /// Constructor
    array_input_buffer(const byte_t * array_, size_t array_size_);

  };


  //! \brief std::istream interface for a plain array of characters (char *)
  class iarraystream
    : private virtual array_input_buffer
    , public std::istream
  {
  public:

    /// Constructor
    iarraystream(const byte_t * array_, size_t array_size_);

  };


  //! \brief Input stream buffer associated to a std::vector of characters (std::vector<char>)
  class vector_input_buffer
    : public std::streambuf
  {
  public:
    /// Constructor
    vector_input_buffer(std::vector<char> & vec_);

  };


  //! \brief std::istream interface for a std::vector of characters (std::vector<char>)
  class ivectorstream
    : private virtual vector_input_buffer
    , public std::istream
  {
  public:

    /// Constructor
    ivectorstream(std::vector<char> &);

  };


  //! \brief Output stream buffer associated to a std::vector of characters (std::vector<char>)
  class vector_output_buffer
    : public std::streambuf
  {
  public:
    static const size_t MIN_CAPACITY = 128;
    static const size_t MIN_INCREMENT = 128;
    static const size_t DEFAULT_CAPACITY = 1024;
    static const size_t DEFAULT_INCREMENT = 0;

    typedef std::vector<char>::const_iterator iterator;

  public:

    /// Constructor
    vector_output_buffer(size_t capacity_ = 1000,
                         size_t increment_ = 1000,
                         size_t max_capacity_ = 0);

    ~vector_output_buffer() override;

    const std::vector<char>& buffer() const;

    void set_max_capacity(size_t max_capacity_);

    void reset(size_t capacity_ = 1000,
               size_t increment_ = 1000,
               size_t max_capacity_ = 0);

    size_t get_size() const;

    const char* get_address() const;

  protected:

    void _init(size_t capacity_);

    void _set_increment(size_t increment_);


  public:
    void dump() const;

    int overflow(int c_) override;

  private:

    size_t              _max_capacity_; //!< Maximum capacity
    size_t              _increment_; //!< Increment
    std::vector<char>   _buffer_; //!< Array of characters

  };

  //! \brief std::ostream interface for a std::vector of characters (std::vector<char>)
  class ovectorstream
    : public vector_output_buffer
    , public std::ostream
  {
  public:
    ovectorstream(size_t capacity_ = DEFAULT_CAPACITY,
                  size_t increment_ = DEFAULT_INCREMENT,
                  size_t max_capacity_ = 0);
    /*
      : vector_output_buffer(capacity, increment, max_capacity),
      std::ostream(this) {}
    */
  };

} // end of namespace datatools

#endif // DATATOOLS_MEMORY_STREAMBUF_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
