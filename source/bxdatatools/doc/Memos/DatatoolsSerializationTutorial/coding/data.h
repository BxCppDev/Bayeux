#ifndef DATA_H_
#define DATA_H_ 1

// Standard headers from the STL :
#include <string>
#include <vector>

// Portable integral types (highly recommended):
#include <boost/cstdint.hpp>

// Support for private access to private attribute of a class :
#include <boost/serialization/access.hpp>

// A class that is serializable via Boost archives :
class data
{
public:
  /* The Boost serialization library requests that a serializable
   * class has a default constructor. Here we provide one (note that
   * the compiler will create one if we miss it).
   */
  data ();

  // Getter/Setter methods :
  void set_bval (bool);
  bool is_bval () const;
    // ... more get/set methods could be added...

private:
  // The private attributes of the class :
  bool     _bval_; // A boolean
  int8_t   _cval_; // A signed char (8 bits); implies 'boost/cstdint.hpp'
  int32_t  _ival_; // A 32 bits signed integral
  float    _fval_; // A 32 bits float
  double   _dval_; // A 64 bits float
  std::string _sval_; // A STL string; implies 'boost/serialization/string.hpp'
  std::vector<double> _dvec_; // a STL vector of 64 bits floats;
			      // implies 'boost/serialization/vector.hpp'
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

#endif // DATA_H_
