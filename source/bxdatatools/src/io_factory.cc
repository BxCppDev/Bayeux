// io_factory.cc

// Ourselves:
#include <datatools/io_factory.h>

// Third Party:
// - Boost:
#include <boost/archive/codecvt_null.hpp>


#if defined (__clang__)
//#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if defined (__gnuc__)
//#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

namespace datatools {

  //----------------------------------------------------------------------
  // The io_factory class

  // static
  const int io_factory::SUCCESS;
  const int io_factory::ERROR;
  const unsigned int io_factory::MASK_RW;
  const unsigned int io_factory::MASK_FORMAT;
  const unsigned int io_factory::MASK_COMPRESSION;
  const unsigned int io_factory::MASK_MULTIARCHIVE;
  const unsigned int io_factory::MASK_APPEND;

  const std::string & io_factory::format::text_extension()
  {
    static std::string value;
    if (value.empty()) {
      value = "txt";
    }
    return value;
  }

  const std::string & io_factory::format::xml_extension()
  {
    static std::string value;
    if (value.empty()) {
      value = "xml";
    }
    return value;
  }

  const std::string & io_factory::format::binary_extension()
  {
    static std::string value;
    if (value.empty()) {
      value = "data";
    }
    return value;
  }

  const std::string & io_factory::format::gzip_extension()
  {
    static std::string value;
    if (value.empty()) {
      value = "gz";
    }
    return value;
  }

  const std::string & io_factory::format::bzip2_extension()
  {
    static std::string value;
    if (value.empty()) {
      value = "bz2";
    }
    return value;
  }

  // Logging features:
  void io_factory::set_logging_priority(::datatools::logger::priority p)
  {
    _logging_priority = p;
  }

  datatools::logger::priority io_factory::get_logging_priority() const
  {
    return _logging_priority;
  }

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
    /*** text archive ***/
    if (this->is_text()) {
      itar_ptr_ = new boost::archive::text_iarchive(
                                                    *in_, boost::archive::no_codecvt);
    }

    /*** XML archive ***/
    else if (this->is_xml()) {
      ixar_ptr_ = new boost::archive::xml_iarchive(
                                                   *in_, boost::archive::no_codecvt);
    }

    /*** binary archive ***/
    else if (this->is_binary()) {
      ibar_ptr_ = new eos::portable_iarchive(*in_);
    }

    else {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Format not supported!");
    }

