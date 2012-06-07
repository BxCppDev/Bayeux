// prog2.cxx

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/properties.ipp>
#include <datatools/utils/things.h>
#include <datatools/utils/things.ipp>

void test_0 ()
{
  std::clog << "Test 0 :" << std::endl;

  std::string filename = "b.data";
  std::string filename2 = "a.data";

  {
    boost::shared_ptr<datatools::utils::properties> sp (new datatools::utils::properties);
    boost::shared_ptr<datatools::utils::properties> sp2 (new datatools::utils::properties);
    boost::shared_ptr<datatools::utils::properties> sp3 (new datatools::utils::properties);
    
    datatools::utils::properties & p = *sp.get();
    datatools::utils::properties & p2 = *sp2.get();
    datatools::utils::properties & p3 = *sp3.get();
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
    opba & sp;
    opba & sp2;
    opba & sp3;
    opba & sp;

    std::string xfilename = "b.xml";
    std::ofstream xfout (xfilename.c_str ());
    boost::archive::xml_oarchive ox (xfout);
    ox & boost::serialization::make_nvp ("sp", sp);
    ox & boost::serialization::make_nvp ("sp2", sp2);
    ox & boost::serialization::make_nvp ("sp3", sp3);
    ox & boost::serialization::make_nvp ("sp", sp);
  }

  {
    boost::shared_ptr<datatools::utils::properties> sp;
    boost::shared_ptr<datatools::utils::properties> sp2;
    boost::shared_ptr<datatools::utils::properties> sp3;
    std::ifstream fin (filename.c_str ());
    boost::archive::portable_binary_iarchive ipba (fin);
    ipba & sp;
    ipba & sp2;
    ipba & sp3;
    ipba & sp;
    const datatools::utils::properties & p  = *sp.get();
    const datatools::utils::properties & p2 = *sp2.get();
    const datatools::utils::properties & p3 = *sp3.get();
    p.tree_dump (std::cout, "p container is loaded as : ");
    p2.tree_dump (std::cout, "p2 container is loaded as : ");
    p3.tree_dump (std::cout, "p3 container is loaded as : ");
  }

  if (boost::filesystem::exists (filename2))
  {
      std::cout << "Crosscheck:\n";
    boost::shared_ptr<datatools::utils::properties> sp;
    boost::shared_ptr<datatools::utils::properties> sp2;
    boost::shared_ptr<datatools::utils::properties> sp3;
    std::ifstream fin (filename2.c_str ());
    boost::archive::portable_binary_iarchive ipba (fin);
    ipba & sp;
    ipba & sp2;
    ipba & sp3;
    ipba & sp;
    const datatools::utils::properties & p  = *sp.get();
    const datatools::utils::properties & p2 = *sp2.get();
    const datatools::utils::properties & p3 = *sp3.get();
    p.tree_dump (std::cout, "p container is loaded as : ");
    p2.tree_dump (std::cout, "p2 container is loaded as : ");
    p3.tree_dump (std::cout, "p3 container is loaded as : ");
  }

  return;
}

int main (void)
{
  int exit_code = EXIT_SUCCESS;

  test_0 ();

  return exit_code;
}

// end of prog2.cxx
