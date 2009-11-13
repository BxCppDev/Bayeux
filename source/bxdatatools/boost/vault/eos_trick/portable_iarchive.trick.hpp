/*****************************************************************************/
/**
 * \file portable_iarchive.hpp
 * \brief Provides an archive to read from portable binary files.
 * \author christian.pfligersdorffer@eos.info
 * \version 3.1
 *
 * This archive (pair) brings the advantanges of binary streams to the cross
 * platform boost::serialization user. While being almost as fast as the native
 * binary archive it allows its files to be exchanged between cpu architectures
 * using different byte order (endianness). Speaking of speed: in serializing
 * numbers the (portable) binary approach is approximately ten times faster than
 * the ascii implementation that is inherently portable!
 *
 * Based on the portable archive example by Robert Ramey this implementation
 * uses Beman Dawes endian library and fp_utilities from Johan Rade, both being
 * in boost since 1.36. Prior to that you need to add them both (header only)
 * to your boost directory before you're able to use the archives provided. 
 * Our archives have been tested successfully for boost versions 1.33 to 1.38!
 *
 * \note Correct behaviour has so far been confirmed using PowerPC-32, x86-32
 *       and x86-64 platforms featuring different byte order. So there is a good
 *       chance it will instantly work for your specific setup. If you encounter
 *       problems or have suggestions please contact the author.
 *
 * \note A few fixes introduced in version 3.1 let the archives pass all of the
 *       serialization tests. Thanks to Sergey Morozov for running the tests.
 *       Wouter Bijlsma pointed out where to find the fp_utilities and endian
 *       libraries headers inside the boost distribution. I would never have
 *       found them so thank him it works now out of the box since boost 1.36.
 *
 * \note With Version 3.0 the archives have been made portable across different
 *       boost versions. For that purpose a header is added to the data that
 *       supplies the underlying serialization library version. Backwards
 *       compatibility is maintained by assuming library version boost-1.33 if
 *       the iarchive is created using the no_header flag. Whether a header is
 *       present or not can be guessed by peeking into the stream: the header's
 *       first byte is the magic number 127 coinciding with 'e' | 'o' | 's' :-)
 *
 * \note Version 2.1 removes several compiler warnings and enhances floating
 *       point diagnostics to inform the user if some preconditions are violated
 *		 on his platform. We do not strive for the universally portable solution
 *       in binary floating point serialization as desired by some boost users.
 *       Instead we support only the most widely used IEEE 754 format and try to
 *       detect when requirements are not met and hence our approach must fail.
 *       Contributions we made by Johan Rade and �kos Mar�y.
 *
 * \note Version 1.0 contained a serious bug that effectively transformed most
 *       of negative integral values into positive values! For example the two
 *       numbers -12 and 234 were stored in the same 8-bit pattern and later
 *       always restored to 234. This was fixed in this version in a way that
 *       does not change the interpretation of existing archives that did work
 *       because there were no negative numbers. The other way round archives
 *       created by version 2.x and containing negative numbers will raise an
 *       integer type size exception when reading it with version 1.0. Thanks to
 *       Markus Frohnmaier for testing the archives and finding the bug.
 *
 * \copyright The boost software license applies.
 */
/*****************************************************************************/

#pragma once

#include <istream>

// basic headers
#include <boost/version.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/archive/binary_iarchive.hpp>

// funny polymorphics
#if BOOST_VERSION < 103500
#include <boost/archive/detail/polymorphic_iarchive_impl.hpp>
#define POLYMORPHIC(T) boost::archive::detail::polymorphic_iarchive_impl<T>

#elif BOOST_VERSION < 103600
#include <boost/archive/detail/polymorphic_iarchive_dispatch.hpp>
#define POLYMORPHIC(T) boost::archive::detail::polymorphic_iarchive_dispatch<T>

#else
#include <boost/archive/detail/polymorphic_iarchive_route.hpp>
#define POLYMORPHIC(T) boost::archive::detail::polymorphic_iarchive_route<T>
#endif

// endian and fpclassify
#if BOOST_VERSION < 103600
#include <boost/integer/endian.hpp>
#include <boost/math/fpclassify.hpp>
#else
#include <boost/spirit/home/support/detail/integer/endian.hpp>
#include <boost/spirit/home/support/detail/math/fpclassify.hpp>
#endif

// namespace alias
#if BOOST_VERSION < 103800
namespace fp = boost::math;
#else
namespace fp = boost::spirit::math;
#endif

// generic type traits for numeric types
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include "portable_archive_exception.hpp"

// hint from Johan Rade: on VMS there is still support for
// the VAX floating point format and this macro detects it
#if defined(__vms) && defined(__DECCXX) && !__IEEE_FLOAT
#error "VAX floating point format is not supported!"
#endif

namespace eos {

	// forward declaration
	class portable_iarchive;

