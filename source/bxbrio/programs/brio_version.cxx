// brio_version.cxx

#include <cstdlib>
#include <iostream>

#include <brio/brio_version.h>
#include <datatools/datatools_version.h>
#include <boost/version.hpp>
#include <RVersion.h> // Root version

using namespace std;

int main (int argc_ , char ** argv_)
{
  cout << "brio version="
       << BRIO_LIB_VERSION 
       << endl;
  cout << "datatools version="
       << DATATOOLS_LIB_VERSION 
       << endl;
  cout << "Boost version="
       << BOOST_LIB_VERSION  
       << endl;
  cout << "ROOT version="
       << ROOT_RELEASE  
       << endl;
  return (EXIT_SUCCESS);
}

// end of brio_version.cxx
