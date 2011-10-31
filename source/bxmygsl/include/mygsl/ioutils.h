// mygsl::ioutils.h

#ifndef __mygsl__ioutils_h 
#define __mygsl__ioutils_h 1

#include <iostream>

using namespace std;

namespace mygsl {

  template <class Type> 
    class OstreamManipulator 
    {
    private:

      ostream & (*_function_) (ostream &, const Type & );

      Type _value_;
  
    public:
    OstreamManipulator(ostream & (*function_) (ostream &, 
                                               const Type & ), 
                       const Type & value_)
      : _function_(function_),_value_(value_) 
      { 
      }

      friend ostream & operator<< (ostream & os_, 
                                   const OstreamManipulator & os_manip_)
      {
        return os_manip_._function_ (os_, os_manip_._value_);
      } 
    };

  template <class Type> 
    class IstreamManipulatorRef 
    {
    private:

      istream & (*_function_) (istream &, Type &);

      Type & _value_;

    public:

      IstreamManipulatorRef (istream & (*function_) (istream &, Type &),
                             Type & value_)
        : _function_ (function_), _value_ (value_) 
      { 
      }
      
      friend istream & operator>> (istream & is_, 
                                   const IstreamManipulatorRef & is_manip_) 
      {
        return is_manip_._function_ (is_, is_manip_._value_);
      } 
    };


  ostream & ostream_odouble (ostream & os_, const double & x_); 
  
  OstreamManipulator<double> odouble (const double & x_); 

  istream & istream_idouble (istream & is_, double & x_); 
  
  IstreamManipulatorRef<double> idouble (double & x_); 
  
  class ioutils
  {
  public:

    static const string NAN_STRING;
    static const string INF_POS_STRING;
    static const string INF_NEG_STRING;
 
  };
 
}

#endif // __mygsl__ioutils_h

// end of mygsl::ioutils.h
