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
      if (_in_ != 0 && _in_->eof ()) return true;
      return false;
    }

    bool
    io_factory::is_read () const
    {
      return (_mode_ & MASK_RW) == 0;
    }

    bool
    io_factory::is_write () const
    {
      return (_mode_ & MASK_RW) != 0;
    }

    bool
    io_factory::is_compressed () const
    {
      return (_mode_ & MASK_COMPRESSION) != 0;
    }

    bool
    io_factory::is_uncompressed () const
    {
      return (_mode_ & MASK_COMPRESSION) == 0;
    }

    bool
    io_factory::is_gzip () const
    {
      return (_mode_ & MASK_COMPRESSION) == MODE_GZIP;
    }

    bool
    io_factory::is_bzip2 () const
    {
      return (_mode_ & MASK_COMPRESSION) == MODE_BZIP2;
    }

    bool
    io_factory::is_text () const
    {
      return (_mode_ & MASK_FORMAT) == MODE_TEXT;
    }

    bool
    io_factory::is_binary () const
    {
      return (_mode_ & MASK_FORMAT) == MODE_BINARY;
    }

    bool
    io_factory::is_xml () const
    {
      return (_mode_ & MASK_FORMAT) == MODE_XML;
    }

    bool
    io_factory::is_single_archive () const
    {
      return (_mode_ & MASK_MULTIARCHIVE) == MODE_UNIQUE_ARCHIVE;
    }

    bool
    io_factory::is_multi_archives () const
    {
      return ! is_single_archive ();
    }

    bool
    io_factory::is_no_append () const
    {
      return (_mode_ & MASK_APPEND) == MODE_NO_APPEND;
    }

    bool
    io_factory::is_append () const
    {
      return ! is_no_append ();
    }

    int io_factory::_init_read_archive_ ()
    {
      bool devel = g_debug;
      if (devel)
	{
	  std::clog << "DEBUG: io_factory::_init_read_archive_: Entering..." << std::endl;
	}

      /*** text archive ***/
      if (is_text ())
	{
	  if (devel) std::clog << "DEBUG: io_factory::_init_read_archive_: "
				 << "text with FPU..."
				 << std::endl;
	  _itar_ptr_ = new boost::archive::text_iarchive (*_in_,
							  boost::archive::no_codecvt);
	  if (devel)
	    {
	      std::clog << "DEBUG: io_factory::_init_read_archive_: "
			<< "'boost::archive::text_iarchive' library version "
			<< _itar_ptr_->get_library_version () << std::endl;
	    }
	}
      /*** XML archive ***/
      else if (is_xml ())
	{
	  _ixar_ptr_ = new boost::archive::xml_iarchive (*_in_,
							 boost::archive::no_codecvt);
	  if (devel)
	    {
	      std::clog << "DEBUG: io_factory::_init_read_archive_: "
			<< "'boost::archive::xml_iarchive' library version "
			<< _ixar_ptr_->get_library_version () << std::endl;
	    }
	}
      /*** binary archive ***/
      else if (is_binary ())
	{
	  _ibar_ptr_ = new boost::archive::portable_binary_iarchive (*_in_);
	  if (devel)
	    {
	      std::cerr << "DEBUG: io_factory::_init_read_archive_: library version = "
			<< _ibar_ptr_->get_library_version ()  << std::endl;
	    }
	  //std::cerr << "DEVEL: io_factory::_init_read_archive_: _ibar_ptr_ = " << _ibar_ptr_ << std::endl;
	}
      else
	{
	  throw std::runtime_error ("io_factory::_init_read_archive_: format not supported!");
	}
      _read_archive_is_initialized_ = true;
      if (devel)
	{
	  std::clog << "DEVEL: io_factory::_init_read_archive_: Exiting." << std::endl;
	}
      return 0;
    }

    int
    io_factory::_reset_read_archive_ ()
    {
      if (! _read_archive_is_initialized_)
	{
	  return 0;
	}

      if (_itar_ptr_ != 0)
	{
	  delete _itar_ptr_;
	  _itar_ptr_ = 0;
	}

      if (_ixar_ptr_ != 0)
	{
	  delete _ixar_ptr_;
	  _ixar_ptr_ = 0;
	}

      if (_ibar_ptr_ != 0)
	{
	  delete _ibar_ptr_;
	  _ibar_ptr_ = 0;
	}

      _read_archive_is_initialized_ = false;
      return 0;
    }


    int
    io_factory::_init_read_ (const std::string & a_stream_name)
    {
      _in_fs_ = new boost::iostreams::filtering_istream;
      if (is_gzip ())
	{
	  _in_fs_->push (boost::iostreams::gzip_decompressor ());
	}

      if (is_bzip2 ())
	{
	  _in_fs_->push (boost::iostreams::bzip2_decompressor ());
	}

      if (a_stream_name.empty ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_read_: cin..."
			<< std::endl;
	    }
	  _in_fs_->push (std::cin);
	}
      else
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_read_: file='"
			<< a_stream_name << "'" << std::endl;
	    }
	  if (is_compressed () || is_binary ())
	    {
	      _fin_ = new std::ifstream (a_stream_name.c_str (),
					 std::ios_base::in | std::ios_base::binary);
	    }
	  else
	    {
	      _fin_ = new std::ifstream (a_stream_name.c_str (),
					 std::ios_base::in);
	    }
	  if (! *_fin_)
	    {
	      throw std::runtime_error ("io_factory::_init_read_: Cannot open input stream!");
	    }
	  _in_fs_->push (*_fin_);
	}

      _in_ = _in_fs_;
      if (is_text () || is_xml ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_read_: "
			<< "FPU: stream.imbue..."
			<< std::endl;
	    }
	  _in_->imbue (*_locale_);
	}

      return 0;
    }

    int
    io_factory::_reset_read_ ()
    {
      if (g_debug)
	{
	  std::clog << "DEBUG: io_factory::_reset_read_:..." << std::endl;
	}

      if (_in_ != 0)
	{
	  _in_ = 0;
	}

      if (_in_fs_ != 0)
	{
	  _in_fs_->reset ();
	  delete _in_fs_;
	  _in_fs_ = 0;
	}

      if (_fin_ != 0)
	{
	  _fin_->close ();
	  delete _fin_;
	  _fin_ = 0;
	}

      return 0;
    }

    int
    io_factory::_init_write_archive_ ()
    {
      if (_write_archive_is_initialized_)
	{
	  return 0;
	}
      if (is_text ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_archive_: "
			<< "with FPU: text archive..."
			<< std::endl;
	    }
	  _otar_ptr_ = new boost::archive::text_oarchive (*_out_,
							  boost::archive::no_codecvt);
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_archive_: "
			<< "'boost::archive::text_oarchive' library version "
			<< _otar_ptr_->get_library_version () << std::endl;
	    }
	}
      else if (is_xml ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_archive_: "
			<< "with FPU: XML archive..."
			<< std::endl;
	    }
	  _oxar_ptr_ = new boost::archive::xml_oarchive (*_out_,
							 boost::archive::no_codecvt);
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_archive_: "
			<< "'boost::archive::xml_oarchive' library version "
			<< _oxar_ptr_->get_library_version () << std::endl;
	    }
	}
      else if (is_binary ())
	{
	  _obar_ptr_ = new boost::archive::portable_binary_oarchive (*_out_);
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_archive_: "
			<< "'boost::archive::portable_binary_oarchive' library version "
			<< _obar_ptr_->get_library_version () << std::endl;
	    }
	}
      else
	{
	  throw std::runtime_error ("io_factory::_init_write_archive_: format not supported!");
	}
      _write_archive_is_initialized_ = true;
      return 0;
    }

    int
    io_factory::_init_write_ (const std::string & a_stream_name)
    {
      _out_fs_ = new boost::iostreams::filtering_ostream;

      if (is_gzip ())
	{
	  _out_fs_->push(boost::iostreams::gzip_compressor ());
	}

      if (is_bzip2 ())
	{
	  _out_fs_->push (boost::iostreams::bzip2_compressor ());
	}

      if (a_stream_name.empty ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_: cout..."
			<< std::endl;
	    }
	  _out_fs_->push (std::cout);
	  return 0;
	}
      else
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_: file='"
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
		  message << "io_factory::_init_write_: "
			  << "append mode does not work for 'io_factory::single_archive' mode! "
			  << "Please consider to use the 'io_factory::multi_archives' mode instead!" ;
		  throw std::runtime_error(message.str ());
		}

	      open_mode |= std::ios_base::app;
	    }
	  _fout_ = new std::ofstream (a_stream_name.c_str (), open_mode);
	  if (!*_fout_)
	    {
	      throw std::runtime_error ("io_factory::_init_write_: Cannot open output stream!");
	    }
	  _out_fs_->push (*_fout_);
	}

      _out_ = _out_fs_;
      if (is_text () || is_xml ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_write_: stream.imbue"
			<< std::endl;
	    }
	  _out_->imbue (*_locale_);
	}

      if (g_debug)
	{
	  std::clog << "DEBUG: io_factory::_init_write_: mode='"
		    << std::hex << _mode_ << std::dec << "'" << std::endl;
	}
      return 0;
    }


    int
    io_factory::_reset_write_archive_ ()
    {
      if (! _write_archive_is_initialized_)
	{
	  return 0;
	}

      if (_otar_ptr_ != 0)
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_reset_write_archive_: "
			<< "delete TXT archive!" << std::endl;
	    }
	  delete _otar_ptr_;
	  _otar_ptr_ = 0;
	  //*_out_ << ' '; // add a white space at the end of text archive.
	  *_out_ << std::endl; // add a new line at the end of text archive.
	}

      if (_oxar_ptr_ != 0)
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_reset_write_archive_: "
			<< "delete XML archive!" << std::endl;
	    }
	  delete _oxar_ptr_;
	  _oxar_ptr_ = 0;
	}

      if (_obar_ptr_ != 0)
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_reset_write_archive_: "
			<< "delete binary archive!" << std::endl;
	    }
	  delete _obar_ptr_;
	  _obar_ptr_ = 0;
	}

      _write_archive_is_initialized_ = false;
      return 0;
    }

    int
    io_factory::_reset_write_ ()
    {
      if (g_debug)
	{
	  std::clog << "DEBUG: io_factory::_reset_write_:..." << std::endl;
	}

      if (_out_ != 0)
	{
	  _out_->flush ();
	  _out_=0;
	}

      if (_out_fs_ != 0)
	{
	  _out_fs_->flush ();
	  _out_fs_->reset ();
	  delete _out_fs_;
	  _out_fs_ = 0;
	}

      if (_fout_ != 0)
	{
	  _fout_->close ();
	  delete _fout_;
	  _fout_ = 0;
	}

      return 0;
    }

    int
    io_factory::_init_ (const std::string & a_stream_name, int a_mode)
    {
      _mode_ = a_mode;
      if (is_read ())
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_: read mode..."
			<< std::endl;
	    }
	  _init_read_ (a_stream_name);
	  if (is_single_archive ())
	    {
	      _init_read_archive_ ();
	    }
	}
      else
	{
	  if (g_debug)
	    {
	      std::clog << "DEBUG: io_factory::_init_: write mode..."
			<< std::endl;
	    }
	  _init_write_ (a_stream_name);
	  if (is_single_archive ())
	    {
	      _init_write_archive_ ();
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
	      _init_read_archive_ ();
	    }
	  if (is_write ())
	    {
	      _init_write_archive_ ();
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
	      _reset_read_archive_ ();
	    }
	  if (is_write ())
	    {
	      _reset_write_archive_ ();
	    }
	}
     return;
    }

    void
    io_factory::_ctor_defaults_ ()
    {
      _write_archive_is_initialized_ = false;
      _read_archive_is_initialized_  = false;
      _in_ = 0;
      _out_ = 0;
      _fin_ = 0;
      _fout_ = 0;
      _itar_ptr_ = 0;
      _otar_ptr_ = 0;
      _ixar_ptr_ = 0;
      _oxar_ptr_ = 0;
      _ibar_ptr_ = 0;
      _obar_ptr_ = 0;
      _in_fs_ = 0;
      _out_fs_ = 0;
      _mode_ = io_factory::MODE_DEFAULT;
      return;
    }

    int
    io_factory::_reset_ ()
    {

      if (is_read ())
	{
	  _reset_read_archive_ ();
	  _reset_read_ ();
	}

      if (is_write ())
	{
	  _reset_write_archive_ ();
	  _reset_write_ ();
	}
      _ctor_defaults_ ();
      if (g_debug) std::clog << "DEBUG: io_factory::_reset_: Use FPU" << std::endl;
      if (_locale_)
	{
	  delete _locale_;
	  _locale_ = 0;
	}
      if (_default_locale_)
	{
	  delete _default_locale_;
	  _default_locale_ = 0;
	}
      return 0;
    }

    // ctor
    io_factory::io_factory (int a_mode)
    {
      _default_locale_ = 0;
      _locale_ = 0;
      _default_locale_ = new std::locale (std::locale::classic (),
					  new boost::archive::codecvt_null<char>);
      bool write = ((a_mode & MASK_RW) != 0);
      if (write)
	{
	  _locale_ = new std::locale (*_default_locale_,
				      new boost::math::nonfinite_num_put<char>);
	}
      else
	{
	  _locale_ = new std::locale (*_default_locale_,
				      new boost::math::nonfinite_num_get<char>);
	}
      _ctor_defaults_ ();
      _init_ ("", a_mode);
      return;
    }

    // ctor
    io_factory::io_factory (const std::string & a_stream_name,
			    int a_mode)
    {
      _default_locale_ = 0;
      _locale_ = 0;
      _default_locale_ = new std::locale (std::locale::classic (),
					  new boost::archive::codecvt_null<char>);
      bool write = ((a_mode & MASK_RW) != 0);
      if (write)
	{
	  _locale_ = new std::locale (*_default_locale_,
				      new boost::math::nonfinite_num_put<char>);
	}
      else
	{
	  _locale_ = new std::locale (*_default_locale_,
				      new boost::math::nonfinite_num_get<char>);
	}
      _ctor_defaults_ ();
      _init_ (a_stream_name, a_mode);
      return;
    }

    // dtor
    io_factory::~io_factory ()
    {
      _reset_ ();
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
	   << "Mode  : " << std::hex << _mode_ << std::dec << std::endl;

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
		  format = xml = true;
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
      a_out << " |-- " << "Status   : " << (_status_== 0? "Ok": "Error") << endl;
      a_out << " |   " << " |-- " << "Initialized      : " << is_initialized () << endl;
      a_out << " |   " << " |-- " << "Mult. arch.      : " << is_multi_archives () << endl;
      a_out << " |   " << " |-- " << "Single arch.     : " << is_single_archive () << endl;
      a_out << " |   " << " |-- " << "Compressed       : " << is_compressed () << endl;
      a_out << " |   " << " |-- " << "Gzipped          : " << is_gzip () << endl;
      a_out << " |   " << " |-- " << "Bzipped          : " << is_bzip2 () << endl;
      a_out << " |   " << " |-- " << "Text arch.       : " << is_text () << endl;
      a_out << " |   " << " |-- " << "XML arch.        : " << is_xml () << endl;
      a_out << " |   " << " `-- " << "Bin arch. (port) : " << is_portable_binary () << endl;
      a_out << " |-- " << "Reader   : " << (_reader_ != 0? "Yes": "No") << endl;
      a_out << " `-- " << "Next tag : '" << (_next_tag_) << "'" << endl;
      return;
    }

    void
    data_reader::_read_next_tag_ ()
    {
      bool devel = false;
      //devel = true;
      if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: Entering..." << std::endl;
      if (_status_ != STATUS_OK)
	{
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
		    << "status != STATUS_Ok" << std::endl;
	  _next_tag_ = EMPTY_RECORD_TAG;
	  return;
	}
      if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
		<< "Continue..." << std::endl;
      try
	{
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
			       << "try block starts..." << std::endl;
	  if (_reader_->is_multi_archives ())
	    {
	      if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
				   << "multi-archives..." << std::endl;
	      _reader_->start_archive ();
	    }
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
			       << "Continue..." << std::endl;
	  std::string tag_id;
	  tag_id = "";
	  _next_tag_ = "";
	  this->_basic_load (tag_id);
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
			       << "_basic_load done with tag_id = '"
			       << tag_id << "'" << std::endl;
	  _next_tag_ = tag_id;
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
			       << "_next_tag_ = " << _next_tag_ << std::endl;
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
	      std::clog << "WARNING: data_reader::_read_next_tag_: runtime_error="
			<< x.what () << std::endl;
	      if (io_factory::g_warning)
		{
		  std::clog << "WARNING: data_reader::_read_next_tag_: runtime_error="
			    << x.what () << std::endl;
		}
	    }
	  //<<<
	  _status_   = STATUS_ERROR;
	  _next_tag_ = EMPTY_RECORD_TAG;
	  if (devel) std::cerr << "DEVEL: data_reader::_read_next_tag_: "
			       << "BOOOM !" << std::endl;
	}
      // 2011-02-25 FM:
     /* catch (boost::archive::archive_exception & x)
	{
	  if (io_factory::g_warning)
	    {
	      std::clog << "WARNING: data_reader::_read_next_tag_: archive exception is: "
			<< x.what () << std::endl;
	    }

	    std::clog << "WARNING: data_reader::_read_next_tag_: archive exception is: "
		    << x.what () << std::endl;

	  _status_   = STATUS_ERROR;
	  _next_tag_ = EMPTY_RECORD_TAG;
	  // throw x;
	}*/
      catch (std::exception & x)
	{
	  bool warn = io_factory::g_warning;
	  if (warn)
	    {
	      std::clog << "WARNING: data_reader::_read_next_tag_: exception="
			<< x.what () << std::endl;
	    }
	  _status_   = STATUS_ERROR;
	  _next_tag_ = EMPTY_RECORD_TAG;
	}
      catch (...)
	{
	  std::clog << "WARNING: data_reader::_read_next_tag_: "
		    << "unexpected exception!" << std::endl;
	  _status_   = STATUS_ERROR;
	  _next_tag_ = EMPTY_RECORD_TAG;
	}
      /*
      std::cerr << "DEVEL: data_reader::_read_next_tag_: Exiting."
		<< std::endl;
      */
      return;
    }

    bool data_reader::is_error () const
    {
      return _status_ == STATUS_ERROR;
    }

    void
    data_reader::_init_reader_ (const std::string & a_filename,
				int a_mode)
    {
      _status_ = STATUS_OK;
      _reader_ = new io_reader (a_filename, a_mode);
      _read_next_tag_ ();
      return;
    }

    void
    data_reader::_reset_reader_ ()
    {
      if (_reader_ != 0)
	{
	  delete _reader_;
	  _reader_ = 0;
	}
      _next_tag_ = "";
      _status_ = STATUS_OK;
      return;
    }

    const std::string
    data_reader::EMPTY_RECORD_TAG = "";

    const std::string &
    data_reader::get_record_tag () const
    {
      if (_status_ != STATUS_OK) return EMPTY_RECORD_TAG;
      return _next_tag_;
    }

    bool
    data_reader::is_initialized () const
    {
      return _reader_ != 0;
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
      return _reader_->is_compressed ();
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
      return _reader_->is_gzip ();
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
      return _reader_->is_text ();
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
      return _reader_->is_binary ();
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
      return _reader_->is_portable_binary ();
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
      return _reader_->is_xml ();
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
      return _reader_->is_bzip2 ();
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
      return _reader_->is_multi_archives ();
    }

    bool
    data_reader::has_record_tag () const
    {
      if (_status_ != STATUS_OK) return false;
      if (_next_tag_.empty()) return false;
      return true;
    }

    bool
    data_reader::record_tag_is (const std::string & a_tag) const
    {
      return _next_tag_ == a_tag;
    }

    void
    data_reader::reset ()
    {
      _reset_reader_ ();
      return;
    }

    void
    data_reader::init (const std::string & a_filename,
		       bool a_multiple_archives)
    {
      _reset_reader_ ();
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
      _init_reader_ (a_filename, mode);
      return;
    }

    void
    data_reader::init (const std::string & a_filename, int a_mode)
    {
      _reset_reader_ ();
      _init_reader_ (a_filename, a_mode);
      return;
    }

    data_reader::data_reader ()
    {
      _reader_ = 0;
      return;
    }

    data_reader::data_reader (const std::string & a_filename,
			      bool a_multiple_archives)
    {
      _reader_ = 0;
      init (a_filename, a_multiple_archives);
      return;
    }

    data_reader::data_reader (const std::string & a_filename, int a_mode)
    {
      _reader_ = 0;
      _init_reader_ (a_filename, a_mode);
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
      return _writer_ != 0;
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
      return _writer_->is_compressed ();
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
      return _writer_->is_gzip ();
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
      return _writer_->is_text ();
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
      return _writer_->is_binary ();
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
      return _writer_->is_portable_binary ();
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
      return _writer_->is_xml ();
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
      return _writer_->is_bzip2 ();
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
      return _writer_->is_multi_archives ();
    }

    void
    data_writer::_init_writer_ (const std::string & a_filename,
				int a_mode)
    {
      _writer_ = new io_writer (a_filename, a_mode);
      return;
    }

    void
    data_writer::_reset_writer_ ()
    {
      if (_writer_ != 0)
	{
	  delete _writer_;
	  _writer_ = 0;
	}
       return;
   }

    void
    data_writer::reset ()
    {
      _reset_writer_ ();
      return;
    }

    void
    data_writer::init (const std::string & a_filename,
		       bool a_multiple_archives,
		       bool a_append_mode)
    {
      _reset_writer_ ();
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
      _init_writer_ (a_filename, mode);
       return;
   }

    void
    data_writer::init (const std::string & a_filename,
		       int a_mode)
    {
      _reset_writer_ ();
      int mode = a_mode;
      _init_writer_ (a_filename, mode);
      return;
    }

    void data_writer::init_multi (const std::string & a_filename)
    {
      init (a_filename, using_multiple_archives);
      return;
    }
    
    void data_writer::init_single (const std::string & a_filename)
    {
      init (a_filename, using_single_archive);
      return;
    }
    
    // ctor
    data_writer::data_writer ()
    {
      _writer_ = 0;
      return;
    }

    // ctor
    data_writer::data_writer (const std::string & a_filename,
			      bool a_multiple_archives,
			      bool a_append_mode)
    {
      _writer_ = 0;
      init (a_filename, a_multiple_archives, a_append_mode);
      return;
    }

    // ctor
    data_writer::data_writer (const std::string & a_filename, int a_mode)
    {
      _writer_ = 0;
      _init_writer_ (a_filename, a_mode);
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
