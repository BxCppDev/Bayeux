// -*- mode: c++; -*- 
/* io_factory.cc
 */

#include <datatools/serialization/io_factory.h>

namespace datatools {
  
  namespace serialization {

    bool io_factory::g_debug=false;

    bool io_factory::eof() const
    {
      if ( is_write() ) return false;
      if ( __in!= 0 && __in->eof() ) return true;
      return false;
    }

    bool io_factory::is_read() const
    {
      return (__mode & MASK_RW ) == 0;
    }

    bool io_factory::is_write() const
    {
      return (__mode & MASK_RW ) != 0;
    }

    bool io_factory::is_compressed() const
    {
      return (__mode & MASK_COMPRESSION ) != 0;
    }

    bool io_factory::is_uncompressed() const
    {
      return (__mode & MASK_COMPRESSION ) == 0;
    }

    bool io_factory::is_gzip() const
    {
      return (__mode & MASK_COMPRESSION ) == MODE_GZIP;
    }

    bool io_factory::is_bzip2() const
    {
      return (__mode & MASK_COMPRESSION ) == MODE_BZIP2;
    }

    bool io_factory::is_text() const
    {
      return (__mode & MASK_FORMAT ) == MODE_TEXT;
    }

    bool io_factory::is_binary() const
    {
      return (__mode & MASK_FORMAT ) == MODE_BINARY;
    }

    bool io_factory::is_xml() const
    {
      return (__mode & MASK_FORMAT ) == MODE_XML;
    }

    int io_factory::__init_read( const std::string & stream_name_ )
    {
      __in_fs = new boost::iostreams::filtering_istream;
      
      if ( is_gzip() ) {
	__in_fs->push(boost::iostreams::gzip_decompressor());
      }
      
      if ( is_bzip2() ) {
	__in_fs->push(boost::iostreams::bzip2_decompressor());
      }
      
      if ( stream_name_.empty() ) {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_read: cin..." 
		    << std::endl;
	}
	__in_fs->push(std::cin);
      }
      else {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_read: file='" 
		    << stream_name_ << "'" << std::endl;
	}
	if ( is_compressed() || is_binary() ) {
	  __fin = new std::ifstream(stream_name_.c_str(),
				    std::ios_base::in|std::ios_base::binary);
	}
	else {
	  __fin = new std::ifstream(stream_name_.c_str(),
				    std::ios_base::in);
	}
	if ( !*__fin ) {
	  throw std::runtime_error("io_factory::__init_read: Cannot open input stream!");
	}
	__in_fs->push(*__fin);
      }

      __in = __in_fs;

