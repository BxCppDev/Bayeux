// -*- mode: C++; -*- 
/* io_factory.h */

#ifndef __datatools__serialization__io_factory_h
#define __datatools__serialization__io_factory_h 1
 
#include <stdexcept>
#include <iostream> 
#include <string>
#include <sstream>
#include <fstream>
#include <locale>
#include <typeinfo>

#ifdef IOFACTORY_USE_EOS_PBA
// Force usage of `Floating point utilities' by Johan Råde
// for `Portable binary archive' library by Christian Pfligersdorffer 
#ifndef IOFACTORY_USE_FPU
#define IOFACTORY_USE_FPU=1
#endif
#endif // IOFACTORY_USE_EOS_PBA


#ifdef IOFACTORY_USE_FPU
#include <boost/math/nonfinite_num_facets.hpp>
#endif

#include <boost/tokenizer.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#ifdef IOFACTORY_USE_EOS_PBA
// portable binary archives by C. Pfligersdorffer
// do not include 
//#include <portable_iarchive.hpp>
//#include <portable_oarchive.hpp>

// 2010-10-29 FM: quasi portable binary I/O archives by C. Pfligersdorffer + F. Mauger
// a replacement for the original portable binary archive by C. Pfligersdorffer that
// do not handle NaNs, Infinites and denorm. floating values.
#include <datatools/serialization/quasi_portable_binary_iarchive.hpp>
#include <datatools/serialization/quasi_portable_binary_oarchive.hpp>
#else
// native portable binary archives: not recommended at all
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

#include <boost/serialization/string.hpp>
#include <datatools/serialization/i_serializable.h>

#ifdef DATATOOLS_USE_TREE_DUMP
#include <datatools/utils/i_tree_dump.h>
#endif // DATATOOLS_USE_TREE_DUMP

namespace datatools {
  
  namespace serialization {
    
