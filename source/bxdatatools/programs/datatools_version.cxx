// datatools_version.cxx

#include <cstdlib>
#include <iostream>

#include <datatools/datatools_version.h>
#include <CLHEP/ClhepVersion.h>
#include <boost/version.hpp>
#include <patchlevel.h> // Python version number

using namespace std;

int main (int argc_ , char ** argv_)
{
  cout << "datatools version="
       << DATATOOLS_LIB_VERSION 
       << endl;
  cout << "CLHEP version="
       << CLHEP::Version::String ()  
       << endl;
  cout << "Boost version="
       << BOOST_LIB_VERSION  
       << endl;
  cout << "Python version="
       << PY_VERSION  
       << endl;
  return (EXIT_SUCCESS);
}

// end of datatools_version.cxx
