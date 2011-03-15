
#include "Ac1.hpp"

namespace A {

  c1::c1 () : base ()
  {
    m_i = 0;
    return;
  }

  c1::c1 (int32_t i) : base ()
  {
    set_i (i);
    return;
  }

  void c1::set_i (int32_t i)
  {
    m_i = i;
    return;
  }

  uint32_t c1::get_i () const
  {
    return m_i;
  }

  void c1::print () const
  {
    std::cout << *this << std::endl;
    return;
  }

  const char * c1::get_key () const
  {
    return "A::c1";
  }

} // end of namespace A

std::ostream & operator<< (std::ostream & out, const A::c1 & c)
{
  out << "c1={i=" << c.get_i () << "}";
  return out;
}

#ifndef HERE
#include "archives.hpp"
#endif

BOOST_CLASS_EXPORT_IMPLEMENT (A::c1)
