// -*- mode: c++; -*-
/* io_factory.cc
 */
// Ourselves
#include <datatools/io_factory.h>

// Standard Library

// Third Party
// - Boost
#include <boost/archive/codecvt_null.hpp>

// Datatools

namespace datatools {

//----------------------------------------------------------------------
// The io_factory class

// file extensions recognized by the library:
const std::string io_factory::TXT_EXT   = "txt";
const std::string io_factory::XML_EXT   = "xml";
const std::string io_factory::BIN_EXT   = "data";
const std::string io_factory::GZ_EXT    = "gz";
const std::string io_factory::BZIP2_EXT = "bz2";

bool io_factory::g_debug = false;
bool io_factory::g_warning = false;

bool io_factory::eof() const {
  if (this->is_write()) return false;
  if (in_ != 0 && in_->eof()) return true;
  return false;
}


bool io_factory::is_read() const {
  return (mode_ & MASK_RW) == 0;
}


bool io_factory::is_write() const {
  return (mode_ & MASK_RW) != 0;
}


bool io_factory::is_compressed() const {
  return (mode_ & MASK_COMPRESSION) != 0;
}


bool io_factory::is_uncompressed() const {
  return (mode_ & MASK_COMPRESSION) == 0;
}


bool io_factory::is_gzip() const {
  return (mode_ & MASK_COMPRESSION) == MODE_GZIP;
}


bool io_factory::is_bzip2() const {
  return (mode_ & MASK_COMPRESSION) == MODE_BZIP2;
}


bool io_factory::is_text() const {
  return (mode_ & MASK_FORMAT) == MODE_TEXT;
}


bool io_factory::is_binary() const {
  return (mode_ & MASK_FORMAT) == MODE_BINARY;
}


bool io_factory::is_portable_binary() const
{
  return this->is_binary() && true;
}


bool io_factory::is_xml() const {
  return (mode_ & MASK_FORMAT) == MODE_XML;
}


bool io_factory::is_single_archive() const {
  return (mode_ & MASK_MULTIARCHIVE) == MODE_UNIQUE_ARCHIVE;
}


bool io_factory::is_multi_archives() const {
  return !this->is_single_archive();
}


bool io_factory::is_no_append() const {
  return (mode_ & MASK_APPEND) == MODE_NO_APPEND;
}


bool io_factory::is_append() const {
  return !this->is_no_append();
}


// return bool instead????
int io_factory::init_read_archive() {
  bool devel = g_debug;
  if (devel) {
    std::clog << "DEBUG: io_factory::init_read_archive: Entering..."
              << std::endl;
  }

  /*** text archive ***/
  if (this->is_text()) {
    if (devel) {
      std::clog << "DEBUG: io_factory::init_read_archive: "
                << "text with FPU..."
                << std::endl;
    }

    itar_ptr_ = new boost::archive::text_iarchive(
        *in_, boost::archive::no_codecvt);

    if (devel) {
      std::clog << "DEBUG: io_factory::init_read_archive: "
                << "'boost::archive::text_iarchive' library version "
                << itar_ptr_->get_library_version() << std::endl;
    }
  }

  /*** XML archive ***/
  else if (this->is_xml()) {
    ixar_ptr_ = new boost::archive::xml_iarchive(
        *in_, boost::archive::no_codecvt);
    if (devel) {
      std::clog << "DEBUG: io_factory::init_read_archive: "
                << "'boost::archive::xml_iarchive' library version "
                << ixar_ptr_->get_library_version() << std::endl;
    }
  }

  /*** binary archive ***/
  else if (this->is_binary()) {
    ibar_ptr_ = new eos::portable_iarchive(*in_);

    if (devel) {
      std::cerr << "DEBUG: io_factory::init_read_archive: "
                << "'eos::portable_iarchive'library version = "
                << ibar_ptr_->get_library_version()
                << std::endl;
    }
  }

  else {
    throw std::runtime_error("io_factory::init_read_archive: format not supported!");
  }

  read_archive_is_initialized_ = true;
  if (devel) {
    std::clog << "DEVEL: io_factory::_init_read_archive_: Exiting."
              << std::endl;
  }
  return 0;
}


int io_factory::reset_read_archive() {
  if (!read_archive_is_initialized_) {
    return 0;
  }

  if (itar_ptr_ != 0) {
    delete itar_ptr_;
    itar_ptr_ = 0;
  }

  if (ixar_ptr_ != 0) {
    delete ixar_ptr_;
    ixar_ptr_ = 0;
  }

  if (ibar_ptr_ != 0) {
    delete ibar_ptr_;
    ibar_ptr_ = 0;
  }

  read_archive_is_initialized_ = false;
  return 0;
}


int io_factory::init_read(const std::string& stream_name) {
  in_fs_ = new boost::iostreams::filtering_istream;
  if (this->is_gzip()) {
    in_fs_->push(boost::iostreams::gzip_decompressor());
  }

  if (this->is_bzip2()) {
    in_fs_->push(boost::iostreams::bzip2_decompressor());
  }

  if (stream_name.empty()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_read: cin..." << std::endl;
    }
    in_fs_->push(std::cin);
  } else {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_read: file='"
                << stream_name << "'" << std::endl;
    }

