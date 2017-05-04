// -*- mode: c++ ; -*-
/// raw_data.cc

// Ourselves:
#include <raw_data.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_data,"raw_data")

std::string toy_cypher(const std::string & name_)
{
  return std::string(name_.rbegin(), name_.rend());
}

raw_data::raw_data()
{
  // Initialize members with arbitrary values:
  _locked_ = false;
  return;
}

raw_data::~raw_data()
{
  return;
}

void raw_data::reset()
{
  DT_THROW_IF(is_locked(), std::logic_error, "Operation prohibited ! Raw data is locked !");
  _hits_.clear();
  _auxiliaries_.clear();
  _locked_ = false;
  _lock_password_.clear();
  return;
}

void raw_data::randomize_hits(unsigned int number_of_hits_)
{
  DT_THROW_IF(is_locked(), std::logic_error, "Operation prohibited ! Raw data is locked !");
  for (int i = 0; i < number_of_hits_; i++) {
    raw_hit RH;
    RH.set_id(i);
    RH.randomize();
    _hits_.push_back(RH);
  }
  return;
}

const std::vector<raw_hit> & raw_data::get_hits() const
{
  return _hits_;
}

std::vector<raw_hit> & raw_data::grab_hits()
{
  DT_THROW_IF(is_locked(), std::logic_error, "Operation prohibited ! Raw data is locked !");
  return _hits_;
}

const datatools::properties & raw_data::get_auxiliaries() const
{
  return _auxiliaries_;
}

datatools::properties & raw_data::grab_auxiliaries()
{
  DT_THROW_IF(is_locked(), std::logic_error, "Operation prohibited ! Raw data is locked !");
  return _auxiliaries_;
}

bool raw_data::is_locked() const
{
  return _locked_;
}

void raw_data::lock(const std::string & password_)
{
  DT_THROW_IF(is_locked(), std::logic_error, "Raw data is already locked !");
  _locked_ = true;
  _lock_password_ = toy_cypher(password_);
  return;
}

void raw_data::unlock(const std::string & password_)
{
  DT_THROW_IF(! is_locked(), std::logic_error, "Raw data is not locked !");
  DT_THROW_IF(toy_cypher(password_) != _lock_password_, std::runtime_error, "Invalid lock password !");
  _locked_ = false;
  _lock_password_.clear();
}

/// Set the lock password
void raw_data::set_lock_password(const std::string & new_password_,
                                 const std::string & old_password_)
{
  DT_THROW_IF(toy_cypher(old_password_) != _lock_password_, std::runtime_error, "Authentication failed ! Invalid lock password !");
  _locked_ = true;
  _lock_password_ = toy_cypher(new_password_);
  return;
}

void raw_data::dump(std::ostream & out_,
                   const std::string & title_,
                   const std::string & indent_) const
{
  if (! title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }
  out_ << indent_ << "|-- " << "Locked        : " << _locked_ << std::endl;
  out_ << indent_ << "|-- " << "Lock password : " << (_lock_password_.empty()? "No": "Yes") << std::endl;
  out_ << indent_ << "|-- " << "Raw hits      : " << _hits_.size() << std::endl;
  out_ << indent_ << "`-- " << "Auxiliary properties : " << std::endl;
  _auxiliaries_.tree_dump(out_,"",indent_ + "    ");
  return;
}