	// convenience archive base class typedef
	typedef boost::archive::binary_iarchive_impl<
		portable_iarchive
	#if BOOST_VERSION >= 103400
		, std::istream::char_type 
		, std::istream::traits_type
	#endif
	> portable_iarchive_base;

	/**
	 * \brief Portable binary input archive using little endian format.
	 *
	 * This archive addresses integer size, endianness and floating point types so
	 * that data can be transferred across different systems. There may still be
	 * constraints as to what systems are compatible and the user will have to take
	 * care that e.g. a very large int being saved on a 64 bit machine will result
	 * in a portable_archive_exception if loaded into an int on a 32 bit system.
	 * A possible workaround to this would be to use fixed types like
	 * boost::uint64_t in your serialization structures.
	 *
	 * \note The class is based on the portable binary example by Robert Ramey and
	 *	     uses Beman Dawes endian library plus fp_utilities by Johan Rade.
	 *
	 * \todo Robert Ramey changed his example in boost 1.36 and no longer inherits
	 *       from binary_iarchive_impl but goes down to basic_binary_iprimitive!?
	 */
	class portable_iarchive : public portable_iarchive_base
	#if BOOST_VERSION >= 103500
		// mix-in helper class for serializing shared_ptr
		, public boost::archive::detail::shared_ptr_helper
	#endif
	{
		// workaround for gcc: use a dummy struct
		// as additional argument type for overloading
		template <int> struct dummy { dummy(int) {}};

		// loads directly from stream
		signed char load_signed_char() 
		{ 
			signed char c; 
			portable_iarchive_base::load(c); 
			return c; 
		}

	public:
		//! Default fall through for non-arithmetic types (ie. strings)
		template <class T>
		typename boost::disable_if<boost::is_arithmetic<T> >::type 
		load(T& t, dummy<1> = 0)
		{
			portable_iarchive_base::load(t);
		}

		//! Special case loading bool type, preserving compatibility
		//! to integer types - this is somewhat redundant but simply
		//! treating bool as integer type generates lots of warnings
		void load(bool& b) 
		{ 
			switch (signed char c = load_signed_char())
			{
			case 0: b = false; break;
			case 1: b = load_signed_char(); break;
			default: throw portable_archive_exception(c);
			}
		}

		/**
		 * \brief Load integer types.
		 *
		 * First we load the size information ie. the number of bytes that 
		 * hold the actual data. Then we retrieve the data and transform it
		 * to the original value by using load_little_endian.
		 */
		template <typename T>
		typename boost::enable_if<boost::is_integral<T> >::type
		load(T & t, dummy<2> = 0)
		{
			// get the number of bytes in the stream
			if (signed char size = load_signed_char())
			{
				// check for negative value in unsigned type
				if (size < 0 && boost::is_unsigned<T>::value)
					throw portable_archive_exception();

				// check that our type T is large enough
				else if ((unsigned) abs(size) > sizeof(T)) 
					throw portable_archive_exception(size);

				// reconstruct the value
				T temp = size < 0 ? -1 : 0;
				load_binary(&temp, abs(size));

				// load the value from little endian - is is then converted
				// to the target type T and fits it because size <= sizeof(T)
				t = boost::detail::load_little_endian<T, sizeof(T)>(&temp);
			}

			else t = 0; // zero optimization
		}

		/** 
		 * \brief Load floating point types.
		 * 
		 * We simply rely on fp_traits to set the bit pattern from the
		 * (unsigned) integral type that was stored in the stream.
		 *
		 * \note by Johan Rade (author of the floating point utilities library):
		 * Be warned that the math::detail::fp_traits<T>::type::get_bits() function 
		 * is *not* guaranteed to give you all bits of the floating point number. It
		 * will give you all bits if and only if there is an integer type that has
		 * the same size as the floating point you are copying from. It will not
		 * give you all bits for double if there is no uint64_t. It will not give
		 * you all bits for long double if sizeof(long double) > 8 or there is no
		 * uint64_t. 
		 * 
		 * The member fp_traits<T>::type::coverage will tell you whether all bits
		 * are copied. This is a typedef for either math::detail::all_bits or
		 * math::detail::not_all_bits. 
		 * 
		 * If the function does not copy all bits, then it will copy the most
		 * significant bits. So if you serialize and deserialize the way you
		 * describe, and fp_traits<T>::type::coverage is math::detail::not_all_bits,
		 * then your floating point numbers will be truncated. This will introduce
		 * small rounding off errors. 
		 *
		 * \todo treat nan values using fpclassify
		 */
		template <typename T>
		typename boost::enable_if<boost::is_floating_point<T> >::type
		load(T & t, dummy<3> = 0)
		{
			typedef typename fp::detail::fp_traits<T>::type traits;

			// if you end here there are three possibilities:
			// 1. you're serializing a long double which is not portable
			// 2. you're serializing a double but have no 64 bit integer
			// 3. your machine is using an unknown floating point format
			// after reading the note above you still might decide to 
			// deactivate this static assert and try if it works out.
			typename traits::bits bits;
			BOOST_STATIC_ASSERT(sizeof(bits) == sizeof(T));
			BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_iec559);

			load(bits);
			traits::set_bits(t, bits);

			// if you end here your floating point type does not support 
			// denormalized numbers. this might be the case even though 
			// your type conforms to IEC 559 (and thus to IEEE 754)
			if (std::numeric_limits<T>::has_denorm == std::denorm_absent 
				&& t && !fp::isnormal(t)) throw portable_archive_exception(t);
		}