    if (this->is_compressed() || this->is_binary()) {
      fin_ = new std::ifstream(stream_name.c_str(),
                                std::ios_base::in | std::ios_base::binary);
    } else {
      fin_ = new std::ifstream(stream_name.c_str(),
                               std::ios_base::in);
    }

    if (! *fin_) {
      throw std::runtime_error("io_factory::init_read: Cannot open input stream!");
    }

    in_fs_->push(*fin_);
  }

  in_ = in_fs_;
  if (this->is_text() || this->is_xml()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::_init_read_: "
                << "FPU: stream.imbue..."
                << std::endl;
    }
    in_->imbue(*locale_);
  }

  return 0;
}


int io_factory::reset_read() {
  if (g_debug) {
    std::clog << "DEBUG: io_factory::_reset_read_:..." << std::endl;
  }

  if (in_ != 0) in_ = 0;

  if (in_fs_ != 0) {
    in_fs_->reset();
    delete in_fs_;
    in_fs_ = 0;
  }

  if (fin_ != 0) {
    fin_->close();
    delete fin_;
    fin_ = 0;
  }

  return 0;
}


int io_factory::init_write_archive() {
  if (write_archive_is_initialized_) {
    return 0;
  }

  if (this->is_text()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write_archive: "
                << "with FPU: text archive..."
                << std::endl;
    }

    otar_ptr_ = new boost::archive::text_oarchive(
        *out_,boost::archive::no_codecvt);

    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write_archive: "
                << "'boost::archive::text_oarchive' library version "
                << otar_ptr_->get_library_version() << std::endl;
    }
  } else if (this->is_xml()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write_archive: "
                << "with FPU: XML archive..."
                << std::endl;
    }

    oxar_ptr_ = new boost::archive::xml_oarchive(
        *out_, boost::archive::no_codecvt);

    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write_archive: "
                << "'boost::archive::xml_oarchive' library version "
                << oxar_ptr_->get_library_version() << std::endl;
    }
  } else if (this->is_binary()) {
    obar_ptr_ = new eos::portable_oarchive(*out_);

    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write_archive: "
                << "'eos::portable_oarchive' library version "
                << obar_ptr_->get_library_version() << std::endl;
    }
  } else {
    throw std::runtime_error("io_factory::init_write_archive: format not supported!");
  }

  write_archive_is_initialized_ = true;
  return 0;
}


int io_factory::init_write(const std::string& stream_name) {
  out_fs_ = new boost::iostreams::filtering_ostream;

  if (this->is_gzip()) {
    out_fs_->push(boost::iostreams::gzip_compressor());
  }

  if (this->is_bzip2()) {
    out_fs_->push(boost::iostreams::bzip2_compressor());
  }

  if (stream_name.empty()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write: cout..." << std::endl;
    }

    out_fs_->push(std::cout);
    return 0;
  } else {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init_write: file='"
                << stream_name << "'"<< std::endl;
    }

    std::ios_base::openmode open_mode = std::ios_base::out;
    if (this->is_compressed() || this->is_binary())
    {
      open_mode |= std::ios_base::binary;
    }

    if (this->is_append()) {
      if (this->is_single_archive()) {
        std::ostringstream message;
        message << "io_factory::init_write: "
                << "append mode does not work for "
                << "'io_factory::single_archive' mode! "
                << "Please consider to use the "
                << "'io_factory::multi_archives' mode instead!";
        throw std::runtime_error(message.str());
      }

      open_mode |= std::ios_base::app;
    }

    fout_ = new std::ofstream(stream_name.c_str(), open_mode);
    if (!*fout_) {
      throw std::runtime_error("io_factory::init_write: Cannot open output stream!");
    }

    out_fs_->push(*fout_);
  }

  out_ = out_fs_;
  if (this->is_text() || this->is_xml()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::_init_write_: stream.imbue"
                << std::endl;
    }
    out_->imbue(*locale_);
  }

  if (g_debug) {
    std::clog << "DEBUG: io_factory::_init_write_: mode='"
              << std::hex << mode_ << std::dec << "'" << std::endl;
  }

  return 0;
}


