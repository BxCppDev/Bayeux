
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

} // end of namespace A

std::ostream & operator<< (std::ostream & out, const A::c2 & c)
{
  out << "c2={name='" << c.get_name () << "'}";
  return out;
}

#ifndef HERE
#include "archives.hpp"
#endif

BOOST_CLASS_EXPORT_IMPLEMENT (A::c2)
