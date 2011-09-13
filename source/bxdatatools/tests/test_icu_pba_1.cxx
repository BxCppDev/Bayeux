/* test_icu_pba_1.cxx */

/**

Check output :
$ od -c test_icu_pba_1 
0000000 177 001  \t 001   2   H   e   l   l   o   ,       W   o   r   l
0000020   d       !       a 303 240 303 244   c 303 247   e 303 251 303
0000040 250 303 252 303 253   i 303 257 303 256   o 303 266 303 264   u
0000060 303 271 303 274   A 303 202
0000067

 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

// TEST NOT SUPPORTED:
// #define BOOST_PBA_ICU_SUPPORT

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>

using namespace std;
void test ()
{
  {
    //          "012345678901234567890123456789012345";
    string s1 = "Hello, World ! aàäcçeéèêëiïîoöôuùüAÂ";
    cout << "s1 len = '" << s1.length () << "'" << endl;
    cout << "s1  = '" << s1 << "'" << endl;
    ofstream fout ("test_icu_pba_1.bin", ios::binary);
    unsigned int flags (0);
#ifdef BOOST_PBA_ICU_SUPPORT
    flags |= boost::archive::icu_support;
#endif
    boost::archive::portable_binary_oarchive opba (fout, flags);
    opba & s1;
    
#ifdef BOOST_PBA_ICU_SUPPORT
    wstring ws1 = L"Hello, World ! aàäcçeéèêëiïîoöôuùüAÂ";
    cout << "ws1 len = '" << ws1.length () << "'" << endl;
    cout << "ws1 = '" << "NO REPRESENTATION" << "'" << endl;
    opba & ws1;
#endif
  }

  setenv ("LANG", "C", 1);

  {
    ifstream fin ("test_icu_pba_1.bin", ios::binary);
    unsigned int flags (0);
#ifdef BOOST_PBA_ICU_SUPPORT
    flags |= boost::archive::icu_support;
#endif
    boost::archive::portable_binary_iarchive ipba (fin, flags);
    string s1;
    ipba & s1;   
    cout << "s1 len = '" << s1.length () << "'" << endl;
    cout << "s1  = '" << s1 << "'" << endl;
#ifdef BOOST_PBA_ICU_SUPPORT
    wstring ws1;
    ipba & ws1;
    cout << "ws1 len = '" << ws1.length () << "'" << endl;
    cout << "ws1 = '" << "NO REPRESENTATION" << "'" << endl;
#endif
   }

  return;
}


int main (int argc_ , char ** argv_) 
{
  try 
    {
      test ();
    }
  catch (exception & x) 
    {
      cerr << "test_nans_ar: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

/* end of test_icu_pba_1.cxx */