    class io_factory 
#ifdef DATATOOLS_USE_TREE_DUMP
      : public datatools::utils::i_tree_dumpable
#endif // DATATOOLS_USE_TREE_DUMP
    {
    public:
      static bool g_debug;
      static bool g_warning;

    public:
      static const int SUCCESS = 0; 
      static const int ERROR   = 1; 

    private:
      int            __mode;
      std::istream * __in;
      std::ostream * __out;
	
      boost::iostreams::filtering_istream * __in_fs;
      boost::iostreams::filtering_ostream * __out_fs;
      std::ifstream  * __fin;
      std::ofstream  * __fout;
#ifdef IOFACTORY_USE_FPU
      std::locale    * __default_locale;
      std::locale    * __locale;
#endif // IOFACTORY_USE_FPU

      bool                                __read_archive_is_initialized;
      bool                                __write_archive_is_initialized;
      boost::archive::text_iarchive     * __itar_ptr;
      boost::archive::text_oarchive     * __otar_ptr;
      boost::archive::xml_iarchive      * __ixar_ptr;
      boost::archive::xml_oarchive      * __oxar_ptr;
#ifdef IOFACTORY_USE_EOS_PBA
      eos::portable_iarchive            * __ibar_ptr;
      eos::portable_oarchive            * __obar_ptr;
#else
      boost::archive::binary_iarchive   * __ibar_ptr;
      boost::archive::binary_oarchive   * __obar_ptr;
#endif // IOFACTORY_USE_EOS_PBA

    public:
	
      static const int MASK_RW           = 0x1;
      static const int MASK_FORMAT       = 0xE;
      static const int MASK_COMPRESSION  = 0x30;
      static const int MASK_MULTIARCHIVE = 0x80;
      static const int MASK_APPEND       = 0x100;
	
      enum mode
	{
	  MODE_READ        = 0x0,
	  MODE_WRITE       = 0x1,
	  read             = MODE_READ,
	  write            = MODE_WRITE,
	    
	  MODE_TEXT        = 0x0,
	  MODE_BINARY      = 0x2,
	  MODE_XML         = 0x4,
	  text             = MODE_TEXT,
	  binary           = MODE_BINARY,
	  xml              = MODE_XML,
	    
	  MODE_NO_COMPRESS = 0x0,
	  MODE_GZIP        = 0x10,
	  MODE_BZIP2       = 0x20,
	  no_compress      = MODE_NO_COMPRESS,
	  gzip             = MODE_GZIP,
	  bzip2            = MODE_BZIP2,
	    
	  MODE_UNIQUE_ARCHIVE = 0x0,
	  MODE_MULTI_ARCHIVES = 0x80,
	  unique_archive      = MODE_UNIQUE_ARCHIVE,
	  multi_archives      = MODE_MULTI_ARCHIVES,
	    
	  MODE_NO_APPEND = 0x0,
	  MODE_APPEND    = 0x100,
	  no_append      = MODE_NO_APPEND,
	  append         = MODE_APPEND,
	    
	  MODE_DEFAULT = 
	  MODE_READ | 
	  MODE_TEXT | 
	  MODE_NO_COMPRESS | 
	  MODE_UNIQUE_ARCHIVE |
	  MODE_NO_APPEND
	};
	
	
    public:

      static const std::string TXT_EXT;
      static const std::string XML_EXT;
      static const std::string BIN_EXT;
      static const std::string GZ_EXT;
      static const std::string BZIP2_EXT;

      static int guess_mode_from_filename (const std::string &, int &);
	
    public:
	
      bool eof () const;
	
      bool is_read () const;
	
      bool is_write () const;
	
      bool is_compressed () const;
	
      bool is_uncompressed () const;
	
      bool is_gzip () const;
	
      bool is_bzip2 () const;
	
      bool is_text () const;
	
      bool is_binary () const;

#ifdef IOFACTORY_USE_EOS_PBA
      bool is_portable_binary () const
      {
	return is_binary () && true;
      }
#else
      bool is_portable_binary () const
      {
	return is_binary () && false;
      }
#endif // IOFACTORY_USE_EOS_PBA	

      bool is_xml () const;

      bool is_append () const;

      bool is_no_append () const;

      bool is_single_archive () const;

      bool is_multi_archives () const;

    private:

      int __init_read_archive ();

      int __init_read (const std::string & stream_name_);

      int __reset_read_archive ();

      int __reset_read ();

      int __init_write_archive ();

      int __init_write (const std::string & stream_name_);

      int __reset_write_archive ();

      int __reset_write ();

      int __init (const std::string & stream_name_, int mode_);

      void __ctor_defaults ();
	
      int __reset ();

    public:

      void start_archive ();

      void stop_archive ();

    public:

      // ctor
      io_factory (int mode_ = io_factory::MODE_DEFAULT);

      io_factory (const std::string & stream_name_, 
		  int mode_ = io_factory::MODE_DEFAULT);
	
      // dtor
      virtual ~io_factory ();

    private:
	
      template <typename Data>
      void __store_text (boost::archive::text_oarchive & ar_, 
			 const Data & data_)
      {
	using namespace std;
	const Data & b = data_;
	ar_ << b; 
	return;
      }

      template <typename Data>
      void __store_xml (boost::archive::xml_oarchive & ar_, 
			const Data & data_)
      {
	const Data & b = data_;
	ar_ << boost::serialization::make_nvp ("record", b);
	return;
      }
	
      template <typename Data>
#ifdef IOFACTORY_USE_EOS_PBA
      void __store_binary (eos::portable_oarchive & ar_, 
			   const Data & data_)
#else
	void __store_binary (boost::archive::binary_oarchive & ar_, 
			     const Data & data_)
#endif // IOFACTORY_USE_EOS_PBA
      {
	const Data & b = data_;
	ar_ << b; 
	return;
      }

      template <typename Data>
      void __load_text (boost::archive::text_iarchive & ar_, 
			Data & data_)
      {
	Data & b = data_;
	ar_ >> b; 
	/*
        // 2009-05-26, FM:  
	std::cerr << "DEVEL: io_factory::__load_text: archive class name='" 
	<< typeid (ar_).name() << "'" 
	<< std::endl;
	std::cerr << "DEVEL: io_factory::__load_text: class name='" 
	<< typeid (b).name() << "'" 
	<< std::endl;
	// Problem!
	try 
	{
	ar_ >> b; 
	std::cerr << "DEVEL: io_factory::__load_text: deserialization is ok."
	<< std::endl;
	}
	catch (std::exception & x)
	{
	std::cerr << "DEVEL: io_factory::__load_text: EXCEPTION="
	<< x.what ()
	<< std::endl;
	    
	}
	std::cerr << "DEVEL: io_factory::__load_text: done" 
	<< std::endl;
	*/
      }

      template <typename Data>
      void __load_xml (boost::archive::xml_iarchive & ar_ , 
		       Data & data_)
      {
	Data & b = data_;
	ar_ >> boost::serialization::make_nvp ("record", b);
	return;
      }

      template <typename Data>
#ifdef IOFACTORY_USE_EOS_PBA
      void __load_binary (eos::portable_iarchive & ar_ , 
			  Data & data_)
#else
	void __load_binary (boost::archive::binary_iarchive & ar_ , 
			    Data & data_)
#endif // IOFACTORY_USE_EOS_PBA
      {
	Data & b = data_;
	ar_ >> b; 
	return;
      }

    public:

      template <typename Data>
      void store (const Data & data_)
      {
	using namespace std;
	if (! is_write ())
	  {
	    throw std::runtime_error ("io_factory::store: Not a writer factory!");
	  }
	if (__otar_ptr != 0)
	  {
	    __store_text<Data> (*__otar_ptr, data_);
	  }
	else if (__oxar_ptr != 0)
	  {
	    __store_xml<Data> (*__oxar_ptr, data_);
	  }
	else if (__obar_ptr != 0)
	  {
	    __store_binary<Data> (*__obar_ptr, data_);
	  }
	return;
      }

      template <typename Data>
      void load (Data & data_)
      {
	if (! is_read ())
	  {
	    throw std::runtime_error ("io_factory::load: not a reader factory!");
	  }
	if (__in_fs == 0)
	  {
	    throw std::runtime_error ("io_factory::load: no input stream!");
	  }
	/* 2008-10-03 FM: add EOF check code*/
	//>>>
	if (*__in_fs) 
	  { 
	    // 2009-03-08 FM: Is it possible to have better EOF check code
	    // here for unzipped text and XML archives?
	    char c = 0; 
	    __in_fs->get (c); 
	    if (__in_fs)  
	      { 
		__in_fs->putback (c); 
	      } 
	  } 
	//<<<
	if (! *__in_fs)
	  {
	    if (__in_fs->eof ()) 
	      {
		throw std::runtime_error ("io_factory::load: input stream at EOF!");
	      }
	    if (__in_fs->fail ())
	      {
		throw std::runtime_error ("io_factory::load: input stream in fail status!");
	      }
	    if (__in_fs->bad ())
	      {
		throw std::runtime_error ("io_factory::load: input stream in bad status!");
	      }
	  }

	try 
	  {
	    if (__itar_ptr != 0)
	      {
		/*
		  std::cerr << "DEVEL: io_factory::load: "
		  << "load data from input text archive..." 
		  << std::endl;	      
		*/
		__load_text<Data> (*__itar_ptr, data_);
		*__in_fs >> std::ws;
	      }
	    else if (__ixar_ptr != 0)
	      {
		__load_xml<Data> (*__ixar_ptr, data_);
		*__in_fs >> std::ws;
	      }
	    else if (__ibar_ptr != 0)
	      {
		__load_binary<Data> (*__ibar_ptr, data_);
	      }
	  }
	// 2011-02-25 FM:
	catch (boost::archive::archive_exception & x) 
	  {
	    if (io_factory::g_warning)
	      {
		std::clog << "WARNING: io_factory::load: archive exception is: " 
			  << x.what () << std::endl;
	      }
	    throw x;
	  }
	catch (std::exception & x) 
	  {
	    if (io_factory::g_warning)
	      {
		std::cerr << "WARNING: io_factory::load: "
			  << "cannot load data from archive: " 
			  << x.what() << "!" 
			  << std::endl;	      
	      }
	    throw x;
	  }
	 
	catch (...) 
	  {
	    std::cerr << "WARNING: io_factory::load: "
		      << "cannot load data from archive: " 
		      << "unexpected exception" << "!" 
		      << std::endl;	      
	    throw std::runtime_error("io_factory::load: internal exception!");
	  }
	    	    
	if (! *__in_fs)
	  {
	    if (__in_fs->fail ())
	      {
		std::cerr << "WARNING: io_factory::load: input stream is now in fail status!" 
			  << std::endl;
	      } 
	    if (__in_fs->eof ())
	      {
		std::cerr << "WARNING: io_factory::load: input stream is now in EOF status!" 
			  << std::endl;
	      }
	    if (__in_fs->bad ())
	      {
		std::cerr << "WARNING: io_factory::load: input stream is now in bad status!" 
			  << std::endl;
	      } 
	  }
	return;
      }

    public:
  
      template <typename Data>
      friend io_factory & operator<< (io_factory & iof_, 
				      const Data & data_)
      {
	iof_.store (data_);
	return iof_;
      }

      template <typename Data>
      friend io_factory & operator>> (io_factory & iof_, 
				      Data & data_)
      {
	iof_.load (data_);
	return iof_;
      }

#ifdef DATATOOLS_USE_TREE_DUMP
      virtual // trick
#endif // DATATOOLS_USE_TREE_DUMP
      void tree_dump (std::ostream & out_         = std::cerr, 
		      const std::string & title_  = "",
		      const std::string & indent_ = "",
		      bool inherit_               = false) const;
  
      void dump (std::ostream & out_) const;

    };


