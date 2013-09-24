// -*- mode: C++; -*-
/* io_factory.h */
/*
 * Description :
 *
 *  Some generic reader and writer based on Boost archives.
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_IO_FACTORY_H_
#define DATATOOLS_IO_FACTORY_H_

// Standard Library
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <locale>
#include <typeinfo>

// Third Party
// - Boost
// 2012-01-09 FM : now use the Boost 1.47 header :
//#include <boost/math/nonfinite_num_facets.hpp>
#include <boost/math/special_functions/nonfinite_num_facets.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/serialization/string.hpp>
#include <boost/tokenizer.hpp>

// Datatools
#include <datatools/archives_list.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

/*! \brief A generic base reader/writer class based on Boost/Serialization
 */
class io_factory : public datatools::i_tree_dumpable {
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
    static const std::string text_extension();
    static const std::string xml_extension();
    static const std::string binary_extension();
    static const std::string gzip_extension();
    static const std::string bzip2_extension();
  };

  static int guess_mode_from_filename(const std::string& a_filename,
                                      int& mode);

  void set_logging_priority(datatools::logger::priority);

  datatools::logger::priority get_logging_priority() const;

  /// Constructor
  io_factory(int mode = io_factory::MODE_DEFAULT);

  /// Constructor
  io_factory(const std::string & a_stream_name,
             int mode = io_factory::MODE_DEFAULT);

  /// Destructor
  virtual ~io_factory();

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

  int init_read_archive();

  int init_read(const std::string& a_stream_nam);

  int reset_read_archive();

  int reset_read();

  int init_write_archive();

  int init_write(const std::string& a_stream_name);

  int reset_write_archive();

  int reset_write();

  int init(const std::string& a_stream_name, int a_mode);

  void ctor_defaults();

  int reset();

 public:

  template <typename Data>
  void store(const Data& data) {
    DT_THROW_IF (!this->is_write(),
                 std::logic_error,
                 "Not a writer factory!");
    if (otar_ptr_ != 0) {
      this->store_text<Data>(*otar_ptr_, data);
    } else if (oxar_ptr_ != 0) {
      this->store_xml<Data>(*oxar_ptr_, data);
    } else if (obar_ptr_ != 0) {
      this->store_binary<Data>(*obar_ptr_, data);
    }
  }

  template <typename Data>
  void load(Data& data) {
    DT_THROW_IF (!this->is_read(),
                 std::logic_error,
                 "Not a reader factory!");
    DT_THROW_IF (in_fs_ == 0,
                 std::runtime_error,
                 "No input stream!");

    // 2008-10-03 FM: add EOF check code
    if (*in_fs_) {
      // 2009-03-08 FM: Is it possible to have better EOF check code
      // here for unzipped text and XML archives?
      char c = 0;
      in_fs_->get(c);
      if (in_fs_) {
        in_fs_->putback(c);
      }
    }

    if (! *in_fs_) {
      DT_THROW_IF (in_fs_->eof(),
                   std::runtime_error,
                   "Input stream at EOF!");
      DT_THROW_IF (in_fs_->fail(),
                   std::runtime_error,
                   "Input stream in fail status!");
      DT_THROW_IF (in_fs_->bad(),
                   std::runtime_error,
                   "Input stream in bad status!");
    }

    try {
      if (itar_ptr_ != 0) {
        this->load_text<Data>(*itar_ptr_, data);
        *in_fs_ >> std::ws;
      } else if (ixar_ptr_ != 0) {
        this->load_xml<Data>(*ixar_ptr_, data);
        *in_fs_ >> std::ws;
      } else if (ibar_ptr_ != 0) {
        this->load_binary<Data>(*ibar_ptr_, data);
      }
    }
    // 2011-02-25 FM:
    catch (boost::archive::archive_exception& x) {
      throw;
    }
    catch (std::exception& x) {
      throw;
    }
    catch (...) {
      DT_LOG_WARNING(get_logging_priority(),
                     "Cannot load data from archive: "
                     << "unexpected exception" << "!");
      DT_THROW_IF(true,std::logic_error,"Internal exception!");
    }

    if (! *in_fs_) {
      if (in_fs_->fail()) {
        DT_LOG_WARNING(get_logging_priority(),
                       "Input stream is now in fail status!");
      }
      if (in_fs_->eof()) {
        DT_LOG_WARNING(get_logging_priority(),
                       "Input stream is now in EOF status!");
      }
      if (in_fs_->bad()) {
        DT_LOG_WARNING(get_logging_priority(),
                       "Input stream is now in bad status!");
      }
    }
  }

  template <typename Data>
  friend io_factory& operator<<(io_factory& iof, const Data& data) {
    iof.store(data);
    return iof;
  }

  template <typename Data>
  friend io_factory& operator>>(io_factory& iof, Data& data) {
    iof.load(data);
    return iof;
  }

  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string& title = "",
                         const std::string& indent = "",
                         bool inherit = false) const;

  void dump(std::ostream& out) const;

 private:
  template <typename Data>
  void store_text(boost::archive::text_oarchive& archive,
                  const Data& data) {
    const Data& b = data;
    archive << b;
  }

  template <typename Data>
  void store_xml(boost::archive::xml_oarchive& archive,
                 const Data& data) {
    const Data& b = data;
    archive << boost::serialization::make_nvp("record", b);
  }

  template <typename Data>
  void store_binary(eos::portable_oarchive& archive,
                    const Data& data) {
    const Data& b = data;
    archive << b;
  }

  template <typename Data>
  void load_text(boost::archive::text_iarchive& archive,
                 Data& data) {
    Data& b = data;
    archive >> b;
  }

  template <typename Data>
  void load_xml(boost::archive::xml_iarchive& archive,
                Data& data) {
    Data& b = data;
    archive >> boost::serialization::make_nvp("record", b);
  }

  template <typename Data>
  void load_binary(eos::portable_iarchive& archive,
                   Data& data) {
    Data& b = data;
    archive >> b;
  }

 private:

  int mode_;

  std::istream *in_;
  std::ostream *out_;

  std::ifstream *fin_;
  std::ofstream *fout_;

  boost::iostreams::filtering_istream *in_fs_;
  boost::iostreams::filtering_ostream *out_fs_;

  std::locale *default_locale_;
  std::locale *locale_;

  bool read_archive_is_initialized_;
  bool write_archive_is_initialized_;

  boost::archive::text_iarchive *itar_ptr_;
  boost::archive::text_oarchive *otar_ptr_;

  boost::archive::xml_iarchive  *ixar_ptr_;
  boost::archive::xml_oarchive  *oxar_ptr_;

  eos::portable_iarchive *ibar_ptr_;
  eos::portable_oarchive *obar_ptr_;

  datatools::logger::priority _logging_priority; /// Logging priority threshold

}; // end of class io_factory ?


