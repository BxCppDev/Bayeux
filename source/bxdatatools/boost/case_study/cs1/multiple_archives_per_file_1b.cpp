// multiple_archives_per_file.cpp

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/tokenizer.hpp>

#include "foo.hpp"

using namespace std;

struct archive_tools
{
  enum archive_mode_t
    {
      unknown = -1,
      txt = 0,
      xml = 1,
      bin = 2
    };

  static string get_archive_format_label (int format_)
  {
    string label;
    switch (format_)
      {
      case txt: label = "ASCII"; break;
      case xml: label = "XML"; break;
      case bin: label = "Binary"; break;
      default: label = "<unknown>";
      }
    return label;
  }

  static string get_archive_format_extension (int format_)
  {
    string ext;
    switch (format_)
      {
      case txt: ext = ".txt"; break;
      case xml: ext = ".xml"; break;
      case bin: ext = ".bin"; break;
      }
    if (ext.empty ())
      {
	throw std::runtime_error ("archive_tools::get_archive_format_extension: Invalid archive format!");
      }
    return ext;
  }

  static int guess_archive_format_from_filename (const string & filename_)
  {
    boost::char_separator<char> sep (".");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens (filename_, sep);
    list<string> ltok;
    for(tokenizer::iterator i = tokens.begin (); i != tokens.end(); ++i) 
      {
	string token = *i;
	ltok.push_front (token);
      }
    if (ltok.size () < 2) return unknown;
    const string & ext = ltok.back ();
    if (ext == "bin")
      {
	return bin;
      }
    if (ext == "xml")
      {
	return xml;
      }
    if (ext == "txt")
      {
	return txt;
      }
    return unknown;
  }
};

class archive_writer
{
private:
  boost::archive::text_oarchive   * __toa;
  boost::archive::xml_oarchive    * __xoa;
  boost::archive::binary_oarchive * __boa;
  ostream * __out;
public:
  archive_writer (ostream & out_, int format_)
  {
    __toa = 0;
    __xoa = 0;
    __boa = 0;
    __out = &out_;
    if (format_ == archive_tools::xml)
      {
	__xoa = new boost::archive::xml_oarchive (*__out);
      }
    else if (format_ == archive_tools::bin)
      {
	__boa = new boost::archive::binary_oarchive (*__out);
      }
    else if (format_ == archive_tools::txt)
      {
	__toa = new boost::archive::text_oarchive (*__out);
      }
    else
      {
	throw runtime_error ("archive_writer::ctor: Invalid archive format!");
      }
  }

  ~archive_writer ()
  {
    if (__toa) delete __toa;
    else if (__xoa) delete __xoa;
    else if (__boa) delete __boa;
  }

  template <class Data>
  void store (const Data & data_)
  {
    if (__toa) 
      {
	*__toa << data_;
      }
    if (__xoa) 
      {
	*__xoa << boost::serialization::make_nvp ("data", data_);
      }
    if (__boa) 
      {
	*__boa << data_;
      }
  }
};

class archive_reader
{
private:
  boost::archive::text_iarchive   * __tia;
  boost::archive::xml_iarchive    * __xia;
  boost::archive::binary_iarchive * __bia;
  istream * __in;
  bool __done;

public:
  bool is_done ()
  {
    return __done;
  }

  void __check_done ()
  {
    //clog << "archive_reader::__check_done: entering..." << endl; 
    if (! *__in) 
      {
	__done = true;
	return;
      }
    // check EOF:
    char c = 0; 
    __in->get (c); 
    if (__in->eof ())
      {
	//clog << "archive_reader::__check_done: EOF!" << endl; 
	__done = true;
	return;
      }
    if (__in->good ())
      {
	//clog << "archive_reader::__check_done: c='" << c << "'" << endl; 
	__in->putback (c); 
      }
    else 
      {
	//clog << "archive_reader::__check_done: EOF!" << endl; 
	__done = true;
	return;
      }
   }

