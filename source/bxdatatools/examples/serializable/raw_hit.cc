// raw_hit.cc

// Ourselves:
#include <raw_hit.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This macro instantiate dedicated serialization code for the
// "raw_hit" class:
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(raw_hit,"raw_hit")

// Implementation of static constants :
const int          raw_hit::INVALID_ID;
const int          raw_hit::INVALID_CHANNEL;
const unsigned int raw_hit::MAX_CHANNEL;
const unsigned int raw_hit::MAX_ADC;
const unsigned int raw_hit::MAX_TDC;
const double       raw_hit::LOW_QUALITY_PROB = 0.25;
const std::string  raw_hit::LOW_QUALITY_KEY =  "low_quality";

raw_hit::raw_hit()
{
  // Initialize members with arbitrary values:
  _id_ = INVALID_ID;
  _simulated_ = false;
  _channel_ = INVALID_CHANNEL;
  _adc_ = 0;
  _tdc_ = 0;
  return;
}

raw_hit::~raw_hit()
{
  return;
}

void raw_hit::reset()
{
  _id_ = INVALID_ID;
  _simulated_ = false;
  _channel_ = INVALID_CHANNEL;
  _adc_ = 0;
  _tdc_ = 0;
  _auxiliaries_.clear();
  return;
}

void raw_hit::randomize()
{
  _simulated_ = true;
  _channel_ = (int) (MAX_CHANNEL * drand48());
  _adc_ = (unsigned int) (MAX_ADC * drand48());
  _tdc_ = (unsigned int) (MAX_TDC * drand48());
  if (drand48() < LOW_QUALITY_PROB) {
    _auxiliaries_.store_flag(LOW_QUALITY_KEY);
  }
  return;
}

void raw_hit::set_id(int id_)
{
  DT_THROW_IF(id_ < INVALID_ID,
              std::domain_error,
              "Invalid hit ID '" << id_ << "' !");
  _id_ = id_;
}

int raw_hit::get_id() const
{
  return _id_;
}

void raw_hit::set_simulated(bool sim_)
{
  _simulated_ = sim_;
}

bool raw_hit::is_simulated() const
{
  return _simulated_;
}

void raw_hit::set_channel(int ch_)
{
  DT_THROW_IF(ch_ < INVALID_CHANNEL || ch_ >= MAX_CHANNEL,
              std::domain_error,
              "Invalid channel ID '" << ch_ << "' !");
  _channel_ = ch_;
  return;
}

int raw_hit::get_channel() const
{
  return _channel_;
}

void raw_hit::set_adc(int adc_)
{
  DT_THROW_IF(adc_ < 0 || adc_ >= MAX_ADC,
              std::domain_error,
              "Invalid ADC value '" << adc_ << "' !");
  _adc_ = adc_;
  return;
}

int raw_hit::get_adc() const
{
  return _adc_;
}

void raw_hit::set_tdc(int tdc_)
{
  DT_THROW_IF(tdc_ < 0 || tdc_ >= MAX_TDC,
              std::domain_error,
              "Invalid TDC value '" << tdc_ << "' !");
  _tdc_ = tdc_;
  return;
}

int raw_hit::get_tdc() const
{
  return _tdc_;
}

const datatools::properties & raw_hit::get_auxiliaries() const
{
  return _auxiliaries_;
}

datatools::properties & raw_hit::grab_auxiliaries()
{
  return _auxiliaries_;
}

void raw_hit::dump(std::ostream & out_,
                   const std::string & title_,
                   const std::string & indent_) const
{
  if (! title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }
  out_ << indent_ << "|-- " << "ID        : " << _id_ << std::endl;
  out_ << indent_ << "|-- " << "Simulated : " << _simulated_ << std::endl;
  out_ << indent_ << "|-- " << "Channel   : " << (int) _channel_ << std::endl;
  out_ << indent_ << "|-- " << "ADC       : " << _adc_ << std::endl;
  out_ << indent_ << "|-- " << "TDC       : " << _tdc_ << std::endl;
  out_ << indent_ << "`-- " << "Auxiliary properties : " << std::endl;
  _auxiliaries_.tree_dump(out_,"",indent_ + "    ");
  return;
}
