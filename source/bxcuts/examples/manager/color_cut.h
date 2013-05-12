// -*- mode: c++ ; -*-
/* color_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-10
 *
 * License:
 */

#ifndef COLOR_CUT_H_
#define COLOR_CUT_H_ 1

#include <iostream>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <data.h>

/// \brief A cut on the color of object of class data
CUT_CLASS_DECLARE(color_cut)
{
 public:

  // Macro to provide the default interface of this cut class
  CUT_INTERFACE_CTOR_DTOR(color_cut);

  void set_color(int a_color);

 private:

  int _color_; /// The color to be selected

  // Macro to automate the registration of the cut
  CUT_REGISTRATION_INTERFACE(color_cut);

};

#endif // COLOR_CUT_H_

// end of color_cut_h