    /*************************************************************/

    class io_reader : public io_factory
    {
    public:

      io_reader (int mode_ = io_factory::MODE_DEFAULT);

      io_reader (const std::string & stream_name_ , 
		 int mode_ = io_factory::MODE_DEFAULT);

      virtual ~io_reader ();

    };

    /*************************************************************/

    class io_writer : public io_factory
    {
    public:

      io_writer (int mode_ = io_factory::MODE_DEFAULT); 

      io_writer (const std::string & stream_name_ , 
		 int mode_ = io_factory::MODE_DEFAULT);

      virtual ~io_writer ();

    };

    /*************************************************************/

    // user friendly constants used in ctors 
    // for data_reader/data_writer:
    static const bool using_multi_archives = true;
    static const bool using_multiple_archives = using_multi_archives;
    static const bool using_single_archive = false;
    static const bool append_mode          = true;
    static const bool no_append_mode       = false;

    class data_reader 
    {
    public:
      enum status_t
	{
	  STATUS_OK    = 0,
	  STATUS_ERROR = 1
	};
      static const std::string EMPTY_RECORD_TAG;

    private:
      int         __status;
      io_reader * __reader;
      std::string __next_tag;

    private:

      void __read_next_tag ();

      void __init_reader (const std::string & filename_, int mode_);

