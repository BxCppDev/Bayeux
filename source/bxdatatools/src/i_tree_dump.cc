/* i_tree_dump.cc */
// Ourselves
#include <datatools/i_tree_dump.h>

// Standard Library
#include <iomanip>
#include <sstream>

// Third Party
// - A

// This Project

namespace datatools {

const std::string i_tree_dumpable::TAG           = "|-- ";
const std::string i_tree_dumpable::LAST_TAG      = "`-- ";
const std::string i_tree_dumpable::SKIP_TAG      = "|   ";
const std::string i_tree_dumpable::LAST_SKIP_TAG = "    ";


std::ostream& i_tree_dumpable::last_skip_tag(std::ostream& out) {
  out << LAST_SKIP_TAG;
  return out;
}


std::ostream& i_tree_dumpable::skip_tag (std::ostream& out) {
  out << SKIP_TAG;
  return out;
}


std::ostream & i_tree_dumpable::last_tag(std::ostream& out) {
  out << LAST_TAG;
  return out;
}


std::ostream& i_tree_dumpable::tag(std::ostream& out) {
  out << TAG;
  return out;
}


//----------------------------------------------------------------------
// inherit_tag inner class

i_tree_dumpable::inherit_tag::inherit_tag(bool inherit) 
    : inherit_(inherit) {}


std::ostream& operator<<(std::ostream& out, 
                         const i_tree_dumpable::inherit_tag& last_tag) {
  if (last_tag.inherit_) {
    out << i_tree_dumpable::tag;
  } else {
    out << i_tree_dumpable::last_tag;
  }
  return out;
}


//----------------------------------------------------------------------
// inherit_skip_tag class

i_tree_dumpable::inherit_skip_tag::inherit_skip_tag(bool inherit) 
    : inherit_ (inherit) {}


std::ostream& operator<<(std::ostream& out, 
                         const i_tree_dumpable::inherit_skip_tag& last_tag) {
  if (last_tag.inherit_) {
    out << i_tree_dumpable::skip_tag;
  } else {
    out << i_tree_dumpable::last_skip_tag;
  }
  return out;
}


} // end of namespace datatools 

/* end of i_tree_dump.cc */
