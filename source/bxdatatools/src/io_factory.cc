// io_factory.cc

// Ourselves:
#include <datatools/io_factory.h>

// Third Party:
// - Boost:
#include <boost/archive/codecvt_null.hpp>
// 2012-01-09 FM : now use the Boost >=1.47 header :
//#include <boost/math/nonfinite_num_facets.hpp>
#include <boost/math/special_functions/nonfinite_num_facets.hpp>

// Wrap Boost's tokenizer header
// This header, causes "unused parameter" warnings from its
// static void assign(Iterator b, Iterator e, Token &t)
// function.
// This should be an innocuous warning, so remove diagnostic for this
// header only.
// We only use clang pragmas for now because GCC's are highly version
// dependent - so need a bit more thought.
// To be removed when Boost fix their headers...
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/tokenizer.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Wrap Boost's iostreams/filter/gzip header
// This header, causes "unused parameter" warnings from its
// static void write_long(long n, Sink& next, boost::mpl::false_)
// function.
// This should be an innocuous warning, so remove diagnostic for this
// header only.
// We only use clang pragmas for now because GCC's are highly version
// dependent - so need a bit more thought.
// To be removed when Boost fix their headers...
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter" // Bug in GCC
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/iostreams/filter/gzip.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <boost/iostreams/filter/bzip2.hpp>

