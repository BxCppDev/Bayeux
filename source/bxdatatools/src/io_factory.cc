// -*- mode: c++; -*- 
/* io_factory.cc  
 */ 
 
#include <datatools/serialization/io_factory.h>
#include <boost/archive/codecvt_null.hpp>

namespace datatools { 
  
  namespace serialization {

    // file extensions recognized by the library:
    const std::string io_factory::TXT_EXT   = "txt";
    const std::string io_factory::XML_EXT   = "xml";
    const std::string io_factory::BIN_EXT   = "data";
    const std::string io_factory::GZ_EXT    = "gz";
    const std::string io_factory::BZIP2_EXT = "bz2";

    bool io_factory::g_debug = false;
    bool io_factory::g_warning = false;

    bool 
    io_factory::eof () const
    {
      if (is_write ()) return false;
      if (__in != 0 && __in->eof ()) return true;
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

    int io_factory::__init_read_archive ()
    {
      bool devel = g_debug;
      if (devel)
	{
	  std::clog << "DEBUG: io_factory::__init_read_archive: Entering..." << std::endl;
	}
 
      /*** text archive ***/
      if (is_text ()) 
	{
	  if (devel) std::clog << "DEBUG: io_factory::__init_read_archive: "
				 << "text with FPU..." 
				 << std::endl;
	  __itar_ptr = new boost::archive::text_iarchive (*__in, 
							  boost::archive::no_codecvt);
	  if (devel) 
	    {
	      std::clog << "DEBUG: io_factory::__init_read_archive: " 
			<< "'boost::archive::text_iarchive' library version " 
			<< __itar_ptr->get_library_version () << std::endl;
	    }
	}
      /*** XML archive ***/
      else if (is_xml ()) 
	{
	  __ixar_ptr = new boost::archive::xml_iarchive (*__in, 
							 boost::archive::no_codecvt);
	  if (devel) 
	    {
	      std::clog << "DEBUG: io_factory::__init_read_archive: " 
			<< "'boost::archive::xml_iarchive' library version " 
			<< __ixar_ptr->get_library_version () << std::endl;
	    }
	}
      /*** binary archive ***/
      else if (is_binary ())
	{
	  __ibar_ptr = new boost::archive::portable_binary_iarchive (*__in);
	  if (devel) 
	    {
	      std::cerr << "DEBUG: io_factory::__init_read_archive: library version = " 
			<< __ibar_ptr->get_library_version ()  << std::endl;
	    }
	  //std::cerr << "DEVEL: io_factory::__init_read_archive: __ibar_ptr = " << __ibar_ptr << std::endl;
	}
      else 
	{
	  throw std::runtime_error ("io_factory::__init_read_archive: format not supported!");
	}
      __read_archive_is_initialized = true;
      if (devel) 
	{ 
	  std::clog << "DEVEL: io_factory::__init_read_archive: Exiting." << std::endl;
	}
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

      if (__ibar_ptr != 0) 
	{
	  delete __ibar_ptr;
	  __ibar_ptr = 0;
	}

      __read_archive_is_initialized = false;
      return 0;
    }
    

    int 
    io_factory::__init_read (const std::string & a_stream_name)
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
      
      if (a_stream_name.empty ()) 
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_read: cin..." 
			<< std::endl;
	    }
	  __in_fs->push (std::cin);
	}
      else 
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_read: file='" 
			<< a_stream_name << "'" << std::endl;
	    }
	  if (is_compressed () || is_binary ()) 
	    {
	      __fin = new std::ifstream (a_stream_name.c_str (), 
					 std::ios_base::in | std::ios_base::binary);
	    }
	  else 
	    {
	      __fin = new std::ifstream (a_stream_name.c_str (), 
					 std::ios_base::in);
	    }
	  if (! *__fin) 
	    {
	      throw std::runtime_error ("io_factory::__init_read: Cannot open input stream!");
	    }
	  __in_fs->push (*__fin);
	}

      __in = __in_fs;
      if (is_text () || is_xml ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::__init_read: "
			<< "FPU: stream.imbue..." 
			<< std::endl;
	    }
	  __in->imbue (*__locale);
	}

      return 0;
    }

    int 
    io_factory::__reset_read ()
    {
      if (g_debug) 
	{
	  std::clog << "DEBUG: io_factory::__reset_read:..." << std::endl;
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
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write_archive: "
			<< "with FPU: text archive..." 
			<< std::endl;
	    }
	  __otar_ptr = new boost::archive::text_oarchive (*__out, 
							  boost::archive::no_codecvt);	  
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write_archive: "
			<< "'boost::archive::text_oarchive' library version " 
			<< __otar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_xml ()) 
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write_archive: "
			<< "with FPU: XML archive..." 
			<< std::endl;
	    }
	  __oxar_ptr = new boost::archive::xml_oarchive (*__out, 
							 boost::archive::no_codecvt);
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write_archive: "
			<< "'boost::archive::xml_oarchive' library version " 
			<< __oxar_ptr->get_library_version () << std::endl;
	    }
	}
      else if (is_binary ()) 
	{
	  __obar_ptr = new boost::archive::portable_binary_oarchive (*__out);
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write_archive: "
			<< "'boost::archive::portable_binary_oarchive' library version " 
			<< __obar_ptr->get_library_version () << std::endl;
	    }
	}
      else 
	{
	  throw std::runtime_error ("io_factory::__init_write_archive: format not supported!");
	}
      __write_archive_is_initialized = true;
      return 0;
    }

    int
    io_factory::__init_write (const std::string & a_stream_name)
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

      if (a_stream_name.empty ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::__init_write: cout..." 
			<< std::endl;
	    }
	  __out_fs->push (std::cout);
	  return 0;
	}
      else 
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write: file='" 
			<< a_stream_name << "'"<< std::endl;
	    }

	  std::ios_base::openmode open_mode = std::ios_base::out;
	  if (is_compressed () || is_binary ())
	    { 
	      open_mode |= std::ios_base::binary;
	    }
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
	  __fout = new std::ofstream (a_stream_name.c_str (), open_mode);
	  if (!*__fout) 
	    {
	      throw std::runtime_error ("io_factory::__init_write: Cannot open output stream!");
	    }
	  __out_fs->push (*__fout);
	}

      __out = __out_fs;
      if (is_text () || is_xml ())
	{ 
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init_write: stream.imbue" 
			<< std::endl;
	    }
	  __out->imbue (*__locale);
	}

      if (g_debug) 
	{
	  std::clog << "DEBUG: io_factory::__init_write: mode='" 
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
	      std::clog << "DEBUG: io_factory::__reset_write_archive: "
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
	      std::clog << "DEBUG: io_factory::__reset_write_archive: "
			<< "delete XML archive!" << std::endl;
	    }
	  delete __oxar_ptr;
	  __oxar_ptr = 0;
	}

      if (__obar_ptr != 0) 
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__reset_write_archive: "
			<< "delete binary archive!" << std::endl;
	    }
	  delete __obar_ptr;
	  __obar_ptr = 0;
	}

      __write_archive_is_initialized = false;
      return 0;
    }

    int 
    io_factory::__reset_write ()
    {
      if (g_debug) 
	{
	  std::clog << "DEBUG: io_factory::__reset_write:..." << std::endl;
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
    io_factory::__init (const std::string & a_stream_name, int a_mode)
    {
      __mode = a_mode;
      if (is_read ()) 
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::__init: read mode..." 
			<< std::endl;
	    }
	  __init_read (a_stream_name);
	  if (is_single_archive ())
	    {
	      __init_read_archive ();
	    }
	}
      else
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::__init: write mode..." 
			<< std::endl;
	    }
	  __init_write (a_stream_name);
	  if (is_single_archive ())
	    {
	      __init_write_archive ();
	    }
	}
      return 0;
    }
    
    void io_factory::start_archive ()
    {
      //std::cerr << "DEVEL: io_factory::start_archive: Entering..." << std::endl;
      if (is_multi_archives ())
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::start_archive: multi..." 
			<< std::endl;
	    }
	  if (is_read ())
	    {
	      __init_read_archive ();
	    }
	  if (is_write ())
	    {
	      __init_write_archive ();
	    }
	}
      //std::cerr << "DEVEL: io_factory::start_archive: Exiting." << std::endl;
      return;
    }
    
    void io_factory::stop_archive ()
    {
      if (is_multi_archives ())
	{
	  if (g_debug) 
	    {
	      std::clog << "DEBUG: io_factory::stop_archive: multi..." 
			<< std::endl;
	    }
	  if (is_read ())
	    {
	      __reset_read_archive ();
	    }
	  if (is_write ())
	    {
	      __reset_write_archive ();
	    }
	}
     return;
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
      __ibar_ptr = 0;
      __obar_ptr = 0;
      __in_fs = 0;
      __out_fs = 0;
      __mode = io_factory::MODE_DEFAULT;
      return;
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
      if (g_debug) std::clog << "DEBUG: io_factory::__reset: Use FPU" << std::endl;
      if (__locale)
	{
	  delete __locale; 
	  __locale = 0; 
	} 
      if (__default_locale)
	{
	  delete __default_locale;
	  __default_locale = 0;
	}
      return 0;
    }

    // ctor
    io_factory::io_factory (int a_mode)
    {
      __default_locale = 0;
      __locale = 0;
      __default_locale = new std::locale (std::locale::classic (), 
					  new boost::archive::codecvt_null<char>);
      bool write = ((a_mode & MASK_RW) != 0);
      if (write)
	{
	  __locale = new std::locale (*__default_locale, 
				      new boost::math::nonfinite_num_put<char>);
	}
      else
	{
	  __locale = new std::locale (*__default_locale, 
				      new boost::math::nonfinite_num_get<char>);
	}
      __ctor_defaults ();
      __init ("", a_mode);
      return;
    }

    // ctor
    io_factory::io_factory (const std::string & a_stream_name, 
			    int a_mode)
    {
      __default_locale = 0;
      __locale = 0;
      __default_locale = new std::locale (std::locale::classic (), 
					  new boost::archive::codecvt_null<char>);
      bool write = ((a_mode & MASK_RW) != 0);
      if (write)
	{
	  __locale = new std::locale (*__default_locale, 
				      new boost::math::nonfinite_num_put<char>);
	}
      else
	{
	  __locale = new std::locale (*__default_locale, 
				      new boost::math::nonfinite_num_get<char>);
	}
      __ctor_defaults ();
      __init (a_stream_name, a_mode);
      return;
    }

    // dtor
    io_factory::~io_factory ()
    {
      __reset ();
      if (io_factory::g_debug) 
	{
	  std::clog << "DEBUG: io_factory::~io_factory." << std::endl;
	}
      return;
    }

    void 
    io_factory::tree_dump (std::ostream & a_out, 
			   const std::string & a_title, 
			   const std::string & a_indent, 
			   bool a_inherit) const
    {
      namespace du = datatools::utils; // tree_trick
      std::ostringstream tag_ss, last_tag_ss; // tree_trick
      tag_ss << du::i_tree_dumpable::tag; // tree_trick
      last_tag_ss << du::i_tree_dumpable::inherit_tag (a_inherit); // tree_trick
      std::string tag = tag_ss.str (); // tree_trick
      std::string last_tag = last_tag_ss.str (); // tree_trick
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty()) 
	{
	  a_out << indent << a_title << std::endl;
	}

      a_out << indent << tag 
	   << "Mode  : " << std::hex << __mode << std::dec << std::endl;

      a_out << indent << tag 
	   << "is_read  : " << is_read() << std::endl;

      a_out << indent << tag 
	   << "is_write : " << is_write() << std::endl;

      a_out << indent << tag 
	   << "is_compressed : " << is_compressed () << std::endl;

      a_out << indent << tag 
	   << "is_uncompressed : " << is_uncompressed () << std::endl;

      a_out << indent << tag 
	   << "is_gzip : " << is_gzip () << std::endl;

      a_out << indent << tag 
	   << "is_bzip2 : " << is_bzip2 () << std::endl;

      a_out << indent << tag 
	   << "is_text : " << is_text () << std::endl;

      a_out << indent << tag 
	   << "is_binary : " << is_binary () << std::endl;

      a_out << indent << tag 
	   << "is_xml : " << is_xml () << std::endl;

      a_out << indent << tag 
	   << "is_single_archive : " << is_single_archive () << std::endl;

      a_out << indent << last_tag 
	   << "is_multi_archives : " << is_multi_archives () << std::endl;

      return;
    }

    void 
    io_factory::dump (std::ostream & a_out) const
    {
      io_factory::tree_dump (a_out, "io_factory::dump:");
      return;
    }

    int 
    io_factory::guess_mode_from_filename (const std::string & a_filename, 
					  int & a_mode)
    {
      int status = io_factory::SUCCESS;
      int mode = 0x0;
      std::string fn = a_filename;
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
	      std::clog << "DEBUG: io_factory::guess_mode_from_filename: token=<" 
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
	      std::clog << "DEBUG: io_factory::guess_mode_from_filename: ltok=<" 
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
		      std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
				<< "mode+=GZIP" << std::endl; 
		    }
		}
	      else if (ext == BZIP2_EXT) // || ext == "BZ2") 
		{
		  comp = bz2 = true;
		  if (io_factory::g_debug) 
		    {
		      std::clog << "DEBUG: io_factory::guess_mode_from_filename: "  
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
		      std::clog << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=TEXT" << std::endl; 
		    }
		}
	      else if (ext == BIN_EXT) // || ext == "DATA") 
		{
		  format = bin = true;
		  if (io_factory::g_debug) 
		    {
		      std::clog << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=BINARY" << std::endl; 
		    }
		}
	      else if (ext == XML_EXT) // || ext == "XML") 
		{
		  format=xml=true;
		  if (io_factory::g_debug) 
		    {
		      std::clog << "DEBUG: io_factory::guess_mode_from_filename: " 
				<< "mode+=XML" << std::endl; 
		    }
		}
	    }
	  if (! format && ! comp) break;
	  if (format) break;
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
      a_mode = mode;
      return status;
    }

    /***********************************************************/

    io_reader::io_reader (int a_mode) 
      : io_factory(io_factory::MODE_READ | a_mode)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_reader::io_reader: cannot force not-read mode!");
	}
      return;
    }

    io_reader::io_reader (const std::string & a_stream_name, 
			  int a_mode)
      : io_factory (a_stream_name, io_factory::MODE_READ|a_mode)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_reader::io_reader: cannot force not-read mode!");
	}
      return;
    }

    io_reader::~io_reader ()
    {
      if (g_debug) 
	{
	  std::clog << "DEBUG: io_reader::~io_reader." << std::endl;
	}
      return;
    }

    /***********************************************************/

    io_writer::io_writer (int a_mode) 
      : io_factory (io_factory::MODE_WRITE | a_mode)
    {
      if (! is_read ()) 
	{
	  throw std::runtime_error ("io_writer::io_writer: cannot force write mode!");
	}
      return;
    }

    io_writer::io_writer (const std::string & a_stream_name, 
			  int a_mode)
      : io_factory (a_stream_name, io_factory::MODE_WRITE | a_mode)
    {
      if (! is_write ()) 
	{
	  throw std::runtime_error ("io_writer::io_writer: cannot force read mode!");
	}
       return;
   }

    io_writer::~io_writer ()
    {
      if (g_debug) 
	{
	  std::clog << "DEBUG: io_writer::~io_writer." << std::endl;
	}
      return;
    }

    /***********************************************************/
    void data_reader::dump (std::ostream & a_out) const
    {
      using namespace std;
      a_out << "data_reader::dump: " << endl;
      a_out << " |-- " << "Status   : " << (__status== 0? "Ok": "Error") << endl;
      a_out << " |   " << " |-- " << "Initialized      : " << is_initialized () << endl;
      a_out << " |   " << " |-- " << "Mult. arch.      : " << is_multi_archives () << endl;
      a_out << " |   " << " |-- " << "Single arch.     : " << is_single_archive () << endl;
      a_out << " |   " << " |-- " << "Compressed       : " << is_compressed () << endl;
      a_out << " |   " << " |-- " << "Gzipped          : " << is_gzip () << endl;
      a_out << " |   " << " |-- " << "Bzipped          : " << is_bzip2 () << endl;
      a_out << " |   " << " |-- " << "Text arch.       : " << is_text () << endl;
      a_out << " |   " << " |-- " << "XML arch.        : " << is_xml () << endl;
      a_out << " |   " << " `-- " << "Bin arch. (port) : " << is_portable_binary () << endl;
      a_out << " |-- " << "Reader   : " << (__reader != 0? "Yes": "No") << endl;
      a_out << " `-- " << "Next tag : '" << (__next_tag) << "'" << endl;
      return;
    }

    void 
    data_reader::__read_next_tag () 
    {
      bool devel = false;
      //devel = true;
      if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: Entering..." << std::endl;
      if (__status != STATUS_OK) 
	{
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: " 
		    << "status != STATUS_Ok" << std::endl;
	  __next_tag = EMPTY_RECORD_TAG;
	  return;
	}
      if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: "
		<< "Continue..." << std::endl;
      try 
	{
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: " 
			       << "try block starts..." << std::endl;
	  if (__reader->is_multi_archives ()) 
	    {
	      if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: "
				   << "multi-archives..." << std::endl;
	      __reader->start_archive ();
	    }
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: "
			       << "Continue..." << std::endl;
	  std::string tag_id;
	  tag_id = "";
	  __next_tag = "";
	  this->_basic_load (tag_id);
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: "
			       << "_basic_load done with tag_id = '" 
			       << tag_id << "'" << std::endl;
	  __next_tag = tag_id;
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: " 
			       << "__next_tag = " << __next_tag << std::endl;
	}
      catch (std::runtime_error & x) 
	{
	  bool warn =io_factory::g_warning;
	  //>>> 2008-11-13 FM: skip EOF message printing
	  std::string msg = x.what ();
	  if (msg.find ("EOF") != msg.npos)
	    {
	      warn = false;
	    }
	  if (warn)
	    {
	      std::clog << "WARNING: data_reader::__read_next_tag: runtime_error=" 
			<< x.what () << std::endl;
	      if (io_factory::g_warning)
		{
		  std::clog << "WARNING: data_reader::__read_next_tag: runtime_error=" 
			    << x.what () << std::endl;
		}
	    }
	  //<<<
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	  if (devel) std::cerr << "DEVEL: data_reader::__read_next_tag: "
			       << "BOOOM !" << std::endl;
	}
      // 2011-02-25 FM: 
     /* catch (boost::archive::archive_exception & x) 
	{
	  if (io_factory::g_warning)
	    {
	      std::clog << "WARNING: data_reader::__read_next_tag: archive exception is: " 
			<< x.what () << std::endl;
	    }
	  
	    std::clog << "WARNING: data_reader::__read_next_tag: archive exception is: " 
		    << x.what () << std::endl;
	  
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	  // throw x;
	}*/
      catch (std::exception & x) 
	{
	  bool warn = io_factory::g_warning;
	  if (warn)
	    {
	      std::clog << "WARNING: data_reader::__read_next_tag: exception=" 
			<< x.what () << std::endl;
	    }
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
      catch (...) 
	{
	  std::clog << "WARNING: data_reader::__read_next_tag: " 
		    << "unexpected exception!" << std::endl;
	  __status   = STATUS_ERROR;
	  __next_tag = EMPTY_RECORD_TAG;
	}
      /*
      std::cerr << "DEVEL: data_reader::__read_next_tag: Exiting." 
		<< std::endl;
      */
      return;
    }

    bool data_reader::is_error () const
    {
      return __status == STATUS_ERROR;
    }

    void 
    data_reader::__init_reader (const std::string & a_filename, 
				int a_mode)
    {
      __status = STATUS_OK;
      __reader = new io_reader (a_filename, a_mode);
      __read_next_tag ();
      return;
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
      return;
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
    data_reader::is_portable_binary () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_reader::is_portable_binary: reader is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __reader->is_portable_binary ();
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
    data_reader::record_tag_is (const std::string & a_tag) const
    {
      return __next_tag == a_tag;
    }

    void 
    data_reader::reset ()
    {
      __reset_reader ();
      return;
    }

    void 
    data_reader::init (const std::string & a_filename, 
		       bool a_multiple_archives)
    {
      __reset_reader ();
      int mode_guess;
      if (io_factory::guess_mode_from_filename (a_filename, mode_guess) 
	  != io_factory::SUCCESS) 
	{
	  std::ostringstream message;
	  message << "data_reader::init: cannot guess mode for file '" 
		  << a_filename << "'!";
	  throw std::runtime_error (message.str ());
	}
      int mode = mode_guess;
      if (a_multiple_archives)
	{
	  mode |= io_factory::multi_archives; 
	}
      __init_reader (a_filename, mode);
      return;
    }

    void 
    data_reader::init (const std::string & a_filename, int a_mode)
    {
      __reset_reader ();
      __init_reader (a_filename, a_mode);
      return;
    }

    data_reader::data_reader () 
    {
      __reader = 0;
      return;
    }

    data_reader::data_reader (const std::string & a_filename, 
			      bool a_multiple_archives)
    {
      __reader = 0;
      init (a_filename, a_multiple_archives);
      return;
    }

    data_reader::data_reader (const std::string & a_filename, int a_mode)
    {
      __reader = 0;
      __init_reader (a_filename, a_mode);
      return;
    }

    data_reader::~data_reader ()
    {
      reset ();
      if (io_factory::g_debug) 
	{
	  std::clog << "DEBUG: data_reader::~data_reader: Done." << std::endl;
	}
      return;
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
    data_writer::is_portable_binary () const
    {
      if (! is_initialized ())
	{
	  std::ostringstream message;
	  message << "data_writer::is_portable_binary: writer is not initialized!"; 
	  throw std::runtime_error (message.str ());
	}
      return __writer->is_portable_binary ();
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
    data_writer::__init_writer (const std::string & a_filename, 
				int a_mode)
    {
      __writer = new io_writer (a_filename, a_mode);
      return;
    }

    void 
    data_writer::__reset_writer ()
    {
      if (__writer != 0)
	{
	  delete __writer;
	  __writer = 0;
	}
       return;
   }

    void 
    data_writer::reset ()
    {
      __reset_writer ();
      return;
    }

    void 
    data_writer::init (const std::string & a_filename, 
		       bool a_multiple_archives, 
		       bool a_append_mode)
    {
      __reset_writer ();
      int mode_guess;
      if (io_factory::guess_mode_from_filename (a_filename, mode_guess) 
	  != io_factory::SUCCESS) 
	{
	  std::ostringstream message;
	  message << "data_writer::init: cannot guess mode for file '" 
		  << a_filename << "'!";
	  throw std::runtime_error(message.str());
	}
      int mode = mode_guess;
      if (a_multiple_archives)
	{
	  mode |= io_factory::multi_archives; 
	}
      if (a_append_mode)
	{
	  mode |= io_factory::append; 
	}
      __init_writer (a_filename, mode);
       return;
   }

    void 
    data_writer::init (const std::string & a_filename, 
		       int a_mode)
    {
      __reset_writer ();
      int mode = a_mode;
      __init_writer (a_filename, mode);
       return;
   }

    // ctor
    data_writer::data_writer () 
    {
      __writer = 0;
      return;
    }

    // ctor
    data_writer::data_writer (const std::string & a_filename, 
			      bool a_multiple_archives, 
			      bool a_append_mode)
    {
      __writer = 0;
      init (a_filename, a_multiple_archives, a_append_mode);
      return;
    }

    // ctor
    data_writer::data_writer (const std::string & a_filename, int a_mode)
    {
      __writer = 0;
      __init_writer (a_filename, a_mode);
      return;
    }

    // dtor
    data_writer::~data_writer ()
    {
      reset ();
      if (io_factory::g_debug) 
	{
	  std::clog << "DEBUG: data_writer::~data_writer." << std::endl;
	}
      return;
    }
 
  } // end of namespace serialization
  
} // end of namespace datatools

// end of io_factory.cc
