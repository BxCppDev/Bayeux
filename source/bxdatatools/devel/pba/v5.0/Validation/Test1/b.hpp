#ifndef __b_hpp__
#define __b_hpp__ 1

#include <boost/cstdint.hpp>
#include <string>
#include <boost/serialization/string.hpp>
#include <datatools/serialization/i_serializable.h>

struct B
#ifdef B_CLASS_ALT
  : public datatools::serialization::i_serializable
#endif // B_CLASS_ALT
{
  std::string s;

  void reset ();

  void print () const;

  template<class Archive>
  void serialize (Archive & a, const unsigned int version);

#ifdef B_CLASS_ALT
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()
#endif // B_CLASS_ALT
};

#ifdef B_CLASS_ALT
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (B,"B")
#endif // B_CLASS_ALT

void B::reset ()
{
  s.clear ();
  return;
}

void B::print () const
{
  std::cout << "B={s='" << s << "'" 
           << '}';
  return;
}

#ifdef A_CLASS_ALT
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#endif

template<class Archive>
void B::serialize (Archive & ar_, const unsigned int version_)
{
#ifdef B_CLASS_ALT
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
#endif
  ar_ & boost::serialization::make_nvp ("s", s);
 return;
}

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(B, 3)
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(B, "B")
BOOST_CLASS_EXPORT_IMPLEMENT(B) 

#endif // __b_hpp__
