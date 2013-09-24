/* i_named.cc */
// Ourselves
#include <datatools/i_named.h>

namespace datatools {

// static
const std::string & i_named::constants::anonymous_label()
{
  static std::string lbl;
  if (lbl.empty()) {
    lbl = "__anonymous__";
  }
  return lbl;
}

bool i_named::has_a_name(const i_named& a_named) {
  if (const_cast<i_named&>(a_named).get_name().empty()) {
    return false;
  }

  if (const_cast<i_named&>(a_named).get_name()
      == i_named::constants::anonymous_label()) {
    return false;
  }
  return true;
}

bool i_named::is_anonymous(const i_named& a_named) {
  return const_cast<i_named&>(a_named).get_name()
    == i_named::constants::anonymous_label();
}

} // end of namespace datatools

