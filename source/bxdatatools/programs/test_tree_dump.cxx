// -*- mode: c++; -*- 
// test_tree_dump.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/i_tree_dump.h>

struct data_t : public datatools::utils::i_tree_dumpable
{
  int i;
  double x;
  std::string name;

  data_t() : i(0) , x(3.145) , name("Hello, world!") {}

  virtual void tree_dump( std::ostream & out_         = std::cerr , 
			  const std::string & title_  = "" ,
			  const std::string & indent_ = "",
			  bool inherit_               = false ) const
  {
    std::string indent;
    if ( !indent_.empty() ) indent = indent_;
    if ( !title_.empty() ) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << i_tree_dumpable::tag 
	 << "i    = " << i << std::endl;
    out_ << indent << i_tree_dumpable::tag 
	 << "x    = "  << x << std::endl;
    out_ << indent << i_tree_dumpable::inherit_tag(inherit_) 
	 << "name = "  
	 << name << std::endl; 
  }
  
};

struct more_data_t : public data_t
{
  int log;
  more_data_t() : data_t() , log(1) {}
  virtual void tree_dump( std::ostream & out_         = std::cerr , 
			  const std::string & title_  = "" ,
			  const std::string & indent_ = "",
			  bool inherit_               = false ) const
  {
    std::string indent;
    if ( !indent_.empty() ) indent = indent_;
    data_t::tree_dump(out_,title_,indent_,true);
    out_ << indent << i_tree_dumpable::inherit_tag(inherit_) 
	 << "log  = "  << log << std::endl;
  }

};

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {

    std::cerr << "Hello, this is a sample program for interface 'i_tree_dumpable'!" << std::endl; 
  
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      iarg++;
    }
    
    data_t my_data;
    
    my_data.tree_dump(std::cout,"my data",">>> ");
    
    more_data_t my_data2;
    
    my_data2.tree_dump(std::cout,"my data (2)",">>> ");

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

// end of test_dummy_datatools.cxx
