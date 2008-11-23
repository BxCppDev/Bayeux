// -*- mode: C++; -*- 
/* io_factory.h */

#ifndef __datatools__serialization__io_factory_h
#define __datatools__serialization__io_factory_h 1

#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
#ifdef DATATOOLS_USE_PBA
#include <boost/pba/portable_binary_oarchive.hpp>
#include <boost/pba/portable_binary_iarchive.hpp>
#endif

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

#include <boost/tokenizer.hpp>

#include <datatools/serialization/i_serializable.h>

#include <datatools/utils/i_tree_dump.h>

namespace datatools {
  
  namespace serialization {
    
    class io_factory : public datatools::utils::i_tree_dumpable
      {
      public:
	static bool g_debug;
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
	
	bool                              __read_archive_is_initialized;
	bool                              __write_archive_is_initialized;
	boost::archive::text_iarchive   * __itar_ptr;
	boost::archive::text_oarchive   * __otar_ptr;
	boost::archive::xml_iarchive    * __ixar_ptr;
	boost::archive::xml_oarchive    * __oxar_ptr;
	//boost::archive::binary_iarchive * __ibar_ptr;
	//boost::archive::binary_oarchive * __obar_ptr;
#ifdef DATATOOLS_USE_PBA
	portable_binary_iarchive * __ibar_ptr;
	portable_binary_oarchive * __obar_ptr;
#endif	
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
	    //#ifdef DATATOOLS_USE_PBA
	    MODE_BINARY      = 0x2,
	    //#endif
	    MODE_XML         = 0x4,
	    text             = MODE_TEXT,
	    //#ifdef DATATOOLS_USE_PBA
	    binary           = MODE_BINARY,
	    //#endif
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
	
	
	/*
	 * 
	 *
	 *
	 *
	 *
	 *
	 */
	
      public:

	static const std::string TXT_EXT;
	static const std::string XML_EXT;
	//#ifdef DATATOOLS_USE_PBA
	static const std::string BIN_EXT;
	//#endif
	static const std::string GZ_EXT;
	static const std::string BZIP2_EXT;

	static int 
	  guess_mode_from_filename (const std::string &, int &);
	
      public:
	
	bool 
	  eof () const;
	
	bool 
	  is_read () const;
	
	bool 
	  is_write () const;
	
	bool 
	  is_compressed () const;
	
	bool 
	  is_uncompressed () const;
	
	bool 
	  is_gzip () const;
	
	bool 
	  is_bzip2 () const;
	
	bool 
	  is_text () const;
	
	bool 
	  is_binary () const;
	
	bool 
	  is_xml () const;

	bool 
	  is_append () const;

	bool 
	  is_no_append () const;

	bool 
	  is_single_archive () const;

	bool 
	  is_multi_archives () const;

      private:

	int 
	  __init_read_archive ();

	int 
	  __init_read (const std::string & stream_name_);

	int 
	  __reset_read_archive ();

	int 
	  __reset_read ();

	int 
	  __init_write_archive ();

	int 
	  __init_write (const std::string & stream_name_);

	int 
	  __reset_write_archive ();

	int 
	  __reset_write ();

	int 
	  __init (const std::string & stream_name_, int mode_);

	void __ctor_defaults ();
	
	int __reset ();

      public:

	void
	  start_archive ();

	void
	  stop_archive ();

      public:

	// ctor
	io_factory (int mode_ = io_factory::MODE_DEFAULT);

	io_factory (const std::string & stream_name_, 
		    int mode_ = io_factory::MODE_DEFAULT);
	
	// dtor
	virtual ~io_factory();

      private:

	template <typename Data>
	  void __store_text (boost::archive::text_oarchive & ar_, 
			     const Data & data_)
	  {
	    const Data & b = data_;
	    ar_ << b; 
	  }

