// -*- mode: c++ ; -*-
/* range_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-17
 */

#ifndef RANGE_CUT_H_
#define RANGE_CUT_H_ 1

#include <iostream>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

// CUT_CLASS_DECLARE(range_cut)
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

  /* The CUT_INTERFACE_CTOR_DTOR macro can be used
   * in place of the full specification of the
   * following methods:
   *  * constructor
   *  * destructor
   *  * initialize
   *  * reset
   *  * _accept(protected)
   *
   */
  // CUT_INTERFACE_CTOR_DTOR(range_cut)

  // CUT_CONSTRUCTOR_DECLARE(range_cut)
  /// Constructor
  range_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

  // CUT_DESTRUCTOR_DECLARE(range_cut)
  /// Destructor
  virtual ~range_cut();

  // CUT_INITIALIZE_DECLARE()
  /// Initialization
  virtual void initialize(const datatools::properties &,
                          datatools::service_manager &,
                          cuts::cut_handle_dict_type &);

  // CUT_RESET_DECLARE();
  /// Reset
  virtual void reset();

  // CUT_ACCEPT_DECLARE();
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

#endif // RANGE_CUT_H_

// end of range_cut_h
