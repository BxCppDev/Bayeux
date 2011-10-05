/** demo_pba_1.cpp
 *
 * (C) Copyright 2011 François Mauger, Christian Pfligersdorffer
 *
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/**
 * The intent of this program is to serve as a tutorial for
 * users of the portable binary archive in the framework of 
 * the Boost/Serialization package. 
 *
 * It shows how to store some basic type (bool, integer and floating
 * numbers) in a portable binary archive format associated to a
 * file stream.
 *
 * Some details are given to explain the underlying binary format.
 *
 */

#include <string>
#include <fstream>

#include <boost/archive/portable_binary_oarchive.hpp>

int main (void)
{
  using namespace std;

  // Some variables of various primary types :
  bool b0                = false;
  bool b1                = true;
  char c0                = 0;
  char c                 = -3;
  unsigned char uc       = +6;
  short s                = -16;
  unsigned char us       = +32;
  long l                 = -128;
  unsigned long ul       = +127;
  long long ll           = -1024;
  unsigned long long ull = +2048;
  float f                = 3.14159;
  double e               = 2.71828182845905;
  
  /** Important remark:
   *
   * As the C/C++ primitive integer types are not garanteed
   * to have the same size on various architectures, it is
   * strongly recommended -- if not mandatory -- to use the 
   * 'aliased' types (int16_t, int32_t...) from the 
   * 'boost/cstdint.hpp' header rather than native types 
   * such (short, int, long types...) when using the Boost/Serialization
   * library.
   */

  {
    // The name for the example data file :  
    string filename = "demo_pba_1.data"; 

    // Output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);
  
    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);
  
    // Storing (serializing) values...
    opba & b0 & b1 & c0 & c & uc & s & us & l & ul & ll & ull & f & e;

    cout << "\tValues have been stored in '" << filename  << "'." << endl;

    /**
     * Understanding the format of portable binary archive(s) :
     *
     * It is possible to display the byte contents of the output file 
     * using some dedicated tool (here the 'od' program available from the
     * GNU coreutils package under Linux):
     *
     *   unix-shell> od -t u1 demo_pba_1.data | tr -s [[:space:]] | cut -d ' ' -f2- | head -3
     *   127 1 9 0 1 84 0 255 253 1 6 255 240 1 32 255
     *   128 1 127 254 0 252 2 0 8 4 208 15 73 64 8 116
     *   87 20 139 10 191 5 64
     * 
     * Detailed comments about the format :
     *
     *   127       : The conventional `magic' byte of the portable binary archive(s)
     *   1 9       : Boost archive version number value is 9 and it is encoded  using 1 storage byte
     *               Here a dedicated scheme for binary encoding of integer numbers is used
     *               (see below for other examples)
     *   0         : b0 == false (0)
     *   1 84      : b1 == true is encoded using 1 storage byte with conventional 
     *               value 84 (ASCII character 'T')
     *   0         : c0 == 0 (using only 1 byte with zero optimization )
     *   255 253   : where 255 -> -1 means a negative integer using 1 storage byte
     *               and value c = -3 (as an unsigned byte 11111101 -> 253)
     *   1 6       : uc == +6 (as a positive integer number using 1 storage byte)
     *   255 240   : where 255 -> -1 means a negative integer using 1 storage byte
     *               and value s = -16 (as an unsigned byte 11110000 -> 240)
     *   1 32      : us == +32 (as a positive integer number using 1 storage byte)
     *   255 128   : where 255 -> -1 means a negative integer using 1 storage byte
     *               and value l = -128 (as an unsigned byte 10000000 -> 128)
     *   1 127     : ul == +127 (as a positive integer number using 1 storage byte)
     *   254 0 252 : where 254 -> -2 means a negative integer using 2 storage bytes
     *               and bytes 252 + 0 (in reversed order we have binary = 11111100/00000000, 
     *               match a positive integer number == 64512) 
     *               it gives the value ll = 64512 - 65536 = -1024
     *   2 0 8     : where 2 means a positive integer using 2 storage bytes
     *               and bytes 8 + 0 (binary = 00001000.00000000) give
     *               value ull = 2048
     *   4 208 15 73 64 : 
     *               where 4 is the number of bytes used for a IEEE 754 encoded
     *               single precison floating number.
     *               the encoding is interpreted after bytes reordering :
     *               64 73 15 208 which corresponds to
     *               [0](1000000 0){1.1001001 00001111 11010000}
     *               with :
     *               * [0] is the sign bit
     *               * (10000000) is the 8-bit exponent == 128 to be shift by 127
     *                 thus 2^(128-127) = 2
     *               * {1.1001001 00001111 1101 0000} is the significant (mantissa)
     *                 note: `1.' is the conventional 'phantom' bit which is not stored.
     *                 Thus the significant part is:
     *                 1 + 1/2 + 1/16 + 1/128 + 1/4096 + 1/8192 + 1/16384
     *                   + 1/32768 + 1/65536 + 1/131072 + 1/524288 == 1.570795059
     *                Finally: f == + 2 * 1.570795059 = 3.141590118 (within a 1e^-7 roundoff error)
     *   8 116 87 20 139 10 191 5 64 : 
     *               where 8 is the number of bytes used for a IEEE 754 encoded
     *               double precison floating number. The same scheme is used as for 
     *               single precision  floating numbers but using 1 sign bit, 11-bit exponent
     *               and 52-bit mantissa.
     *               See also : 'http://en.wikipedia.org/wiki/IEEE_754-1985'
     */
  }

  return 0;   
}

// end of demo_pba_1.cpp
