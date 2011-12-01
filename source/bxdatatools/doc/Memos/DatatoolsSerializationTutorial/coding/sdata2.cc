#include "sdata2.h"

// Shortcut macro to implement the 'serialization tag' stuff :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (sdata, "sdata")

// Implementation of the constructor:
sdata::sdata ()
{
  _bval_ = true;
  _ival_ = 666;
  _fval_ = 3.14159;
  _dval_ = 1. / 3;
  _sval_ = "Hello world !";
  _dvec_.reserve (3);
  _dvec_.push_back (_dval_);
  _dvec_.push_back (_dval_ * 2);
  _dvec_.push_back (_dval_ * 3); 
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
