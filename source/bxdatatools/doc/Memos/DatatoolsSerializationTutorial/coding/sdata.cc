#include "sdata.h"

// The 'sdata' class serialization tag :
const std::string sdata::SERIAL_TAG = "sdata";

// The virtual method from the 'datatools::serialization::i_serializable' 
// interface :
const std::string & sdata::get_serial_tag () const
{					
  return sdata::SERIAL_TAG;
}

// Implementation of the constructor:
sdata::sdata ()
{
  // Initialize members with arbitrary values :
  _bval_ = true;
  _cval_ = '?'; // ASCII code == 63
  _ival_ = 666;
  _fval_ = 3.14159;
  _dval_ = 1. / 3;
  _sval_ = "Hello world !";
  _dvec_.reserve (3); /* pre-allocate the vector's capacity 
			* for memory optimization
			*/
  _dvec_.push_back (_dval_);
  _dvec_.push_back (_dval_ * 2);
  _dvec_.push_back (_dval_ * 3); // only add 3 elements 
  return;
}
  
// Implementation of a setter method :
void sdata::set_bval (bool bval_)
{
  _bval_ = bval_;
  return;
}
  
// Implementation of a getter method :
bool sdata::is_bval () const
{
  return _bval_;
}
