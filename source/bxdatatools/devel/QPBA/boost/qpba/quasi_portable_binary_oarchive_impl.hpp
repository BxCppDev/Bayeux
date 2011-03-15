// boost::archive::quasi_portable_binary_oarchive_impl.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_IMPL_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_IMPL_HPP 1

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// quasi_portable_binary_oarchive_impl.hpp

// (C) Copyright 2011 Francois Mauger - <mauger@lpccaen.in2p3.fr>. 

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <ostream>
#include <string>
#include <stdexcept>

#include <boost/version.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/config.hpp>
#include <boost/serialization/pfto.hpp>
#include <boost/archive/basic_binary_oprimitive.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>

// endian and fpclassify
#if BOOST_VERSION < 103600
#include <boost/integer/endian.hpp>
#include <boost/math/fpclassify.hpp>
#else
#include <boost/spirit/home/support/detail/integer/endian.hpp>
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
#endif

// generic type traits for numeric types
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include <boost/qpba/quasi_portable_binary_archive_exception.hpp>
#include <boost/qpba/quasi_portable_binary_archive_common.hpp>

// namespace alias
#if BOOST_VERSION < 103800
namespace fp = boost::math;
#else
namespace fp = boost::spirit::math;
#endif

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

// hint from Johan Rade: on VMS there is still support for
// the VAX floating point format and this macro detects it
#if defined(__vms) && defined(__DECCXX) && !__IEEE_FLOAT
#error "VAX floating point format is not supported!"
#endif

namespace boost { 
  namespace archive {


    template<class Archive, class Elem, class Tr>
    class quasi_portable_binary_oarchive_impl : 
      public boost::archive::basic_binary_oprimitive<Archive, Elem, Tr>,
      public boost::archive::basic_binary_oarchive<Archive>
    {
    private:
      unsigned int m_portability;

    public:
      typedef boost::archive::basic_binary_oprimitive<Archive, Elem, Tr> portable_oprimitive;

#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    public:
#else
      friend class boost::archive::detail::interface_oarchive<Archive>;
      friend class boost::archive::basic_binary_oarchive<Archive>;
      friend class boost::archive::save_access;
    protected:
#endif
      // note: the following should not needed - but one compiler (vc 7.1)
      // fails to compile one test (test_shared_ptr) without it !!!
      // make this protected so it can be called from a derived archive
      template<class T>
      void save_override(T & t, BOOST_PFTO int)
      {
        this->boost::archive::basic_binary_oarchive<Archive>::save_override(t, 0L);
	return;
      }

      void init (unsigned int flags) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::init: Entering..." << std::endl;
	if (m_portability == (unsigned int) quasi_portable_binary_archive_common::strict_portability)
	  {
	    throw quasi_portable_binary_archive_exception ("quasi_portable_binary_oarchive_impl::init: strict portability mode is not implemented ! volunteers ?");
	  }
	
        if (0 != (flags & boost::archive::no_header))
	  {
	    return;
	  }

	// Check:
	//bool bbb= true;
	//this->save (bbb);

	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::init: Storing magic bytes..." << std::endl;
	// special signature for qpba (magic bytes):
	const std::string qpba_signature(BOOST_ARCHIVE_QPBA_SIGNATURE());

	this->save_signed_char (qpba_signature[0]);
	this->save_signed_char (qpba_signature[1]);
	this->save_signed_char (qpba_signature[2]);
	this->save_signed_char (qpba_signature[3]);

	// write boost archive library version :
	const library_version_type archive_library_version(BOOST_ARCHIVE_VERSION());
	this->save (archive_library_version);

	// write qpba library version :
	const library_version_type qpba_library_version(BOOST_ARCHIVE_QPBA_VERSION());
	this->save (qpba_library_version);

	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::init: Exiting." << std::endl;
	return;
      }