//----------------------------------------------------------------------
// Reader factory tag type
//
/*! \brief A generic reader class inherited from the io_factory class
 *
 *
 */
class io_reader : public io_factory {
 public:
  io_reader(int mode = io_factory::MODE_DEFAULT);

  io_reader(const std::string& stream_name,
            int mode = io_factory::MODE_DEFAULT);

  virtual ~io_reader();
};


//----------------------------------------------------------------------
// Writer factory tag type
//
/*! \brief A generic writer class inherited from the io_factory class
 *
 *
 */
class io_writer : public io_factory {
 public:
  io_writer(int mode = io_factory::MODE_DEFAULT);
  io_writer(const std::string& stream_name,
            int mode = io_factory::MODE_DEFAULT);

  virtual ~io_writer();
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
 *   datatools::data_reader reader ("test.xml");
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
class data_reader {

 public:

  enum status_t {
    STATUS_OK    = 0,
    STATUS_ERROR = 1
  };

  static const std::string EMPTY_RECORD_TAG;

  /// Constructor
  data_reader();

  /// Constructor
  data_reader(const std::string& filename,
              bool use_multiple_archives = using_single_archive);

  /// Constructor
  data_reader(const std::string& filename, int mode);

  /// Destructor
  virtual ~data_reader();

  bool is_error() const;

  const std::string& get_record_tag() const;

  bool has_record_tag() const;

  bool record_tag_is(const std::string& tag) const;

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

  void init(const std::string& filename,
            bool use_multiple_archives = using_single_archive);

  void init_multi(const std::string& filename) {
    this->init(filename, using_multiple_archives);
  }

  void init_single(const std::string& filename) {
    this->init(filename, using_single_archive);
  }

  void init(const std::string& filename, int mode);

  void dump(std::ostream& out = std::clog) const;

