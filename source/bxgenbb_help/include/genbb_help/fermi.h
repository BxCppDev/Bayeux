// -*- mode: c++; -*- 
/* fermi.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2011-02-13
 * 
 * License: 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef __genbb_help__fermi_h
#define __genbb_help__fermi_h 1

extern "C"
{
  double a_from_z (double z_);

  // Non-relativistic approximation of the Fermi function:
  float fermi_func_nr_approx (float z_, float e_);

  float fermi_func_shape_only (float z_, float e_);
  
  float fermi_func (float z_, float e_, bool use_l0_ = false);
  
  float fermi_func_decay0 (float z_, float e_, bool devel_ = false);

  float fermi_wrap (float * z_, float * e_);

}

#endif // __genbb_help__fermi_h

// end of fermi.h
