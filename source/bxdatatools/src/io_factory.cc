// -*- mode: c++; -*- 
/* io_factory.cc
 */

#include <datatools/serialization/io_factory.h>

namespace datatools {
  
  namespace serialization {

    const std::string io_factory::TXT_EXT   = "txt";
    const std::string io_factory::XML_EXT   = "xml";
    //#ifdef DATATOOLS_USE_PBA
    const std::string io_factory::BIN_EXT   = "data";
    //#endif
    const std::string io_factory::GZ_EXT    = "gz";
    const std::string io_factory::BZIP2_EXT = "bz2";

    bool 
    io_factory::g_debug = false;

    bool 
    io_factory::eof () const
    {
      if (is_write ()) return false;
      if ( __in != 0 && __in->eof () ) return true;
      return false;
    }

    bool 
    io_factory::is_read () const
    {
      return (__mode & MASK_RW) == 0;
    }

    bool 
    io_factory::is_write () const
    {
      return (__mode & MASK_RW) != 0;
    }

    bool 
    io_factory::is_compressed () const
    {
      return (__mode & MASK_COMPRESSION) != 0;
    }

    bool 
    io_factory::is_uncompressed () const
    {
      return (__mode & MASK_COMPRESSION) == 0;
    }

    bool 
    io_factory::is_gzip () const
    {
      return (__mode & MASK_COMPRESSION) == MODE_GZIP;
    }

    bool 
    io_factory::is_bzip2 () const
    {
      return (__mode & MASK_COMPRESSION) == MODE_BZIP2;
    }

    bool 
    io_factory::is_text () const
    {
      return (__mode & MASK_FORMAT) == MODE_TEXT;
    }

    bool 
    io_factory::is_binary () const
    {
      return (__mode & MASK_FORMAT) == MODE_BINARY;
    }

    bool 
    io_factory::is_xml () const
    {
      return (__mode & MASK_FORMAT) == MODE_XML;
    }

    bool 
    io_factory::is_single_archive () const
    {
      return (__mode & MASK_MULTIARCHIVE) == MODE_UNIQUE_ARCHIVE;
    }
    
    bool 
    io_factory::is_multi_archives () const
    {
      return ! is_single_archive ();
    }

    bool 
    io_factory::is_no_append () const
    {
      return (__mode & MASK_APPEND) == MODE_NO_APPEND;
    }
    
    bool 
    io_factory::is_append () const
    {
      return ! is_no_append ();
    }

    int 
    io_factory::__init_read_archive ()
    {

      if (is_text ()) 
	{
	  __itar_ptr = new boost::archive::text_iarchive (*__in);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read_archive: " 
			<< "'boost::archive::text_iarchive' library version" 
			<< __itar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_xml ()) 
	{
	  __ixar_ptr = new boost::archive::xml_iarchive (*__in);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read_archive: "
			<< "'boost::archive::xml_iarchive' library version" 
			<< __ixar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_binary ()) 
	{
#ifdef DATATOOLS_USE_PBA
	  //__ibar_ptr = new boost::archive::binary_iarchive(*__in);
	  __ibar_ptr = new portable_binary_iarchive (*__in);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read_archive: "
			<< "'portable_binary_iarchive' library version" 
			<< __ibar_ptr->get_library_version () << std::endl;
	    }
#else
	  throw std::runtime_error ("io_factory::__init_read_archive: binary format is not supported!");
#endif
	}
      else 
	{
	  throw std::runtime_error ("io_factory::__init_read_archive: format not supported!");
	}
      __read_archive_is_initialized = true;
      return 0;
    }
    
    int 
    io_factory::__reset_read_archive ()
    {
      if (! __read_archive_is_initialized)
	{
	  return 0;
	}

      if (__itar_ptr != 0) 
	{
	  delete __itar_ptr;
	  __itar_ptr = 0;
	}

      if (__ixar_ptr != 0) 
	{
	  delete __ixar_ptr;
	  __ixar_ptr = 0;
	}

#ifdef DATATOOLS_USE_PBA
      if (__ibar_ptr != 0) 
	{
	  delete __ibar_ptr;
	  __ibar_ptr = 0;
	}
#endif
      __read_archive_is_initialized = false;
      return 0;
    }
    

