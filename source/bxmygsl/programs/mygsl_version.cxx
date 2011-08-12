// mygsl_version.cxx

#include <cstdlib>
#include <iostream>

#include <mygsl/mygsl_version.h>
#include <gsl/gsl_version.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  cout << "mygsl version="
       << MYGSL_LIB_VERSION 
       << endl;
  cout << "GSL version="
       << GSL_VERSION  
       << endl;
  return (EXIT_SUCCESS);
}

// end of mygsl_version.cxx
