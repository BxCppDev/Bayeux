// -*- mode: c++ ; -*-
/* foo.cc
 */

#include <cstdlib>
#include <foo.h>

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (foo,"foo")

// ctor:
foo::foo ()
{
  // Initialize members with arbitrary values:
  _bval_ = false;
  _cval_ = '\0';
  _ival_ = 0;
  _fval_ = 0.0F;
  _dval_ = 0.0;
  _sval_ = "Hello world !";
  _dval_vec_.reserve (10);
  _dval_vec_.push_back (_dval_);
  return;
}

// dtor:
foo::~foo ()
{
  return;
}

void foo::randomize ()
{
  _bval_ = drand48 () < 0.5 ? false : true;
  _cval_ = 'A' + (char) (drand48 () * 26);
  _ival_ = 0 + (char) (drand48 () * 100);
  _fval_ = (float) (drand48 () * -1.0);
  _dval_ = drand48 () * 10.;
  _sval_ = "Hello world !";
  _dval_vec_.clear ();
  size_t sz = (size_t) (drand48 () * 10);
  _dval_vec_.reserve (sz);
  for (int i = 0; i < sz; i++){
    _dval_vec_.push_back (i * 1.0);
  }
  return;
}

void foo::dump (std::ostream & a_out, const std::string & a_title) const
{
  if (! a_title.empty ()) {
    a_out << a_title << std::endl;
  }
  a_out << "|-- " << "bval  =  " << _bval_ << std::endl;
  a_out << "|-- " << "cval  =  '" << _cval_ << "'" << std::endl;
  a_out << "|-- " << "ival  =  " << _ival_ << std::endl;
  a_out << "|-- " << "fval  =  " << _fval_ << std::endl;
  a_out << "|-- " << "dval  =  " << _dval_ << std::endl;
  a_out << "|-- " << "sval  =  '" << _sval_ << "'" << std::endl;
  a_out << "`-- " << "dval_vec = {";
  for (int i = 0; i < _dval_vec_.size (); i++){
    if (i != 0) a_out << "; ";
    a_out << _dval_vec_[i];
  }
  a_out << "}" << std::endl;
  return;
}

// end of foo.cc
