// i_tree_dump.cc

// Ourselves:
#include <datatools/i_tree_dump.h>

// Standard Library:
#include <iomanip>
#include <sstream>

namespace datatools {

  i_tree_dumpable::i_tree_dumpable()
  {
    return;
  }

  i_tree_dumpable::~i_tree_dumpable()
  {
    return;
  }

  const std::string & i_tree_dumpable::tags::item()
  {
    static const std::string value("|-- ");
    return value;
  }

  const std::string & i_tree_dumpable::tags::last_item()
  {
    static const std::string value("`-- ");
    return value;
  }

  const std::string & i_tree_dumpable::tags::skip_item()
  {
    static const std::string value("|   ");
    return value;
  }

  const std::string & i_tree_dumpable::tags::last_skip_item()
  {
    static const std::string value("    ");
    return value;
  }

  std::ostream & i_tree_dumpable::last_skip_tag(std::ostream & out_)
  {
    out_ << tags::last_skip_item();
    return out_;
  }

  std::ostream & i_tree_dumpable::skip_tag(std::ostream & out_)
  {
    out_ << tags::skip_item();
    return out_;
  }

  std::ostream & i_tree_dumpable::last_tag(std::ostream & out_)
  {
    out_ << tags::last_item();
    return out_;
  }

  std::ostream & i_tree_dumpable::tag(std::ostream & out_)
  {
    out_ << tags::item();
    return out_;
  }

  void i_tree_dumpable::tree_dump (int out_type_,
                                   const std::string& title_,
                                   const std::string& indent_,
                                   bool inherit_) const
  {
    std::ostream * out = &std::clog;
    if (out_type_ == OSTREAM_COUT) out = &std::cout;
    if (out_type_ == OSTREAM_CERR) out = &std::cerr;
    tree_dump(*out, title_, indent_, inherit_);
    return;
  }

  void i_tree_dumpable::tree_print (int out_type_,
                                    const std::string & title_) const
  {
    tree_dump(out_type_, title_, "", false);
    return;
  }

  void i_tree_dumpable::smart_print (int out_type_,
                                     const std::string & title_,
                                     const std::string & indent_) const
  {
    tree_dump(out_type_, title_, indent_, false);
    return;
  }

  //----------------------------------------------------------------------
  // inherit_tag inner class

  i_tree_dumpable::inherit_tag::inherit_tag(bool inherit_)
    : _inherit_(inherit_)
  {
    return;
  }

  std::ostream & operator<<(std::ostream & out_,
                            const i_tree_dumpable::inherit_tag & last_tag_)
  {
    if (last_tag_._inherit_) {
      out_ << i_tree_dumpable::tag;
    } else {
      out_ << i_tree_dumpable::last_tag;
    }
    return out_;
  }

  //----------------------------------------------------------------------
  // inherit_skip_tag class

  i_tree_dumpable::inherit_skip_tag::inherit_skip_tag(bool inherit_)
    : _inherit_(inherit_)
  {
    return;
  }

  std::ostream & operator<<(std::ostream & out,
                            const i_tree_dumpable::inherit_skip_tag & last_tag)
  {
    if (last_tag._inherit_) {
      out << i_tree_dumpable::skip_tag;
    } else {
      out << i_tree_dumpable::last_skip_tag;
    }
    return out;
  }

} // end of namespace datatools
