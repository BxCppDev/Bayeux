// -*- mode: c++; -*- 
// test_geom_id.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <GeomUtils/geom_id.h>

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      // <here you may add more switches...>

      iarg++;
    }
    
    geomutils::geom_id my_id1;
    my_id1.init(0,3); // category==0 depth==3
    my_id1.set_address_values(3,1,4);
    std::cout << "ID1=" << my_id1 <<  std::endl; 

    geomutils::geom_id my_id2;
    my_id2=my_id1;
    my_id2.set_address_values(2,1,6);
    std::cout << "ID2=" << my_id2 <<  std::endl; 
    
    if ( my_id1 == my_id2 ) std::cout << "ID1 and ID2 match!" << std::endl;
    else std::cout << "ID1 and ID2 don't match!" << std::endl;

    geomutils::geom_id my_id3;
    my_id3.init(1,2); // category==1 depth==2
    geomutils::geom_id::extract(my_id2,my_id3);
    std::cout << "ID3=" << my_id3 <<  std::endl; 
   
  }
  catch(std::exception & x){
    std::cerr << "error: " << x.what() << std::endl; 
    error_code=EXIT_FAILURE;
  }
  catch(...){
    std::cerr << "error: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_geom_id.cxx