      // ctor :
      quasi_portable_binary_oarchive_impl (
					   std::basic_streambuf<Elem, Tr> & bsb, 
					   unsigned int flags, 
					   unsigned int portability
					   ) :
        boost::archive::basic_binary_oprimitive<Archive, Elem, Tr> (
								    bsb, 
								    0 != (flags & boost::archive::no_codecvt)
								    ),
        boost::archive::basic_binary_oarchive<Archive> (flags),
	m_portability (portability)
      {
        init (flags);
	return;
      }

      // ctor :
      quasi_portable_binary_oarchive_impl (
					   std::basic_ostream<Elem, Tr> & os, 
					   unsigned int flags, 
					   unsigned int portability
					   ) :
        boost::archive::basic_binary_oprimitive<Archive, Elem, Tr> (
								    * os.rdbuf (), 
								    0 != (flags & boost::archive::no_codecvt)
								    ),
        boost::archive::basic_binary_oarchive<Archive> (flags),
	m_portability (portability)
      {
        init (flags);
	return;
      }

    private:

      // workaround for gcc: use a dummy struct
      // as additional argument type for overloading
      template<int> struct dummy { dummy(int) {}};

      // stores a signed char directly to stream
      void save_signed_char (const signed char& c) 
      { 
	portable_oprimitive::save (c); 
	return;
      }
  
    public:

      //! Default fall through for non-arithmetic types (ie. strings)
      template<class T>
      typename boost::disable_if<boost::is_arithmetic<T> >::type 
      save (const T & t, dummy<1> = 0)
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save<>: Found Default !" << std::endl;
	
     	portable_oprimitive::save (t);
     	return;
      }

