// boost::archive::quasi_portable_binary_iarchive_impl.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_IMPL_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_IMPL_HPP 1

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// quasi_portable_binary_iarchive_impl.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>
#include <string>
#include <stdexcept>

#include <boost/version.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/config.hpp>
#include <boost/serialization/pfto.hpp>
#include <boost/archive/basic_binary_iprimitive.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>

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
#include <boost/type_traits/is_unsigned.hpp>
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
    class quasi_portable_binary_iarchive_impl : 
      public basic_binary_iprimitive<Archive, Elem, Tr>,
      public basic_binary_iarchive<Archive>
    {
    private:
      unsigned int         m_portability;
      library_version_type m_qpba_version;
      
    public:
      typedef basic_binary_iprimitive<Archive, Elem, Tr> portable_iprimitive;

#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    public:
#else
      friend class detail::interface_iarchive<Archive>;
      friend class basic_binary_iarchive<Archive>;
      friend class load_access;
    protected:
#endif
      // note: the following should not needed - but one compiler (vc 7.1)
      // fails to compile one test (test_shared_ptr) without it !!!
      // make this protected so it can be called from a derived archive
      template<class T>
      void load_override (T & t, BOOST_PFTO int)
      {
        this->basic_binary_iarchive<Archive>::load_override(t, 0L);
	return;
      }

      void init (unsigned int flags)
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::init: Entering..." << std::endl;
	if (m_portability == (unsigned int) quasi_portable_binary_archive_common::strict_portability)
	  {
	    throw quasi_portable_binary_archive_exception ("quasi_portable_binary_iarchive_impl::init: strict portability mode is not implemented ! volunteers ?");
	  }
        if(0 != (flags & no_header))
	  {
	    return;
	  }

	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::init: Loading magic bytes..." << std::endl;
	// special signature for qpba (magic bytes):
	const std::string qpba_signature (BOOST_ARCHIVE_QPBA_SIGNATURE ());
	bool qpba_signature_is_valid = true;
	signed char check[4];
	for (int i = 0; i < (int) qpba_signature.length (); i++)
	  {
	    check[i] = this->load_signed_char ();
	    if (check[i] != qpba_signature[i])
	      {
		qpba_signature_is_valid = false;
		break;
	      }	    
	  }
	if (! qpba_signature_is_valid)
	  {
	    boost::serialization::throw_exception (
						   archive_exception (archive_exception::invalid_signature));
	  }

	// read boost archive library version :
	library_version_type input_archive_library_version;
	this->load (input_archive_library_version);
	if (BOOST_ARCHIVE_VERSION() < input_archive_library_version)
	  {
	    boost::serialization::throw_exception(
						  archive_exception(archive_exception::unsupported_version)
						  );
	  }
	this->set_library_version (input_archive_library_version);

	// read qpba library version :
	library_version_type input_qpba_version;
	this->load (input_qpba_version);
	if (BOOST_ARCHIVE_QPBA_VERSION() < input_qpba_version)
	  {
	    boost::serialization::throw_exception(
						  archive_exception(archive_exception::unsupported_version)
						  );
	  }
	this->set_qpba_version (input_qpba_version);

	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::init: Exiting." << std::endl;
	return;
      }

      // ctor :
      quasi_portable_binary_iarchive_impl(std::basic_streambuf<Elem, Tr> & bsb, 
					  unsigned int flags, 
					  unsigned int portability
					  ) :
        basic_binary_iprimitive<Archive, Elem, Tr>(bsb, 
						   0 != (flags & no_codecvt)
						   ),
        basic_binary_iarchive<Archive>(flags),
	m_portability (portability),
	m_qpba_version (BOOST_ARCHIVE_QPBA_VERSION())
      {
        init(flags);
	return;
      }

      // ctor :
      quasi_portable_binary_iarchive_impl(std::basic_istream<Elem, Tr> & is, 
					  unsigned int flags, 
					  unsigned int portability
					  ) :
        basic_binary_iprimitive<Archive, Elem, Tr>(* is.rdbuf (), 
						   0 != (flags & no_codecvt)
						   ),
        basic_binary_iarchive<Archive>(flags),
	m_portability (portability),
	m_qpba_version (BOOST_ARCHIVE_QPBA_VERSION())
      {
        init (flags);
	return;
      }

    private:

      // workaround for gcc: use a dummy struct
      // as additional argument type for overloading
      template <int> struct dummy { dummy(int) {}};

      // loads directly from stream
      signed char load_signed_char () 
      { 
	signed char c; 
	portable_iprimitive::load (c); 
	return c; 
      }

    public:

      void set_qpba_version (library_version_type qpba_version)
      {
        m_qpba_version = qpba_version;
	return;
      }
      
      library_version_type get_qpba_version () const
      {
	return m_qpba_version;
      }

      //! Default fall through for non-arithmetic types (ie. strings)
      template <class T>
      typename boost::disable_if<boost::is_arithmetic<T> >::type 
      load (T& t, dummy<1> = 0)
      {
	//std::cerr << "DEVEL: " << "eos::portable_iarchive::load<T>: " << "typeid=" << typeid(t).name () << std::endl;
	portable_iprimitive::load (t);
	return;
      }

      //! Special case loading bool type, preserving compatibility
      //! to integer types - this is somewhat redundant but simply
      //! treating bool as integer type generates lots of warnings
      void load (bool& b) 
      { 
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found bool !" << std::endl;
	switch (signed char c = load_signed_char ())
	  {
	  case 0: b = false; break;
	  case 1: b = load_signed_char(); break;
	  default: throw quasi_portable_binary_archive_exception (c);
	  }
	return;
      }

      /**
       * \brief Load integer types.
       *
       * First we  load the size information  ie. the number
       * of  bytes  that  hold  the  actual  data.  Then  we
       * retrieve the data and  transform it to the original
       * value by using load_little_endian.
       *
       */
      template <typename T>
      typename boost::enable_if<boost::is_integral<T> >::type
      load (T & t, dummy<2> = 0)
      {
	// get the number of bytes in the stream
	signed char size = load_signed_char();
	//if (signed char size = load_signed_char())
	if (size)
	  {
	    // check for negative value in unsigned type
	    if (size < 0 && boost::is_unsigned<T>::value)
	      throw quasi_portable_binary_archive_exception();

	    // check that our type T is large enough
	    else if ((unsigned) abs(size) > sizeof(T)) 
	      throw quasi_portable_binary_archive_exception(size);

	    // reconstruct the value
	    T temp = size < 0 ? -1 : 0;
	    load_binary(&temp, abs(size));

	    // load the value from little endian - is is then converted
	    // to the target type T and fits it because size <= sizeof(T)
	    t = boost::detail::load_little_endian<T, sizeof(T)>(&temp);
	  }

	else t = 0; // zero optimization
	return;
      }

      /** 
       * \brief Load floating point types.
       * 
       * We simply rely on  fp_traits to set the bit pattern
       * from the  (unsigned) integral type  that was stored
       * in the stream.
       *
       * \note by  Johan Rade (author of  the floating point
       * utilities    library):   Be    warned    that   the
       * math::detail::fp_traits<T>::type::get_bits()
       * function is  *not* guaranteed to give  you all bits
       * of the floating point  number. It will give you all
       * bits if and  only if there is an  integer type that
       * has  the same size  as the  floating point  you are
       * copying  from. It will  not give  you all  bits for
       * double if  there is no  uint64_t. It will  not give
       * you all bits for long double if sizeof(long double)
       * > 8 or there is no uint64_t.
       * 
       * The  member fp_traits<T>::type::coverage  will tell
       * you whether all bits  are copied. This is a typedef
       * for      either      math::detail::all_bits      or
       * math::detail::not_all_bits.
       * 
       * If  the function does  not copy  all bits,  then it
       * will  copy the  most  significant bits.  So if  you
       * serialize and deserialize the way you describe, and
       * fp_traits<T>::type::coverage                      is
       * math::detail::not_all_bits,   then   your  floating
       * point   numbers  will   be  truncated.   This  will
       * introduce small rounding off errors.
       *
       * \todo treat nan values using fpclassify
       */
      template <typename T>
      typename boost::enable_if<boost::is_floating_point<T> >::type
      load(T & t, dummy<3> = 0)
      {
	typedef typename fp::detail::fp_traits<T>::type traits;

	if (m_portability == quasi_portable_binary_archive_common::restricted_portability)
	  {
	    // if you end here there are three possibilities:
	    // 1. you're serializing a long double which is not portable
	    // 2. you're serializing a double but have no 64 bit integer
	    // 3. your machine is using an unknown floating point format
	    // after reading the note above you still might decide to 
	    // deactivate this static assert and try if it works out.
	    typename traits::bits bits;
	    BOOST_STATIC_ASSERT(sizeof(bits) == sizeof(T));
	    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_iec559);

	    load (bits);
	    traits::set_bits (t, bits);

	    // if you end here your floating point type does not support 
	    // denormalized numbers. this might be the case even though 
	    // your type conforms to IEC 559 (and thus to IEEE 754)
	    if (std::numeric_limits<T>::has_denorm == std::denorm_absent 
		&& t && !fp::isnormal(t)) throw quasi_portable_binary_archive_exception (t);
	  }
	else
	  {
	    typename traits::bits bits;
	    BOOST_STATIC_ASSERT(sizeof(bits) == sizeof(T));
	    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_iec559);
	    load (bits);

	    /*
	      typename traits::bits sign;
	      typename traits::bits mantissa;
	      typename traits::bits exponent;
	      if (bits == 0)// XXX
	      {
	      t = 0;
	      }
	      else if 
	      traits::set_bits(t, bits);
	      if (std::numeric_limits<T>::has_denorm == std::denorm_absent 
	      && t && !fp::isnormal(t)) 
	      {
	      //throw quasi_portable_archive_exception(t);
	      }
	    */
	    traits::set_bits(t, bits);
	  }
	return;
      }

      // Types in boost/archive/basic_archive.hpp must be 
      // loaded in a portable way :

      //! Saving library_version_type :
      void load (library_version_type & lv) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found library_version_type !" << std::endl;
	// 'boost::archive::library_version_type' has a value 
	// of type 'typedef uint_least16_t base_type'
	uint16_t library_version_value;
	this->load<uint16_t> (library_version_value);
	lv = (library_version_type) library_version_value;
	return;
      }

      //! Saving version_type :
      void load (version_type & v) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found version_type !" << std::endl;
	// 'boost::archive::version_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	uint32_t version_value;
	this->load<uint32_t> (version_value);
	v = (version_type) version_value;
	return;
      }

      //! Saving class_id_type :
      void load (class_id_type & cid) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found class_id_type !" << std::endl;
	// 'boost::archive::class_id_type' has a value 
	// of type 'typedef int_least16_t base_type'
	int16_t cid_value;
	this->load<int16_t> (cid_value);
	cid = (class_id_type) cid_value;
	return;
      }

      //! Saving object_id_type :
      void load (object_id_type & oid) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found object_id_type !" << std::endl;
	// 'boost::archive::object_id_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	uint32_t oid_value;
	this->load<uint32_t> (oid_value);
	oid = (object_id_type) oid_value;
	return;
      }

      //! Saving tracking_type :
      void load (tracking_type & t) 
      {
	//std::cerr << "DEVEL: quasi_portable_binary_iarchive_impl::load: Found tracking_type !" << std::endl;
	// 'boost::archive::tracking_type' has a value 
	// of type 'typedef uint_least32_t base_type'
	bool tracking_value;
	this->load (tracking_value);
	t = (tracking_type) tracking_value;
	return;
      }

      /*
      //! Saving class_name_type :
      void load (class_name_type & cn) 
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

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_IMPL_HPP

// end of boost::archive::quasi_portable_binary_iarchive_impl.hpp

