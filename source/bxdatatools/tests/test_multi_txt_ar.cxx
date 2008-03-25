/* Sample file to show
 * how to work with multiple text archives
 * stored within the same file.
 * This program is free, free, free!
 */

#include <iostream>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

int main(void)
{
  bool add_separator = false;
  add_separator = true; // please comment this line to make it fail. 

  {
    boost::filesystem::path       out_path("test.txt"); 
    std::ofstream                 fout(out_path.string().c_str());
    boost::filesystem::path       xout_path("test.xml"); 
    std::ofstream                 xfout(xout_path.string().c_str());
    
    std::cerr << "writing..." << std::endl;
    uint32_t ndata = 3;
    std::cerr << "DEBUG: store: ndata=" << ndata << std::endl;
    { 
      const uint32_t & ndata_ref = 3;
      boost::archive::text_oarchive oar(fout);
      boost::archive::xml_oarchive xoar(xfout);
      oar  << ndata_ref;
      xoar << boost::serialization::make_nvp("ndata",ndata_ref);
    }
    
    if (add_separator) fout << std::endl;
    
    for (int i = 0; i < ndata; i++)
      {
	boost::archive::text_oarchive oar(fout);
	boost::archive::xml_oarchive xoar(xfout);
	std::cerr << "DEBUG: store: i=" << i << std::endl;
	const int & i_ref=i;
	oar << i_ref;
	xoar << boost::serialization::make_nvp("data",i_ref);
	if (add_separator) fout << std::endl;
      }
    fout.close();
    std::cerr << "write: done." << std::endl;
  }

  {  
    boost::filesystem::path       in_path("test.txt"); 
    std::ifstream                 fin(in_path.string().c_str());
    
    uint32_t ndata;
    { 
      boost::archive::text_iarchive iar(fin);
      iar >> ndata;
      std::cerr << "DEBUG: load: ndata=" << ndata << std::endl;
    }
    for (int i = 0; i < ndata; i++)
      {
	boost::archive::text_iarchive iar(fin);
	int j;
	iar >> j;
	std::cerr << "load: j=" << j << std::endl;
      }
  }
  return 0;
}

/* the end */
