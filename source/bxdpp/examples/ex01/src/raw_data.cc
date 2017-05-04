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
  }

} // namespace dpp_ex01
