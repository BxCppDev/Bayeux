#include <dpp_ex01/raw_data.h>

#include <stdexcept>

namespace dpp_ex01 {

  // Macro for automatic implementation of serialization (advanced version) :
  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(raw_data,"dpp_ex01::raw_data")

  raw_data::raw_data ()
  {
  }

  raw_data::~raw_data ()
  {
  }

  const std::vector<hit> & raw_data::get_hits() const
  {
    return _hits_;
  }

  std::vector<hit> & raw_data::grab_hits()
  {
    return _hits_;
  }

  const datatools::properties & raw_data::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & raw_data::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  void raw_data::dump(std::ostream & out_, const std::string & title_) const
  {
    if (! title_.empty()) {
      out_ << title_ << std::endl;
    }
    out_ << "|-- Hits : " << _hits_.size() << std::endl;
    out_ << "`-- Auxiliaries : " << std::endl;
    _auxiliaries_.tree_dump(out_,"", "    ");
    return;
  }

  void raw_data::tree_dump(std::ostream& out_,
                           const std::string& title_,
                           const std::string& indent_,
                           bool inherit_) const {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Hits        : " << _hits_.size() << std::endl;

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Auxiliaries : ";
    if (! get_auxiliaries().size()) {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (get_auxiliaries().size()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::inherit_skip_tag(inherit_);
      get_auxiliaries().tree_dump(out_, "", indent2.str());
    }

    return;
  }

} // namespace dpp_ex01