int io_factory::reset_write_archive() {
  if (!write_archive_is_initialized_) {
    return 0;
  }

  if (otar_ptr_ != 0) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::reset_write_archive: "
                << "delete TXT archive!" << std::endl;
    }

    delete otar_ptr_;
    otar_ptr_ = 0;
    *out_ << std::endl; // add a new line at the end of text archive.
  }

  if (oxar_ptr_ != 0) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::reset_write_archive: "
                << "delete XML archive!" << std::endl;
    }

    delete oxar_ptr_;
    oxar_ptr_ = 0;
  }

  if (obar_ptr_ != 0) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::reset_write_archive: "
                << "delete binary archive!" << std::endl;
    }

    delete obar_ptr_;
    obar_ptr_ = 0;
  }

  write_archive_is_initialized_ = false;
  return 0;
}


int io_factory::reset_write() {
  if (g_debug) {
    std::clog << "DEBUG: io_factory::reset_write:..." << std::endl;
  }

  if (out_ != 0) {
    out_->flush();
    out_=0;
  }

  if (out_fs_ != 0) {
    out_fs_->flush();
    out_fs_->reset();
    delete out_fs_;
    out_fs_ = 0;
  }

  if (fout_ != 0) {
    fout_->close();
    delete fout_;
    fout_ = 0;
  }

  return 0;
}


int io_factory::init(const std::string& stream_name, int mode) {
  mode_ = mode;

  if (this->is_read()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init: read mode..."
                << std::endl;
    }

    this->init_read(stream_name);
    if (this->is_single_archive()) this->init_read_archive();
  } else {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::init: write mode..." << std::endl;
    }

    this->init_write(stream_name);

    if (this->is_single_archive()) this->init_write_archive();
  }

  return 0;
}


void io_factory::start_archive() {
  if (this->is_multi_archives()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::start_archive: multi..." << std::endl;
    }

    if (this->is_read()) this->init_read_archive();
    if (this->is_write()) this->init_write_archive();
  }
}


void io_factory::stop_archive() {
  if (this->is_multi_archives()) {
    if (g_debug) {
      std::clog << "DEBUG: io_factory::stop_archive: multi..." << std::endl;
    }

    if (this->is_read()) this->reset_read_archive();
    if (this->is_write()) this->reset_write_archive();
  }
}


void io_factory::ctor_defaults() {
  write_archive_is_initialized_ = false;
  read_archive_is_initialized_  = false;
  in_ = 0;
  out_ = 0;
  fin_ = 0;
  fout_ = 0;
  itar_ptr_ = 0;
  otar_ptr_ = 0;
  ixar_ptr_ = 0;
  oxar_ptr_ = 0;
  ibar_ptr_ = 0;
  obar_ptr_ = 0;
  in_fs_ = 0;
  out_fs_ = 0;
  mode_ = io_factory::MODE_DEFAULT;
}


int io_factory::reset() {
  if (this->is_read()) {
    this->reset_read_archive();
    this->reset_read();
  }

  if (this->is_write()) {
    this->reset_write_archive();
    this->reset_write();
  }

  this->ctor_defaults();

  if (g_debug) std::clog << "DEBUG: io_factory::_reset_: Use FPU" << std::endl;

  if (locale_) {
    delete locale_;
    locale_ = 0;
  }

  if (default_locale_) {
    delete default_locale_;
    default_locale_ = 0;
  }
  return 0;
}