	template <typename Data>
	  void __store_xml (boost::archive::xml_oarchive & ar_, 
			    const Data & data_)
	  {
	    const Data & b = data_;
	    ar_ << boost::serialization::make_nvp ("record", b);
	  }

#ifdef DATATOOLS_USE_PBA
	template <typename Data>
	  /*
	    void __store_binary(boost::archive::binary_oarchive & ar_, 
	    const Data & data_)
	  */
	  void __store_binary (portable_binary_oarchive & ar_, 
			       const Data & data_)
	  {
	    const Data & b = data_;
	    ar_ << b; 
	  }
#endif

	template <typename Data>
	  void __load_text (boost::archive::text_iarchive & ar_, 
			    Data & data_)
	  {
	    Data & b = data_;
	    ar_ >> b; 
	  }

	template <typename Data>
	  void __load_xml (boost::archive::xml_iarchive & ar_ , 
			   Data & data_)
	  {
	    Data & b = data_;
	    ar_ >> boost::serialization::make_nvp ("record", b);
	  }

#ifdef DATATOOLS_USE_PBA
	template <typename Data>
	  /*
	    void __load_binary(boost::archive::binary_iarchive & ar_ , 
	    Data & data_)
	  */
	  void __load_binary (portable_binary_iarchive & ar_ , 
			      Data & data_)
	  {
	    Data & b = data_;
	    ar_ >> b; 
	  }
#endif

      public:

	template <typename Data>
	  void 
	  store (const Data & data_)
	  {
	    if (! is_write ())
	      {
		throw std::runtime_error ("io_factory::store: Not a writer factory!");
	      }
	    if (__otar_ptr != 0)
	      {
		__store_text (*__otar_ptr,data_);
	      }
	    if (__oxar_ptr != 0)
	      {
		__store_xml (*__oxar_ptr,data_);
	      }
#ifdef DATATOOLS_USE_PBA
	    if (__obar_ptr != 0)
	      {
		__store_binary (*__obar_ptr,data_);
	      }
#endif
	  }

	template <typename Data>
	  void 
	  load (Data & data_)
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
		    __load_text (*__itar_ptr, data_);
		    *__in_fs >> std::ws;
		  }
		if (__ixar_ptr != 0)
		  {
		    __load_xml (*__ixar_ptr, data_);
		    *__in_fs >> std::ws;
		  }
#ifdef DATATOOLS_USE_PBA
		if (__ibar_ptr != 0)
		  {
		    __load_binary (*__ibar_ptr, data_);
		  }
#endif
	      }
	    catch (std::exception & x) 
	      {
		std::cerr << "WARNING: io_factory::load: "
			  << "cannot load data from archive: " 
			  << x.what() << "!" 
			  << std::endl;	      
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
	  }

      public:
  
	template <typename Data>
	  friend io_factory & 
	  operator<< (io_factory & iof_, 
		      const Data & data_)
	  {
	    iof_.store (data_);
	    return iof_;
	  }

	template <typename Data>
	  friend io_factory & 
	  operator>> (io_factory & iof_, 
		      Data & data_)
	  {
	    iof_.load (data_);
	    return iof_;
	  }

	virtual void
	  tree_dump (std::ostream & out_         = std::cerr, 
		     const std::string & title_  = "",
		     const std::string & indent_ = "",
		     bool inherit_               = false) const;
  
