// prog_2.cxx

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

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/properties.ipp>
#include <datatools/utils/things.h>
#include <datatools/utils/things.ipp>

void test_00 ()
{
  std::clog << "Test 00 :" << std::endl;

  std::string filename = "b0.data";

  {
    double x = 3.14159;
    std::cout << "x = " << x << std::endl;
    std::ofstream fout (filename.c_str ());
    boost::archive::portable_binary_oarchive opba (fout);
    opba & x;
  }

  {
    double x;
    std::ifstream fin (filename.c_str ());
    boost::archive::portable_binary_iarchive ipba (fin);
    ipba & x;
    std::cout << "x = " << x << std::endl;
  }
  return;
}

void test_0 ()
{
  std::clog << "Test 0 :" << std::endl;

  std::string filename = "b.data";

  {
    datatools::utils::properties p;
    datatools::utils::properties p2;
    datatools::utils::properties p3;
    p.set_description ("test container");
    p2.set_description ("abcdefgh");
    p3.set_description ("ijkl");
    p.store_flag ("test");
    p.store ("pba_version", "5.0");
    p.store ("pba_major_version", 5);
    p.store ("pba_minor_version", 0);
    p.store ("pba_tag_version", "alpha");
    double x = std::numeric_limits<double>::quiet_NaN ();
    p.store ("nan", x);
    double y = std::numeric_limits<double>::infinity ();
    p.store ("infinity", y);
    
    p.tree_dump (std::cout, "p container before storing : ");
    std::ofstream fout (filename.c_str ());
    boost::archive::portable_binary_oarchive opba (fout);
    opba & p;
    opba & p2;
    opba & p3;

    std::string xfilename = "b.xml";
    std::ofstream xfout (xfilename.c_str ());
    boost::archive::xml_oarchive ox (xfout);
    ox & boost::serialization::make_nvp ("p", p);
    ox & boost::serialization::make_nvp ("p2", p2);
    ox & boost::serialization::make_nvp ("p3", p3);
  }

  {
    datatools::utils::properties p;
    datatools::utils::properties p2;
    datatools::utils::properties p3;
    std::ifstream fin (filename.c_str ());
    boost::archive::portable_binary_iarchive ipba (fin);
    ipba & p;
    ipba & p2;
    ipba & p3;
    p.tree_dump (std::cout, "p container is loaded as : ");
    p2.tree_dump (std::cout, "p2 container is loaded as : ");
    p3.tree_dump (std::cout, "p3 container is loaded as : ");
  }

  if (boost::filesystem::exists ("a.data"))
  {
    std::clog << "Crosscheck : " << '\n';
    std::string af = "b.data";
    datatools::utils::properties p;
    datatools::utils::properties p2;
    datatools::utils::properties p3;
    std::ifstream fin (af.c_str ());
    boost::archive::portable_binary_iarchive ipba (fin);
    ipba & p;
    ipba & p2;
    ipba & p3;
    p.tree_dump (std::cout, "p container is loaded as : ");
    p2.tree_dump (std::cout, "p2 container is loaded as : ");
    p3.tree_dump (std::cout, "p3 container is loaded as : ");
  }
  return;
}

int main (void)
{
  int exit_code = EXIT_SUCCESS;
  //test_00 ();
  test_0 ();

  return exit_code;
}

// end of prog_2.cxx
