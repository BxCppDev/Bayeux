// -*- mode: c++ ; -*-
/* sphere_cut.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-17
 */

/**
 * Description:
 *   A cut to test the position of a 3D-point
 *   relatively to a sphere.
 *
 */

#ifndef SPHERE_CUT_H_
#define SPHERE_CUT_H_ 1

#include <iostream>

#include <cuts/i_cut.h>

/// \brief A cut to test the position of a 3D-point relatively to a sphere.
class sphere_cut : public cuts::i_cut
{
 public:

  void set_radius(double a_radius);

  void set_center(double a_x0, double a_y0, double a_z0);

  void set_reversed(bool a_reversed);

  /// Constructor
  sphere_cut(datatools::logger::priority a_logging_priority =
             datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~sphere_cut();

  /// Initialization
  virtual void initialize(const datatools::properties &,
                          datatools::service_manager &,
                          cuts::cut_handle_dict_type &);

  /// Reset
  virtual void reset();

protected :

  /// Selection
  virtual int _accept();

 private:

  double _x0_; //!< X coordinate of the center of the cut sphere
  double _y0_; //!< Y coordinate of the center of the cut sphere
  double _z0_; //!< Z coordinate of the center of the cut sphere
  double _radius_; //!< Radius of the cut sphere
  bool   _reversed_; //!< Cut reverse flag

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(sphere_cut);

};

#endif // SPHERE_CUT_H_

// end of sphere_cut.h