	void 
	  dump (std::ostream & out_) const;

      };


    /*************************************************************/

    class io_reader : public io_factory
      {
      public:

	// ctor
	io_reader (int mode_ = io_factory::MODE_DEFAULT);

	io_reader (const std::string & stream_name_ , 
		   int mode_ = io_factory::MODE_DEFAULT);

	// dtor
	virtual ~io_reader ();

      };

    /*************************************************************/

    class io_writer : public io_factory
      {
      public:

	// ctor
	io_writer (int mode_ = io_factory::MODE_DEFAULT); 

	io_writer (const std::string & stream_name_ , 
		   int mode_ = io_factory::MODE_DEFAULT);

	// dtor
	virtual ~io_writer ();

      };

    /*************************************************************/

    // user friendly constants used in ctors 
    // for data_reader/data_writer:
    static const bool using_multi_archives = true;
    static const bool using_single_archive = false;
    static const bool append_mode    = true;
    static const bool no_append_mode = false;

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

	void 
	  __read_next_tag ();

	void 
	  __init_reader (const std::string & filename_, int mode_);

	void 
	  __reset_reader ();

      public:

	const std::string & 
	  get_record_tag () const;

	bool 
	  has_record_tag () const;

	bool 
	  record_tag_is (const std::string & tag_) const;
	
	void 
	reset ();

	bool
	is_initialized () const;
	
	bool
	is_multi_archives () const;

	bool
	is_single_archive () const;

	void 
	  init (const std::string & filename_, 
		bool multiple_archives_ = using_single_archive);

	void 
	  init (const std::string & filename_ , int mode_);

	// ctor
	data_reader ();

	data_reader (const std::string & filename_, 
		     bool multiple_archives_ = using_single_archive);

	data_reader (const std::string & filename_, int mode_);
	
	// dtor
	virtual ~data_reader ();

      protected:
	template <typename Data>
	  void 
	  _basic_load (Data & data_)
	  {
	    if (__reader == 0) 
	      {
		throw std::runtime_error ("data_reader::_basic_load(...): not initialized!");
	      }
	    try 
	      {
		//std::cerr << "DEVEL: data_reader::_basic_load: load???" << std::endl;
		__reader->load (data_);
	      }
	    /*
	      catch(std::runtime_error & x) 
	      {
	      std::cerr << "WARNING: data_reader::_basic_load(...): "
	      << "cannot read data: "
	      << x.what() << '!'
	      << std::endl;
	      throw x;
	      }
	    */
	    catch (std::exception & x) 
	      {
		bool warn = true;

		//>>> 2008-11-13 FM: skip EOF message printing
		std::string msg = x.what();
		if (msg.find("EOF") != msg.npos)
		  {
		    warn = false;
		  }
		if (warn)
		  {
		    std::cerr << "WARNING: data_reader::_basic_load(...): "
			      << "cannot read data: "
			      << x.what () << '!'
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
	  }
	
      public:
	template <typename Data>
	  void 
	  load (const std::string & tag_, Data & data_)
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
	  }

	template <typename Data>
	  void 
	  load (Data & data_)
	  {
	    datatools::serialization::i_serializable & i_ser=
	      static_cast<datatools::serialization::i_serializable &> (data_);
	    this->load (i_ser.get_serial_tag(), data_);
	  }
	
      };

    /***********************************************************/

    class data_writer 
      {
	io_writer * __writer;

      private:

	void 
	  __init_writer (const std::string & filename_ , int mode_);

	void 
	  __reset_writer ();

      public:
	bool
	is_initialized () const;	
		
	bool
	is_multi_archives () const;

	bool
	is_single_archive () const;

	void 
	  reset ();

	void 
	  init (const std::string & filename_, 
		bool multiple_archives_ = using_single_archive,
		bool append_mode_ = no_append_mode);

	void 
	  init (const std::string & filename_, 
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
	  void 
	  _basic_store (const Data & data_)
	  {
	    if (__writer == 0)
	      {
		throw std::runtime_error ("data_writer::_basic_store(...): not initialized!");
	      }
	    __writer->store (data_);
	  }

      public:
	template <typename Data>
	  void 
	  store (const std::string & tag_, const Data & data_)
	  {
	    if (__writer->is_multi_archives ()) __writer->start_archive ();
	    this->_basic_store (tag_);
	    this->_basic_store (data_);
	    if (__writer->is_multi_archives ()) __writer->stop_archive ();
	  }

	template <typename Data>
	  void 
	  store (const Data & data_)
	  {
	    const datatools::serialization::i_serializable & i_ser=
	      static_cast<const datatools::serialization::i_serializable &> (data_);
	    this->store (i_ser.get_serial_tag(), data_);
	  }

      };

  } // end of namespace serialization 

} // end of namespace datatools 

#endif // __datatools__serialization__io_factory_h 1

/* end of io_factory.h */
