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
 * It shows how to store some basic types (integer, floating numbers)
 * and also STL objects (std::string) in a portable binary archive format 
 * associated to a file stream.
 *
 * Particularly, it uses non-finite floating numbers (infinity, NaN).
 * 
 */

#include <string>
#include <fstream>
#include <limits>

#include <boost/cstdint.hpp> // Support for portable integer types:

#include <boost/archive/portable_binary_oarchive.hpp>

void demo_integer ()
{
  using namespace std;

  cout << "\ndemo_pba_2 : demo_integer: " << endl;

  // Some (unsigned) integer numbers with various sizes :

  // char :
  uint8_t c0 =   0;
  uint8_t c1 = 'a';
  uint8_t c2 = 255;

  // short-like int:
  uint16_t s0 =     1;
  uint16_t s1 = 65535;

  // long-like int :
  uint32_t l0 = 1;
  uint32_t l1 = 0xFFFFFFFF;

  // long long-like int :
  uint64_t ll0 = 0;
  uint64_t ll1 = 0xFFFFFFFFFFFFFFFF;

  cout << "\tc0  = `" << c0 << "' (byte=" << (int) c0 << ", not printable)" << endl;
  cout << "\tc1  = `" << c1 << "' (byte=" << (int) c1 << ", from ASCII table)" << endl;
  cout << "\tc2  = `" << c2 << "' (byte=" << (int) c2 << ", not an ASCII character)" << endl;
  cout << "\ts0  = " << s0 << endl;
  cout << "\ts1  = " << s1 << endl;
  cout << "\tl0  = " << l0 << endl;
  cout << "\tl1  = " << l1 << endl;
  cout << "\tll0 = " << ll0 << endl;
  cout << "\tll1 = " << ll1 << endl;

  {
    // The name for the example data file :  
    string filename = "demo_pba_2a.data";

    // Output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);

    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);

    // Saving numbers...
    opba & c0 & c1 & c2 & s0 & s1 & l0 & l1 & ll0 & ll1;

    cout << "\tInteger values have been stored in '" << filename  << "'." << endl;

    /**
     * Display and check the contents of the output file (unix terminal):
     *
     *   unix-shell> od -t u1 demo_pba_2a.data | tr -s [[:space:]] | cut -d ' ' -f2- | head -2
     *   127 1 9 0 1 97 1 255 1 1 2 255 255 1 1 4
     *   255 255 255 255 0 8 255 255 255 255 255 255 255 255
     *
     * Where :
     *  127 1 9           : `magic' byte and Boost serialization archive 
     *                       version (here the integer value `9' using only 1 byte)
     *  0                 : c0 == 0
     *  1 97              : c1 == 97 (ASCII code for `a' using only 1 byte)
     *  1 255             : c2 == 255 (8-bit code for a non printable character using only 1 byte)
     *  1 1               : s0 == 1 (using only 1 byte)
     *  2 255 255         : s1 == 65535 (using only 2 bytes)
     *  1 1               : l0 == 1 (using only 1 byte)
     *  4 255 255 255 255 : l1 == 4294967295 (using only 4 bytes)
     *  0                 : ll0 == 0 (a shortcut for saving space)
     *  8 255 255 255 255 255 255 255 255 : 
     *                      ll1 == 18446744073709551615 as the max positive 64-bit integer, 
     *                      using 8 storage bytes.
     *
     */

  }
  return;
}

