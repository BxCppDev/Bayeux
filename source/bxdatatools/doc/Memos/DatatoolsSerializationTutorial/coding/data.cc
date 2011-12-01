#include "data.h"

// Implementation of the constructor:
data::data ()
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
void data::set_bval (bool bval_)
{
  _bval_ = bval_;
  return;
}
  
// Implementation of a getter method :
bool data::is_bval () const
{
  return _bval_;
}
