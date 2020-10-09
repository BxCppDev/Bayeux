/// \file datatools/io_factory.h
/*
 * Description :
 *
 *  Some generic reader and writer based on Boost archives.
 *
 * Copyright (C) 2011-2019 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef DATATOOLS_IO_FACTORY_H
#define DATATOOLS_IO_FACTORY_H

// Standard Library:
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <locale>
// #include <typeinfo>

// Third Party:
// - Boost:
// 2012-01-09 FM : now use the Boost >=1.47 header :
//#include <boost/math/nonfinite_num_facets.hpp>
//#include <boost/math/special_functions/nonfinite_num_facets.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/iostreams/filtering_stream.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
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
// #ifdef __clang__
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wunused-parameter"
// #pragma clang diagnostic ignored "-Wshadow"
// #endif
// #ifdef __GNUC__
// #pragma GCC diagnostic push
// // #pragma GCC diagnostic ignored "-Wunused-parameter" // Bug in GCC
// #pragma GCC diagnostic ignored "-Wshadow"
// #endif
// #include <boost/iostreams/filter/gzip.hpp>
// #ifdef __GNUC__
// #pragma GCC diagnostic pop
// #endif
// #ifdef __clang__
// #pragma clang diagnostic pop
// #endif

// #include <boost/iostreams/filter/bzip2.hpp>
#include <boost/serialization/string.hpp>

// Wrap Boost's tokenizer header
// This header, causes "unused parameter" warnings from its
// static void assign(Iterator b, Iterator e, Token &t)
// function.
// This should be an innocuous warning, so remove diagnostic for this
// header only.
// We only use clang pragmas for now because GCC's are highly version
// dependent - so need a bit more thought.
// To be removed when Boost fix their headers...
// #ifdef __clang__
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wunused-parameter"
// #endif
// #include <boost/tokenizer.hpp>
// #ifdef __clang__
// #pragma clang diagnostic pop
// #endif

// Datatools
#include <datatools/archives_list.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  //! \brief A generic base reader/writer class based on Boost/Serialization
  class io_factory
    : public datatools::i_tree_dumpable
  {
  public:
    static const int SUCCESS = 0;
    static const int ERROR   = 1;

    static const unsigned int MASK_RW           = 0x1;
    static const unsigned int MASK_FORMAT       = 0xE;
    static const unsigned int MASK_COMPRESSION  = 0x30;
    static const unsigned int MASK_MULTIARCHIVE = 0x80;
    static const unsigned int MASK_APPEND       = 0x100;

    enum mode_flag_type {
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

    struct format {
      static const std::string & text_extension();
      static const std::string & xml_extension();
      static const std::string & binary_extension();
      static const std::string & gzip_extension();
      static const std::string & bzip2_extension();
    };

    static int guess_mode_from_filename(const std::string & filename_,
                                        int & mode_);

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const;

    /// Constructor
    io_factory(int mode_ = io_factory::MODE_DEFAULT);

    /// Constructor
    io_factory(const std::string & stream_name_,
               int mode_ = io_factory::MODE_DEFAULT);

    /// Destructor
    ~io_factory() override;

    bool eof() const;

    bool is_read() const;

    bool is_write() const;

    bool is_compressed() const;

    bool is_uncompressed() const;

    bool is_gzip() const;

    bool is_bzip2() const;

    bool is_text() const;

    bool is_binary() const;

    bool is_portable_binary() const;

    bool is_xml() const;

    bool is_append() const;

    bool is_no_append() const;

    bool is_single_archive() const;

    bool is_multi_archives() const;

    void start_archive();

    void stop_archive();

  private:

    void init_read_archive();

    void init_read(const std::string & stream_name_);

    void reset_read_archive();

    void reset_read();

    void init_write_archive();

    void init_write(const std::string & stream_name_);

    void reset_write_archive();

    void reset_write();

    void init(const std::string& stream_name_, int mode_);

    void ctor_defaults();

    void reset();

  public:

    template <typename Data>
    void store(const Data & data_)
    {
      DT_THROW_IF (!this->is_write(),
                   std::logic_error,
                   "Not a writer factory!");
      if (_otar_ptr_ != nullptr) {
        this->store_text<Data>(*_otar_ptr_, data_);
      } else if (_oxar_ptr_ != nullptr) {
        this->store_xml<Data>(*_oxar_ptr_, data_);
      } else if (_obar_ptr_ != nullptr) {
        this->store_binary<Data>(*_obar_ptr_, data_);
      }
    }

    template <typename Data>
    void load(Data & data_)
    {
      DT_THROW_IF (!this->is_read(),
                   std::logic_error,
                   "Not a reader factory!");
      DT_THROW_IF (_in_fs_ == nullptr,
                   std::runtime_error,
                   "No input stream!");

      if (*_in_fs_) {
        // 2008-10-03 FM: add EOF check code
        // 2009-03-08 FM: Is it possible to have better EOF check code
        // here for unzipped text and XML archives?
        char c = 0;
        _in_fs_->get(c);
        if (_in_fs_) {
          _in_fs_->putback(c);
        }
      }

      if (! *_in_fs_) {
        DT_THROW_IF (_in_fs_->eof(),
                     std::runtime_error,
                     "Input stream at EOF!");
        DT_THROW_IF (_in_fs_->fail(),
                     std::runtime_error,
                     "Input stream in fail status!");
        DT_THROW_IF (_in_fs_->bad(),
                     std::runtime_error,
                     "Input stream in bad status!");
      }

      try {
        if (_itar_ptr_ != nullptr) {
          this->load_text<Data>(*_itar_ptr_, data_);
          *_in_fs_ >> std::ws;
        } else if (_ixar_ptr_ != nullptr) {
          this->load_xml<Data>(*_ixar_ptr_, data_);
          *_in_fs_ >> std::ws;
        } else if (_ibar_ptr_ != nullptr) {
          this->load_binary<Data>(*_ibar_ptr_, data_);
        }
      } catch (boost::archive::archive_exception & x) {
        throw;
      } catch (std::exception & x) {
        throw;
      } catch (...) {
        DT_LOG_WARNING(get_logging_priority(),
                       "Cannot load data from archive: "
                       << "unexpected exception" << "!");
        DT_THROW(std::runtime_error, "Internal exception!");
      }

      if (! *_in_fs_) {
        if (_in_fs_->fail()) {
          DT_LOG_WARNING(get_logging_priority(),
                         "Input stream is now in fail status!");
        }
        if (_in_fs_->eof()) {
          DT_LOG_WARNING(get_logging_priority(),
                         "Input stream is now in EOF status!");
        }
        if (_in_fs_->bad()) {
          DT_LOG_WARNING(get_logging_priority(),
                         "Input stream is now in bad status!");
        }
      }
    }

    template <typename Data>
    friend io_factory & operator<<(io_factory & iof_, const Data & data_)
    {
      iof_.store(data_);
      return iof_;
    }

    template <typename Data>
    friend io_factory & operator>>(io_factory & iof_, Data & data_)
    {
      iof_.load(data_);
      return iof_;
    }

    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    void dump(std::ostream & out_) const;

  private:

    template <typename Data>
    void store_text(boost::archive::text_oarchive & archive_,
                    const Data & data_)
    {
      const Data & b = data_;
      archive_ << b;
      return;
    }

    template <typename Data>
    void store_xml(boost::archive::xml_oarchive & archive_,
                   const Data & data_)
    {
      const Data & b = data_;
      archive_ << boost::serialization::make_nvp("record", b);
      return;
    }

    template <typename Data>
    void store_binary(datatools::portable_oarchive & archive_,
                      const Data & data_)
    {
      const Data & b = data_;
      archive_ << b;
      return;
    }

    template <typename Data>
    void load_text(boost::archive::text_iarchive & archive_,
                   Data & data_) {
      Data & b = data_;
      archive_ >> b;
      return;
    }

    template <typename Data>
    void load_xml(boost::archive::xml_iarchive & archive_,
                  Data & data_)
    {
      Data & b = data_;
      archive_ >> boost::serialization::make_nvp("record", b);
      return;
    }

    template <typename Data>
    void load_binary(datatools::portable_iarchive & archive_,
                     Data & data_)
    {
      Data & b = data_;
      archive_ >> b;
      return;
    }

  private:


    datatools::logger::priority _logging_priority_ = datatools::logger::PRIO_FATAL; ///< Logging priority threshold
    unsigned int _mode_ = 0; ///< Mode bitset of the I/O factory

    std::istream * _in_  = nullptr; ///< Handle to the input stream
    std::ostream * _out_ = nullptr; ///< Handle to the output stream

    std::ifstream * _fin_  = nullptr; ///< Handle to the input file stream
    std::ofstream * _fout_ = nullptr; ///< Handle to the output file stream

    boost::iostreams::filtering_istream * _in_fs_  = nullptr; ///< Handle to the filtering input stream
    boost::iostreams::filtering_ostream * _out_fs_ = nullptr; ///< Handle to the filtering output stream

    std::locale * _default_locale_ = nullptr; ///< Handle to the default locale instance
    std::locale * _locale_         = nullptr; ///< Handle to the current locale instance

    bool _read_archive_is_initialized_  = false;
    bool _write_archive_is_initialized_ = false;

    boost::archive::text_iarchive * _itar_ptr_ = nullptr; ///< Handle to the text input archive
    boost::archive::text_oarchive * _otar_ptr_ = nullptr; ///< Handle to the text output archive

    boost::archive::xml_iarchive  * _ixar_ptr_ = nullptr; ///< Handle to the XML input archive
    boost::archive::xml_oarchive  * _oxar_ptr_ = nullptr; ///< Handle to the XML output archive

    datatools::portable_iarchive * _ibar_ptr_ = nullptr;  ///< Handle to the portable binary input archive
    datatools::portable_oarchive * _obar_ptr_ = nullptr;  ///< Handle to the portable binary output archive
    
  }; // end of class io_factory

  //----------------------------------------------------------------------
  // Reader factory tag type
  //
  /*! \brief A generic reader class inherited from the io_factory class
   *
   *
   */
  class io_reader
    : public io_factory
  {
  public:
    io_reader(int mode_ = io_factory::MODE_DEFAULT);

    io_reader(const std::string & stream_name_,
              int mode_ = io_factory::MODE_DEFAULT);

    ~io_reader() override;
  };


  //----------------------------------------------------------------------
  // Writer factory tag type
  //
  /*! \brief A generic writer class inherited from the io_factory class
   *
   *
   */
  class io_writer
    : public io_factory
  {
  public:
    io_writer(int mode_ = io_factory::MODE_DEFAULT);
    io_writer(const std::string & stream_name_,
              int mode_ = io_factory::MODE_DEFAULT);

    ~io_writer() override;
  };

  //----------------------------------------------------------------------
  // user friendly constants used in ctors
  // for data_reader/data_writer:
  static const bool using_multi_archives    = true;
  static const bool using_multiple_archives = using_multi_archives;
  static const bool using_single_archive    = false;
  static const bool append_mode             = true;
  static const bool no_append_mode          = false;

  //----------------------------------------------------------------------
  // data_reader class
  //
  /*! \brief A generic data reader based on Boost/Serialization
   *
   * The data_reader class supports :
   * - portable text (ASCII) archives
   * - portable XML archives
   * - portable binary archives
   *
   * Example :
   * \code
   * #include <datatools/io_factory.h>
   * #include <datatools/properties.h>
   *
   * int main()
   * {
   *   datatools::data_reader reader ("test.xml", datatools::using_multi_archives);
   *   datatools::properties setup;
   *   if (reader.has_record_tag () &&
   *       reader.record_tag_is (datatools::properties::SERIAL_TAG))
   *   {
   *     reader.load (setup);
   *   }
   *   return 0;
   * }
   * \endcode
   */
  class data_reader
  {

  public:

    enum status_type {
      STATUS_OK    = 0,
      STATUS_ERROR = 1,
      STATUS_EOF   = 2
    };

    static const std::string & empty_record_tag();

    /// Constructor
    data_reader();

    /// Constructor
    data_reader(const std::string & filename_,
                bool use_multiple_archives_ = using_single_archive);

    /// Constructor
    data_reader(const std::string & filename_, int mode_);

    /// Destructor
    virtual ~data_reader();

    bool is_error() const;

    bool is_eof() const;

    const std::string & get_record_tag() const;

    bool has_record_tag() const;

    bool record_tag_is(const std::string & tag_) const;

    void reset();

    bool is_initialized() const;

    bool is_multi_archives() const;

    bool is_single_archive() const;

    bool is_compressed() const;

    bool is_uncompressed() const;

    bool is_gzip() const;

    bool is_bzip2() const;

    bool is_text() const;

    bool is_binary() const;

    bool is_portable_binary() const;

    bool is_xml() const;

    void init(const std::string & filename_,
              bool use_multiple_archives_ = using_single_archive);

    void init_multi(const std::string & filename_)
    {
      this->init(filename_, using_multiple_archives);
      return;
    }

    void init_single(const std::string & filename_)
    {
      this->init(filename_, using_single_archive);
    }

    void init(const std::string & filename_, int mode_);

    void dump(std::ostream & out_ = std::clog) const;

    template <typename Data>
    void load(const std::string & tag_, Data & data_)
    {
      DT_THROW_IF (!this->has_record_tag(),
                   std::logic_error,
                   "No more record tag!");
      DT_THROW_IF(this->get_record_tag() != tag_,
                  std::logic_error,
                  "Unexpected tag ('" << this->get_record_tag()
                  << " != " << tag_ << "')!");
      this->basic_load(data_);
      if (_reader_->is_multi_archives()) _reader_->stop_archive();
      this->read_next_tag();
      return;
    }

    template <typename Data>
    void load_alt(const std::string & tag_, const std::string & alt_tag_, Data & data_)
    {
      DT_THROW_IF (!this->has_record_tag(),
                   std::logic_error,
                   "No more record tag!");
      if (this->get_record_tag() != tag_) {
        DT_THROW_IF (this->get_record_tag() != alt_tag_,
                     std::logic_error,
                     "Unexpected tag ('"
                     << this->get_record_tag()
                     << " != " << tag_ << "')!");
      }
      this->basic_load(data_);
      if (_reader_->is_multi_archives()) _reader_->stop_archive();
      this->read_next_tag();
      return;
    }

    template <typename Data>
    void load_serializable(Data & data_,
                           typename boost::disable_if< has_bsts<Data> >::type * dummy_ = 0)
    {
      if(!dummy_) dummy_ = 0;
      this->load(data_.get_serial_tag(), data_);
      return;
    }

    template <typename Data>
    void load_serializable(Data & data_,
                           typename boost::enable_if< has_bsts<Data> >::type * dummy_ = 0)
    {
      if(!dummy_) dummy_ = 0;
      this->load_alt(data_.get_serial_tag(),
                     ::datatools::backward_serial_tag<Data> (0),
                     data_);
      return;
    }

    template <typename Data>
    void load(Data & data_)
    {
      load_serializable(data_);
      return;
    }

  protected:
    template <typename Data>
    void basic_load(Data & data_)
    {
      DT_THROW_IF (_reader_ == nullptr,
                   std::logic_error,
                   "Not initialized!");
      try {
        _reader_->load(data_);
      } catch (std::exception & x) {
        _status_ = STATUS_ERROR;
        bool warn = false;
        //>>> 2008-11-13 FM: skip EOF message printing
        std::string msg = x.what();
        if (msg.find("EOF") != msg.npos) {
          _status_ = STATUS_EOF;
          warn = false;
        }
        if (warn) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Cannot read data: exception="
                       << x.what() << " !");
        }
        //<<<
        _next_tag_ = empty_record_tag();
        DT_THROW_IF(true, std::logic_error, x.what());
      } catch (...) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot read data: "
                     << "Unexpected exception" << " !");
        _status_   = STATUS_ERROR;
        _next_tag_ = empty_record_tag();
        DT_THROW_IF(true, std::logic_error, "Unexpected error!");
      }
      return;
    }


  private:

    void read_next_tag();

    void init_reader(const std::string & filename_, int mode_);

    void reset_reader();

  private:

    int           _status_ = STATUS_OK; 
    io_reader   * _reader_ = nullptr;
    std::string   _next_tag_;

  };

  //----------------------------------------------------------------------
  // data_writer class
  //
  /*! \brief A generic data writer based on Boost/Serialization
   *
   * The data_reader class supports :
   * - portable text (ASCII) archives
   * - portable XML archives
   * - portable binary archives
   * Example :
   * \code
   * #include <datatools/io_factory.h>
   * #include <datatools/properties.h>
   *
   * int main()
   * {
   *   datatools::data_writer writer ("test.xml", datatools::using_multi_archives);
   *   datatools::properties setup;
   *   writer.store (setup);
   *   return 0;
   * }
   * \endcode
   */
  class data_writer
  {
  public:
    /// Default constructor
    data_writer();

    /// Constructor
    data_writer(const std::string & filename_, int mode_);

    /// Constructor
    data_writer(const std::string & filename_,
                bool use_multiple_archives_ = using_single_archive,
                bool append_mode_ = no_append_mode);

    /// Destructor
    virtual ~data_writer();

    bool is_initialized() const;

    bool is_multi_archives() const;

    bool is_single_archive() const;

    bool is_compressed() const;

    bool is_uncompressed() const;

    bool is_gzip() const;

    bool is_bzip2() const;

    bool is_text() const;

    bool is_binary() const;

    bool is_portable_binary() const;

    bool is_xml() const;

    void reset();

    void init(const std::string & filename_,
              bool a_multiple_archives_ = using_single_archive,
              bool a_append_mode_ = no_append_mode);

    void init_multi(const std::string & filename_);

    void init_single(const std::string & filename_);

    void init(const std::string & filename_, int mode_);

  public:

    template <typename Data>
    void store(const std::string & tag_, const Data & data_)
    {
      if (_writer_->is_multi_archives()) {
        _writer_->start_archive();
      }
      this->basic_store<std::string>(tag_);
      this->basic_store<Data>(data_);
      if (_writer_->is_multi_archives()) {
        _writer_->stop_archive();
      }
      return;
    }

    template <typename Data>
    void store(const Data & data_)
    {
      const datatools::i_serializable& i_ser =
        static_cast<const datatools::i_serializable &>(data_);
      this->store<Data>(i_ser.get_serial_tag(), data_);
      return;
    }

  protected:

    template <typename Data>
    void basic_store(const Data & data_)
    {
      DT_THROW_IF(_writer_ == nullptr,
                  std::logic_error,
                  "Not initialized!");
      _writer_->store<Data>(data_);
      return;
    }

  private:

    void init_writer(const std::string & filename_, int mode_);

    void reset_writer();

  private:

    io_writer * _writer_ = nullptr;

  };

} // end of namespace datatools

#endif // DATATOOLS_IO_FACTORY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
