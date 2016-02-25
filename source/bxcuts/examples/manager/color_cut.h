// -*- mode: c++ ; -*-
/* color_cut.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-17
 *
 */

#ifndef COLOR_CUT_H
#define COLOR_CUT_H

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/cuts:
#include <cuts/i_cut.h>

/// \brief A cut on the color of object of class data
class color_cut : public cuts::i_cut
{
 public:

  /// Constructor
  color_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~color_cut();

  /// Initialization
  virtual void initialize(const datatools::properties &,
                          datatools::service_manager &,
                          cuts::cut_handle_dict_type &);

  /// Reset
  virtual void reset();

protected :

  /// Selection
  virtual int _accept();

  void set_color(int a_color);

 private:

  int _color_; //!< The color to be selected

  // Macro to automate the registration of the cut
  CUT_REGISTRATION_INTERFACE(color_cut);

};

#endif // COLOR_CUT_H
