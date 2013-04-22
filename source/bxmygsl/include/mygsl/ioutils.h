// -*- mode: c++; -*-
// ioutils.h

#ifndef MYGSL_IOUTILS_H_
#define MYGSL_IOUTILS_H_ 1

#include <iostream>

namespace mygsl {

  template <class Type>
    class OstreamManipulator
    {
    private:

      std::ostream & (*_function_) (std::ostream &, const Type & );

      Type _value_;

    public:
    OstreamManipulator(std::ostream & (*function_) (std::ostream &,
                                               const Type & ),
                       const Type & value_)
      : _function_(function_),_value_(value_)
      {
      }

      friend std::ostream & operator<< (std::ostream & os_,
                                   const OstreamManipulator & os_manip_)
      {
        return os_manip_._function_ (os_, os_manip_._value_);
      }
    };

  template <class Type>
    class IstreamManipulatorRef
    {
    private:

      std::istream & (*_function_) (std::istream &, Type &);

      Type & _value_;

    public:

      IstreamManipulatorRef (std::istream & (*function_) (std::istream &, Type &),
                             Type & value_)
        : _function_ (function_), _value_ (value_)
      {
      }

      friend std::istream & operator>> (std::istream & is_,
                                   const IstreamManipulatorRef & is_manip_)
      {
        return is_manip_._function_ (is_, is_manip_._value_);
      }
    };


  std::ostream & ostream_odouble (std::ostream & os_, const double & x_);

  OstreamManipulator<double> odouble (const double & x_);

  std::istream & istream_idouble (std::istream & is_, double & x_);

  IstreamManipulatorRef<double> idouble (double & x_);

  class ioutils
  {
  public:

    static const std::string NAN_STRING;
    static const std::string INF_POS_STRING;
    static const std::string INF_NEG_STRING;

  };

}

#endif // MYGSL_IOUTILS_H_

// end of ioutils.h