      //! Saving bool directly
      void save (const bool& b) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found bool !" << std::endl;
	save_signed_char(b);
	if (b) save_signed_char('T');
	return;
      }

      /**
       * \brief Save integer types.
       *
       * First we save the size information ie. the number of bytes that
       * hold the actual data.  We subsequently transform the data using
       * store_little_endian and store non-zero bytes to the stream.
       *
       */
      template <typename T>
      typename boost::enable_if<boost::is_integral<T> >::type
      save (const T & t, dummy<2> = 0)
      {
	//std::cerr << "DEVEL: " << "eos::portable_oarchive::save<integral>: " << "typeid=" << typeid(t).name () << " value=" << t << std::endl;
	if (T temp = t)
	  {
	    // examine the number of bytes
	    // needed to represent the number
	    signed char size = 0;
	    do { temp >>= CHAR_BIT; ++size; } 
	    while (temp != 0 && temp != (T) -1);

	    // encode the sign bit into the size
	    save_signed_char (t > 0 ? size : -size);
	    BOOST_ASSERT(t > 0 || boost::is_signed<T>::value);

	    // we choose to use little endian because this way we just
	    // save the first size bytes to the stream and skip the rest
	    boost::detail::store_little_endian<T, sizeof (T)> (&temp, t);
	    save_binary (&temp, size);
	  }
	// zero optimization
	else save_signed_char (0);
	return;
      }

      /**
       * \brief Save floating point types.
       * 
       * We simply rely on fp_traits  to extract the bit pattern into an
       * (unsigned) integral type and store that into the stream.
       *
       * \note by Johan Rade (author of the floating point utilities library):
       *
       * Be warned that the math::detail::fp_traits<T>::type::get_bits()
       * function  is *not*  guaranteed  to  give you  all  bits of  the
       * floating point number. It will give you all bits if and only if
       * there is an integer type that has the same size as the floating
       * point you are  copying from. It will not give  you all bits for
       * double if there  is no uint64_t. It will not  give you all bits
       * for  long double  if sizeof(long  double) >  8 or  there  is no
       * uint64_t.
       * 
       * The member  fp_traits<T>::type::coverage will tell  you whether
       * all   bits  are   copied.  This   is  a   typedef   for  either
       * math::detail::all_bits or math::detail::not_all_bits.
       * 
       * If the function  does not copy all bits, then  it will copy the
       * most significant bits. So  if you serialize and deserialize the
       * way   you   describe,   and   fp_traits<T>::type::coverage   is
       * math::detail::not_all_bits,  then your  floating  point numbers
       * will  be  truncated. This  will  introduce  small rounding  off
       * errors.
       *
       * \todo treat nan values using fpclassify
       */
      template <typename T>
      typename boost::enable_if<boost::is_floating_point<T> >::type
      save (const T & t, dummy<3> = 0)
      {
	typedef typename fp::detail::fp_traits<T>::type traits;

	if (m_portability == quasi_portable_binary_archive_common::restricted_portability)
	  {
	    // it is not supported to serialize infinity or not-a-number
	    if (! fp::isfinite (t)) throw quasi_portable_binary_archive_exception(t);
			    
	    // if you end here there are three possibilities:
	    // 1. you're serializing a long double which is not portable
	    // 2. you're serializing a double but have no 64 bit integer
	    // 3. your machine is using an unknown floating point format
	    // after reading the note above you still might decide to 
	    // deactivate this static assert and try if it works out.
	    typename traits::bits bits;
	    BOOST_STATIC_ASSERT(sizeof (bits) == sizeof (T));
	    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_iec559);

	    traits::get_bits(t, bits);
	    save(bits);
	  }
	else
	  {
	    typename traits::bits bits;
	    BOOST_STATIC_ASSERT(sizeof (bits) == sizeof (T));
	    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_iec559);
	    int fpc = fp::fpclassify (t);
	    if (fpc == FP_ZERO)
	      {
		bits = 0;
	      }
	    if (fpc == FP_NORMAL || fpc == FP_SUBNORMAL)
	      {
		traits::get_bits(t, bits);
	      }
	    else if (fpc == FP_NAN)
	      {
		bits = traits::exponent | traits::mantissa;
	      }
	    else if (fpc == FP_INFINITE)
	      {
		if (t < 0.0)
		  {
		    bits = traits::sign | traits::exponent;
		  }
		else
		  {
		    bits = traits::exponent;
		  }
	      }
	    save(bits);
	  }
	return;
      }

      // Types in boost/archive/basic_archive.hpp must be 
      // stored in a portable way :

      //! Saving library_version_type :
      void save (const library_version_type & lv) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found library_version_type !" << std::endl;
	// 'boost::archive::library_version_type' has a value 
	// of type 'typedef uint_least16_t base_type'
	uint16_t library_version_value = lv;
	this->save<uint16_t> (library_version_value);
	return;
      }

      //! Saving version_type :
      void save (const version_type & v) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found version_type !" << std::endl;
	// 'boost::archive::version_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	uint32_t version_value = v;
	this->save<uint32_t> (version_value);
	return;
      }

      //! Saving class_id_type :
      void save (const class_id_type & cid) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found class_id_type !" << std::endl;
	// 'boost::archive::class_id_type' has a value 
	// of type 'typedef int_least16_t base_type'
	int16_t cid_value = cid;
	this->save<int16_t> (cid_value);
	return;
      }

      //! Saving object_id_type :
      void save (const object_id_type & oid) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found object_id_type !" << std::endl;
	// 'boost::archive::object_id_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	uint32_t oid_value = oid;
	this->save<uint32_t> (oid_value);
	return;
      }

      //! Saving tracking_type :
      void save (const tracking_type & t) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_oarchive_impl::save: Found tracking_type !" << std::endl;
	// 'boost::archive::tracking_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	bool tracking_value = t;
	this->save (tracking_value);
	return;
      }

      /*
      //! Saving class_name_type :
      void save (const class_name_type & cn) 
      {
      std::string class_name = cn;
      this->save<std::string> (class_name);
      return;
      }
      */

    };

  } // namespace archive
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_IMPL_HPP

// end of boost::archive::quasi_portable_binary_oarchive_impl.hpp
