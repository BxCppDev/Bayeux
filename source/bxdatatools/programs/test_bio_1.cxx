/* test_bio_1.cxx */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <algorithm>

#include <datatools/serialization/memory_streambuf.h>
#include <datatools/utils/time_tools.h>


#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/range/iterator_range.hpp>


#include <datatools/serialization/quasi_portable_binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

using namespace std;
using namespace datatools::serialization;

void print_char (char c_) 
{
  cout << " '" << c_ << "'";
}

void print_info (const vector<char> & v_)
{
  clog << "Print_Info:" << endl;
  clog << "|-- Contains  = `";
  
  size_t imax = 40;
  bool b = false;
  for (int i = 0; i < v_.size ();)
    {
      clog << v_[i];
      i++;
      if (i == imax) 
	{
	  b = true;
	  break;
	}
    }
  if (b)
    {
      clog << " ... ";
      for (int i = v_.size () - 20; i < v_.size (); i++)
	{
	  clog << v_[i];
	}
    }
  clog << "'" << endl;
  clog << "|-- Size      = " << v_.size () << endl;
  clog << "`-- Capacity  = " << v_.capacity () << " (max size=" << v_.max_size () << ")" << endl;
  return;
}

int main (int argc_, char ** argv_) 
{

  size_t max_data_sz  = 1000000;
  
  try  
    {
      bool debug = false;
      debug = true;
 
      {
	clog << "-----------------------------------------------------------------" << endl;
	clog << "Reading from a std::vector..." << endl;

	vector<char> v1;
	v1.assign (max_data_sz, '\0');
	for (int i = 0; i < v1.size (); i++)
	  {
	    v1[i] = 32 + i % 64; 
	  }
	v1[10] = 0;
	v1[15] = 0;
	print_info (v1);

	clog << ">>>>> Read = `";
	boost::iostreams::filtering_istream in (boost::make_iterator_range (v1));
	size_t count = 0;
 
	while (in)
	  {
	    char c;
	    in.get (c);
	    if (in.eof ()) break;
	    if (count < 40) clog << c;
	    else if (count == 40) clog << " ... ";
	    else if (count > (max_data_sz - 10)) clog << c;
	    count++;
	  }
	clog << "'" << " with count=" << count << endl;

	{
	  boost::iostreams::filtering_istream in2 (boost::make_iterator_range (v1));
	  datatools::utils::computing_time CT;
	  CT.start ();
	  while (in2)
	    {
	      char c;
	      in2.get (c);
	      if (in2.eof ()) break;
	    }
	  CT.stop ();
	  clog << "CPU elapsed time = " <<  CT.get_last_elapsed_time () / CLHEP::second << " sec" << endl;
	}
 	clog << endl << endl;
      }
      
      {
	clog << "-----------------------------------------------------------------" << endl;
	clog << "Reading from a char array (version 1)..." << endl;

	char * data = new char[max_data_sz];
	for (int i = 0; i < max_data_sz; i++) 
	  {
	    data[i] =  32 + i % 64;
	  }
	//clog << "Data = `" << data << "'" << " with size = " << max_data_sz << endl;
	data[10] = 0;
	data[15] = 0;

	datatools::serialization::iarraystream in (data, max_data_sz);
	size_t count = 0;
	clog << ">>>>> Read : `";
	while (! in.eof ()) 
	  { 
	    char c;
	    in.get (c);
	    if (in.eof ()) break;
	    count++;
	    if (count == 50) clog << " ... ";
	    else if (count < 50) clog << c;
	    else if (count > (max_data_sz - 10)) clog << c;
	  }   
	clog << "'" << " with count = " << count << endl;
	{
	  datatools::serialization::iarraystream in2 (data, max_data_sz);
	  datatools::utils::computing_time CT;
	  CT.start ();
	  while (in2)
	    {
	      char c;
	      in2.get (c);
	      if (in2.eof ()) break;
	    }
	  CT.stop ();
	  clog << "CPU elapsed time = " <<  CT.get_last_elapsed_time () / CLHEP::second << " sec" << endl;
	}
	delete []  data;
	clog << endl << endl;
      }	

      {
	clog << "-----------------------------------------------------------------" << endl;
	clog << "Reading from a char array (version 2)..." << endl;

	char * data = new char[max_data_sz];
	for (int i = 0; i < max_data_sz; i++) 
	  {
	    data[i] =  32 + i % 64;
	  }
	//clog << "Data = `" << data << "'" << " with size = " << max_data_sz << endl;
	data[10] = 0;
	data[15] = 0;

	boost::iostreams::stream<boost::iostreams::array_source>  in (data, max_data_sz);
	size_t count = 0;
	clog << ">>>>> Read : `";
	while (! in.eof ()) 
	  { 
	    char c;
	    in.get (c);
	    if (in.eof ()) break;
	    count++;
	    if (count == 50) clog << " ... ";
	    else if (count < 50) clog << c;
	    else if (count > (max_data_sz - 10)) clog << c;
	  }   
	clog << "'" << " with count = " << count << endl;
	{
	  boost::iostreams::stream<boost::iostreams::array_source>  in2 (data, max_data_sz);
	  datatools::utils::computing_time CT;
	  CT.start ();
	  while (in2)
	    {
	      char c;
	      in2.get (c);
	      if (in2.eof ()) break;
	    }
	  CT.stop ();
	  clog << "CPU elapsed time = " <<  CT.get_last_elapsed_time () / CLHEP::second << " sec" << endl;
	}
	delete []  data;
 	clog << endl << endl;
      }	

      max_data_sz  = 100000;

      {
	clog << "-----------------------------------------------------------------" << endl;
	clog << "Writing to a vector of char (version 1)..." << endl;
	namespace io = boost::iostreams;
	typedef std::vector<char> buffer_type;
	buffer_type buffer;

	io::stream<io::back_insert_device<buffer_type> > output_stream (buffer);

	eos::portable_oarchive oa (output_stream);
	//boost::archive::text_oarchive oa (output_stream);

	std::vector<double> data (max_data_sz, 0.0);
	srand48 (12345);
	for (int i = 0; i < data. size(); i++) 
	  {
	    data[i] = drand48 ();
	  }

	datatools::utils::computing_time CT;
	CT.start ();
	oa << data;
	output_stream.flush();
	CT.stop ();
	cout << "Final buffer is : " << endl;
	print_info (buffer);
	clog << "CPU elapsed time = " <<  CT.get_last_elapsed_time () / CLHEP::second << " sec" << endl;
 
	clog << endl << endl;
      }

      {
	clog << "-----------------------------------------------------------------" << endl;
	clog << "Writing to a vector of char (version 2)..." << endl;
	namespace io = boost::iostreams;

	ovectorstream output_stream (4096); 

	eos::portable_oarchive oa (output_stream);
	//boost::archive::text_oarchive oa (output_stream);

	std::vector<double> data (max_data_sz, 0.0);
	srand48 (12345);
	for (int i = 0; i < data. size(); i++) 
	  {
	    data[i] = drand48 ();
	  }

	datatools::utils::computing_time CT;
	CT.start ();
	oa << data;
	output_stream.flush ();
	CT.stop ();
	cout << "Final buffer is : " << endl;
	print_info (output_stream.buffer ());
	clog << "CPU elapsed time = " <<  CT.get_last_elapsed_time () / CLHEP::second << " sec" << endl;

	clog << endl << endl;
      }   

    } 
  catch (exception & x) 
    {
      cerr << "test_bio_1: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
 
  return (EXIT_SUCCESS);
}

/* end of test_bio_1.cxx */
