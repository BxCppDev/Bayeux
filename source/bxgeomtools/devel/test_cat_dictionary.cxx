// -*- mode: c++; -*- 
// test_cat_dictionary.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <GeomUtils/cat_dictionary.h>

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
    
    geomutils::cat_dictionary my_dict;

    std::string   cat_db_filename = "./resources/cat1.dict";
    std::string   cat_db_filename2 = "./resources/cat2.dict";

    std::ifstream cat_db(cat_db_filename.c_str());
    if ( ! cat_db ) {
      std::ostringstream message;
      message << "File '" << cat_db_filename << "' does not exist!";
      throw std::runtime_error(message.str());
    }
    std::ifstream cat_db2(cat_db_filename2.c_str());
    if ( ! cat_db2 ) {
      std::ostringstream message;
      message << "File '" << cat_db_filename2 << "' does not exist!";
      throw std::runtime_error(message.str());
    }
    my_dict.load(cat_db);
    my_dict.load(cat_db2);
    my_dict.dump(std::cout);

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

// end of test_cat_dictionary.cxx
