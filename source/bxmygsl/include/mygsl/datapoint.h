// -*- mode:c++; -*- 
// datapoint.h

#ifndef MYGSL_DATAPOINT_H_ 
#define MYGSL_DATAPOINT_H_ 1

#include <iostream>
#include <limits>

// Serialization interface :
#include <datatools/i_serializable.h>

#include <mygsl/ioutils.h>

namespace mygsl {

  /// \brief A (x,y,sigma(y)) triplet
  class datapoint :
    DATATOOLS_SERIALIZABLE_CLASS
  {

  public:

    datapoint ();

    datapoint (double x_, 
               double y_, 
               double sigma_y_ = std::numeric_limits<double>::quiet_NaN ());

    virtual ~datapoint ();

    const double & x () const;

    const double & y () const;

    const double & sigma_y () const;

    bool has_sigma_y () const;

    bool is_weighted () const;

    friend std::ostream & operator<< (std::ostream &, const datapoint &);

    friend std::istream & operator>> (std::istream &, datapoint &);

    static bool comp_by_x (const datapoint & p1_, 
                           const datapoint & p2_);

  private:

    double _x_;
    double _y_;
    double _sigma_y_;

    DATATOOLS_SERIALIZATION_DECLARATION();

  };

}

#endif // MYGSL_DATAPOINT_H_

// end of datapoint.h