// ctor
io_factory::io_factory(int mode) {
  default_locale_ = 0;
  locale_ = 0;
  default_locale_ = new std::locale(
      std::locale::classic(),
      new boost::archive::codecvt_null<char>);

  bool write = ((mode & MASK_RW) != 0);

  if (write) {
    locale_ = new std::locale(*default_locale_,
                               new boost::math::nonfinite_num_put<char>);
  } else {
    locale_ = new std::locale(*default_locale_,
                               new boost::math::nonfinite_num_get<char>);
  }

  this->ctor_defaults();
  this->init("", mode);
}


// ctor
io_factory::io_factory(const std::string& stream_name, int mode) {
  default_locale_ = 0;
  locale_ = 0;
  default_locale_ = new std::locale(
      std::locale::classic(),
      new boost::archive::codecvt_null<char>);

  bool write = ((mode & MASK_RW) != 0);

  if (write) {
    locale_ = new std::locale(*default_locale_,
                               new boost::math::nonfinite_num_put<char>);
  } else {
    locale_ = new std::locale(*default_locale_,
                               new boost::math::nonfinite_num_get<char>);
  }

  this->ctor_defaults();
  this->init(stream_name, mode);
}


// dtor
io_factory::~io_factory() {
  this->reset();
  if (io_factory::g_debug) {
    std::clog << "DEBUG: io_factory::~io_factory." << std::endl;
  }
}


void io_factory::tree_dump(std::ostream& out,
                           const std::string& a_title,
                           const std::string& a_indent,
                           bool inherit) const {
  std::ostringstream tag_ss, last_tag_ss; // tree_trick
  tag_ss << i_tree_dumpable::tag; // tree_trick
  last_tag_ss << i_tree_dumpable::inherit_tag(inherit); // tree_trick
  std::string tag = tag_ss.str(); // tree_trick
  std::string last_tag = last_tag_ss.str(); // tree_trick
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) out << indent << a_title << std::endl;

  out << indent << tag
      << "Mode  : " << std::hex << mode_ << std::dec << std::endl;

  out << indent << tag
      << "is_read  : " << this->is_read() << std::endl;

  out << indent << tag
      << "is_write : " << this->is_write() << std::endl;

  out << indent << tag
      << "is_compressed : " << this->is_compressed() << std::endl;

  out << indent << tag
      << "is_uncompressed : " << this->is_uncompressed() << std::endl;

  out << indent << tag
      << "is_gzip : " << this->is_gzip() << std::endl;

  out << indent << tag
      << "is_bzip2 : " << this->is_bzip2() << std::endl;

  out << indent << tag
      << "is_text : " << this->is_text() << std::endl;

  out << indent << tag
      << "is_binary : " << this->is_binary() << std::endl;

  out << indent << tag
      << "is_xml : " << this->is_xml() << std::endl;

  out << indent << tag
      << "is_single_archive : " << this->is_single_archive() << std::endl;

  out << indent << last_tag
      << "is_multi_archives : " << this->is_multi_archives() << std::endl;
}


void io_factory::dump(std::ostream& out) const {
  this->io_factory::tree_dump(out, "io_factory::dump:");
}