    int 
    io_factory::__init_read (const std::string & stream_name_)
    {
      __in_fs = new boost::iostreams::filtering_istream;
      
      if (is_gzip ()) 
	{
	  __in_fs->push (boost::iostreams::gzip_decompressor ());
	}
      
      if (is_bzip2 ()) 
	{
	  __in_fs->push (boost::iostreams::bzip2_decompressor ());
	}
      
      if (stream_name_.empty ()) 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read: cin..." 
			<< std::endl;
	    }
	  __in_fs->push (std::cin);
	}
      else 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read: file='" 
			<< stream_name_ << "'" << std::endl;
	    }
#ifdef DATATOOLS_USE_PBA
	  if (is_compressed () || is_binary ()) 
	    {
	      __fin = new std::ifstream (stream_name_.c_str (),
					 std::ios_base::in|std::ios_base::binary);
	    }
	  else 
	    {
#endif
	      __fin = new std::ifstream (stream_name_.c_str (),
					 std::ios_base::in);
#ifdef DATATOOLS_USE_PBA
	    }
#endif
	  if (! *__fin) 
	    {
	      throw std::runtime_error ("io_factory::__init_read: Cannot open input stream!");
	    }
	  __in_fs->push (*__fin);
	}

      __in = __in_fs;

      return 0;
    }

    int 
    io_factory::__reset_read ()
    {
      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_factory::__reset_read:..." << std::endl;
	}

      if (__in != 0) 
	{
	  __in = 0;
	}

      if (__in_fs != 0) 
	{
	  __in_fs->reset (); 
	  delete __in_fs;
	  __in_fs = 0;
	}

      if (__fin != 0) 
	{
	  __fin->close ();
	  delete __fin;
	  __fin = 0;
	}

      return 0;
    }

    int 
    io_factory::__init_write_archive ()
    {
      if (__write_archive_is_initialized)
	{
	  return 0;
	}
      if (is_text ()) 
	{
	  __otar_ptr = new boost::archive::text_oarchive (*__out);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_write_archive: "
			<< "'boost::archive::text_oarchive' library version" 
			<< __otar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_xml ()) 
	{
	  __oxar_ptr = new boost::archive::xml_oarchive (*__out);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_write_archive: "
			<< "'boost::archive::xml_oarchive' library version" 
			<< __oxar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_binary ()) 
	{
#ifdef DATATOOLS_USE_PBA
	  //__obar_ptr = new boost::archive::binary_oarchive(*__out);
	  __obar_ptr = new portable_binary_oarchive (*__out);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_write_archive: "
			<< "'portable_binary_oarchive' library version" 
			<< __obar_ptr->get_library_version () << std::endl;
	    }
#else
	  throw std::runtime_error ("io_factory::__init_write_archive: binary format is not supported!");
#endif
	}
      else 
	{
	  throw std::runtime_error ("io_factory::__init_write_archive: format not supported!");
	}
      __write_archive_is_initialized = true;
      return 0;
    }

    int
    io_factory::__init_write (const std::string & stream_name_)
    {
      __out_fs = new boost::iostreams::filtering_ostream;

      if (is_gzip ())
	{
	  __out_fs->push(boost::iostreams::gzip_compressor ());
	}

      if (is_bzip2 ()) 
	{
	  __out_fs->push (boost::iostreams::bzip2_compressor ());
	}

      if (stream_name_.empty ())
	{
	  if (g_debug)
	    {
	      std::cerr << "DEBUG: io_factory::__init_write: cout..." 
			<< std::endl;
	    }
	  __out_fs->push (std::cout);
	  return 0;
	}
      else 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_write: file='" 
			<< stream_name_ << "'"<< std::endl;
	    }

	  std::ios_base::openmode open_mode = std::ios_base::out;
#ifdef DATATOOLS_USE_PBA
	  if (is_compressed () || is_binary ())
	    { 
	      open_mode |= std::ios_base::binary;
	    }