      void __reset_reader ();

    public:

      bool is_error () const;

      const std::string & get_record_tag () const;

      bool has_record_tag () const;

      bool record_tag_is (const std::string & tag_) const;
	
      void reset ();

      bool is_initialized () const;
	
      bool is_multi_archives () const;

      bool is_single_archive () const;

      bool is_compressed () const;

      bool is_uncompressed () const;

      bool is_gzip () const;

      bool is_bzip2 () const;

      bool is_text () const;
	
      bool is_binary () const;

      bool is_portable_binary () const;
	
      bool is_xml () const;

      void init (const std::string & filename_, 
		 bool multiple_archives_ = using_single_archive);

      void init_multi (const std::string & filename_)
      {
	init (filename_, using_multiple_archives);
	return;
      }

      void init_single (const std::string & filename_)
      {
	init (filename_, using_single_archive);
	return;
      }

      void init (const std::string & filename_ , int mode_);

      // ctor
      data_reader ();

      data_reader (const std::string & filename_, 
		   bool multiple_archives_ = using_single_archive);

      data_reader (const std::string & filename_, int mode_);
	
      // dtor
      virtual ~data_reader ();

      void dump (std::ostream & out_ = std::clog) const;

    protected:
      template <typename Data>
      void _basic_load (Data & data_)
      {
	if (__reader == 0) 
	  {
	    throw std::runtime_error ("data_reader::_basic_load(...): not initialized!");
	  }
	try 
	  {
	    /*
	      std::cerr << "DEVEL: io_factory::_basic_load: class name='" 
	      << typeid (data_).name() << "'" 
	      << std::endl;
	    */
	    __reader->load (data_);
	    /*
	      std::cerr << "DEVEL: io_factory::_basic_load: ok" 
	      << std::endl;
	    */
	  }
	/*
	// 2011-02-25 FM:
	catch (boost::archive::archive_exception & x) 
	{
	bool warn = io_factory::g_warning;
	if (warn)
	{
	std::clog << "WARNING: data_reader::_basic_load: archive exception is: " 
	<< x.what () << std::endl;
	}
	__status   = STATUS_ERROR;
	__next_tag = EMPTY_RECORD_TAG;
	throw std::runtime_error (x.what ());
	}
	*/
	catch (std::exception & x) 
	  {
	    bool warn = io_factory::g_warning;

	    //>>> 2008-11-13 FM: skip EOF message printing
	    std::string msg = x.what ();
	    if (msg.find ("EOF") != msg.npos)
	      {
		warn = false;
	      }
	    if (warn)
	      {
		std::cerr << "WARNING: data_reader::_basic_load(...): "
			  << "cannot read data: exception="
			  << x.what () << " !"
			  << std::endl;
	      }
	    //<<<

	    __status   = STATUS_ERROR;
	    __next_tag = EMPTY_RECORD_TAG;

	    throw std::runtime_error (x.what ());
	  }
	catch (...) 
	  {
	    std::cerr << "WARNING: data_reader::_basic_load(...): "
		      << "cannot read data: "
		      << "unexpected exception" << '!'
		      << std::endl;
	    __status   = STATUS_ERROR;
	    __next_tag = EMPTY_RECORD_TAG;
	    throw std::runtime_error ("data_reader::_basic_load: unexpected error!");
	  }
	return;
      }
	
