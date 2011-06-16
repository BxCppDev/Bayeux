
#include "Bdd1.hpp"

namespace B {

  dd1::dd1 () : A::c1 ()
  {
    m_j = 0;
    return;
  }

  dd1::dd1 (int32_t i, int32_t j) : A::c1 (i)
  {
    set_j (j);
    return;
  }

  void dd1::set_j (int32_t j)
  {
    m_j = j;
    return;
  }

  uint32_t dd1::get_j () const
  {
    return m_j;
  }

  void dd1::print () const
  {
    std::cout << *this << std::endl;
    return;
  }

  const char * dd1::get_key () const
  {
    return "B::dd1";
  }

} // end of namespace B

std::ostream & operator<< (std::ostream & out, const B::dd1 & d)
{
  out << d.get_key () << "={i=" << d.get_i () << ",j=" << d.get_j () << "}";
  return out;
}
