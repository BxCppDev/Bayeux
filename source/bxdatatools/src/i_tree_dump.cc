// i_tree_dump.cc

// Ourselves:
#include <datatools/i_tree_dump.h>

// Standard Library:
#include <iomanip>
#include <sstream>
#include <typeinfo>

// Third party:
// - Boost:
#include <boost/core/demangle.hpp>
// Workaround for Boost pragma:
#ifndef BOOST_BIND_GLOBAL_PLACEHOLDERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif
#include <boost/property_tree/json_parser.hpp>
#undef BOOST_BIND_GLOBAL_PLACEHOLDERS

namespace datatools {

  i_tree_dumpable::i_tree_dumpable()
  {
    return;
  }

  i_tree_dumpable::~i_tree_dumpable()
  {
    return;
  }

  // static
  const boost::property_tree::ptree & i_tree_dumpable::empty_options()
  {
    static const boost::property_tree::ptree _empty;
    return _empty;
  }

  // static
  boost::property_tree::ptree i_tree_dumpable::make_base_print_options(const std::string & title_,
                                                                       const std::string & indent_,
                                                                       const bool inherit_)
  {
    boost::property_tree::ptree opts;
    if (!title_.empty()) {
      opts.put(i_tree_dumpable::base_print_options::title_key(), title_);
    }
    if (!indent_.empty()) {
      opts.put(i_tree_dumpable::base_print_options::indent_key(), indent_);
    }
    if (inherit_) {
      opts.put(i_tree_dumpable::base_print_options::inherit_key(), inherit_);
    }
    return opts;
  }

  void i_tree_dumpable::base_print_options::configure_from(const boost::property_tree::ptree & options_)
  {
    inherit = options_.get<bool>(base_print_options::inherit_key(), false);
    title   = options_.get<std::string>(base_print_options::title_key(), "");
    indent  = options_.get<std::string>(base_print_options::indent_key(), "");
    return;
  }

  void i_tree_dumpable::base_print_options::export_to(boost::property_tree::ptree & options_)
  {
    options_.put(base_print_options::inherit_key(), inherit);
    if (title.empty()) {
      options_.erase(base_print_options::inherit_key());
    }
    options_.put(base_print_options::title_key(), title);
    if (indent.empty()) {
      options_.erase(base_print_options::indent_key());
    }
    options_.put(base_print_options::indent_key(), indent);
    return;
  }

  void i_tree_dumpable::base_print_options::reset()
  {
    inherit = false;
    title.clear();
    indent.clear();
    return;
  }

  // static
  boost::property_tree::ptree i_tree_dumpable::base_print_options::force_inheritance(const boost::property_tree::ptree & options_)
  {
    boost::property_tree::ptree pt = options_;
    pt.put(inherit_key(), true);
    return pt;
  }

  const std::string & i_tree_dumpable::base_print_options::inherit_key()
  {
    static const std::string value("inherit");
    return value;
  }

  const std::string & i_tree_dumpable::base_print_options::title_key()
  {
    static const std::string value("title");
    return value;
  }

  const std::string & i_tree_dumpable::base_print_options::indent_key()
  {
    static const std::string value("indent");
    return value;
  }

  const std::string & i_tree_dumpable::tags::item(bool last_, bool skip_)
  {
    if (last_ && skip_) return last_skip_item();
    if (last_ && !skip_) return last_item();
    if (!last_ && skip_) return skip_item();
    return item();
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

  void i_tree_dumpable::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    base_print_options opts;
    opts.inherit = inherit_;
    opts.title = title_;
    opts.indent = indent_;
    boost::property_tree::ptree options;
    opts.export_to(options);
    print_tree(out_, options);
    return;
  }

  // virtual
  void i_tree_dumpable::print_tree(std::ostream & out_,
                                   const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    if (! popts.title.empty ()) {
      out_ << popts.indent << popts.title << std::endl;
    }

    char const * type_name = typeid(*this).name();
    out_ << popts.indent << datatools::i_tree_dumpable::tag
         << "The 'print_tree' method is not implemented for class '"
         << boost::core::demangle(type_name)
         << "'" << std::endl;

    out_ << popts.indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
         << "Object's address: " << std::hex << this << std::dec << std::endl;

    return;
  }

  void i_tree_dumpable::print_tree_json(std::ostream & out_,
                                        const std::string & json_options_) const
  {
    boost::property_tree::ptree options;
    std::stringstream jsoniss;
    jsoniss << json_options_;
    boost::property_tree::json_parser::read_json(jsoniss, options);
    print_tree(out_, options);
    return;
  }

  void i_tree_dumpable::print_tree_json(std::ostream & out_,
                                        const char * json_options_) const
  {
    std::string json_opts;
    if (json_options_ != nullptr) {
      json_opts = json_options_;
    }
    print_tree_json(out_, json_opts);
    return;
  }

  void i_tree_dumpable::tree_dump_id(const int out_type_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     const bool inherit_) const
  {
    std::ostream * out = &std::clog;
    if (out_type_ == OSTREAM_COUT) out = &std::cout;
    if (out_type_ == OSTREAM_CERR) out = &std::cerr;
    tree_dump(*out, title_, indent_, inherit_);
    return;
  }

  void i_tree_dumpable::tree_print_id(const int out_type_,
                                      const std::string & title_) const
  {
    tree_dump_id(out_type_, title_, "", false);
    return;
  }

  void i_tree_dumpable::smart_print_id(const int out_type_,
                                       const std::string & title_,
                                       const std::string & indent_) const
  {
    tree_dump_id(out_type_, title_, indent_, false);
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
