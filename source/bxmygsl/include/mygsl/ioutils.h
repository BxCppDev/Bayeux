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

      ostream & (*__function) (ostream &, const Type & );

      Type             __value;
  
    public:
      OstreamManipulator(ostream & (*function_) (ostream &, 
						 const Type & ), 
			 const Type & value_)
	: __function(function_),__value(value_) 
	{ 
	}

      friend ostream & operator<< (ostream & os_, 
				   const OstreamManipulator & os_manip_)
	{
	  return os_manip_.__function (os_, os_manip_.__value);
	} 
    };

  template <class Type> 
    class IstreamManipulatorRef 
    {
    private:

      istream & (*__function) (istream &, Type &);

      Type         &   __value;

    public:

      IstreamManipulatorRef (istream & (*function_) (istream &, Type &),
			     Type & value_)
	: __function (function_), __value (value_) 
	{ 
	}
      
      friend istream & operator>> (istream & is_, 
				   const IstreamManipulatorRef & is_manip_) 
	{
	  return is_manip_.__function (is_, is_manip_.__value);
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
