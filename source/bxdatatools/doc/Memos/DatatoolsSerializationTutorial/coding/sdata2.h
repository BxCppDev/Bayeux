#ifndef SDATA2_H_
#define SDATA2_H_ 1

#include <string>
#include <vector>
#include <boost/cstdint.hpp>

// Use the 'datatools::i_serializable' interface :
#include <datatools/i_serializable.h>

// A serializable class :
class sdata : DATATOOLS_SERIALIZABLE_CLASS  // Shortcut macro
{
public:
  sdata ();
  void set_bval (bool);
  bool is_bval () const;

private:
  bool     _bval_;
  int8_t   _cval_;
  int32_t  _ival_;
  float    _fval_;
  double   _dval_;
  std::string _sval_;
  std::vector<double> _dvec_;

  // Shortcut macro for serializable class interface :
  DATATOOLS_SERIALIZATION_DECLARATION();

};

#endif // SDATA2_H_