    public:
      template <typename Data>
      void load (const std::string & tag_, Data & data_)
      {
	if (! has_record_tag ()) 
	  {
	    throw std::runtime_error ("data_reader::load(...): no more record tag!");
	  }
	    
	if (get_record_tag () != tag_) 
	  {
	    std::ostringstream message;
	    message << "data_reader::load(...): unexpected tag ('" 
		    << get_record_tag () 
		    << " != " << tag_ << "')!" ;
	    throw std::runtime_error(message.str());	      
	  }
	    
	this->_basic_load (data_);
	if (__reader->is_multi_archives ()) __reader->stop_archive ();
	__read_next_tag ();
	return;
      }

      template <typename Data>
      void load (Data & data_)
      {
	datatools::serialization::i_serializable & i_ser =
	  static_cast<datatools::serialization::i_serializable &> (data_);
	this->load (i_ser.get_serial_tag (), data_);
	return;
      }
	
    };

    /***********************************************************/

    class data_writer 
    {
      io_writer * __writer;

    private:

      void __init_writer (const std::string & filename_ , int mode_);

      void __reset_writer ();

    public:
      bool is_initialized () const;	
		
      bool is_multi_archives () const;

      bool is_single_archive () const;

      bool is_compressed () const;

      bool is_uncompressed () const;

      bool is_gzip () const;

      bool is_bzip2 () const;

      bool is_text () const;
	
      bool is_binary () const;

      bool is_portable_binary () const;
	
      bool is_xml () const;

      void reset ();

      void init (const std::string & filename_, 
		 bool multiple_archives_ = using_single_archive,
		 bool append_mode_ = no_append_mode);

      void init (const std::string & filename_, 
		 int mode_);

      // ctor
      data_writer ();

      data_writer (const std::string & filename_, 
		   bool multiple_archives_ = using_single_archive,
		   bool append_mode_ = no_append_mode);
	
      data_writer (const std::string & filename_, 
		   int mode_);

      // dtor
      virtual ~data_writer ();

    protected:
      template <typename Data>
      void _basic_store (const Data & data_)
      {
	using namespace std;
	if (__writer == 0)
	  {
	    throw std::runtime_error ("data_writer::_basic_store(...): not initialized!");
	  }
	__writer->store<Data> (data_);
	return;
      }

    public:

      template <typename Data>
      void store (const std::string & tag_, const Data & data_)
      {
	using namespace std;
	if (__writer->is_multi_archives ()) 
	  {
	    __writer->start_archive ();
	  }
	this->_basic_store<std::string> (tag_);
	this->_basic_store<Data> (data_);
	if (__writer->is_multi_archives ()) 
	  {
	    __writer->stop_archive ();
	  }
	return;
      }

      template <typename Data>
      void store (const Data & data_)
      {
	using namespace std;
	const datatools::serialization::i_serializable & i_ser =
	  static_cast<const datatools::serialization::i_serializable &> (data_);
	this->store<Data> (i_ser.get_serial_tag (), data_);
	return;
      }

    };

  } // end of namespace serialization 

} // end of namespace datatools 

#endif // __datatools__serialization__io_factory_h 1

/* end of io_factory.h */
