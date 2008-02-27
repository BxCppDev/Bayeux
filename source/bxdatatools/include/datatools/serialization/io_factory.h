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
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

#include <boost/tokenizer.hpp>

namespace datatools {

  namespace serialization {

    class io_factory
      {
      public:
	static bool g_debug;
      public:
	static const int SUCCESS=0; 
	static const int ERROR=1; 
      private:
	int            __mode;
	std::istream * __in;
	std::ostream * __out;
  
	boost::iostreams::filtering_istream * __in_fs;
	boost::iostreams::filtering_ostream * __out_fs;
	std::ifstream  * __fin;
	std::ofstream  * __fout;

	boost::archive::text_iarchive   * __itar_ptr;
	boost::archive::text_oarchive   * __otar_ptr;
	boost::archive::xml_iarchive    * __ixar_ptr;
	boost::archive::xml_oarchive    * __oxar_ptr;
	boost::archive::binary_iarchive * __ibar_ptr;
	boost::archive::binary_oarchive * __obar_ptr;
  
      public:
  
	static const int MASK_RW          = 0x1;
	static const int MASK_FORMAT      = 0xE;
	static const int MASK_COMPRESSION = 0x30;

	enum mode
	  {
	    MODE_READ        = 0x0,
	    MODE_WRITE       = 0x1,

	    MODE_TEXT        = 0x0,
	    MODE_BINARY      = 0x2,
	    MODE_XML         = 0x4,

	    MODE_NO_COMPRESS = 0x0,
	    MODE_GZIP        = 0x10,
	    MODE_BZIP2       = 0x20,

	    MODE_DEFAULT = MODE_READ | MODE_TEXT | MODE_NO_COMPRESS,
	  };

      public:
  
	static int guess_mode_from_filename( const std::string & , int & );

      public:
	
	bool eof() const;
	
	bool is_read() const;
	
	bool is_write() const;
	
	bool is_compressed() const;
	
	bool is_uncompressed() const;
	
	bool is_gzip() const;
	
	bool is_bzip2() const;
	
	bool is_text() const;
	
	bool is_binary() const;
	
	bool is_xml() const;

      private:

	int __init_read( const std::string & stream_name_ );

	int __reset_read();

	int __init_write( const std::string & stream_name_ );

	int __reset_write();

	int __init( const std::string & stream_name_ , int mode_ );

  
	void __ctor_defaults();
	
	int __reset();

      public:

	io_factory( int mode_ = io_factory::MODE_DEFAULT );

	io_factory( const std::string & stream_name_ , 
		    int mode_ = io_factory::MODE_DEFAULT );
	
	virtual ~io_factory();

      private:

	template <typename Data>
	  void __store_text( boost::archive::text_oarchive & ar_ , 
			     const Data & data_ )
	  {
	    const Data & b = data_;
	    ar_ << boost::serialization::make_nvp("record",b);
	  }

	template <typename Data>
	  void __store_xml( boost::archive::xml_oarchive & ar_ , 
			    const Data & data_ )
	  {
	    const Data & b = data_;
	    ar_ << boost::serialization::make_nvp("record",b);
	  }

	template <typename Data>
	  void __store_binary( boost::archive::binary_oarchive & ar_ , 
			       const Data & data_ )
	  {
	    const Data & b = data_;
	    ar_ << boost::serialization::make_nvp("record",b);
	  }

	template <typename Data>
	  void __load_text( boost::archive::text_iarchive & ar_ , 
			    Data & data_ )
	  {
	    Data & b = data_;
	    ar_ >> boost::serialization::make_nvp("record",b);
	  }

	template <typename Data>
	  void __load_xml( boost::archive::xml_iarchive & ar_ , 
			   Data & data_ )
	  {
	    Data & b = data_;
	    ar_ >> boost::serialization::make_nvp("record",b);
	  }

	template <typename Data>
	  void __load_binary( boost::archive::binary_iarchive & ar_ , 
			      Data & data_ )
	  {
	    Data & b = data_;
	    ar_ >> boost::serialization::make_nvp("record",b);
	  }

      public:


	template <typename Data>
	  void store( const Data & data_ )
	  {
	    if ( ! is_write() ) {
	      throw std::runtime_error("io_factory::load: Not a writer factory!");
	    }
	    if ( __otar_ptr != 0 ) {
	      __store_text(*__otar_ptr,data_);
	    }
	    if ( __oxar_ptr != 0 ) {
	      __store_xml(*__oxar_ptr,data_);
	    }
	    if ( __obar_ptr != 0 ) {
	      __store_binary(*__obar_ptr,data_);
	    }
	  }

