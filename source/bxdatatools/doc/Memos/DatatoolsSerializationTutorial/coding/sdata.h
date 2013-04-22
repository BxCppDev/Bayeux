#ifndef SDATA_H_
#define SDATA_H_ 1

#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// Use the 'datatools::i_serializable' interface :
#include <datatools/i_serializable.h>

// A class that is serializable via datatools :
class sdata : public datatools::i_serializable
{
public:
  sdata ();
  void set_bval (bool);
  bool is_bval () const;

private:
  bool     _bval_; // A boolean
  int8_t   _cval_; // A signed char (8 bits); implies 'boost/cstdint.hpp'
  int32_t  _ival_; // A 32 bits signed integral
  float    _fval_; // A 32 bits float
  double   _dval_; // A 64 bits float
  std::string _sval_; // A STL string; implies 'boost/serialization/string.hpp'
  std::vector<double> _dvec_; // a STL vector of 64 bits floats;
			      // implies 'boost/serialization/vector.hpp'

public:

  /***********************************
   * datatools/serializable specific *
   ***********************************/

  static const std::string SERIAL_TAG;

  virtual const std::string & get_serial_tag () const;

private:

  /********************************
   * Boost/Serialization specific *
   ********************************/

  /* Manage a priviledge access to class' private members
   * by the Boost/Serialization library tools :
   */
  friend class boost::serialization::access;

  /* The main template serialization method :
   *  @arg ar- is of archive type (could be ASCII, XML, binary...)
   *  @arg version_ is the class version number (not used here)
   */
  template<class Archive>
    void serialize (Archive & ar_, const unsigned int version_);
};

#endif // SDATA_H_