int io_factory::guess_mode_from_filename(const std::string& a_filename,
                                         int& a_mode) {
  int status = io_factory::SUCCESS;
  int mode = 0x0;
  std::string fn = a_filename;
  boost::char_separator<char> sep (".");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  tokenizer tokens(fn, sep);
  std::list<std::string> ltok;

  for(tokenizer::iterator i = tokens.begin();
      i != tokens.end();
      ++i)
  {
    std::string token = *i;
    if (io_factory::g_debug) {
      std::clog << "DEBUG: io_factory::guess_mode_from_filename: token=<"
                << token << ">" << std::endl;
    }
    ltok.push_front(token);
  }

  bool gz = false;
  bool bz2 = false;
  bool txt = false;
  bool bin = false;
  bool xml = false;
  bool comp = false;
  bool format = false;
  size_t ext_count = 0;

  for (std::list<std::string>::const_iterator i = ltok.begin();
       i != ltok.end();
       ++i)
  {
    if (io_factory::g_debug) {
      std::clog << "DEBUG: io_factory::guess_mode_from_filename: ltok=<"
                << *i << ">" << std::endl;
    }

    std::string ext = *i;
    if (!comp) {
      if (ext == GZ_EXT) {
        comp = gz = true;
        if (io_factory::g_debug) {
          std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
                    << "mode+=GZIP" << std::endl;
        }
      } else if (ext == BZIP2_EXT) {
        comp = bz2 = true;
        if (io_factory::g_debug) {
          std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
                    << "mode+=BZIP2" << std::endl;
        }
      }
    }

    if (!format) {
      if (ext == TXT_EXT) {
        format = txt = true;
        if (io_factory::g_debug) {
          std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
                    << "mode+=TEXT" << std::endl;
        }
      } else if (ext == BIN_EXT) {
        format = bin = true;
        if (io_factory::g_debug) {
          std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
                    << "mode+=BINARY" << std::endl;
        }
      } else if (ext == XML_EXT) {
        format = xml = true;
        if (io_factory::g_debug) {
          std::clog << "DEBUG: io_factory::guess_mode_from_filename: "
                    << "mode+=XML" << std::endl;
        }
      }
    }

    if (!format && !comp) break;
    if (format) break;
    ext_count++;
    if (ext_count == 2) break;
  }

  if (!format) {
    // cannot guess format from extension:
    status = io_factory::ERROR;
  } else {
    mode &= ~ io_factory::MASK_COMPRESSION;
    if (comp) {
      if (gz)  mode |= io_factory::MODE_GZIP;
      if (bz2) mode |= io_factory::MODE_BZIP2;
    }

    mode &= ~ io_factory::MASK_FORMAT;
    if (format) {
      if (bin) mode |= io_factory::MODE_BINARY;
      if (txt) mode |= io_factory::MODE_TEXT;
      if (xml) mode |= io_factory::MODE_XML;
    }
  }

  a_mode = mode;
  return status;
}


//----------------------------------------------------------------------
// The io_reader class

io_reader::io_reader(int mode)
    : io_factory(io_factory::MODE_READ | mode) {
  if (!this->is_read()) {
    throw std::runtime_error("io_reader::io_reader: cannot force not-read mode!");
  }
}


io_reader::io_reader(const std::string& stream_name, int mode)
    : io_factory(stream_name, io_factory::MODE_READ | mode) {
  if (!this->is_read()) {
    throw std::runtime_error("io_reader::io_reader: cannot force not-read mode!");
  }
}


io_reader::~io_reader() {
  if (g_debug) {
    std::clog << "DEBUG: io_reader::~io_reader." << std::endl;
  }
}


//----------------------------------------------------------------------
// The io_writer_class

io_writer::io_writer(int mode)
    : io_factory(io_factory::MODE_WRITE | mode) {
  if (!this->is_write()) {
    throw std::runtime_error("io_writer::io_writer: cannot force write mode!");
  }
}


io_writer::io_writer(const std::string & stream_name, int mode)
    : io_factory(stream_name, io_factory::MODE_WRITE | mode) {
  if (!this->is_write()) {
    throw std::runtime_error("io_writer::io_writer: cannot force read mode!");
  }
}


io_writer::~io_writer() {
  if (g_debug) {
    std::clog << "DEBUG: io_writer::~io_writer." << std::endl;
  }
}


//----------------------------------------------------------------------
// The data_reader class
//
const std::string data_reader::EMPTY_RECORD_TAG = "";

// ctor
data_reader::data_reader() {
  reader_ = 0;
}

// ctor
data_reader::data_reader(const std::string& filename,
                         bool use_multiple_archives) {
  reader_ = 0;
  this->init(filename, use_multiple_archives);
}

// ctor
data_reader::data_reader(const std::string& filename, int mode) {
  reader_ = 0;
  this->init_reader(filename, mode);
}

// dtor
data_reader::~data_reader() {
  this->reset();
  if (io_factory::g_debug) {
    std::clog << "DEBUG: data_reader::~data_reader: Done." << std::endl;
  }
}


void data_reader::dump(std::ostream& out) const {
  out << "data_reader::dump: " << std::endl;
  out << " |-- "
      << "Status   : " << (status_== 0 ? "Ok" : "Error") << std::endl;
  out << " |   "
      << " |-- "
      << "Initialized      : " << this->is_initialized() << std::endl;
  out << " |   "
      << " |-- "
      << "Multi archive    : " << this->is_multi_archives() << std::endl;
  out << " |   "
      << " |-- "
      << "Single archive   : " << this->is_single_archive() << std::endl;
  out << " |   "
      << " |-- "
      << "Compressed       : " << this->is_compressed() << std::endl;
  out << " |   "
      << " |-- "
      << "Gzipped          : " << this->is_gzip() << std::endl;
  out << " |   "
      << " |-- "
      << "Bzipped          : " << this->is_bzip2() << std::endl;
  out << " |   "
      << " |-- "
      << "Text archive     : " << this->is_text() << std::endl;
  out << " |   "
      << " |-- "
      << "XML archive      : " << this->is_xml() << std::endl;
  out << " |   "
      << " `-- "
      << "Bin archive(port): " << this->is_portable_binary() << std::endl;
  out << " |-- "
      << "Reader   : "
      << (reader_ != 0 ? "Yes" : "No") << std::endl;
  out << " `-- "
      << "Next tag : '" << (next_tag_) << "'" << std::endl;
}


