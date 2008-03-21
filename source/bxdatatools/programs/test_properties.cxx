// -*- mode: c++; -*- 
// test_properties.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/serialization/io_factory.h>

#include <datatools/utils/properties.h>

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try 
    {
      
      std::cerr << "Hello, this is a sample program for class 'properties'!" << std::endl; 
  
      bool debug=false;
      bool use_validator=true;
      long seed=12345;

      int iarg=1;
      while (iarg < argc_) 
	{
	  std::string arg=argv_[iarg];
	  if ( arg == "-d" || arg == "--debug" ) debug=true;
	  if ( arg == "-v" || arg == "--no-validator" ) use_validator=false;
	  iarg++;
	}

      srand48(seed);

      datatools::utils::properties::g_debug = debug;

      std::cerr << "========================================" << std::endl;

      datatools::utils::properties::data my_data('I',3);
      my_data.dump(std::cerr);
      std::cerr << std::endl;

      if (int error=my_data.set_value(666)) 
	{
	  std::cerr << "1 - datatools::utils::properties::data::set_value: " 
		    << datatools::utils::properties::data::get_error_message(error) 
		    << "!" << std::endl;
	}
      my_data.lock();
      my_data.dump(std::cerr);
      std::cerr << std::endl;

      if (int error=my_data.set_value(666,2)) 
	{
	  std::cerr << "2 - datatools::utils::properties::data::set_value: " 
		    << datatools::utils::properties::data::get_error_message(error) << "!" << std::endl;
	}
      my_data.unlock();
      my_data.dump(std::cerr);
      std::cerr << std::endl;

      if (int error=my_data.set_value(666,2)) 
	{
	  std::cerr << "3 - datatools::utils::properties::data::set_value: " 
		    << datatools::utils::properties::data::get_error_message(error) << "!" << std::endl;
	}
      my_data.dump(std::cerr);
      std::cerr << std::endl;
    
      if (int error=my_data.set_value(666,8)) 
	{
	  std::cerr << "4 - datatools::utils::properties::data::set_value: " 
		    << datatools::utils::properties::data::get_error_message(error) << "!" << std::endl;
	}
      my_data.dump(std::cerr);
      std::cerr << std::endl;
    
      if (int error=my_data.set_value("bad value"))
	{
	  std::cerr << "5 - datatools::utils::properties::data::set_value: " 
		    << datatools::utils::properties::data::get_error_message(error) << "!" << std::endl;
	}
      my_data.dump(std::cerr);
      std::cerr << std::endl;

      std::cerr << "========================================" << std::endl;
      std::cerr << "========================================" << std::endl;
      std::cerr << "========================================" << std::endl;

      datatools::utils::properties my_dict("a list of user properties");
      if ( !use_validator ) 
	{
	  my_dict.unset_key_validator();
	}
      my_dict.dump(std::cerr);
      std::cerr << std::endl;
      std::cerr << "========================================" << std::endl;

      //datatools::utils::properties::g_debug=true;
      my_dict.store("name","my name");
      std::cerr << "========================================" << std::endl;
      my_dict.store("firstname","my firstname");


      try 
	{
	  my_dict.store("#number",666);
	}
      catch(std::exception & x) 
	{
	  std::cerr << "ERROR: " << x.what() << std::endl;
	}
      my_dict.store("age",24,"the age of the captain");

      try 
	{
	  my_dict.store("007","James Bond");
	}
      catch(std::exception & x) 
	{
	  std::cerr << "ERROR: " << x.what() << std::endl;
	}
    
      try 
	{
	  my_dict.store("","Money Penny");
	}
      catch(std::exception & x) 
	{
	  std::cerr << "ERROR: " << x.what() << std::endl;
	}

      try 
	{
	  my_dict.store("bad_token","a bad \"char\"");
	}
      catch(std::exception & x) 
	{
	  std::cerr << "ERROR: " << x.what() << std::endl;
	}
    
      my_dict.store("weight",23.4);
      my_dict.store("male",true);
      std::vector<double> vals;
      vals.push_back(1.0);
      vals.push_back(2.0);
      vals.push_back(3.0);
      my_dict.store("position",vals,"the position of an object (mm)");
      my_dict.dump(std::cerr);
      std::cerr << std::endl;

      my_dict.change("position",3.14,1);
      my_dict.change("age",77);
      my_dict.lock("age");
      my_dict.change("male",false);

      my_dict.erase("weight");
      my_dict.dump(std::cerr);
      bool male;
      my_dict.fetch("male",male);
      int age;
      my_dict.fetch("age",age);
      std::vector<double> vals1;
      my_dict.fetch("position",vals1);
      std::cerr << std::endl;

      std::vector<double> vals2;
      double s=3.14159;
      for ( int i=1; i<13; i++ ) 
	{
	  s/=i;
	  vals2.push_back(s);
	}
      my_dict.change("position",vals2);
      if ( debug ) my_dict.tree_dump(std::cerr,
				     "datatools::utils::properties","DEBUG: ");
      std::cerr << std::endl;

      std::vector<bool> bits;
      for ( int i=0; i<23; i++ ) bits.push_back(drand48()>0.6?true:false);
      my_dict.store("bits",bits);
      std::cerr << std::endl;

      std::vector<std::string> tokens;
      for ( int i=0; i<12; i++ ) 
	{
	  std::ostringstream tok;
	  tok << "token_" << i;
	  tokens.push_back(tok.str());
	}
      tokens.push_back(">DEBUG<");
      my_dict.store("tokens",tokens);

      std::vector<int> counts;
      my_dict.store("counts",counts,"Event counts");

      my_dict.dump(std::cerr);
      if ( debug ) my_dict.tree_dump(std::cerr,"datatools::utils::properties","DEBUG: ");
      std::cerr << std::endl;

      std::cerr << "========================================" << std::endl;

      std::string filename = "test_properties.xml";
      std::cerr << "serialize: writing to '" << filename << "' ..." << std::endl;
      {
	datatools::serialization::data_writer writer(filename);
	writer.store(my_dict);
      }
      std::cerr << "serialize: writing done." << std::endl;
      std::cerr << std::endl;

      my_dict.clear();
      my_dict.tree_dump(std::cerr,"Dict after clear:");
      std::cerr << std::endl;
    
      std::cerr << "serialize: reading from '" << filename << "'..." 
		<< std::endl;
      {
	datatools::serialization::data_reader reader(filename);
	if ( reader.has_record_tag() ) 
	  {
	    if (reader.get_record_tag() == datatools::utils::properties::SERIAL_TAG) 
	      {
		datatools::utils::properties a_dict;
		reader.load(a_dict);
		my_dict = a_dict;
	      }
	    else 
	      {
		std::cerr << "Record tag : '"<< reader.get_record_tag() << "'" << std::endl;
	      }
	  }
      }
    
      std::cerr << "serialize: reading done." << std::endl;
      std::cerr << std::endl; 
      my_dict.tree_dump(std::cerr,"Dict after reading:");

      std::cerr << "========================================" << std::endl;

      std::string filename_cfg = "test_properties.conf";

      datatools::utils::properties::write_config(filename_cfg,my_dict);
      datatools::utils::properties::write_config("",my_dict);
      my_dict.clear();
      my_dict.dump(std::cerr);
      std::cerr << std::endl;

      std::cerr << "========================================" << std::endl;

      datatools::utils::properties::read_config(filename_cfg,my_dict);

      my_dict.dump(std::cerr);
      std::cerr << std::endl;

      std::cerr << "========================================" << std::endl;

      std::cerr << "Enter a list of strings (\"hello\",\"world\"): " 
		<< std::endl;
      std::string as;
      std::getline(std::cin,as);

      std::string s2;
      std::istringstream iss(as);

      do 
	{
	  iss >> std::ws;
	  if ( iss.eof() ) break;
	  s2="";
	  if (datatools::utils::properties::config::read_quoted_string(iss,s2))
	    {
	      std::cerr << "String = '" << s2 << "'" << std::endl;
	    }
	  else 
	    {
	      std::cerr << "Cannot parse quoted string from '" << as 
			<< "'!" << std::endl;
	    }
	} while (iss);
    
      std::cerr << "========================================" << std::endl;

      std::string s3;

      s3 = "azerty";

      if (datatools::utils::properties::data::has_forbidden_char(s3)) 
	{
	  std::cerr << "ok! there are forbidden chars found in 's3'!" 
		    << std::endl;
	}
      else 
	{
	  std::cerr << "'s3' is ok!" << std::endl;
	}
    
      std::string s4 = "aze\"rty";

      if (datatools::utils::properties::data::has_forbidden_char(s4))
	{
	  std::cerr << "ok! there are forbidden chars found in 's4'!"
		    << std::endl;
	}
      else 
	{
	  std::cerr << "'s4' is ok!" << std::endl;
	}
 
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

// end of test_simple_properties.cxx
