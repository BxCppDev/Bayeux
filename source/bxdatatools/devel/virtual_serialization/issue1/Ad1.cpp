
#include "Ad1.hpp"

namespace A {

  d1::d1 () : c1 ()
  {
    m_j = 0;
    return;
  }

  d1::d1 (int32_t i, int32_t j) : c1 (i)
  {
    set_j (j);
    return;
  }

  void d1::set_j (int32_t j)
  {
    m_j = j;
    return;
  }

  uint32_t d1::get_j () const
  {
    return m_j;
  }

  void d1::print () const
  {
    std::cout << *this << std::endl;
    return;
  }

  const char * d1::get_key () const
  {
    return "A::d1";
  }

} // end of namespace A

std::ostream & operator<< (std::ostream & out, const A::d1 & d)
{
  out << "d1={i=" << d.get_i () << ",j=" << d.get_j () << "}";
  return out;
}
