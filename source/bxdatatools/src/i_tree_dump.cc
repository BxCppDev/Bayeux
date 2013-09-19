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

const std::string i_tree_dumpable::tags::item()
{
  static std::string value;
  if (value.empty()) {
    value = "|-- ";
  }
  return value;
}

const std::string i_tree_dumpable::tags::last_item()
{
  static std::string value;
  if (value.empty()) {
    value = "`-- ";
  }
  return value;
}

const std::string i_tree_dumpable::tags::skip_item()
{
  static std::string value;
  if (value.empty()) {
    value = "|   ";
  }
  return value;
}

const std::string i_tree_dumpable::tags::last_skip_item()
{
  static std::string value;
  if (value.empty()) {
    value = "    ";
  }
  return value;
}


// const std::string i_tree_dumpable::TAG           = "|-- ";
// const std::string i_tree_dumpable::LAST_TAG      = "`-- ";
// const std::string i_tree_dumpable::SKIP_TAG      = "|   ";
// const std::string i_tree_dumpable::LAST_SKIP_TAG = "    ";


std::ostream& i_tree_dumpable::last_skip_tag(std::ostream& out) {
  out << tags::last_skip_item();
  return out;
}


std::ostream& i_tree_dumpable::skip_tag(std::ostream& out) {
  out << tags::skip_item();
  return out;
}


std::ostream & i_tree_dumpable::last_tag(std::ostream& out) {
  out << tags::last_item();
  return out;
}


std::ostream& i_tree_dumpable::tag(std::ostream& out) {
  out << tags::item();
  return out;
}


void i_tree_dumpable::tree_dump (int out_type,
                                 const std::string& title,
                                 const std::string& indent,
                                 bool inherit) const
{
  std::ostream * out = &std::clog;
  if (out_type == OSTREAM_COUT) out = &std::cout;
  if (out_type == OSTREAM_CERR) out = &std::cerr;
  tree_dump(*out, title, indent, inherit);
}

void i_tree_dumpable::tree_print (int out_type,
                                  const std::string& title) const
{
  tree_dump(out_type, title, "", false);
}

void i_tree_dumpable::smart_print (int out_type,
                                   const std::string& title,
                                   const std::string& indent) const
{
  tree_dump(out_type, title, indent, false);
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
