// -*- mode: c++;-*-
// vector_streambuf.h

#include <streambuf>
#include <iostream>
#include <vector>
#include <stdexcept>

namespace datatools {

  namespace serialization {
  
    // input from a plain array of characters/bytes:
    class array_input_buffer : public std::streambuf
    {
    public:
      typedef char byte_t;
    public:
      array_input_buffer (const byte_t * array_, size_t array_size_) 
      {
	if (array_ ==0)
	  {
	    throw std::runtime_error ("array_input_buffer: Null array pointer !");
	  }
	byte_t * base = const_cast<byte_t *> (array_);
	byte_t * endp = base + array_size_;
	this->setg (base, base, endp);
      }

    };
  
    class iarraystream : private virtual array_input_buffer, public std::istream
    {
    public:
      iarraystream (const byte_t * array_, size_t array_size_) : 
	array_input_buffer (array_, array_size_),
	std::istream (this)
      {
      }

    };
  
    // input from vector of characters:
    class vector_input_buffer : public std::streambuf
    {
    public:
      vector_input_buffer (std::vector<char> & vec_) 
      {
	char * base = const_cast<char*> (&vec_[0]);
	char * endp = base + vec_.size ();
	this->setg (base, base, endp);
	//&vec_[0], &vec_[0], &vec_[0] + vec_.size ());
      }

    };
  
    class ivectorstream : private virtual vector_input_buffer, public std::istream
    {
    public:
      ivectorstream (std::vector<char> & vec_) : 
	vector_input_buffer (vec_),
	std::istream (this)
      {
      }

    };
  
    // output to vector of characters:
    /*
     * 
     *
     *  [                                    ]
     *  pbase    pptr   epptr
     *
     */
    class vector_output_buffer : public std::streambuf
    {
    public:
      static const size_t MIN_CAPACITY = 128;
      static const size_t MIN_INCREMENT = 128;
      static const size_t DEFAULT_CAPACITY = 1024;
      static const size_t DEFAULT_INCREMENT = 0;

    private:

      size_t              __capacity;
      size_t              __max_capacity;
      size_t              __increment;
      std::vector<char>   __buffer;

    public:

      typedef std::vector<char>::const_iterator iterator;

    public:

      const std::vector<char> & buffer () const
      {
	return __buffer;
      }

      void set_max_capacity (size_t max_capacity_)
      {
	if (max_capacity_ > 0)
	  {
	    size_t new_max_capacity = max_capacity_;
	    __max_capacity = std::max (__buffer.capacity (), new_max_capacity);
	  }
	else
	  {
	    __max_capacity = __buffer.max_size ();
	  }
	return;
      }

      void reset (size_t capacity_ = 1000,
		  size_t increment_ = 1000,
		  size_t max_capacity_ = 0)
      {
	__max_capacity = __buffer.max_size ();
	__buffer.clear ();
	_init (capacity_);
	_set_increment (increment_);
	set_max_capacity (max_capacity_);
	return;
      }

      size_t get_size () const
      {
	return __buffer.size ();
      }

      const char * get_address () const
      {
	if (__buffer.size () > 0)
	  {
	    return const_cast<char*> (&__buffer[0]);
	  }
	return 0;
      }
    protected:

      void _init (size_t capacity_)
      {
	size_t min_capacity = MIN_CAPACITY;
	size_t capacity = std::max (min_capacity, capacity_);
	__buffer.reserve (capacity);
	char * base =  const_cast<char*>(&__buffer[0]);
	char * endp = base + __buffer.size ();
	*base = '\0';
	setp (base, endp); 
	return;
      }

      void _set_increment (size_t increment_)
      {
	if (increment_ > 0)
	  {
	    size_t min_increment = MIN_INCREMENT;
	    __increment = std::max (min_increment, increment_);
	  }
	else
	  {
	    __increment = 0;
	  }
	return;
      }

    public:

      void dump ()
      {
	using namespace std;
	cerr << "DUMP: " << endl;
	cerr << "|-- Increment:       " << __increment << endl;
	cerr << "|-- Max capacity:    " << __max_capacity << endl;
	cerr << "|-- Buffer size:     " << __buffer.size () << endl;
	cerr << "|-- Buffer capacity: " << __buffer.capacity () << endl;
	cerr << "|-- Buffer contains: `";
	for (int i = 0; i < __buffer.size (); i++)
	  {
	    cerr << __buffer[i];
	  }
	cerr << "'" << endl;
	cerr << "|-- pptr ():         " << hex << reinterpret_cast<void *> (this->pptr ()) << dec << endl;
	cerr << "|-- epptr ():        " << hex << reinterpret_cast<void *> (this->epptr ()) << dec << endl;
	cerr << "`-- " << "end" << endl;
      }

