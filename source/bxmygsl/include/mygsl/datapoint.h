/// \file mygsl/datapoint.h

#ifndef MYGSL_DATAPOINT_H
#define MYGSL_DATAPOINT_H 1

// Standard library:
#include <iostream>
#include <limits>

// Third party :
// - Bayeux/deatatools:
#include <datatools/i_serializable.h>

// This project:
#include <mygsl/ioutils.h>

namespace mygsl {

  /// \brief A serializable (x,y,sigma(y)) triplet
  class datapoint : public datatools::i_serializable
  {

  public:

    /// Default constructor
    datapoint();

    /// Constructor with a (x, y, sigma_y) triplet
    datapoint(double x_,
              double y_,
              double sigma_y_ = std::numeric_limits<double>::quiet_NaN());

    /// Destructor
    ~datapoint() override;

    /// Return the X coordinate
    const double & x() const;

    /// Return the Y coordinate
    const double & y() const;

    /// Return the error on the Y coordinate
    const double & sigma_y() const;

    /// Check if the error on the Y coordinate is provided
    bool has_sigma_y() const;

    /// Check if the datapoints in weighted with the error on the Y coordinate
    bool is_weighted() const;

    friend std::ostream & operator<<(std::ostream &, const datapoint &);

    friend std::istream & operator>>(std::istream &, datapoint &);

    /// Compare two datapoint using their X coordinate
    static bool comp_by_x(const datapoint & p1_,
                          const datapoint & p2_);

  private:

    double _x_;       ///< X coordinate
    double _y_;       ///< Y coordinate
    double _sigma_y_; ///< Error on Y coordinate

    DATATOOLS_SERIALIZATION_DECLARATION()

  };

}

#endif // MYGSL_DATAPOINT_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