	template <typename Data>
	  void load( Data & data_ )
	  {
	    if ( ! is_read() ) {
	      throw std::runtime_error("io_factory::load: not a reader factory!");
	    }
	    if ( __in_fs == 0 ) {
	      throw std::runtime_error("io_factory::load: no input stream!");
	    }
	    if ( ! *__in_fs ) {
	      if ( __in_fs->eof() ) {
		throw std::runtime_error("io_factory::load: input stream at EOF!");
	      }
	      if ( __in_fs->fail() ) {
		throw std::runtime_error("io_factory::load: input stream in fail status!");
	      }
	      if ( __in_fs->bad() ) {
		throw std::runtime_error("io_factory::load: input stream in bad status!");
	      }
	    }

	    try {
	      if ( __itar_ptr != 0 ) {
		__load_text(*__itar_ptr,data_);
		*__in_fs >> std::ws;
	      }
	      if ( __ixar_ptr != 0 ) {
		__load_xml(*__ixar_ptr,data_);
		*__in_fs >> std::ws;
	      }
	      if ( __ibar_ptr != 0 ) {
		__load_binary(*__ibar_ptr,data_);
	      }
	    }
	    catch(std::exception & x) {
	      std::cerr << "WARNING: io_factory::load: cannot load data from archive: " << x.what() << "!" 
			<< std::endl;	      
	      throw;
	    }

	    
	    if ( ! *__in_fs ) {
	      
	      if ( __in_fs->fail() ) {
		std::cerr << "WARNING: io_factory::load: input stream is now in fail status!" 
			  << std::endl;
		//throw std::runtime_error("io_factory::load: input stream in fail status!");
	      } 
	      if ( __in_fs->eof() ) {
		std::cerr << "WARNING: io_factory::load: input stream is now in EOF status!" 
			  << std::endl;
		//throw std::runtime_error("io_factory::load: input stream in fail status!");
	      }
	      if ( __in_fs->bad() ) {
		  std::cerr << "WARNING: io_factory::load: input stream is now in bad status!" 
			    << std::endl;
		  //throw std::runtime_error("io_factory::load: input stream in fail status!");
	      } 
	      
	    }
	    
	  }

      public:
  
	template <typename Data>
	  friend io_factory & operator<<( io_factory & iof_ , 
					  const Data & data_ )
	  {
	    iof_.store(data_);
	  }

	template <typename Data>
	  friend io_factory & operator>>( io_factory & iof_ , 
					  Data & data_ )
	  {
	    iof_.load(data_);
	  }
  
	void dump( std::ostream & out_ ) const;

      };


    /*************************************************************/

    class io_reader : public io_factory
      {
      public:

	io_reader( int mode_ = io_factory::MODE_DEFAULT );

	io_reader( const std::string & stream_name_ , 
		   int mode_ = io_factory::MODE_DEFAULT );

	virtual ~io_reader();

      };

    /*************************************************************/

    class io_writer : public io_factory
      {
      public:

	io_writer( int mode_ = io_factory::MODE_DEFAULT ); 

	io_writer( const std::string & stream_name_ , 
		   int mode_ = io_factory::MODE_DEFAULT );

	virtual ~io_writer();

      };

    /*************************************************************/

    class data_reader 
      {
	int         __status;
	io_reader * __reader;
	std::string __next_tag;

      private:

	void __read_next_tag();

	void __init_reader( const std::string & filename_ , int mode_ );

	void __reset_reader();

      public:

	const std::string & get_record_tag() const;

	bool has_record_tag() const;

	bool record_tag_is( const std::string & tag_ ) const;

	void reset();

	void init( const std::string & filename_ );

	void init( const std::string & filename_ , int mode_ );

	data_reader();

	data_reader( const std::string & filename_ );

	data_reader( const std::string & filename_ , int mode_ );
	
	virtual ~data_reader();

	template <typename Data>
	  void load( Data & data_ )
	  {
	    if ( __reader == 0 ) {
	      throw std::runtime_error("data_reader::load(...): not initialized!");
	    }
	    try {
	      __reader->load(data_);
	    }
	    catch( std::exception & x ) {
	      std::cerr << "data_reader::load(...): cannot read data!" 
			<< std::endl;
	      throw;
	    }
	    __read_next_tag();
	  }

      };

    /***********************************************************/

    class data_writer 
      {
	io_writer * __writer;

      private:

	void __init_writer( const std::string & filename_ , int mode_ );

	void __reset_writer();

      public:

	void reset();

	void init( const std::string & filename_ );

	void init( const std::string & filename_ , int mode_ );

	data_writer();

	data_writer( const std::string & filename_ );

	data_writer( const std::string & filename_ , int mode_ );

	virtual ~data_writer();

	template <typename Data>
	  void store( const std::string & tag_ , Data & data_ )
	  {
	    if ( __writer == 0 ) {
	      throw std::runtime_error("data_writer::store(...): not initialized!");
	    }
	    __writer->store(tag_);
	    __writer->store(data_);
	  }

      };

  } // end of namespace serialization 

} // end of namespace datatools 

#endif // __datatools__serialization__io_factory_h 1

/* end of io_factory.h */