		//! always construct on a stream using ios::binary mode!
		portable_iarchive(std::istream& is, unsigned flags = 0)

			// the base class constructor calls basic_binary_iarchive::init
			// but also basic_binary_iprimitive::init which loads type sizes
			// and throws an exception when they are different - no way!
			: portable_iarchive_base(is, flags | boost::archive::no_header) 
		{
			using namespace boost::archive;

			// it is vital to have version information!
			// if we don't have any we assume boost 1.33
			if (flags & no_header) set_library_version(3);

			// extract and check the magic eos byte
			else if (load_signed_char() != magic_byte) 
				throw archive_exception(archive_exception::invalid_signature);

			else
			{
				// extract version information
				version_type input_library_version;
				operator>>(input_library_version);

				// throw if file version is newer than we are
				if (input_library_version > archive_version)
					throw archive_exception(archive_exception::unsupported_version);

				// else set the library version accordingly
				else set_library_version(input_library_version);
			}
		}
	};

	// polymorphic portable binary iarchive typdef
	typedef POLYMORPHIC(portable_iarchive) polymorphic_portable_iarchive;
	#undef POLYMORPHIC

} // namespace eos

// this is required by export which registers all of your
// classes with all the inbuilt archives plus our archive.
#if BOOST_VERSION < 103500
#define BOOST_ARCHIVE_CUSTOM_IARCHIVE_TYPES eos::portable_iarchive
#else
BOOST_SERIALIZATION_REGISTER_ARCHIVE(eos::portable_iarchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(eos::polymorphic_portable_iarchive)
#endif

// if you include this header multiple times and your compiler is picky
// about multiple template instantiations (eg. gcc is) then you need to
// define NO_EXPLICIT_TEMPLATE_INSTANTIATION before every include but one
// or you move the instantiation section into an implementation file
#ifndef NO_EXPLICIT_TEMPLATE_INSTANTIATION

#include <boost/archive/impl/basic_binary_iarchive.ipp>
#if BOOST_VERSION < 104000
#include <boost/archive/impl/archive_pointer_iserializer.ipp>
#else 
/*  
 * I use this #ifndef/#define/#endif block because otherwise I get some
 * error at compile time with my gcc "(GCC) 4.2.4 (Ubuntu 4.2.4-1ubuntu4)"
 *
 * >>>
 * /opt/boost/install-1_40_0-Linux-i686-gcc42/include/boost/archive/impl/archive_serializer_map.ipp:26:
 * erreur: redefinition of "class boost::archive::detail::<unnamed>::map<Archive>
 *
 * /opt/boost/install-1_40_0-Linux-i686-gcc42/include/boost/archive/impl/archive_serializer_map.ipp:27: erreur: previous definition of "class boost::archive::detail::<unnamed>::map<Archive>"
 *
 * /opt/boost/install-1_40_0-Linux-i686-gcc42/include/boost/archive/impl/archive_serializer_map.ipp:32:erreur: redefinition of "static void
 * boost::archive::detail::archive_serializer_map<Archive>::insert(const
 * boost::archive::detail::basic_serializer*)"
 * ...
 * <<<
 */
#ifndef __eos_archive_serializer_map
#define __eos_archive_serializer_map
#include <boost/archive/impl/archive_serializer_map.ipp>
#endif
#endif 
#include <boost/archive/impl/basic_binary_iprimitive.ipp>

namespace boost { namespace archive {

	// explicitly instantiate for this type of binary stream
	template class basic_binary_iarchive<eos::portable_iarchive>;

	template class basic_binary_iprimitive<
		eos::portable_iarchive
	#if BOOST_VERSION < 103400
		, std::istream
	#else
		, std::istream::char_type 
		, std::istream::traits_type
	#endif
	>;

	template class binary_iarchive_impl<
		eos::portable_iarchive
	#if BOOST_VERSION >= 103400
		, std::istream::char_type 
		, std::istream::traits_type
	#endif
	>;

#if BOOST_VERSION < 104000
	template class detail::archive_pointer_iserializer<eos::portable_iarchive>;
#else 
	template class detail::archive_serializer_map<eos::portable_iarchive>;
#endif 

} } // namespace boost::archive

#endif
