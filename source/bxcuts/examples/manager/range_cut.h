// -*- mode: c++ ; -*-
/* range_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-17
 */

#ifndef RANGE_CUT_H
#define RANGE_CUT_H

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/cuts:
#include <cuts/i_cut.h>

/// \brief A cut to test the position of a 3D-point relatively to a range on X, Y or Z axis.
class range_cut : public cuts::i_cut
{
public:

  enum mode_type {
    MODE_INVALID = -1,
    MODE_X = 0,
    MODE_Y = 1,
    MODE_Z = 2
  };

  /// Set the selection axis
  void set_mode(int a_mode);

  /// Reset the selection axis
  void reset_mode();

  /// Set the selection range
  void set_range(double a_min, double a_max);

  /// Set the reversed selection flag
  void set_reversed(bool a_reversed);

  /// Constructor
  range_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~range_cut();

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

  int    _mode_;       //!< Cut mode : X, Y or Z axis
  double _min_, _max_; //!< Bounds of the cut range
  bool   _reversed_;   //!< Cut reverse flag

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(range_cut);

};

#endif // RANGE_CUT_H