#if defined (__clang__)
//#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if defined (__GNUC__)
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
  void io_factory::set_logging_priority(::datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  datatools::logger::priority io_factory::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool io_factory::eof() const
  {
    if (this->is_write()) return false;
    if (_in_ != nullptr && _in_->eof()) return true;
    return false;
  }


  bool io_factory::is_read() const
  {
    return (_mode_ & MASK_RW) == 0;
  }


  bool io_factory::is_write() const
  {
    return (_mode_ & MASK_RW) != 0;
  }


  bool io_factory::is_compressed() const
  {
    return (_mode_ & MASK_COMPRESSION) != 0;
  }


  bool io_factory::is_uncompressed() const
  {
    return (_mode_ & MASK_COMPRESSION) == 0;
  }


  bool io_factory::is_gzip() const
  {
    return (_mode_ & MASK_COMPRESSION) == MODE_GZIP;
  }


  bool io_factory::is_bzip2() const {
    return (_mode_ & MASK_COMPRESSION) == MODE_BZIP2;
  }


  bool io_factory::is_text() const {
    return (_mode_ & MASK_FORMAT) == MODE_TEXT;
  }


  bool io_factory::is_binary() const {
    return (_mode_ & MASK_FORMAT) == MODE_BINARY;
  }


  bool io_factory::is_portable_binary() const
  {
    return this->is_binary() && true;
  }


  bool io_factory::is_xml() const {
    return (_mode_ & MASK_FORMAT) == MODE_XML;
  }


  bool io_factory::is_single_archive() const {
    return (_mode_ & MASK_MULTIARCHIVE) == MODE_UNIQUE_ARCHIVE;
  }


  bool io_factory::is_multi_archives() const {
    return !this->is_single_archive();
  }


  bool io_factory::is_no_append() const {
    return (_mode_ & MASK_APPEND) == MODE_NO_APPEND;
  }


  bool io_factory::is_append() const {
    return !this->is_no_append();
  }


  // return nothing instead????
  int io_factory::init_read_archive()
  {
    if (this->is_text()) {
      _itar_ptr_ = new boost::archive::text_iarchive(*_in_, boost::archive::no_codecvt);
    } else if (this->is_xml()) {
      _ixar_ptr_ = new boost::archive::xml_iarchive(*_in_, boost::archive::no_codecvt);
    } else if (this->is_binary()) {
      _ibar_ptr_ = new eos::portable_iarchive(*_in_);
    } else {
      DT_THROW(std::logic_error, "Format not supported!");
    }
    _read_archive_is_initialized_ = true;
    return 0;
  }


  int io_factory::reset_read_archive()
  {
    if (!_read_archive_is_initialized_) {
      return 0;
    }

    if (_itar_ptr_ != nullptr) {
      delete _itar_ptr_;
      _itar_ptr_ = nullptr;
    }

    if (_ixar_ptr_ != nullptr) {
      delete _ixar_ptr_;
      _ixar_ptr_ = nullptr;
    }

    if (_ibar_ptr_ != nullptr) {
      delete _ibar_ptr_;
      _ibar_ptr_ = nullptr;
    }

    _read_archive_is_initialized_ = false;
    return 0;
  }

  int io_factory::init_read(const std::string & stream_name_)
  {
    _in_fs_ = new boost::iostreams::filtering_istream;
    if (this->is_gzip()) {
      _in_fs_->push(boost::iostreams::gzip_decompressor());
    }

    if (this->is_bzip2()) {
      _in_fs_->push(boost::iostreams::bzip2_decompressor());
    }

    if (stream_name_.empty()) {
      _in_fs_->push(std::cin);
    } else {
      if (this->is_compressed() || this->is_binary()) {
        _fin_ = new std::ifstream(stream_name_.c_str(),
                                  std::ios_base::in | std::ios_base::binary);
      } else {
        _fin_ = new std::ifstream(stream_name_.c_str(),
                                  std::ios_base::in);
      }
      DT_THROW_IF(! *_fin_,
                  std::runtime_error,
                  "Cannot open input file stream '" << stream_name_ << "' !");
      _in_fs_->push(*_fin_);
    }

    _in_ = _in_fs_;
    if (this->is_text() || this->is_xml()) {
      _in_->imbue(*_locale_);
    }

    return 0;
  }

  int io_factory::reset_read()
  {
    if (_in_ != nullptr) _in_ = nullptr;

    if (_in_fs_ != nullptr) {
      _in_fs_->reset();
      delete _in_fs_;
      _in_fs_ = nullptr;
    }

    if (_fin_ != nullptr) {
      _fin_->close();
      delete _fin_;
      _fin_ = nullptr;
    }

    return 0;
  }

  int io_factory::init_write_archive()
  {
    if (_write_archive_is_initialized_) {
      return 0;
    }
    if (this->is_text()) {
      _otar_ptr_ = new boost::archive::text_oarchive(
                                                     *_out_, boost::archive::no_codecvt);
    } else if (this->is_xml()) {
      _oxar_ptr_ = new boost::archive::xml_oarchive(
                                                    *_out_, boost::archive::no_codecvt);
    } else if (this->is_binary()) {
      _obar_ptr_ = new eos::portable_oarchive(*_out_);
    } else {
      DT_THROW_IF(true,
                  std::logic_error,
                  "Format not supported !");
    }

    _write_archive_is_initialized_ = true;
    return 0;
  }


  int io_factory::init_write(const std::string & stream_name_)
  {
    _out_fs_ = new boost::iostreams::filtering_ostream;
    if (this->is_gzip()) {
      _out_fs_->push(boost::iostreams::gzip_compressor());
    }
    if (this->is_bzip2()) {
      _out_fs_->push(boost::iostreams::bzip2_compressor());
    }
    if (stream_name_.empty()) {
      _out_fs_->push(std::cout);
      return 0;
    } else {
      std::ios_base::openmode open_mode = std::ios_base::out;
      if (this->is_compressed() || this->is_binary()) {
        open_mode |= std::ios_base::binary;
      }
      if (this->is_append()) {
        DT_THROW_IF(this->is_single_archive(),
                    std::logic_error,
                    "Append mode does not work for "
                    << "'single archive' mode ! "
                    << "Please to use the "
                    << "'multi archives' mode !");
        open_mode |= std::ios_base::app;
      }
      _fout_ = new std::ofstream(stream_name_.c_str(), open_mode);
      DT_THROW_IF(!*_fout_,
                  std::runtime_error,
                  "Cannot open output file stream '" << stream_name_ << "' !");
      _out_fs_->push(*_fout_);
    }
    _out_ = _out_fs_;
    if (this->is_text() || this->is_xml()) {
      _out_->imbue(*_locale_);
    }
    return 0;
  }

  int io_factory::reset_write_archive()
  {
    if (!_write_archive_is_initialized_) {
      return 0;
    }
    if (_otar_ptr_ != nullptr) {
      delete _otar_ptr_;
      _otar_ptr_ = nullptr;
      *_out_ << std::endl; // add a new line at the end of text archive.
    }
    if (_oxar_ptr_ != nullptr) {
      delete _oxar_ptr_;
      _oxar_ptr_ = nullptr;
    }
    if (_obar_ptr_ != nullptr) {
      delete _obar_ptr_;
      _obar_ptr_ = nullptr;
    }
    _write_archive_is_initialized_ = false;
    return 0;
  }

  int io_factory::reset_write()
  {
    if (_out_ != nullptr) {
      _out_->flush();
      _out_ = nullptr;
    }
    if (_out_fs_ != nullptr) {
      _out_fs_->flush();
      _out_fs_->reset();
      delete _out_fs_;
      _out_fs_ = nullptr;
    }
    if (_fout_ != nullptr) {
      _fout_->close();
      delete _fout_;
      _fout_ = nullptr;
    }
    return 0;
  }

  int io_factory::init(const std::string & stream_name_, int mode_)
  {
    _mode_ = mode_;
    if (this->is_read()) {
      this->init_read(stream_name_);
      if (this->is_single_archive()) this->init_read_archive();
    } else {
      this->init_write(stream_name_);
      if (this->is_single_archive()) this->init_write_archive();
    }
    return 0;
  }

  void io_factory::start_archive()
  {
    if (this->is_multi_archives()) {
      if (this->is_read()) this->init_read_archive();
      if (this->is_write()) this->init_write_archive();
    }
    return;
  }

  void io_factory::stop_archive()
  {
    if (this->is_multi_archives()) {
      if (this->is_read()) this->reset_read_archive();
      if (this->is_write()) this->reset_write_archive();
    }
    return;
  }

  void io_factory::ctor_defaults()
  {
    _write_archive_is_initialized_ = false;
    _read_archive_is_initialized_  = false;
    _in_ = nullptr;
    _out_ = nullptr;
    _fin_ = nullptr;
    _fout_ = nullptr;
    _itar_ptr_ = nullptr;
    _otar_ptr_ = nullptr;
    _ixar_ptr_ = nullptr;
    _oxar_ptr_ = nullptr;
    _ibar_ptr_ = nullptr;
    _obar_ptr_ = nullptr;
    _in_fs_ = nullptr;
    _out_fs_ = nullptr;
    _mode_ = io_factory::MODE_DEFAULT;
    return;
  }

  int io_factory::reset()
  {
    if (this->is_read()) {
      this->reset_read_archive();
      this->reset_read();
    }

    if (this->is_write()) {
      this->reset_write_archive();
      this->reset_write();
    }

    this->ctor_defaults();
    if (_locale_) {
      delete _locale_;
      _locale_ = nullptr;
    }

    if (_default_locale_) {
      delete _default_locale_;
      _default_locale_ = nullptr;
    }
    return 0;
  }

  io_factory::io_factory(int mode_)
  {
    set_logging_priority(datatools::logger::PRIO_FATAL);
    _default_locale_ = nullptr;
    _locale_ = nullptr;
    _default_locale_ = new std::locale(std::locale::classic(),
                                       new boost::archive::codecvt_null<char>);

    bool local_write = ((mode_ & MASK_RW) != 0);

    if (local_write) {
      _locale_ = new std::locale(*_default_locale_,
                                 new boost::math::nonfinite_num_put<char>);
    } else {
      _locale_ = new std::locale(*_default_locale_,
                                 new boost::math::nonfinite_num_get<char>);
    }

    this->ctor_defaults();
    this->init("", mode_);
    return;
  }

  io_factory::io_factory(const std::string & stream_name_, int mode_)
  {
    set_logging_priority(datatools::logger::PRIO_FATAL);
    _default_locale_ = nullptr;
    _locale_ = nullptr;
    _default_locale_ = new std::locale(std::locale::classic(),
                                       new boost::archive::codecvt_null<char>);
    bool local_write = ((mode_ & MASK_RW) != 0);

    if (local_write) {
      _locale_ = new std::locale(*_default_locale_,
                                 new boost::math::nonfinite_num_put<char>);
    } else {
      _locale_ = new std::locale(*_default_locale_,
                                 new boost::math::nonfinite_num_get<char>);
    }
    this->ctor_defaults();
    this->init(stream_name_, mode_);
    return;
  }

  io_factory::~io_factory()
  {
    this->reset();
    return;
  }

  void io_factory::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    std::ostringstream tag_ss, last_tag_ss; // tree_trick
    tag_ss << i_tree_dumpable::tag; // tree_trick
    last_tag_ss << i_tree_dumpable::inherit_tag(inherit_); // tree_trick
    std::string local_tag = tag_ss.str(); // tree_trick
    std::string local_last_tag = last_tag_ss.str(); // tree_trick
    if (!title_.empty()) out_ << indent_ << title_ << std::endl;
    out_ << indent_ << local_tag
         << "Mode  : " << std::hex << _mode_ << std::dec << std::endl;

    out_ << indent_ << local_tag
         << "is_read  : " << this->is_read() << std::endl;

    out_ << indent_ << local_tag
         << "is_write : " << this->is_write() << std::endl;

    out_ << indent_ << local_tag
         << "is_compressed : " << this->is_compressed() << std::endl;

    out_ << indent_ << local_tag
         << "is_uncompressed : " << this->is_uncompressed() << std::endl;

    out_ << indent_ << local_tag
         << "is_gzip : " << this->is_gzip() << std::endl;

    out_ << indent_ << local_tag
         << "is_bzip2 : " << this->is_bzip2() << std::endl;

    out_ << indent_ << local_tag
         << "is_text : " << this->is_text() << std::endl;

    out_ << indent_ << local_tag
         << "is_binary : " << this->is_binary() << std::endl;

    out_ << indent_ << local_tag
         << "is_xml : " << this->is_xml() << std::endl;

    out_ << indent_ << local_tag
         << "is_single_archive : " << this->is_single_archive() << std::endl;

    out_ << indent_ << local_last_tag
         << "is_multi_archives : " << this->is_multi_archives() << std::endl;
    return;
  }

  void io_factory::dump(std::ostream & out_) const
  {
    this->io_factory::tree_dump(out_, "io_factory::dump:");
    return;
  }

  int io_factory::guess_mode_from_filename(const std::string & filename_,
                                           int & mode_)
  {
    int status = io_factory::SUCCESS;
    int mode = 0x0;
    std::string fn = filename_;
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

    mode_ = mode;
    return status;
  }

  //----------------------------------------------------------------------
  // The io_reader class

  io_reader::io_reader(int mode_)
    : io_factory(io_factory::MODE_READ | mode_)
  {
    DT_THROW_IF(!this->is_read(),
                std::logic_error,
                "Cannot force non-read mode!");
    return;
  }

  io_reader::io_reader(const std::string & stream_name_, int mode_)
    : io_factory(stream_name_, io_factory::MODE_READ | mode_)
  {
    DT_THROW_IF(!this->is_read(),
                std::logic_error,
                "Cannot force non-read mode!");
    return;
  }

  io_reader::~io_reader()
  {
    return;
  }

  //----------------------------------------------------------------------
  // The io_writer_class

  io_writer::io_writer(int mode_)
    : io_factory(io_factory::MODE_WRITE | mode_)
  {
    DT_THROW_IF(!this->is_write(),
                std::logic_error,
                "Cannot force non-write mode!");
    return;
  }

  io_writer::io_writer(const std::string & stream_name_, int mode_)
    : io_factory(stream_name_, io_factory::MODE_WRITE | mode_)
  {
    DT_THROW_IF(!this->is_write(),
                std::logic_error,
                "Cannot force non-write mode!");
    return;
  }

  io_writer::~io_writer()
  {
    return;
  }

  //----------------------------------------------------------------------
  // The data_reader class
  const std::string & data_reader::empty_record_tag()
  {
    static std::string _tag;
    return _tag;
  }

  data_reader::data_reader()
  {
    _reader_ = nullptr;
    return;
  }

  data_reader::data_reader(const std::string & filename_,
                           bool use_multiple_archives_)
  {
    _reader_ = nullptr;
    this->init(filename_, use_multiple_archives_);
    return;
  }

  data_reader::data_reader(const std::string & filename_, int mode_)
  {
    _reader_ = nullptr;
    this->init_reader(filename_, mode_);
    return;
  }

  data_reader::~data_reader()
  {
    this->reset();
    return;
  }

  void data_reader::dump(std::ostream & out_) const
  {
    out_ << "data_reader::dump: " << std::endl;
    out_ << " |-- "
         << "Status           : " << ((_status_ == STATUS_OK) ? "Ok" : "Error") << std::endl;
    out_ << " |-- "
         << "EOF              : " << ((_status_ == STATUS_EOF) ? "EOF" : "-") << std::endl;
    out_ << " |-- "
         << "Initialized      : " << std::boolalpha << this->is_initialized() << std::endl;
    out_ << " |-- "
         << "Multi archive    : " << std::boolalpha << this->is_multi_archives() << std::endl;
    out_ << " |-- "
         << "Single archive   : " << std::boolalpha << this->is_single_archive() << std::endl;
    out_ << " |-- "
         << "Compressed       : " << std::boolalpha << this->is_compressed() << std::endl;
    out_ << " |-- "
         << "Gzipped          : " << std::boolalpha << this->is_gzip() << std::endl;
    out_ << " |-- "
         << "Bzipped          : " << std::boolalpha << this->is_bzip2() << std::endl;
    out_ << " |-- "
         << "Text archive     : " << std::boolalpha << this->is_text() << std::endl;
    out_ << " |-- "
         << "XML archive      : " << std::boolalpha << this->is_xml() << std::endl;
    out_ << " |-- "
         << "Bin archive(port): " << std::boolalpha << this->is_portable_binary() << std::endl;
    out_ << " |-- "
         << "Reader           : " << (_reader_ != nullptr ? "Yes" : "No") << std::endl;
    out_ << " `-- "
         << "Next tag         : '" << (_next_tag_) << "'" << std::endl;
    return;
  }

  void data_reader::read_next_tag()
  {
    DT_LOG_TRACE_ENTERING(_reader_->get_logging_priority());
    if (_reader_->eof()) {
      _next_tag_ = empty_record_tag();
      _status_ = STATUS_EOF;
      return;
    }
    if (_status_ != STATUS_OK) {
      DT_LOG_TRACE(_reader_->get_logging_priority(),"status != STATUS_OK");
      _next_tag_ = empty_record_tag();
      DT_LOG_TRACE_EXITING(_reader_->get_logging_priority());
      return;
    }
    DT_LOG_TRACE(_reader_->get_logging_priority(),"Continue...");
    try {
      DT_LOG_TRACE(_reader_->get_logging_priority(),"Try block starts...");
      if (_reader_->is_multi_archives()) {
        DT_LOG_TRACE(_reader_->get_logging_priority(),"multi-archives...");
        _reader_->start_archive();
      }
      DT_LOG_TRACE(_reader_->get_logging_priority(),"Continue...");
      std::string tag_id;
      _next_tag_ = "";
      this->basic_load(tag_id);
      DT_LOG_TRACE(_reader_->get_logging_priority(),"basic_load done with tag_id = '" << tag_id << "'");
      _next_tag_ = tag_id;
      DT_LOG_TRACE(_reader_->get_logging_priority(),"next_tag_ '= " << _next_tag_ << "'");
    } catch (std::runtime_error & x) {
      std::string msg = x.what();
      if (msg.find("EOF") != msg.npos) {
      }
      _status_   = STATUS_ERROR;
      _next_tag_ = empty_record_tag();
      DT_LOG_ERROR(_reader_->get_logging_priority(), x.what());
    } catch (std::exception & x) {
      _status_   = STATUS_ERROR;
      _next_tag_ = empty_record_tag();
      DT_LOG_ERROR(_reader_->get_logging_priority(), x.what());
    } catch (...) {
      _status_   = STATUS_ERROR;
      _next_tag_ = empty_record_tag();
      DT_LOG_ERROR(_reader_->get_logging_priority(), "Unexpected exception!");
    }
    DT_LOG_TRACE_EXITING(_reader_->get_logging_priority());
    return;
  }

  bool data_reader::is_error() const
  {
    return _status_ != STATUS_OK;
  }

  bool data_reader::is_eof() const
  {
    return _status_ != STATUS_EOF;
  }

  void data_reader::init_reader(const std::string & filename_, int mode_)
  {
    _status_ = STATUS_OK;
    _reader_ = new io_reader(filename_, mode_);
    this->read_next_tag();
    return;
  }

  void data_reader::reset_reader()
  {
    if (_reader_ != nullptr) {
      delete _reader_;
      _reader_ = nullptr;
    }
    _next_tag_.clear();
    _status_ = STATUS_OK;
    return;
  }

  const std::string & data_reader::get_record_tag() const
  {
    if (_status_ != STATUS_OK) return empty_record_tag();
    return _next_tag_;
  }

  bool data_reader::is_initialized() const
  {
    return _reader_ != nullptr;
  }

  bool data_reader::is_uncompressed() const
  {
    return !this->is_compressed();
  }

  bool data_reader::is_compressed() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_compressed();
  }

  bool data_reader::is_gzip() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_gzip();
  }

  bool data_reader::is_text() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_text();
  }

  bool data_reader::is_binary() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_binary();
  }

  bool data_reader::is_portable_binary() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_portable_binary();
  }

  bool data_reader::is_xml() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_xml();
  }

  bool data_reader::is_bzip2() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_bzip2();
  }

  bool data_reader::is_single_archive() const
  {
    return !this->is_multi_archives();
  }

  bool data_reader::is_multi_archives() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Reader is not initialized!");
    return _reader_->is_multi_archives();
  }

  bool data_reader::has_record_tag() const
  {
    if (_status_ != STATUS_OK) return false;
    if (_next_tag_.empty()) return false;
    return true;
  }

  bool data_reader::record_tag_is(const std::string & tag_) const
  {
    return _next_tag_ == tag_;
  }

  void data_reader::reset()
  {
    this->reset_reader();
    return;
  }

  void data_reader::init(const std::string& filename_,
                         bool use_multiple_archives_)
  {
    this->reset_reader();
    int mode_guess;
    DT_THROW_IF(io_factory::guess_mode_from_filename(filename_, mode_guess)
                != io_factory::SUCCESS,
                std::runtime_error,
                "Cannot guess mode for file '" << filename_ << "'!");
    int mode = mode_guess;
    if (use_multiple_archives_) mode |= io_factory::multi_archives;
    this->init_reader(filename_, mode);
    return;
  }

  void data_reader::init(const std::string & filename_, int mode_)
  {
    this->reset_reader();
    this->init_reader(filename_, mode_);
    return;
  }

  //----------------------------------------------------------------------
  // The data_writer class
  data_writer::data_writer()
  {
    _writer_ = nullptr;
    return;
  }

  data_writer::data_writer(const std::string& filename_,
                           bool multiple_archives_,
                           bool this_append_mode_) {
    _writer_ = nullptr;
    this->init(filename_, multiple_archives_, this_append_mode_);
    return;
  }

  data_writer::data_writer (const std::string & filename_, int mode_)
  {
    _writer_ = nullptr;
    this->init_writer(filename_, mode_);
    return;
  }

  data_writer::~data_writer()
  {
    this->reset();
    return;
  }

  bool data_writer::is_initialized() const
  {
    return _writer_ != nullptr;
  }

  bool data_writer::is_uncompressed() const
  {
    return !this->is_compressed();
  }

  bool data_writer::is_compressed() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_compressed();
  }

  bool data_writer::is_gzip() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_gzip();
  }

  bool data_writer::is_text() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_text();
  }

  bool data_writer::is_binary() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_binary();
  }

  bool data_writer::is_portable_binary() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_portable_binary();
  }


  bool data_writer::is_xml() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_xml();
  }


  bool data_writer::is_bzip2() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return _writer_->is_bzip2();
  }


  bool data_writer::is_single_archive() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::logic_error,
                "Writer is not initialized!");
    return !this->is_multi_archives();
  }


  bool data_writer::is_multi_archives() const
  {
    DT_THROW_IF(!this->is_initialized(),
                std::runtime_error,
                "Writer is not initialized!");
    return _writer_->is_multi_archives();
  }

  void data_writer::init_writer(const std::string & filename_, int mode_)
  {
    _writer_ = new io_writer(filename_, mode_);
    return;
  }

  void data_writer::reset_writer()
  {
    if (_writer_ != nullptr) {
      delete _writer_;
      _writer_ = nullptr;
    }
    return;
  }

  void data_writer::reset()
  {
    this->reset_writer();
    return;
  }

  void data_writer::init(const std::string & filename_,
                         bool use_multiple_archives_,
                         bool use_append_mode_)
  {
    this->reset_writer();
    int mode_guess;
    DT_THROW_IF(io_factory::guess_mode_from_filename(filename_, mode_guess)
                != io_factory::SUCCESS,
                std::logic_error,
                "Cannot guess mode for file '" << filename_ << "' !");
    int mode = mode_guess;
    if (use_multiple_archives_) mode |= io_factory::multi_archives;
    if (use_append_mode_) mode |= io_factory::append;
    this->init_writer(filename_, mode);
    return;
  }

  void data_writer::init(const std::string & filename_, int mode_)
  {
    this->reset_writer();
    this->init_writer(filename_, mode_);
    return;
  }

  void data_writer::init_multi(const std::string& filename_)
  {
    this->init(filename_, using_multiple_archives);
    return;
  }

  void data_writer::init_single(const std::string& filename_)
  {
    this->init(filename_, using_single_archive);
    return;
  }

} // end of namespace datatools
