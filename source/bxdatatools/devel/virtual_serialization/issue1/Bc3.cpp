
#include "Bc3.hpp"

namespace B {

  c3::c3 () : A::base ()
  {
    m_c = 0;
    return;
  }

  c3::c3 (int8_t c) : A::base ()
  {
    set_c (c);
    return;
  }

  void c3::set_c (int8_t c)
  {
    m_c = c;
    return;
  }

  uint8_t c3::get_c () const
  {
    return m_c;
  }

  const char * c3::get_key () const
  {
    return "B::c3";
  }

  void c3::print () const
  {
    std::cout << *this << std::endl;
    return;
  }

} // end of namespace B

std::ostream & operator<< (std::ostream & out, const B::c3 & c)
{
  out << "c3={c=" << (int) c.get_c () << "}";
  return out;
}
