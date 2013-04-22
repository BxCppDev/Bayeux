// -*- mode: c++ ; -*-
/*! \file datatools/bit_mask.h */
/* bit_mask.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-02-07
 * Last modified: 2013-22-04
 *
 * License:
 *
 * Description:
 *
 *   Some utilities for manipulating bit masks.
 *
 * History:
 *
 */
#ifndef DATATOOLS_BIT_MASK_H_
#define DATATOOLS_BIT_MASK_H_
// Standard Library

// Third Party
// - A
#include <boost/cstdint.hpp>

// This Project


namespace datatools {
/**!  \struct bit_mask
  \brief Some useful constants to manipulate bits typically within an unsigned integer of 32 bits.
  */
struct bit_mask {
 public:
  static const uint32_t bit00   = 0x00000001;
  static const uint32_t bit01   = 0x00000002;
  static const uint32_t bit02   = 0x00000004;
  static const uint32_t bit03   = 0x00000008;
  static const uint32_t bit04   = 0x00000010;
  static const uint32_t bit05   = 0x00000020;
  static const uint32_t bit06   = 0x00000040;
  static const uint32_t bit07   = 0x00000080;
  static const uint32_t bit08   = 0x00000100;
  static const uint32_t bit09   = 0x00000200;
  static const uint32_t bit10   = 0x00000400;
  static const uint32_t bit11   = 0x00000800;
  static const uint32_t bit12   = 0x00001000;
  static const uint32_t bit13   = 0x00002000;
  static const uint32_t bit14   = 0x00004000;
  static const uint32_t bit15   = 0x00008000;
  static const uint32_t bit16   = 0x00010000;
  static const uint32_t bit17   = 0x00020000;
  static const uint32_t bit18   = 0x00040000;
  static const uint32_t bit19   = 0x00080000;
  static const uint32_t bit20   = 0x00100000;
  static const uint32_t bit21   = 0x00200000;
  static const uint32_t bit22   = 0x00400000;
  static const uint32_t bit23   = 0x00800000;
  static const uint32_t bit24   = 0x01000000;
  static const uint32_t bit25   = 0x02000000;
  static const uint32_t bit26   = 0x04000000;
  static const uint32_t bit27   = 0x08000000;
  static const uint32_t bit28   = 0x10000000;
  static const uint32_t bit29   = 0x20000000;
  static const uint32_t bit30   = 0x40000000;
  static const uint32_t bit31   = 0x80000000;

  static const uint32_t nbits00 = 0x00000000;
  static const uint32_t nbits01 = 0x00000001;
  static const uint32_t nbits02 = 0x00000003;
  static const uint32_t nbits03 = 0x00000007;
  static const uint32_t nbits04 = 0x0000000f;
  static const uint32_t nbits05 = 0x0000001f;
  static const uint32_t nbits06 = 0x0000003f;
  static const uint32_t nbits07 = 0x0000007f;
  static const uint32_t nbits08 = 0x000000ff;
  static const uint32_t nbits09 = 0x000001ff;
  static const uint32_t nbits10 = 0x000003ff;
  static const uint32_t nbits11 = 0x000007ff;
  static const uint32_t nbits12 = 0x00000fff;
  static const uint32_t nbits13 = 0x00001fff;
  static const uint32_t nbits14 = 0x00003fff;
  static const uint32_t nbits15 = 0x00007fff;
  static const uint32_t nbits16 = 0x0000ffff;
  static const uint32_t nbits17 = 0x0001ffff;
  static const uint32_t nbits18 = 0x0003ffff;
  static const uint32_t nbits19 = 0x0007ffff;
  static const uint32_t nbits20 = 0x000fffff;
  static const uint32_t nbits21 = 0x001fffff;
  static const uint32_t nbits22 = 0x003fffff;
  static const uint32_t nbits23 = 0x007fffff;
  static const uint32_t nbits24 = 0x00ffffff;
  static const uint32_t nbits25 = 0x01ffffff;
  static const uint32_t nbits26 = 0x03ffffff;
  static const uint32_t nbits27 = 0x07ffffff;
  static const uint32_t nbits28 = 0x0fffffff;
  static const uint32_t nbits29 = 0x1fffffff;
  static const uint32_t nbits30 = 0x3fffffff;
  static const uint32_t nbits31 = 0x7fffffff;
};

} // end of namespace datatools

#endif // DATATOOLS_BIT_MASK_H_