  archive_reader (istream & in_, int format_)
  {
    //clog << "archive_reader::ctor: entering..." << endl; 
    __tia = 0;
    __xia = 0;
    __bia = 0;
    __in  = &in_;
    __done = false;
    __check_done ();
    if (__done) 
      {
	throw runtime_error ("archive_reader::ctor: bad or EOF archive!");
      }
    if (format_ == archive_tools::xml)
      {
	__xia = new boost::archive::xml_iarchive (*__in);
      }
    else if (format_ == archive_tools::bin)
      {
	__bia = new boost::archive::binary_iarchive (*__in);
      }
    else if (format_ == archive_tools::txt)
      {
	__tia = new boost::archive::text_iarchive (*__in);
      }
    else
      {
	throw runtime_error ("archive_reader::ctor: Invalid archive format!");
      }
    //clog << "archive_reader::ctor: exiting." << endl; 
  }

  ~archive_reader ()
  {
    //clog << "archive_reader::dtor: entering..." << endl; 
    if (__tia) 
      {
	delete __tia;
	*__in >> ws; // eat trailing 'space' chars
      }
    else if (__xia) 
      {
	delete __xia;
	//clog << "archive_reader::dtor: eat trailing '\\n'..." << endl; 
	*__in >> ws; // eat trailing '\n'
	if (__in->eof ())
	  {
	    //clog << "archive_reader::dtor: EOF!" << endl; 
	  }
      }
    else if (__bia) 
      {
	delete __bia;
      }
    //clog << "archive_reader::dtor: exiting." << endl; 
  }

  template <class Data>
  void load (Data & data_)
  {
    //clog << "archive_reader::load: entering..." << endl; 
    if (__done)
      {
	throw std::runtime_error ("archive_reader::load: input stream is done!");
      }
    if (__tia) 
      {
	*__tia >> data_;
      }
    if (__xia) 
      {
	*__xia >> boost::serialization::make_nvp ("data", data_);
      }
    if (__bia) 
      {
	*__bia >> data_;
      }
    __check_done ();
    //clog << "archive_reader::load: exiting." << endl; 
  }
};

int main (int argc_, char ** argv_)
{
  clog << "Hello!" << endl << endl;

  try
    {
      int format = archive_tools::txt;
      if (argc_ > 1)
	{
	  clog << "Archive format argument is '" << argv_[1] << "'" << endl;
	  if (string (argv_[1]) == "txt") format = archive_tools::txt;
	  if (string (argv_[1]) == "xml") format = archive_tools::xml;
	  if (string (argv_[1]) == "bin") format = archive_tools::bin;
	}
      clog << "Archive format is '" 
	   << archive_tools::get_archive_format_label (format) << "'" << endl;
      
      ostringstream fname_ss;
      fname_ss << "data_1";
      fname_ss << archive_tools::get_archive_format_extension (format);
      
      string fname = fname_ss.str ();
      
      {
	clog << "Store data in file '" << fname << "':" << endl;
	ofstream fout;
	fout.open (fname.c_str (), ios_base::out | ios_base::binary);
	
	for (int i = 0; i < 10; i++)
	  {
	    // setup the data:
	    foo the_foo;
	    the_foo.set_i (i);
	    the_foo.set_x (i * 3.14);
	    ostringstream name_ss;
	    name_ss << "name_" << i;
	    the_foo.set_name (name_ss.str ());

	    clog << "Data=" << the_foo << endl;
	    
	    {
	      // setup the general archive writer:
	      archive_writer arw (fout, format);
	       
	      // store the 'foo' data:
	      arw.store (the_foo);
	    }
	  }
	clog << endl;
      }
      
      {
	clog << "Load data from file '" << fname << "':" << endl;
	ifstream fin;
	fin.open (fname.c_str (), ios_base::in | ios_base::binary);
	
	while (fin && ! fin.eof ())
	  {
	    foo the_foo;

	    {
	      // setup the general archive writer:
	      archive_reader arr (fin, format);

	      // load the 'foo' data:
	      arr.load (the_foo);
	    }

	    clog << "Data=" << the_foo << endl;
	  }
	clog << endl;
      }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      return 1;
    }
  clog << "Bye!" << endl;
  return 0;
}
  
// end of multiple_archives_per_file.cpp