      if ( is_text() ) {
	__itar_ptr = new boost::archive::text_iarchive(*__in);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_read: " 
		    << "'boost::archive::text_iarchive' library version" 
		    << __itar_ptr->get_library_version() << std::endl;
	}
      }
      else if ( is_xml() ) {
	__ixar_ptr = new boost::archive::xml_iarchive(*__in);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_read: "
		    << "'boost::archive::xml_iarchive' library version" 
		    << __ixar_ptr->get_library_version() << std::endl;
	}
      }
      else if ( is_binary() ) {
	__ibar_ptr = new boost::archive::binary_iarchive(*__in);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_read: "
		    << "'boost::archive::binary_iarchive' library version" 
		    << __ibar_ptr->get_library_version() << std::endl;
	}
      }
      else {
	throw std::runtime_error("io_factory::__init_read: format not supported!");
      }
      return 0;
    }


    int io_factory::__reset_read()
    {
      if ( g_debug ) {
	std::cerr << "DEBUG: io_factory::__reset_read:..." << std::endl;
      }

      if ( __itar_ptr != 0 ) {
	delete __itar_ptr;
	__itar_ptr=0;
      }
      if ( __ixar_ptr != 0 ) {
	delete __ixar_ptr;
	__ixar_ptr=0;
      }
      if ( __ibar_ptr != 0 ) {
	delete __ibar_ptr;
	__ibar_ptr=0;
      }

      if ( __in != 0 ) {
	__in=0;
      }

      if ( __in_fs != 0 ) {
	__in_fs->reset(); 
	delete __in_fs;
	__in_fs=0;
      }

      if ( __fin != 0 ) {
	__fin->close();
	delete __fin;
	__fin=0;
      }

      return 0;
    }

    int io_factory::__init_write( const std::string & stream_name_ )
    {
      __out_fs = new boost::iostreams::filtering_ostream;

      if ( is_gzip() ) {
	__out_fs->push(boost::iostreams::gzip_compressor());
      }

      if ( is_bzip2() ) {
	__out_fs->push(boost::iostreams::bzip2_compressor());
      }

      if ( stream_name_.empty() ) {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_write: cout..." 
		    << std::endl;
	}
	__out_fs->push(std::cout);
	return 0;
      }
      else {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_write: file='" 
		    << stream_name_ << "'"<< std::endl;
	}
	if ( is_compressed() || is_binary() ) {
	  __fout = new std::ofstream(stream_name_.c_str(),
				     std::ios_base::out|std::ios_base::binary);
	}
	else {
	  __fout = new std::ofstream(stream_name_.c_str(),
				     std::ios_base::out);
	}
	if ( !*__fout ) {
	  throw std::runtime_error("io_factory::__init_write: Cannot open output stream!");
	}
	__out_fs->push(*__fout);
      }

      __out = __out_fs;

      if ( g_debug ) {
	std::cerr << "DEBUG: io_factory::__init_write: mode='" 
		  << std::hex << __mode << std::dec << "'" << std::endl;
      }
      if ( is_text() ) {
	__otar_ptr = new boost::archive::text_oarchive(*__out);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_write: "
		    << "'boost::archive::text_oarchive' library version" 
		    << __otar_ptr->get_library_version() << std::endl;
	}
      }
      else if ( is_xml() ) {
	__oxar_ptr = new boost::archive::xml_oarchive(*__out);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_write: "
		    << "'boost::archive::xml_oarchive' library version" 
		    << __oxar_ptr->get_library_version() << std::endl;
	}
      }
      else if ( is_binary() ) {
	__obar_ptr = new boost::archive::binary_oarchive(*__out);
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init_write: "
		    << "'boost::archive::binary_oarchive' library version" 
		    << __obar_ptr->get_library_version() << std::endl;
	}
      }
      else {
	throw std::runtime_error("io_factory::__init_write: format not supported!");
      }
      return 0;
    }

    int io_factory::__reset_write()
    {
      if ( g_debug ) {
	std::cerr << "DEBUG: io_factory::__reset_write:..." << std::endl;
      }
      if ( __otar_ptr != 0 ) {
	delete __otar_ptr;
	__otar_ptr=0;
      }
      if ( __oxar_ptr != 0 ) {
	delete __oxar_ptr;
	__oxar_ptr=0;
      }
      if ( __obar_ptr != 0 ) {
	delete __obar_ptr;
	__obar_ptr=0;
      }

      if ( __out != 0 ) {
	__out->flush();
	__out=0;
      }

      if ( __out_fs != 0 ) {
	__out_fs->flush();
	__out_fs->reset(); 
	delete __out_fs;
	__out_fs=0;
      }

      if ( __fout != 0 ) {
	__fout->close();
	delete __fout;
	__fout=0;
      }

      return 0;
    }

    int io_factory::__init( const std::string & stream_name_ , int mode_ )
    {
      __mode = mode_;
      if ( is_read() ) {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init: read mode..." 
		    << std::endl;
	}
	return __init_read(stream_name_);
      }
      else {
	if ( g_debug ) {
	  std::cerr << "DEBUG: io_factory::__init: write mode..." 
		    << std::endl;
	}
	return __init_write(stream_name_);
      }
      return 1;
    }

  
    void io_factory::__ctor_defaults()
    {
      __in=0;
      __out=0;
      __fin=0;
      __fout=0;
      __itar_ptr=0;
      __otar_ptr=0;
      __ixar_ptr=0;
      __oxar_ptr=0;
      __ibar_ptr=0;
      __obar_ptr=0;
      __in_fs=0;
      __out_fs=0;
      __mode = io_factory::MODE_DEFAULT;
    }
  
    int io_factory::__reset()
    {
      if ( is_read() ) {
	__reset_read();
      }
      
      if ( is_write() ) {
	__reset_write();
      }
      __ctor_defaults();
    }

    io_factory::io_factory( int mode_ )
    {
      __ctor_defaults();
      __init("", mode_ );
    }

    io_factory::io_factory( const std::string & stream_name_ , 
			    int mode_ )
    {
      __ctor_defaults();
      __init( stream_name_, mode_);
    }

    io_factory::~io_factory()
    {
      __reset();
      if ( io_factory::g_debug ) {
	std::cerr << "DEBUG: io_factory::~io_factory." << std::endl;
      }
    }

    void io_factory::dump( std::ostream & out_ ) const
    {
      std::string tag  = "|-- ";
      std::string ltag = "`-- ";
      out_ << "io_factory::dump:" << std::endl;
      out_ << tag << "mode:" 
	   << std::hex << __mode << std::dec << std::endl;
      out_ << tag << "is_read:" 
	   << is_read() << std::endl;
      out_ << tag << "is_write:" 
	   << is_write() << std::endl;
      out_ << tag << "is_compressed:" 
	   << is_compressed() << std::endl;
      out_ << tag << "is_uncompressed:" 
	   << is_uncompressed() << std::endl;
      out_ << tag << "is_gzip:" 
	   << is_gzip() << std::endl;
      out_ << tag << "is_bzip2:" 
	   << is_bzip2() << std::endl;
      out_ << tag << "is_text:"  
	   << is_text() << std::endl;
      out_ << tag << "is_binary:" 
	   << is_binary() << std::endl;
      out_ << ltag << "is_xml:" 
	   << is_xml() << std::endl;
    }

    int io_factory::guess_mode_from_filename( const std::string & filename_ , 
        int & mode_ )
    {
      int status = io_factory::SUCCESS;
      int mode = 0x0;
      std::string fn = filename_;
      boost::char_separator<char> sep(".");
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      tokenizer tokens(fn,sep);
      std::list<std::string> ltok;
      for( tokenizer::iterator i = tokens.begin() ; 
          i!=tokens.end() ; 
          ++i ) {
        std::string token=*i;
        if ( io_factory::g_debug ) {
          std::cerr << "DEBUG: io_factory::guess_mode_from_filename: token=<" 
            << token << ">" << std::endl;
        }
        ltok.push_front(token);
      }

      bool gz=false;
      bool bz2=false;
      bool txt=false;
      bool bin=false;
      bool xml=false;
      bool comp=false;
      bool format=false;
      size_t ext_count=0;
      for ( std::list<std::string>::const_iterator i=ltok.begin();
          i!=ltok.end();
          i++ ) {
        if ( io_factory::g_debug ) {
          std::cerr << "DEBUG: io_factory::guess_mode_from_filename: ltok=<" 
            << *i << ">" << std::endl;
        }
        std::string ext = *i;
        if ( !comp ) {
          if ( ext == "gz" || ext == "GZ" ) {
            comp=gz=true;
            if ( io_factory::g_debug ) {
              std::cerr << "DEBUG: io_factory::guess_mode_from_filename: mode+=GZIP" << std::endl; 
            }
          }
          else if ( ext == "bz2" || ext == "BZ2" ) {
            comp=bz2=true;
            if ( io_factory::g_debug ) {
              std::cerr << "DEBUG: io_factory::guess_mode_from_filename: mode+=BZIP2" << std::endl; 
            }
          }
        }
        if ( !format ) {
          if ( ext == "txt" || ext == "TXT" ) {
            format=txt=true;
            if ( io_factory::g_debug ) {
              std::cerr << "DEBUG: io_factory::guess_mode_from_filename: mode+=TEXT" << std::endl; 
            }
          }
          else if ( ext == "data" || ext == "DATA" ) {
            format=bin=true;
            if ( io_factory::g_debug ) {
              std::cerr << "DEBUG: io_factory::guess_mode_from_filename: mode+=BINARY" << std::endl; 
            }
          }
          else if ( ext == "xml" || ext == "XML" ) {
            format=xml=true;
            if ( io_factory::g_debug ) {
              std::cerr << "DEBUG: io_factory::guess_mode_from_filename: mode+=XML" << std::endl; 
            }
          }
        }
        if ( ! format && ! comp ) break;
        if ( format ) break;
        ext_count++;
        if ( ext_count==2 ) break;
      }

      if ( ! format ) {
        // cannot guess format from extension:
        status=io_factory::ERROR;
      }
      else {
        mode &= ~ io_factory::MASK_COMPRESSION;
        if ( comp ) {
          if ( gz ) mode |= io_factory::MODE_GZIP;
          if ( bz2 ) mode |= io_factory::MODE_BZIP2;
        }

        mode &= ~ io_factory::MASK_FORMAT;
        if ( format ) {
          if ( bin ) mode |= io_factory::MODE_BINARY;
          if ( txt ) mode |= io_factory::MODE_TEXT;
          if ( xml ) mode |= io_factory::MODE_XML;
        }
      }
      mode_ = mode;
      return status;
    }

    /***********************************************************/

    io_reader::io_reader( int mode_ ) 
      : io_factory(io_factory::MODE_READ|mode_)
    {
      if ( !is_read() ) {
	throw std::runtime_error("io_reader::io_reader: cannot force not-read mode!");
      }
    }

    io_reader::io_reader( const std::string & stream_name_ , 
			  int mode_  )
      : io_factory(stream_name_,io_factory::MODE_READ|mode_)
    {
      if ( !is_read() ) {
	throw std::runtime_error("io_reader::io_reader: cannot force not-read mode!");
      }
    }

    io_reader::~io_reader()
    {
      if ( g_debug ) {
	std::cerr << "DEBUG: io_reader::~io_reader." << std::endl;
      }
    }

    /***********************************************************/

    io_writer::io_writer( int mode_ ) 
      : io_factory(io_factory::MODE_WRITE|mode_)
    {
      if ( !is_read() ) 
	{
	  throw std::runtime_error("io_writer::io_writer: cannot force write mode!");
	}
    }

    io_writer::io_writer(const std::string & stream_name_ , 
			 int mode_)
      : io_factory(stream_name_,io_factory::MODE_WRITE|mode_)
    {
      if (!is_write()) 
	{
	  throw std::runtime_error("io_writer::io_writer: cannot force read mode!");
	}
    }

    io_writer::~io_writer()
    {
      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_writer::~io_writer." << std::endl;
	}
    }

    /***********************************************************/

    void data_reader::__read_next_tag()
    {
      try 
	{
	  std::string tag_id;
	  this->_basic_load(tag_id);
	  __next_tag=tag_id;
	}
      catch(std::runtime_error & x) 
	{
	  std::cerr << "WARNING: data_reader::__read_next_tag: " 
		    << x.what() << std::endl;
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
      catch(std::exception & x) 
	{
	  std::cerr << "WARNING: data_reader::__read_next_tag: " 
		    << x.what() << std::endl;
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
    }

    void 
    data_reader::__init_reader( const std::string & filename_ , 
				int mode_ )
    {
      __status=STATUS_OK;
      __reader = new io_reader(filename_,mode_);
      __read_next_tag();
    }

    void 
    data_reader::__reset_reader()
    {
      if ( __reader != 0 ) {
	delete __reader;
	__reader = 0;
      }
      __next_tag="";
      __status=STATUS_OK;
    }

    const std::string data_reader::EMPTY_RECORD_TAG = "";

    const std::string & 
    data_reader::get_record_tag() const
    {
      if (__status != STATUS_OK) return EMPTY_RECORD_TAG;
      return __next_tag;
    }

    bool 
    data_reader::has_record_tag() const
    {
      if (__status != STATUS_OK) return false;
      if (__next_tag.empty()) return false;
      return true;
    }

    bool 
    data_reader::record_tag_is( const std::string & tag_ ) const
    {
      return __next_tag == tag_;
    }

    void 
    data_reader::reset()
    {
      __reset_reader();
    }

    void 
    data_reader::init( const std::string & filename_ )
    {
      __reset_reader();
      int mode_guess;
      if ( io_factory::guess_mode_from_filename(filename_,mode_guess) 
	   != io_factory::SUCCESS ) {
	std::ostringstream message;
	message << "data_reader::init: cannot guess mode for file '" 
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      __init_reader(filename_,mode_guess);
    }

    void 
    data_reader::init( const std::string & filename_ , int mode_ )
    {
      __reset_reader();
      __init_reader(filename_,mode_);
    }

    data_reader::data_reader() 
    {
      __reader=0;
    }

    data_reader::data_reader( const std::string & filename_ )
    {
      __reader=0;
      init(filename_);
    }

    data_reader::data_reader( const std::string & filename_ , int mode_ )
    {
      __reader=0;
      __init_reader(filename_,mode_);
    }

    data_reader::~data_reader()
    {
      reset();
      if ( io_factory::g_debug ) {
	std::cerr << "DEBUG: data_reader::~data_reader." << std::endl;
      }
    }

    /***********************************************************/
	
    void 
    data_writer::__init_writer( const std::string & filename_ , 
				     int mode_ )
    {
      __writer = new io_writer(filename_,mode_);
    }

    void 
    data_writer::__reset_writer()
    {
      if ( __writer != 0 ) {
	delete __writer;
	__writer = 0;
      }
    }

    void 
    data_writer::reset()
    {
      __reset_writer();
    }

    void 
    data_writer::init( const std::string & filename_ )
    {
      __reset_writer();
      int mode_guess;
      if ( io_factory::guess_mode_from_filename(filename_,mode_guess) 
	   != io_factory::SUCCESS ) {
	std::ostringstream message;
	message << "data_writer::init: cannot guess mode for file '" 
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      __init_writer(filename_,mode_guess);
    }

    void 
    data_writer::init( const std::string & filename_ , int mode_ )
    {
      __reset_writer();
      __init_writer(filename_,mode_);
    }

    data_writer::data_writer() 
    {
      __writer=0;
    }

    data_writer::data_writer( const std::string & filename_ )
    {
      __writer=0;
      init(filename_);
    }

    data_writer::data_writer( const std::string & filename_ , int mode_ )
    {
      __writer=0;
      __init_writer(filename_,mode_);
    }

    data_writer::~data_writer()
    {
      reset();
      if ( io_factory::g_debug ) {
	std::cerr << "DEBUG: data_writer::~data_writer." << std::endl;
      }
    }

  } // end of namespace serialization
  
} // end of namespace datatools

  // end of io_factory.cc