#endif
	  if (is_append ())
	    {
	      if (is_single_archive ())
		{
		  std::ostringstream message;
		  message << "io_factory::__init_write: "
			  << "append mode does not work for 'io_factory::single_archive' mode! "
			  << "Please consider to use the 'io_factory::multi_archives' mode instead!" ;
		  throw std::runtime_error(message.str ()); 
		}

	      open_mode |= std::ios_base::app; 
	    } 
	  /*
	    if (is_compressed() || is_binary()) 
	    {
	    __fout = new std::ofstream(stream_name_.c_str(),
	    std::ios_base::out|std::ios_base::binary);
	    }
	    else 
	    {
	    __fout = new std::ofstream(stream_name_.c_str(),
	    std::ios_base::out);
	    }
	  */
	  __fout = new std::ofstream (stream_name_.c_str (), open_mode);
	  if (!*__fout) 
	    {
	      throw std::runtime_error ("io_factory::__init_write: Cannot open output stream!");
	    }
	  __out_fs->push (*__fout);
	}

      __out = __out_fs;

      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_factory::__init_write: mode='" 
		    << std::hex << __mode << std::dec << "'" << std::endl;
	}
      return 0;
    }


    int 
    io_factory::__reset_write_archive ()
    {
      if (! __write_archive_is_initialized)
	{
	  return 0;
	}

      if (__otar_ptr != 0) 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__reset_write_archive: "
			<< "delete TXT archive!" << std::endl;
	    }
	  delete __otar_ptr;
	  __otar_ptr = 0;
	  //*__out << ' '; // add a white space at the end of text archive.
	  *__out << std::endl; // add a new line at the end of text archive.
	}

      if (__oxar_ptr != 0) 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__reset_write_archive: "
			<< "delete XML archive!" << std::endl;
	    }
	  delete __oxar_ptr;
	  __oxar_ptr = 0;
	}

#ifdef DATATOOLS_USE_PBA
      if (__obar_ptr != 0) 
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__reset_write_archive: "
			<< "delete BIN archive!" << std::endl;
	    }
	  delete __obar_ptr;
	  __obar_ptr = 0;
	}
#endif

      __write_archive_is_initialized = false;
      return 0;
    }

    int 
    io_factory::__reset_write ()
    {
      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_factory::__reset_write:..." << std::endl;
	}

      if (__out != 0) 
	{
	  __out->flush ();
	  __out=0;
	}

      if (__out_fs != 0) 
	{
	  __out_fs->flush ();
	  __out_fs->reset (); 
	  delete __out_fs;
	  __out_fs = 0;
	}

      if (__fout != 0) 
	{
	  __fout->close ();
	  delete __fout;
	  __fout = 0;
	}

      return 0;
    }

    int 
    io_factory::__init(const std::string & stream_name_ , int mode_)
    {
      __mode = mode_;
      if (is_read ()) 
	{
	  if (g_debug)
	    {
	      std::cerr << "DEBUG: io_factory::__init: read mode..." 
			<< std::endl;
	    }
	  __init_read (stream_name_);
	  if (is_single_archive ())
	    {
	      __init_read_archive ();
	    }
	}
      else
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::__init: write mode..." 
			<< std::endl;
	    }
	  __init_write (stream_name_);
	  if (is_single_archive ())
	    {
	      __init_write_archive ();
	    }
	}
      return 0;
    }
    
    void
    io_factory::start_archive ()
    {
      if (is_multi_archives ())
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::start_archive: multi..." 
			<< std::endl;
	    }
	  if(is_read ())
	    {
	      __init_read_archive ();
	    }
	  if(is_write ())
	    {
	      __init_write_archive ();
	    }
	}
    }
    
    void
    io_factory::stop_archive ()
    {
      if (is_multi_archives ())
	{
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::stop_archive: multi..." 
			<< std::endl;
	    }
	  if(is_read ())
	    {
	      __reset_read_archive ();
	    }
	  if(is_write ())
	    {
	      __reset_write_archive ();
	    }
	}
    }
    
    void 
    io_factory::__ctor_defaults ()
    {
      __write_archive_is_initialized = false;
      __read_archive_is_initialized  = false;
      __in = 0;
      __out = 0;
      __fin = 0;
      __fout = 0;
      __itar_ptr = 0;
      __otar_ptr = 0;
      __ixar_ptr = 0;
      __oxar_ptr = 0;
#ifdef DATATOOLS_USE_PBA
      __ibar_ptr = 0;
      __obar_ptr = 0;
#endif
      __in_fs = 0;
      __out_fs = 0;
      __mode = io_factory::MODE_DEFAULT;
    }
  
    int 
    io_factory::__reset ()
    {

      if (is_read ()) 
	{
	  __reset_read_archive ();
	  __reset_read ();
	}
      
      if (is_write ())
	{
	  __reset_write_archive ();
	  __reset_write ();
	}
      __ctor_defaults ();
      return 0;
    }

    // ctor
    io_factory::io_factory (int mode_)
    {
      __ctor_defaults ();
      __init("", mode_ );
    }

    // ctor
    io_factory::io_factory (const std::string & stream_name_ , 
			    int mode_ )
    {
      __ctor_defaults ();
      __init(stream_name_, mode_);
    }

    // dtor
    io_factory::~io_factory ()
    {
      __reset ();
      if (io_factory::g_debug) 
	{
	  std::cerr << "DEBUG: io_factory::~io_factory." << std::endl;
	}
    }

    void io_factory::tree_dump(std::ostream & out_, 
			       const std::string & title_,
			       const std::string & indent_,
			       bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty()) 
	{
	  out_ << indent << title_ << std::endl;
	}

      out_ << indent << du::i_tree_dumpable::tag 
	   << "Mode  : " << std::hex << __mode << std::dec << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_read  : " << is_read() << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_write : " << is_write() << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_compressed : " << is_compressed () << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_uncompressed : " << is_uncompressed () << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_gzip : " << is_gzip () << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_bzip2 : " << is_bzip2 () << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_text : " << is_text () << std::endl;