      // ctor:
      vector_output_buffer (size_t capacity_ = 1000,
			    size_t increment_ = 1000,
			    size_t max_capacity_ = 0) 
      {
	__max_capacity = __buffer.max_size ();
	_init (capacity_);
	_set_increment (increment_);
	set_max_capacity (max_capacity_);
	using namespace std;
	/*
	  cerr << "DEVEL: " << "vector_output_buffer::ctor: "
	  << " capacity = " << capacity_
	  << " increment = " << increment_
	  << " max capacity  = " << max_capacity_
	  << endl;
	  dump ();
	*/
	return;
      }
    
      // dtor:
      virtual ~vector_output_buffer ()
      {
      }

      /*
	int overflow(int c)
	{
	if (c != std::char_traits<char>::eof ())
	__buffer.push_back (c);
	return std::char_traits<char>::not_eof (c);
	}
      */

      int overflow (int c_)
      {
	using namespace std;
	/*
	  std::cerr << endl 
	  << "vector_output_buffer::overflow: Entering..." << std::endl;
	*/
	const char conv = std::char_traits<char>::to_char_type (c_);
	/*
	  std::cerr << "vector_output_buffer::overflow: conv = `" << conv << "'" << std::endl;
	  dump ();
	*/
	const bool __testeof = std::char_traits<char>::eq_int_type(c_, std::char_traits<char>::eof());
	if (__builtin_expect(__testeof, false))
	  {
	    //std::cerr << "vector_output_buffer::overflow: testeof..." << std::endl;
	    //std::cerr << "vector_output_buffer::overflow: Exiting." << std::endl;
	    return std::char_traits<char>::not_eof (c_);
	  }

	const size_t capacity = __buffer.capacity ();
	const size_t max_size = __buffer.max_size ();
	const size_t size     = __buffer.size ();
	/*
	  std::cerr << "vector_output_buffer::overflow: capacity=" << capacity << std::endl;
	  std::cerr << "vector_output_buffer::overflow: capacity=" << __max_capacity << std::endl;
	  std::cerr << "vector_output_buffer::overflow: max_size=" << max_size << std::endl;
	  std::cerr << "vector_output_buffer::overflow: size=" << size << std::endl;
	*/
	const bool testput = this->pptr () < this->epptr ();
	/*
	  std::cerr << "vector_output_buffer::overflow: pptr=" << std::hex << reinterpret_cast<void *> (this->pptr ()) << std::dec << std::endl;
	  std::cerr << "vector_output_buffer::overflow: epptr=" << std::hex <<  reinterpret_cast<void *> (this->epptr ()) << std::dec << std::endl;
	  std::cerr << "vector_output_buffer::overflow: testput=" << testput << std::endl;
	*/
	if (! testput)
	  { 
	    //std::cerr << "vector_output_buffer::overflow: case ! testput..." << std::endl;

	    if (__buffer.size () == __buffer.capacity ())
	      {
		//std::cerr << "vector_output_buffer::overflow: We need to extend the vector !" << std::endl;
		if (capacity == __max_capacity)
		  {
		    //std::cerr << "vector_output_buffer::overflow: Cannot extend the vector !" << std::endl;
		    return std::char_traits<char>::eof ();
		  }
		/*
		  const size_t opt_len = std::max (__buffer.capacity (), 
		  __buffer.size () + __increment);
		*/
		size_t opt_len = __buffer.capacity ();
		if (__increment > 0)
		  {
		    opt_len += __increment;
		  }
		else
		  {
		    opt_len *= 3;
		  }
		const size_t len = std::min (opt_len, __max_capacity);
		//std::cerr << "vector_output_buffer::overflow: new capacity =  '" << len << "'" << std::endl;
		__buffer.reserve (len);
		//dump ();
	      }
	    __buffer.push_back (conv);
	    char * base = const_cast<char*>(&__buffer[0]);
	    char * endp = base + __buffer.size ();
	    setp (base + __buffer.size (), endp); 
	    //dump ();
	  }
	else
	  {
	    //std::cerr << "vector_output_buffer::overflow: case testput..." << std::endl;
	    //*this->pptr () = conv;
	    __buffer.push_back (conv);
	    this->pbump (1);
	  }
	//dump ();
	//std::cerr << "vector_output_buffer::overflow: Exiting." << std::endl;
	return std::char_traits<char>::not_eof (c_);
      }

    };

    class ovectorstream :
      public vector_output_buffer,
      public std::ostream
    {
    public:

      ovectorstream (size_t capacity_ = DEFAULT_CAPACITY,
		     size_t increment_ = DEFAULT_INCREMENT,
		     size_t max_capacity_ = 0) :
	vector_output_buffer (capacity_, increment_, max_capacity_),
	std::ostream (this)
      {
	using namespace std;
	/*
	  cerr << "DEVEL: " << "ovectorstream::ctor: "
	  << " capacity = " << capacity_
	  << " increment = " << increment_
	  << " max capacity  = " << max_capacity_
	  << endl;
	*/
      }

    };


  } // end of namespace serialization
} // end of namespace datatools

// end of vector_streambuf.h