  template <typename Data>
  void load(const std::string& tag, Data& data) {
    DT_THROW_IF (!this->has_record_tag(),
                 std::logic_error,
                 "No more record tag!");
    DT_THROW_IF(this->get_record_tag() != tag,
                 std::logic_error,
                "Unexpected tag ('" << this->get_record_tag()
                << " != " << tag << "')!");
    this->basic_load(data);
    if (reader_->is_multi_archives()) reader_->stop_archive();
    this->read_next_tag();
  }

  template <typename Data>
  void load_alt(const std::string& tag, const std::string& alt_tag, Data& data) {
    DT_THROW_IF (!this->has_record_tag(),
                 std::logic_error,
                 "No more record tag!");
    if (this->get_record_tag() != tag) {
      DT_THROW_IF (this->get_record_tag() != alt_tag,
                   std::logic_error,
                   "Unexpected tag ('"
                   << this->get_record_tag()
                   << " != " << tag << "')!");
    }
    this->basic_load(data);
    if (reader_->is_multi_archives()) reader_->stop_archive();
    this->read_next_tag();
  }

  template <typename Data>
  void load_serializable(Data& data,
    typename boost::disable_if< has_bsts<Data> >::type* dummy = 0) {
    this->load(data.get_serial_tag(), data);
  }

  template <typename Data>
  void load_serializable(
    Data& data,
    typename boost::enable_if< has_bsts<Data> >::type* dummy = 0) {
    this->load_alt(data.get_serial_tag(),
                   ::datatools::backward_serial_tag<Data> (0),
                   data);
  }

  template <typename Data>
  void load(Data& data) {
    load_serializable (data);
  }

 protected:
  template <typename Data>
  void basic_load(Data& data) {
    DT_THROW_IF (reader_ == 0,
                 std::logic_error,
                 "Not initialized!");
    try {
      reader_->load(data);
    }
    catch (std::exception& x) {
      bool warn = false;
      //>>> 2008-11-13 FM: skip EOF message printing
      std::string msg = x.what();
      if (msg.find("EOF") != msg.npos) {
        warn = false;
      }
      if (warn) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot read data: exception="
                     << x.what() << " !");
      }
      //<<<
      status_   = STATUS_ERROR;
      next_tag_ = EMPTY_RECORD_TAG;
      DT_THROW_IF(true, std::logic_error, x.what());
    }
    catch (...) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot read data: "
                   << "Unexpected exception" << " !");
      status_   = STATUS_ERROR;
      next_tag_ = EMPTY_RECORD_TAG;
      DT_THROW_IF(true, std::logic_error, "Unexpected error!");
    }
  }


 private:
  void read_next_tag();

  void init_reader(const std::string& filename, int a_mode);

  void reset_reader();

 private:
  int         status_;
  io_reader   *reader_;
  std::string next_tag_;

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
 *   datatools::data_writer writer ("test.xml");
 *   datatools::properties setup;
 *   writer.store (setup);
 *   return 0;
 * }
 * \endcode
 */
class data_writer {
 public:
  // ctor
  data_writer();

  data_writer(const std::string& filename, int mode);

  data_writer(const std::string& filename,
              bool use_multiple_archives = using_single_archive,
              bool append_mode = no_append_mode);

  // dtor
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

  void init(const std::string& filename,
            bool a_multiple_archives = using_single_archive,
            bool a_append_mode = no_append_mode);

  void init_multi(const std::string& filename);

  void init_single(const std::string& filename);

  void init(const std::string& filename, int mode);

 public:
  template <typename Data>
  void store(const std::string& tag, const Data& data) {
    if (writer_->is_multi_archives()) {
      writer_->start_archive();
    }

    this->basic_store<std::string>(tag);
    this->basic_store<Data>(data);

    if (writer_->is_multi_archives()) {
      writer_->stop_archive();
    }
  }

  template <typename Data>
  void store(const Data& data) {
    const datatools::i_serializable& i_ser =
      static_cast<const datatools::i_serializable&>(data);
    this->store<Data>(i_ser.get_serial_tag(), data);
  }

 protected:
  template <typename Data>
  void basic_store(const Data& data) {
    DT_THROW_IF (writer_ == 0,
                 std::logic_error,
                 "Not initialized!");
    writer_->store<Data>(data);
  }

 private:
  void init_writer(const std::string& filename, int mode);

  void reset_writer();

 private:
  io_writer *writer_;
};

} // end of namespace datatools

#endif // DATATOOLS_IO_FACTORY_H_

