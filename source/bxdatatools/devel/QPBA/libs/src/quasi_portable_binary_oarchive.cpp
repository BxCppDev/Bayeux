// boost::archive::quasi_portable_binary_oarchive.cpp

#include <ostream>

#define BOOST_ARCHIVE_SOURCE
#include <boost/qpba/quasi_portable_binary_oarchive.hpp>
#include <boost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of binary stream
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_binary_oprimitive.ipp>
#include <boost/archive/impl/basic_binary_oarchive.ipp>

namespace boost {
namespace archive {

  template class detail::archive_serializer_map<boost::archive::quasi_portable_binary_oarchive>;
  template class boost::archive::basic_binary_oprimitive<
    boost::archive::quasi_portable_binary_oarchive, 
    std::ostream::char_type, 
    std::ostream::traits_type
>;
  template class basic_binary_oarchive<boost::archive::quasi_portable_binary_oarchive> ;
template class boost::archive::quasi_portable_binary_oarchive_impl<
    boost::archive::quasi_portable_binary_oarchive, 
    std::ostream::char_type, 
    std::ostream::traits_type
>;

} // namespace archive
} // namespace boost

// end of boost::archive::quasi_portable_binary_oarchive.cpp