void data_reader::read_next_tag() {
  bool devel = false;

  if (devel) {
    std::cerr << "DEVEL: data_reader::read_next_tag: Entering..."
              << std::endl;
  }

  if (status_ != STATUS_OK) {
    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "status != STATUS_Ok" << std::endl;
    }
    next_tag_ = EMPTY_RECORD_TAG;
    return;
  }

  if (devel) {
    std::cerr << "DEVEL: data_reader::read_next_tag: "
              << "Continue..." << std::endl;
  }

  try {
    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "try block starts..." << std::endl;
    }

    if (reader_->is_multi_archives()) {
      if (devel) {
        std::cerr << "DEVEL: data_reader::read_next_tag: "
                  << "multi-archives..." << std::endl;
      }
      reader_->start_archive();
    }

    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "Continue..." << std::endl;
    }
    std::string tag_id;
    tag_id = "";
    next_tag_ = "";
    this->basic_load(tag_id);
    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "_basic_load done with tag_id = '"
                << tag_id << "'" << std::endl;
    }
    next_tag_ = tag_id;
    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "next_tag_ = " << next_tag_ << std::endl;
    }
  }
  catch (std::runtime_error& x) {
    bool warn = io_factory::g_warning;
    //>>> 2008-11-13 FM: skip EOF message printing
    std::string msg = x.what();
    if (msg.find ("EOF") != msg.npos) {
      warn = false;
    }
    if (warn) {
      std::clog << "WARNING: data_reader::read_next_tag: runtime_error="
                << x.what() << std::endl;
      if (io_factory::g_warning) {
        std::clog << "WARNING: data_reader::read_next_tag: runtime_error="
                  << x.what() << std::endl;
      }
    }
    //<<<
    status_   = STATUS_ERROR;
    next_tag_ = EMPTY_RECORD_TAG;
    if (devel) {
      std::cerr << "DEVEL: data_reader::read_next_tag: "
                << "BOOOM !" << std::endl;
    }
  }
  catch (std::exception& x) {
    bool warn = io_factory::g_warning;
    if (warn) {
      std::clog << "WARNING: data_reader::read_next_tag: exception="
                << x.what() << std::endl;
    }
    status_   = STATUS_ERROR;
    next_tag_ = EMPTY_RECORD_TAG;
  }
  catch (...) {
    std::clog << "WARNING: data_reader::read_next_tag: "
              << "unexpected exception!" << std::endl;
    status_   = STATUS_ERROR;
    next_tag_ = EMPTY_RECORD_TAG;
  }
}


bool data_reader::is_error() const {
  return status_ == STATUS_ERROR;
}


void data_reader::init_reader(const std::string& filename, int mode) {
  status_ = STATUS_OK;
  reader_ = new io_reader(filename, mode);
  this->read_next_tag();
}


void data_reader::reset_reader() {
  if (reader_ != 0) {
    delete reader_;
    reader_ = 0;
  }
  next_tag_ = "";
  status_ = STATUS_OK;
}


const std::string& data_reader::get_record_tag() const {
  if (status_ != STATUS_OK) return EMPTY_RECORD_TAG;
  return next_tag_;
}


bool data_reader::is_initialized() const {
  return reader_ != 0;
}


bool data_reader::is_uncompressed() const {
  return !this->is_compressed();
}


bool data_reader::is_compressed() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_compressed: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_compressed();
}


bool data_reader::is_gzip() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_gzip: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_gzip();
}


bool data_reader::is_text() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_text: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_text();
}


bool data_reader::is_binary() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_binary: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_binary();
}


bool data_reader::is_portable_binary() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_portable_binary: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_portable_binary();
}


bool data_reader::is_xml() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_xml: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_xml();
}


