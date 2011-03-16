// boost::archive::quasi_portable_binary_iarchive.cpp

// (C) Copyright 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>. 
//                    Christian Pfligersdorffer <christian.pfligersdorffer@eos.info>. 
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <istream>

#define BOOST_ARCHIVE_SOURCE
#include <boost/qpba/quasi_portable_binary_iarchive.hpp>
#include <boost/archive/detail/archive_serializer_map.hpp>

#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_binary_iprimitive.ipp>
#include <boost/archive/impl/basic_binary_iarchive.ipp>

namespace boost {
namespace archive {

// explicitly instantiate for this type of stream
template class detail::archive_serializer_map<boost::archive::naked_quasi_portable_binary_iarchive>;
template class basic_binary_iprimitive<
    boost::archive::naked_quasi_portable_binary_iarchive,
    std::istream::char_type, 
    std::istream::traits_type
>;
template class basic_binary_iarchive<boost::archive::naked_quasi_portable_binary_iarchive> ;
template class quasi_portable_binary_iarchive_impl<
    boost::archive::naked_quasi_portable_binary_iarchive, 
    std::istream::char_type, 
    std::istream::traits_type
>;

// explicitly instantiate for this type of stream
template class detail::archive_serializer_map<boost::archive::quasi_portable_binary_iarchive>;
template class basic_binary_iprimitive<
    boost::archive::quasi_portable_binary_iarchive,
    std::istream::char_type, 
    std::istream::traits_type
>;
template class basic_binary_iarchive<boost::archive::quasi_portable_binary_iarchive> ;
template class boost::archive::quasi_portable_binary_iarchive_impl<
    boost::archive::quasi_portable_binary_iarchive, 
    std::istream::char_type, 
    std::istream::traits_type
>;

} // namespace archive
} // namespace boost

// end of boost::archive::quasi_portable_binary_iarchive.cpp
