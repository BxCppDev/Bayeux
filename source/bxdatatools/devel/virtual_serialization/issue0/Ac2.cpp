
#include "Ac2.hpp"

namespace A {

  c2::c2 () : base ()
  {
    m_name = "<anonymous>";
    return;
  }

  c2::c2 (const std::string & name) : base ()
  {
    set_name (name);
    return;
  }

  void c2::set_name (const std::string & name)
  {
    m_name = name;
    return;
  }

  const std::string & c2::get_name () const
  {
    return m_name;
  }

  void c2::print () const
  {
    std::cout << *this << std::endl;
    return;
  }

  const char * c2::get_key () const
  {
    return "A::c2";
  }

  template<class Archive>
  void c2::serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
    ar & boost::serialization::make_nvp("name", m_name);
    return;
  }

} // end of namespace A

std::ostream & operator<< (std::ostream & out, const A::c2 & c)
{
  out << "c2={name='" << c.get_name () << "'}";
  return out;
}

#ifndef INCLUDE_ARCHIVES_BASE
#include "archives.hpp"
#endif

BOOST_CLASS_EXPORT_IMPLEMENT (A::c2)
template void A::c2::serialize (boost::archive::text_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::text_oarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::xml_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::xml_oarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::binary_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::binary_oarchive & ar, const unsigned int file_version);