#ifdef DATATOOLS_USE_PBA
      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_binary : " << is_binary () << std::endl;
#endif

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_xml : " << is_xml () << std::endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "is_single_archive : " << is_single_archive () << std::endl;

      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
	   << "is_multi_archives : " << is_multi_archives () << std::endl;

    }

    void 
    io_factory::dump (std::ostream & out_) const
    {
      io_factory::tree_dump (out_, "io_factory::dump:");
    }

    int 
    io_factory::guess_mode_from_filename (const std::string & filename_, 
					  int & mode_ )
    {
      int status = io_factory::SUCCESS;
      int mode = 0x0;
      std::string fn = filename_;
      boost::char_separator<char> sep (".");
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      tokenizer tokens (fn, sep);
      std::list<std::string> ltok;
      for(tokenizer::iterator i = tokens.begin (); 
	  i != tokens.end(); 
	  ++i) 
	{
	  std::string token = *i;
	  if (io_factory::g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: token=<" 
			<< token << ">" << std::endl;
	    }
	  ltok.push_front (token);
	}
      
      bool gz = false;
      bool bz2 = false;
      bool txt = false;
      bool bin = false;
      bool xml = false;
      bool comp = false;
      bool format = false;
      size_t ext_count = 0;
      for (std::list<std::string>::const_iterator i = ltok.begin ();
	   i != ltok.end ();
	   i++) 
	{
	  if (io_factory::g_debug) 
	    {
	      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: ltok=<" 
			<< *i << ">" << std::endl;
	    }
	  std::string ext = *i;
	  if (! comp) 
	    {
	      if (ext == GZ_EXT) // || ext == "GZ") 
		{
		  comp = gz = true;
		  if (io_factory::g_debug) 
		    {
		      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: "
				<< "mode+=GZIP" << std::endl; 
		    }
		}
	      else if (ext == BZIP2_EXT) // || ext == "BZ2") 
		{
		  comp = bz2 = true;
		  if (io_factory::g_debug) 
		    {
		      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: "  
				<< "mode+=BZIP2" << std::endl; 
		    }
		}
	    }
	  if (! format) 
	    {
	      if (ext == TXT_EXT) // || ext == "TXT") 
		{
		  format = txt = true;
		  if (io_factory::g_debug) 
		    {
		      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=TEXT" << std::endl; 
		    }
		}
	      else if (ext == BIN_EXT) // || ext == "DATA") 
		{
		  format = bin = true;
		  if (io_factory::g_debug) 
		    {
		      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=BINARY" << std::endl; 
		    }
		}
	      else if (ext == XML_EXT) // || ext == "XML") 
		{
		  format=xml=true;
		  if (io_factory::g_debug) 
		    {
		      std::cerr << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=XML" << std::endl; 
		    }
		}
	    }
	  if (! format && ! comp) break;
	  if (format ) break;
	  ext_count++;
	  if (ext_count == 2) break;
	}

      if (! format) 
	{
	  // cannot guess format from extension:
	  status = io_factory::ERROR;
	}
      else 
	{
	  mode &= ~ io_factory::MASK_COMPRESSION;
	  if (comp)  
	    {
	      if (gz)  mode |= io_factory::MODE_GZIP;
	      if (bz2) mode |= io_factory::MODE_BZIP2;
	    }

	  mode &= ~ io_factory::MASK_FORMAT;
	  if (format) 
	    {
	      if (bin) mode |= io_factory::MODE_BINARY;
	      if (txt) mode |= io_factory::MODE_TEXT;
	      if (xml) mode |= io_factory::MODE_XML;
	    }
	}
      mode_ = mode;
      return status;
    }

    /***********************************************************/

    io_reader::io_reader (int mode_) 
      : io_factory(io_factory::MODE_READ | mode_)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_reader::io_reader: cannot force not-read mode!");
	}
    }

    io_reader::io_reader (const std::string & stream_name_ , 
			  int mode_)
      : io_factory (stream_name_, io_factory::MODE_READ|mode_)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_reader::io_reader: cannot force not-read mode!");
	}
    }

    io_reader::~io_reader ()
    {
      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_reader::~io_reader." << std::endl;
	}
    }

    /***********************************************************/

    io_writer::io_writer (int mode_) 
      : io_factory (io_factory::MODE_WRITE | mode_)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_writer::io_writer: cannot force write mode!");
	}
    }

    io_writer::io_writer(const std::string & stream_name_, 
			 int mode_)
      : io_factory (stream_name_, io_factory::MODE_WRITE | mode_)
    {
      if (! is_write ()) 
	{
	  throw std::runtime_error ("io_writer::io_writer: cannot force read mode!");
	}
    }

    io_writer::~io_writer ()
    {
      if (g_debug) 
	{
	  std::cerr << "DEBUG: io_writer::~io_writer." << std::endl;
	}
    }

    /***********************************************************/

    void 
    data_reader::__read_next_tag ()
    {
      if (__status != STATUS_OK) 
	{
	  __next_tag = EMPTY_RECORD_TAG;
	  return;
	}
      try 
	{
	  if (__reader->is_multi_archives ()) __reader->start_archive ();
	  std::string tag_id;
	  tag_id = "";
	  __next_tag = "";
	  this->_basic_load (tag_id);
	  __next_tag = tag_id;
	}
      catch (std::runtime_error & x) 
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
	      std::cerr << "WARNING: data_reader::__read_next_tag: " 
			<< x.what () << std::endl;
	    }
	  //<<<
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
      catch (std::exception & x) 
	{
	  std::cerr << "WARNING: data_reader::__read_next_tag: " 
		    << x.what () << std::endl;
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
      catch (...) 
	{
	  std::cerr << "WARNING: data_reader::__read_next_tag: " 
		    << "unexpected exception!" << std::endl;
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
    }

    void 
    data_reader::__init_reader (const std::string & filename_, 
				int mode_)
    {
      __status = STATUS_OK;
      __reader = new io_reader (filename_, mode_);
      __read_next_tag ();
    }

    void 
    data_reader::__reset_reader ()
    {
      if (__reader != 0) 
	{
	  delete __reader;
	  __reader = 0;
	}
      __next_tag = "";
      __status = STATUS_OK;
    }

    const std::string 
    data_reader::EMPTY_RECORD_TAG = "";

    const std::string & 
    data_reader::get_record_tag () const
    {
      if (__status != STATUS_OK) return EMPTY_RECORD_TAG;
      return __next_tag;
    }

    bool
    data_reader::is_initialized () const
    {
      return __reader != 0;
    }

    bool
    data_reader::is_uncompressed () const
    {
      return ! is_compressed ();
    }

    bool
    data_reader::is_compressed () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_compressed: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_compressed ();
    }

    bool
    data_reader::is_gzip () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_gzip: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_gzip ();
    }

    bool
    data_reader::is_text () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_text: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_text ();
    }

    bool
    data_reader::is_binary () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_binary: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_binary ();
    }

    bool
    data_reader::is_xml () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_xml: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_xml ();
    }

    bool
    data_reader::is_bzip2 () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_bzip2: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_bzip2 ();
    }

    bool
    data_reader::is_single_archive () const
    {
      return ! is_multi_archives ();
    }

    bool
    data_reader::is_multi_archives () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_multi_archives: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_multi_archives ();
    }

    bool 
    data_reader::has_record_tag () const
    {
      if (__status != STATUS_OK) return false;
      if (__next_tag.empty()) return false;
      return true;
    }

    bool 
    data_reader::record_tag_is (const std::string & tag_) const
    {
      return __next_tag == tag_;
    }

    void 
    data_reader::reset ()
    {
      __reset_reader ();
    }

    void 
    data_reader::init (const std::string & filename_, 
		       bool multiple_archives_)
    {
      __reset_reader ();
      int mode_guess;
      if (io_factory::guess_mode_from_filename (filename_, mode_guess) 
	  != io_factory::SUCCESS) 
	{
	  std::ostringstream message;
	  message << "data_reader::init: cannot guess mode for file '" 
		  << filename_ << "'!";
	  throw std::runtime_error (message.str ());
	}
      int mode = mode_guess;
      if (multiple_archives_)
	{
	  mode |= io_factory::multi_archives; 
	}
      __init_reader (filename_, mode);
    }

    void 
    data_reader::init (const std::string & filename_, int mode_)
    {
      __reset_reader ();
      __init_reader (filename_, mode_);
    }

    data_reader::data_reader () 
    {
      __reader = 0;
    }

    data_reader::data_reader (const std::string & filename_, 
			      bool multiple_archives_)
    {
      __reader = 0;
      init (filename_, multiple_archives_);
    }

    data_reader::data_reader (const std::string & filename_, int mode_)
    {
      __reader = 0;
      __init_reader (filename_, mode_);
    }

    data_reader::~data_reader ()
    {
      reset ();
      if (io_factory::g_debug) 
	{
	  std::cerr << "DEBUG: data_reader::~data_reader." << std::endl;
	}
    }

    /***********************************************************/

    bool
    data_writer::is_initialized () const
    {
      return __writer != 0;
    }

    bool
    data_writer::is_uncompressed () const
    {
      return ! is_compressed ();
    }

    bool
    data_writer::is_compressed () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_compressed: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_compressed ();
    }

    bool
    data_writer::is_gzip () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_gzip: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_gzip ();
    }

    bool
    data_writer::is_text () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_text: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_text ();
    }

    bool
    data_writer::is_binary () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_binary: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_binary ();
    }

    bool
    data_writer::is_xml () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_xml: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_xml ();
    }

    bool
    data_writer::is_bzip2 () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_bzip2: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_bzip2 ();
    }

    bool
    data_writer::is_single_archive () const
    {
      return ! is_multi_archives ();
    }

    bool
    data_writer::is_multi_archives () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_multi_archives: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_multi_archives ();
    }
	
    void 
    data_writer::__init_writer (const std::string & filename_ , 
				int mode_)
    {
      __writer = new io_writer (filename_, mode_);
    }

    void 
    data_writer::__reset_writer ()
    {
      if (__writer != 0)
	{
	  delete __writer;
	  __writer = 0;
	}
    }

    void 
    data_writer::reset ()
    {
      __reset_writer ();
    }

    void 
    data_writer::init (const std::string & filename_, 
		       bool multiple_archives_,
		       bool append_mode_)
    {
      __reset_writer ();
      int mode_guess;
      if (io_factory::guess_mode_from_filename (filename_, mode_guess) 
	  != io_factory::SUCCESS) 
	{
	  std::ostringstream message;
	  message << "data_writer::init: cannot guess mode for file '" 
		  << filename_ << "'!";
	  throw std::runtime_error(message.str());
	}
      int mode = mode_guess;
      if (multiple_archives_)
	{
	  mode |= io_factory::multi_archives; 
	}
      if (append_mode_)
	{
	  mode |= io_factory::append; 
	}
      __init_writer (filename_, mode);
    }

    void 
    data_writer::init (const std::string & filename_, 
		       int mode_)
    {
      __reset_writer ();
      int mode = mode_;
      __init_writer (filename_, mode);
    }

    // ctor
    data_writer::data_writer () 
    {
      __writer = 0;
    }

    // ctor
    data_writer::data_writer (const std::string & filename_, 
			      bool multiple_archives_,
			      bool append_mode_)
    {
      __writer = 0;
      init (filename_, multiple_archives_, append_mode_);
    }

    // ctor
    data_writer::data_writer (const std::string & filename_, int mode_)
    {
      __writer = 0;
      __init_writer (filename_, mode_);
    }

    // dtor
    data_writer::~data_writer ()
    {
      reset ();
      if (io_factory::g_debug) 
	{
	  std::cerr << "DEBUG: data_writer::~data_writer." << std::endl;
	}
    }
 
  } // end of namespace serialization
  
} // end of namespace datatools

// end of io_factory.cc
