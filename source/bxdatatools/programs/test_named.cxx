// -*- mode: c++; -*- 
// test_named.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <datatools/utils/i_named.h>

struct thing : public datatools::utils::i_named
{
private:
  std::string __prefix;
  int         __id;
public:
  thing( const std::string & prefix_ , int id_ ) :
    __prefix(prefix_), __id(id_)
  {
  }
  virtual std::string get_name()
  {
    std::ostringstream name_ss;
    name_ss << __prefix << __id;
    return name_ss.str();
  }
};

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {

    std::cerr << "Hello, this is a sample program for interface 'i_named'!" << std::endl; 
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      iarg++;
    }
    
    thing t1("t",1);
    thing t2("t",2);
    thing t3("t",666);

    std::cerr << "t1 is named '" << t1.get_name() << "'" <<  std::endl;
    std::cerr << "t2 is named '" << t2.get_name() << "'" <<  std::endl;
    std::cerr << "t3 is named '" << t3.get_name() << "'" <<  std::endl;


  }
  catch(std::exception & x)
    { 
      std::cerr << "error: " << x.what() << std::endl; 
      error_code=EXIT_FAILURE;
    }
  catch(...) 
    { 
      std::cerr << "error: " << "unexpected error!" << std::endl;  
      error_code=EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_named.cxx 
  
