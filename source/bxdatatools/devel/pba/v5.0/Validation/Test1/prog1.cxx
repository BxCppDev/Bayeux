// prog_1.cxx

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include <portable_oarchive.hpp>
#include <portable_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "a.hpp"
#include "b.hpp"
#include "utils.hpp"


void test_0 ()
{
  std::clog << "Test 0 :" << std::endl;

  if (std::numeric_limits<double>::has_quiet_NaN)
    {
      std::clog << "  double quiet NaN = " 
                << std::numeric_limits<double>::quiet_NaN ()
                << " " <<  to_bin_format(std::numeric_limits<double>::quiet_NaN ())
                << std::endl;
    }
  else
    {
      std::clog << "  double has no quiet NaN" << std::endl; 
    }
  if (std::numeric_limits<double>::has_signaling_NaN)
    {
      std::clog << "  double signaling NaN = " 
                << std::numeric_limits<double>::signaling_NaN ()
                << " " <<  to_bin_format(std::numeric_limits<double>::quiet_NaN ())
                << std::endl;
    }
  else
    {
      std::clog << "  double has no signaling NaN" << std::endl; 
    }
  return;
}

void test_1 ()
{
  std::clog << "Test 1 :" << std::endl;

  std::string filename = "a.data";
#ifdef A_CLASS_ALT
  filename = "a_alt.data";
#endif
  std::cout << "PBA file is : " << filename << "\n"; 

  A a;
  a.u = 123;
  a.f = 3.141592;
  a.d = 2.7182818284590452354;
  a.s = "6.78 €, 10 £";
#ifdef A_WITH_WSTRING
  a.w = L"6.78 €, 10 £";
#endif // A_WITH_WSTRING

  B b;
  b.s = "Hello";

  {
    std::cout << "a is stored as : "; 
    a.print ();
    std::cout << std::endl;
    std::cout << "b is stored as : "; 
    b.print ();
    std::cout << std::endl;
    std::ofstream fout (filename.c_str ());
    eos::portable_oarchive opba (fout);
    opba & a;
    opba & b;
  }
  {
    std::string xfilename = "a.xml";
#ifdef A_CLASS_ALT
    xfilename = "a_alt.xml";
#endif
    std::ofstream xfout (xfilename.c_str ());
    boost::archive::xml_oarchive ox (xfout);
    ox & boost::serialization::make_nvp ("a", a);
    ox & boost::serialization::make_nvp ("b", b);
    std::cout << "XML file is : " << xfilename << "\n"; 
  }
  a.reset ();
  b.reset ();

  {
    std::ifstream fin (filename.c_str ());
    eos::portable_iarchive ipba (fin);
    ipba & a;
    ipba & b;
    std::cout << "a is loaded as : "; 
    a.print ();
    std::cout << std::endl;
    std::cout << "b is loaded as : "; 
    b.print ();
    std::cout << std::endl;
  }

#ifndef A_CLASS_ALT
  if (boost::filesystem::exists ("b.data"))
  {
    std::cout << "Crosscheck as : \n"; 
    std::ifstream fin ("b.data");
    eos::portable_iarchive ipba (fin);
    ipba & a;
    ipba & b;
    std::cout << "a is loaded as : "; 
    a.print ();
    std::cout << std::endl;
    std::cout << "b is loaded as : "; 
    b.print ();
    std::cout << std::endl;
  }
#endif
  return;
}

void test_2 ()
{
  std::clog << "Test 2 :" << std::endl;

  std::string filename = "c.data";
#ifdef A_CLASS_ALT
  filename = "c_alt.data";
#endif

  A a;
  a.u = 0xFFFFFFFF;
  a.f = std::numeric_limits<float>::quiet_NaN ();
  a.d = std::numeric_limits<double>::infinity ();
  a.s = "€£eéèêçàäüiïîoöô東京فارسیгород";
#ifdef A_WITH_WSTRING
  a.w = L"東京فارسیгород";
#endif // A_WITH_WSTRING

  {
    std::cout << "a is stored as : "; 
    a.print ();
    std::cout << std::endl;
    std::ofstream fout (filename.c_str ());
    eos::portable_oarchive opba (fout);
    opba & a;
  }

  a.reset ();

  {
    std::ifstream fin (filename.c_str ());
    eos::portable_iarchive ipba (fin);
    ipba & a;
    std::cout << "a is loaded as : "; 
    a.print ();
    std::cout << std::endl;
  }

  return;
}

int main (void)
{
  int exit_code = EXIT_SUCCESS;
  //test_0 ();
  test_1 ();
  //test_2 ();

  return exit_code;
}

// end of prog_1.cxx
