#ifndef __a_hpp__
#define __a_hpp__ 1

#include <boost/cstdint.hpp>
#include <string>
#include <boost/serialization/string.hpp>
#include <datatools/serialization/i_serializable.h>

struct A
#ifdef A_CLASS_ALT
  : public datatools::serialization::i_serializable
#endif // A_CLASS_ALT
{
  uint32_t u;
  float    f;
  double   d;
  std::string s;
#ifdef A_WITH_WSTRING
  std::wstring w;
#endif // A_WITH_WSTRING

  void reset ();

  void print () const;

  template<class Archive>
  void serialize (Archive & a, const unsigned int version);

#ifdef A_CLASS_ALT
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()
#endif // A_CLASS_ALT
};

#ifdef A_CLASS_ALT
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (A,"A")
#endif // A_CLASS_ALT

void A::reset ()
{
  u = 0;
  f = 0.0f;
  d = 0.0;
  s.clear ();
#ifdef A_WITH_WSTRING
  w.clear ();
#endif // A_WITH_WSTRING
  return;
}

void A::print () const
{
  std::cout << "A={u=" << u  << ",f=" << f << ",d=" << d 
       << ",s='" << s << "'" 
#ifdef A_WITH_WSTRING
            << ",w='";
  std::wcout << w;
  std::cout << "'" 
#endif // A_WITH_WSTRING
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
void A::serialize (Archive & ar_, const unsigned int version_)
{
#ifdef A_CLASS_ALT
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
#endif
  ar_ & boost::serialization::make_nvp ("u", u);
  ar_ & boost::serialization::make_nvp ("f", f);
  ar_ & boost::serialization::make_nvp ("d", d);
  ar_ & boost::serialization::make_nvp ("s", s);
#ifdef A_WITH_WSTRING
  ar_ & boost::serialization::make_nvp ("w", w);
#endif // A_WITH_WSTRING
 return;
}

// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(A, 0)
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(A, "A")
BOOST_CLASS_EXPORT_IMPLEMENT(A) 

#endif // __a_hpp__
