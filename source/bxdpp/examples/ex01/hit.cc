#include <hit.h>

#include <limits>
#include <stdexcept>

#include <datatools/clhep_units.h>

namespace dpp_ex01 {

  // Macro for automatic implementation of serialization
  DATATOOLS_SERIALIZATION_IMPLEMENTATION(hit,"dpp_ex01::hit")

  hit::hit ()
  {
    _id_ = -1;
    _energy_ = std::numeric_limits<double>::quiet_NaN();
  }

  hit::~hit ()
  {
  }

  int32_t hit::get_id() const
  {
    return _id_;
  }

  void hit::set_id(int32_t id_)
  {
    _id_ = id_;
  }

  double hit::get_energy() const
  {
    return _energy_;
  }

  void hit::set_energy(double e_)
  {
    if (_energy_ < 0.0) {
      throw std::domain_error("dpp_ex01::hit::set_energy: Invalid negative energy value !");
    }
    _energy_ = e_;
  }

  const datatools::properties & hit::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & hit::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  void hit::dump(std::ostream & out_, const std::string & title_) const
  {
    if (! title_.empty()) {
      out_ << title_ << std::endl;
    }
    out_ << "|-- ID : " << _id_ << std::endl;
    out_ << "|-- Energy : " << _energy_ / CLHEP::MeV << " MeV" << std::endl;
    out_ << "`-- Auxiliaries : " << std::endl;
    _auxiliaries_.tree_dump(out_,"", "    ");
  }

} // namespace dpp_ex01