void demo_double ()
{
  using namespace std;

  cout << "\ndemo_pba_2 : demo_double: " << endl;
  // A normalized double precision number :
  double pi = 3.14159265358979;

  // A double precision number with `+Infinity' value :
  double plus_infinity = numeric_limits<double>::infinity ();

  // A double precision `Not-a-Number' (NaN):
  double nan = numeric_limits<double>::quiet_NaN ();

  cout.precision (15);
  cout << "\tpi            = " << pi << endl;
  cout << "\tplus_infinity = " << plus_infinity << " (warning: not portable print format for cout)" << endl;
  cout << "\tnan           = " << nan << " (warning: not portable print format for cout)" << endl;


  {
    // The name for the example data file :  
    string filename = "demo_pba_2b.data";

    // Output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);

    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);

    // Saving numbers...
    opba & pi & plus_infinity & nan;

    cout << "\tDouble values have been stored in '" << filename  << "'." << endl;

    /**
     * Understanding the format of portable binary archive(s) :
     *
     * Display the byte contents of the output file:
     *
     *   unix-shell> od -t u1 demo_pba_2b.data | tr -s [[:space:]] | \
     *     cut -d ' ' -f2- | head -2 | tr '\n' ' ' | sed -e 's/ 8 / \n8 /g'
     *   127 1 9 
     *   8 17 45 68 84 251 33 9 64 
     *   8 0 0 0 0 0 0 240 127 
     *   8 255 255 255 255 255 255 255 127
     * 
     * Detailed comments about the format :
     *
     *  * Coding the archive header :
     *
     *    Here '127' is the `magic' byte used by the portable binary archive(s).
     *    Then '1 9' is the Boost archive version encoded as a positive integer using only one stored 
     *    byte to save space (hence the `1'); the `9' value is the version associated the Boost library 
     *    release 1.47.
     *
     *  * Coding 'pi' gives :
     *
     *    8 17 45 68 84 251 33 9 64 
     *
     *    with `8' bytes :
     *      17 == 00010001}
     *      45 == 00101101
     *      68 == 01000100
     *      84 == 01010100
     *     251 == 11111011
     *      33 == 00100001
     *       9 == 0000){1001
     *      64 == [0](1000000
     *
     *    where :
     *     - [0] is the sign bit : +
     *     - (10000000000) is the 11-bit exponent == 1024 (with pedestal 1023) = 2^{1024-1023) = 2
     *     - {1.001001000011111101101010100010001000010110100010001} is the 52-bit significand (mantissa) =
     *        1. + 2^-1 + 2^-4 + 2^-7 + 2^-12 + 2^-13 + 2^-14 + ... = 1.570796327 
     *       Note: `1.' is the conventional phantom bit.
     *   
     *  * Coding 'infinity' gives:
     *
     *    8 0 0 0 0 0 0 240 127 
     *
     *    with `8' bytes:
     *      0 == 0)
     *      ...
     *      0 == 0
     *    240 == 1111){0000 
     *    127 == [0](1111111
     *
     *    where :
     *     - [0] is the sign bit : +
     *     - (11111111111) is the 11-bit exponent == 2047) = 2^{1024-1023) = 2
     *     - {0000000000000000000000000000000000000000000000000000} is the 52-bit zeroed significand (mantissa) 
     *       this is +infinity after IEEE 754
     *   
     *  * Coding 'NaN' gives :
     *
     *    8 0 0 0 0 0 0 240 127 
     *
     *    with `8' bytes:
     *    255 == 11111111)
     *    ...
     *    255 == 1111){1111 
     *    127 == [0](1111111
     *
     *    where :
     *     - [0] is the sign bit : +
     *     - (11111111111) is the 11-bit exponent == 2047) = 2^{1024-1023) = 2
     *     - {1111111111111111111111111111111111111111111111111111} is the 52-bit non-zero significand (mantissa) 
     *       this is a `NaN' after IEEE 754
     *
     *  See also : 'http://en.wikipedia.org/wiki/IEEE_754-1985'
     *
     */
  }

  return;
}

void demo_string ()
{
  using namespace std;
 
  cout << "\ndemo_pba_2 : demo_string: " << endl;

  // A normalized double precision number :
  string s0 =  "";
  string s1 =  "Hello, World !";

  cout << "\ts0 = `" << s0 << "'" << endl;
  cout << "\ts1 = `" << s1 << "'" << endl;

  {
    // The name for the example data file :  
    string filename = "demo_pba_2c.data";

    // Output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);

    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);

    // Saving numbers...
    opba & s0 & s1;

    cout << "\tStrings have been stored in '" << filename  << "'." << endl;

    /**
     * Display and check the contents of the output file (unix terminal):
     *
     *   unix-shell> od -t u1 demo_pba_2c.data | tr -s [[:space:]] | cut -d ' ' -f2- | head -2 
     *   127 1 9 0 1 14 72 101 108 108 111 44 32 87 111 114 108 100 32 33
     *
     * Where :
     *  127 1 9           : `magic' byte and Boost serialization archive  version
     *  0                 : s0 == is an empty string which has zero length 
     *                      encoded as a zero-optimized integer number
     *  1 14              : s1 == is an string with length 14 encoded using a 1 byte integer number
     *                      then comes the ASCII codes of the 14 characters in the string :
     *  72 101 108 108 111  44  32  87 111 114 108 100  32  33
     * 'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' ' ' '!'
     *
     */
  }

  return;
}

int main (void)
{
  // run the demo function for serialization of integer numbers :
  demo_integer ();

  // run the demo function for serialization of double-precision floating numbers :
  demo_double (); 

  // run the demo function for serialization of STL strings :
  demo_string ();

  return 0;   
}

// end of demo_pba_2.cpp