    read_archive_is_initialized_ = true;
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
      in_fs_->push(std::cin);
    } else {
      if (this->is_compressed() || this->is_binary()) {
        fin_ = new std::ifstream(stream_name.c_str(),
                                 std::ios_base::in | std::ios_base::binary);
      } else {
        fin_ = new std::ifstream(stream_name.c_str(),
                                 std::ios_base::in);
      }
      DT_THROW_IF (! *fin_,
                   std::runtime_error,
                   "Cannot open input file stream '" << stream_name << "' !");
      in_fs_->push(*fin_);
    }

    in_ = in_fs_;
    if (this->is_text() || this->is_xml()) {
      in_->imbue(*locale_);
    }

    return 0;
  }


  int io_factory::reset_read() {

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
      otar_ptr_ = new boost::archive::text_oarchive(
                                                    *out_,boost::archive::no_codecvt);
    } else if (this->is_xml()) {
      oxar_ptr_ = new boost::archive::xml_oarchive(
                                                   *out_, boost::archive::no_codecvt);
    } else if (this->is_binary()) {
      obar_ptr_ = new eos::portable_oarchive(*out_);
    } else {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Format not supported !");
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
      out_fs_->push(std::cout);
      return 0;
    } else {
      std::ios_base::openmode open_mode = std::ios_base::out;
      if (this->is_compressed() || this->is_binary()) {
        open_mode |= std::ios_base::binary;
      }
      if (this->is_append()) {
        DT_THROW_IF (this->is_single_archive(),
                     std::logic_error,
                     "Append mode does not work for "
                     << "'single archive' mode ! "
                     << "Please to use the "
                     << "'multi archives' mode !");
        open_mode |= std::ios_base::app;
      }
      fout_ = new std::ofstream(stream_name.c_str(), open_mode);
      DT_THROW_IF (!*fout_,
                   std::runtime_error,
                   "Cannot open output file stream '" << stream_name << "' !");
      out_fs_->push(*fout_);
    }

    out_ = out_fs_;
    if (this->is_text() || this->is_xml()) {
      out_->imbue(*locale_);
    }

    return 0;
  }


  int io_factory::reset_write_archive() {
    if (!write_archive_is_initialized_) {
      return 0;
    }

    if (otar_ptr_ != 0) {
      delete otar_ptr_;
      otar_ptr_ = 0;
      *out_ << std::endl; // add a new line at the end of text archive.
    }

    if (oxar_ptr_ != 0) {
      delete oxar_ptr_;
      oxar_ptr_ = 0;
    }

    if (obar_ptr_ != 0) {
      delete obar_ptr_;
      obar_ptr_ = 0;
    }

    write_archive_is_initialized_ = false;
    return 0;
  }


  int io_factory::reset_write() {
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
      this->init_read(stream_name);
      if (this->is_single_archive()) this->init_read_archive();
    } else {
      this->init_write(stream_name);
      if (this->is_single_archive()) this->init_write_archive();
    }

    return 0;
  }


  void io_factory::start_archive() {
    if (this->is_multi_archives()) {
      if (this->is_read()) this->init_read_archive();
      if (this->is_write()) this->init_write_archive();
    }
  }


  void io_factory::stop_archive() {
    if (this->is_multi_archives()) {
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


  // Constructor :
  io_factory::io_factory(int mode) {
    set_logging_priority(datatools::logger::PRIO_ERROR);
    default_locale_ = 0;
    locale_ = 0;
    default_locale_ = new std::locale(
                                      std::locale::classic(),
                                      new boost::archive::codecvt_null<char>);

    bool local_write = ((mode & MASK_RW) != 0);

    if (local_write) {
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
    set_logging_priority(datatools::logger::PRIO_ERROR);
    default_locale_ = 0;
    locale_ = 0;
    default_locale_ = new std::locale(
                                      std::locale::classic(),
                                      new boost::archive::codecvt_null<char>);

    bool local_write = ((mode & MASK_RW) != 0);

    if (local_write) {
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
        ++i) {
      std::string token = *i;
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
         ++i) {
      std::string ext = *i;
      if (!comp) {
        if (ext == io_factory::format::gzip_extension()) {
          comp = gz = true;
        } else if (ext == io_factory::format::bzip2_extension()) {
          comp = bz2 = true;
        }
      }

      if (!format) {
        if (ext == io_factory::format::text_extension()) {
          format = txt = true;
        } else if (ext == io_factory::format::binary_extension()) {
          format = bin = true;
        } else if (ext == io_factory::format::xml_extension()) {
          format = xml = true;
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
    DT_THROW_IF (!this->is_read(),
                 std::logic_error,
                 "Cannot force non-read mode!");
  }


  io_reader::io_reader(const std::string& stream_name, int mode)
    : io_factory(stream_name, io_factory::MODE_READ | mode) {
    DT_THROW_IF (!this->is_read(),
                 std::logic_error,
                 "Cannot force non-read mode!");
  }


  io_reader::~io_reader() {
  }


  //----------------------------------------------------------------------
  // The io_writer_class

  io_writer::io_writer(int mode)
    : io_factory(io_factory::MODE_WRITE | mode) {
    DT_THROW_IF (!this->is_write(),
                 std::logic_error,
                 "Cannot force non-write mode!");
  }


  io_writer::io_writer(const std::string & stream_name, int mode)
    : io_factory(stream_name, io_factory::MODE_WRITE | mode) {
    DT_THROW_IF (!this->is_write(),
                 std::logic_error,
                 "Cannot force non-write mode!");
  }


  io_writer::~io_writer() {
  }


  //----------------------------------------------------------------------
  // The data_reader class
  //
  const std::string & data_reader::empty_record_tag()
  {
    static std::string tag;
    return tag;
  }

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
    DT_LOG_TRACE(reader_->get_logging_priority(),"Entering...");
    if (status_ != STATUS_OK) {
      DT_LOG_TRACE(reader_->get_logging_priority(),"status != STATUS_Ok");
      next_tag_ = empty_record_tag();
      return;
    }
    DT_LOG_TRACE(reader_->get_logging_priority(),"Continue...");
    try {
      DT_LOG_TRACE(reader_->get_logging_priority(),"Try block starts...");
      if (reader_->is_multi_archives()) {
        DT_LOG_TRACE(reader_->get_logging_priority(),"multi-archives...");
        reader_->start_archive();
      }
      DT_LOG_TRACE(reader_->get_logging_priority(),"Continue...");
      std::string tag_id;
      tag_id = "";
      next_tag_ = "";
      this->basic_load(tag_id);
      DT_LOG_TRACE(reader_->get_logging_priority(),"_basic_load done with tag_id = '" << tag_id << "'");
      next_tag_ = tag_id;
      DT_LOG_TRACE(reader_->get_logging_priority(),"next_tag_ '= " << next_tag_ << "'");
    }
    catch (std::runtime_error& x) {
      std::string msg = x.what();
      if (msg.find ("EOF") != msg.npos) {
      }
      status_   = STATUS_ERROR;
      next_tag_ = empty_record_tag();
      DT_LOG_TRACE(reader_->get_logging_priority(), x.what());
    }
    catch (std::exception& x) {
      status_   = STATUS_ERROR;
      next_tag_ = empty_record_tag();
    }
    catch (...) {
      DT_LOG_WARNING(reader_->get_logging_priority(),"Unexpected exception!");
      status_   = STATUS_ERROR;
      next_tag_ = empty_record_tag();
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
    if (status_ != STATUS_OK) return empty_record_tag();
    return next_tag_;
  }


  bool data_reader::is_initialized() const {
    return reader_ != 0;
  }


  bool data_reader::is_uncompressed() const {
    return !this->is_compressed();
  }


  bool data_reader::is_compressed() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_compressed();
  }


  bool data_reader::is_gzip() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_gzip();
  }


  bool data_reader::is_text() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_text();
  }


  bool data_reader::is_binary() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_binary();
  }


  bool data_reader::is_portable_binary() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_portable_binary();
  }


  bool data_reader::is_xml() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_xml();
  }


  bool data_reader::is_bzip2() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
    return reader_->is_bzip2();
  }


  bool data_reader::is_single_archive() const {
    return !this->is_multi_archives();
  }


  bool data_reader::is_multi_archives() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Reader is not initialized!");
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
    DT_THROW_IF (io_factory::guess_mode_from_filename(filename, mode_guess)
                 != io_factory::SUCCESS,
                 std::runtime_error,
                 "Cannot guess mode for file '" << filename << "'!");
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
                           bool this_append_mode) {
    writer_ = 0;
    this->init(filename, multiple_archives, this_append_mode);
  }

  // ctor
  data_writer::data_writer (const std::string& filename, int mode) {
    writer_ = 0;
    this->init_writer(filename, mode);
  }

  // dtor
  data_writer::~data_writer() {
    this->reset();
  }


  bool data_writer::is_initialized() const {
    return writer_ != 0;
  }


  bool data_writer::is_uncompressed() const {
    return !this->is_compressed();
  }


  bool data_writer::is_compressed() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_compressed();
  }


  bool data_writer::is_gzip() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_gzip();
  }


  bool data_writer::is_text() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_text();
  }


  bool data_writer::is_binary() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_binary();
  }


  bool data_writer::is_portable_binary() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_portable_binary();
  }


  bool data_writer::is_xml() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_xml();
  }


  bool data_writer::is_bzip2() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return writer_->is_bzip2();
  }


  bool data_writer::is_single_archive() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::logic_error,
                 "Writer is not initialized!");
    return !this->is_multi_archives();
  }


  bool data_writer::is_multi_archives() const {
    DT_THROW_IF (!this->is_initialized(),
                 std::runtime_error,
                 "Writer is not initialized!");
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
    DT_THROW_IF (io_factory::guess_mode_from_filename(filename, mode_guess)
                 != io_factory::SUCCESS,
                 std::logic_error,
                 "Cannot guess mode for file '" << filename << "' !");
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
