// i_named.cc

// Ourselves:
#include <datatools/i_named.h>

namespace datatools {

  // static
  const std::string & i_named::explicit_anonymous_label()
  {
    static const std::string lbl("__anonymous__");
    return lbl;
  }

  // static
  bool i_named::is_named(const i_named & named_)
  {
    std::string name = named_.fetch_name();
    return !name.empty() && name != explicit_anonymous_label();
  }

  // static
  bool i_named::is_unnamed(const i_named & named_)
  {
    return !is_named(named_);
  }

  // static
  bool i_named::is_nameless(const i_named & named_)
  {
    return !is_named(named_);
  }

  // static
  bool i_named::is_anonymous(const i_named & named_)
  {
    return is_nameless(named_) || named_.fetch_name() == explicit_anonymous_label();
  }

} // end of namespace datatools