bool data_reader::is_bzip2() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_bzip2: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_bzip2();
}


bool data_reader::is_single_archive() const {
  return !this->is_multi_archives();
}


bool data_reader::is_multi_archives() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_reader::is_multi_archives: reader is not initialized!";
    throw std::runtime_error(message.str());
  }
  return reader_->is_multi_archives();
}


bool data_reader::has_record_tag() const {
  if (status_ != STATUS_OK) return false;
  if (next_tag_.empty()) return false;
  return true;
}


bool data_reader::record_tag_is(const std::string& tag) const {
  return next_tag_ == tag;
}


void data_reader::reset() {
  this->reset_reader();
}


void data_reader::init(const std::string& filename,
                       bool use_multiple_archives) {
  this->reset_reader();
  int mode_guess;
  if (io_factory::guess_mode_from_filename(filename, mode_guess)
      != io_factory::SUCCESS) {
    std::ostringstream message;
    message << "data_reader::init: cannot guess mode for file '"
            << filename << "'!";
    throw std::runtime_error(message.str());
  }

  int mode = mode_guess;
  if (use_multiple_archives) mode |= io_factory::multi_archives;

  this->init_reader(filename, mode);
}


void data_reader::init(const std::string& filename, int mode) {
  this->reset_reader();
  this->init_reader(filename, mode);
}


//----------------------------------------------------------------------
// The data_writer class
// ctor
data_writer::data_writer() {
  writer_ = 0;
}

// ctor
data_writer::data_writer(const std::string& filename,
                         bool multiple_archives,
                         bool append_mode) {
  writer_ = 0;
  this->init(filename, multiple_archives, append_mode);
}

// ctor
data_writer::data_writer (const std::string& filename, int mode) {
  writer_ = 0;
  this->init_writer(filename, mode);
}

// dtor
data_writer::~data_writer() {
  this->reset();
  if (io_factory::g_debug) {
    std::clog << "DEBUG: data_writer::~data_writer." << std::endl;
  }
}


bool data_writer::is_initialized() const {
  return writer_ != 0;
}


bool data_writer::is_uncompressed() const {
  return !this->is_compressed();
}


bool data_writer::is_compressed() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_compressed: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_compressed();
}


bool data_writer::is_gzip() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_gzip: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_gzip();
}


bool data_writer::is_text() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_text: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_text();
}


bool data_writer::is_binary() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_binary: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_binary();
}


bool data_writer::is_portable_binary() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_portable_binary: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_portable_binary();
}


bool data_writer::is_xml() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_xml: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_xml();
}


bool data_writer::is_bzip2() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_bzip2: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_bzip2();
}


bool data_writer::is_single_archive() const {
  return !this->is_multi_archives();
}


bool data_writer::is_multi_archives() const {
  if (!this->is_initialized()) {
    std::ostringstream message;
    message << "data_writer::is_multi_archives: writer is not initialized!";
    throw std::runtime_error(message.str());
  }
  return writer_->is_multi_archives();
}


void data_writer::init_writer(const std::string& filename, int mode) {
  writer_ = new io_writer(filename, mode);
}


void data_writer::reset_writer() {
  if (writer_ != 0) {
    delete writer_;
    writer_ = 0;
  }
}


void data_writer::reset() {
  this->reset_writer();
}


void data_writer::init(const std::string& filename,
                       bool use_multiple_archives,
                       bool use_append_mode) {
  this->reset_writer();
  int mode_guess;
  if (io_factory::guess_mode_from_filename(filename, mode_guess)
      != io_factory::SUCCESS) {
    std::ostringstream message;
    message << "data_writer::init: cannot guess mode for file '"
            << filename << "'!";
    throw std::runtime_error(message.str());
  }

  int mode = mode_guess;
  if (use_multiple_archives) mode |= io_factory::multi_archives;

  if (use_append_mode) mode |= io_factory::append;

  this->init_writer(filename, mode);
}


void data_writer::init(const std::string& filename, int mode) {
  this->reset_writer();
  int tmp_mode = mode; //????
  this->init_writer(filename, tmp_mode);
}


void data_writer::init_multi(const std::string& filename) {
  this->init(filename, using_multiple_archives);
}


void data_writer::init_single(const std::string& filename) {
  this->init(filename, using_single_archive);
}


} // end of namespace datatools


