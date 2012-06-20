// -*- mode: C++; -*-
/* io_factory.h */
/*
 * Description :
 *
 *  Some generic reader and writer based on Boost archives.
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_SERIALIZATION_IO_FACTORY_H_
#define DATATOOLS_SERIALIZATION_IO_FACTORY_H_

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
#include <boost/tokenizer.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/serialization/string.hpp>

// Datatools
#include <datatools/serialization/archives_list.h>
#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_tree_dump.h>


namespace datatools {
namespace serialization {

class io_factory : public datatools::utils::i_tree_dumpable {
 public:
  static bool g_debug;
  static bool g_warning;
  static const int SUCCESS = 0;
  static const int ERROR   = 1;

  static const int MASK_RW           = 0x1;
  static const int MASK_FORMAT       = 0xE;
  static const int MASK_COMPRESSION  = 0x30;
  static const int MASK_MULTIARCHIVE = 0x80;
  static const int MASK_APPEND       = 0x100;

  enum mode {
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

  static const std::string TXT_EXT;
  static const std::string XML_EXT;
  static const std::string BIN_EXT;
  static const std::string GZ_EXT;
  static const std::string BZIP2_EXT;

 public:
  static int guess_mode_from_filename (const std::string & a_filename, int & a_mode);


 public: 
  // ctor
  io_factory(int a_mode = io_factory::MODE_DEFAULT);

  io_factory(const std::string & a_stream_name,
             int a_mode = io_factory::MODE_DEFAULT);

  // dtor
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
  int _init_read_archive_();

  int _init_read_(const std::string& a_stream_nam);

  int _reset_read_archive_();

  int _reset_read_();

  int _init_write_archive_();

  int _init_write_(const std::string& a_stream_name);

  int _reset_write_archive_();

  int _reset_write_();

  int _init_(const std::string& a_stream_name, int a_mode);

  void _ctor_defaults_();

  int _reset_();

 private:
  template <typename Data>
  void _store_text_(boost::archive::text_oarchive& a_ar, 
                    const Data& a_data) {
    const Data & b = a_data;
    a_ar << b;
  }


  template <typename Data>
  void _store_xml_(boost::archive::xml_oarchive& a_ar,
                   const Data& a_data) {
    const Data & b = a_data;
    a_ar << boost::serialization::make_nvp("record", b);
  }


  template <typename Data>
  void _store_binary_(eos::portable_oarchive& a_ar,
                      const Data& a_data) {
    const Data & b = a_data;
    a_ar << b;
  }

  template <typename Data>
  void _load_text_(boost::archive::text_iarchive& a_ar,
                   Data& a_data) {
    Data& b = a_data;
    a_ar >> b;
  }

  template <typename Data>
  void _load_xml_(boost::archive::xml_iarchive& a_ar,
                  Data& a_data) {
    Data& b = a_data;
    a_ar >> boost::serialization::make_nvp("record", b);
  }

  template <typename Data>
  void _load_binary_(eos::portable_iarchive& a_ar,
                     Data & a_data) {
    Data& b = a_data;
    a_ar >> b;
  }

 public:
  template <typename Data>
  void store(const Data& a_data) {
    if (!this->is_write()) {
      throw std::logic_error("io_factory::store: Not a writer factory!");
    }

    if (_otar_ptr_ != 0) {
      this->_store_text_<Data>(*_otar_ptr_, a_data);
    } else if (_oxar_ptr_ != 0) {
      this->_store_xml_<Data>(*_oxar_ptr_, a_data);
    } else if (_obar_ptr_ != 0) {
      this->_store_binary_<Data>(*_obar_ptr_, a_data);
    }
  }


  template <typename Data>
  void load(Data& a_data) {
    if (!this->is_read()) {
      throw std::logic_error("io_factory::load: not a reader factory!");
    }

    if (_in_fs_ == 0) {
      throw std::logic_error ("io_factory::load: no input stream!");
    }

    // 2008-10-03 FM: add EOF check code
    if (*_in_fs_) {
      // 2009-03-08 FM: Is it possible to have better EOF check code
      // here for unzipped text and XML archives?
      char c = 0;
      _in_fs_->get(c);
     
      if (_in_fs_) {
        _in_fs_->putback(c);
      }
    } 

    if (! *_in_fs_) {
      if (_in_fs_->eof()) {
        throw std::logic_error("io_factory::load: input stream at EOF!");
      }

      if (_in_fs_->fail()) {
        throw std::logic_error("io_factory::load: input stream in fail status!");
      }

      if (_in_fs_->bad()) {
        throw std::logic_error("io_factory::load: input stream in bad status!");
      }
    }
     
    try {
      if (_itar_ptr_ != 0) {
        this->_load_text_<Data>(*_itar_ptr_, a_data);
        *_in_fs_ >> std::ws;
      } else if (_ixar_ptr_ != 0) {
        this->_load_xml_<Data>(*_ixar_ptr_, a_data);
        *_in_fs_ >> std::ws;
      } else if (_ibar_ptr_ != 0) {
        this->_load_binary_<Data>(*_ibar_ptr_, a_data);
      }
    }
    // 2011-02-25 FM:
    catch (boost::archive::archive_exception& x) {
      if (io_factory::g_warning) {
        std::clog << "WARNING: io_factory::load: archive exception is: "
                  << x.what() 
                  << std::endl;
      }
      throw x;
    }
    catch (std::exception& x) {
      if (io_factory::g_warning) {
        std::cerr << "WARNING: io_factory::load: "
                  << "cannot load data from archive: "
                  << x.what() 
                  << "!"
                  << std::endl;
      }
      throw x;
    }
    catch (...) {
      std::cerr << "WARNING: io_factory::load: "
                << "cannot load data from archive: "
                << "unexpected exception" << "!"
                << std::endl;
      throw std::logic_error("io_factory::load: internal exception!");
    }

    if (! *_in_fs_) {
      if (_in_fs_->fail()) {
        std::cerr << "WARNING: io_factory::load: "
                  << "input stream is now in fail status!"
                  << std::endl;
      }

      if (_in_fs_->eof()) {
        std::cerr << "WARNING: io_factory::load: "
                  << "input stream is now in EOF status!"
                  << std::endl;
      }

      if (_in_fs_->bad()) {
        std::cerr << "WARNING: io_factory::load: "
                  << "input stream is now in bad status!"
                  << std::endl;
      }
    }
  }


 public:
  template <typename Data>
  friend io_factory& operator<<(io_factory& iof, const Data& a_data) {
    iof.store(a_data);
    return iof;
  }


  template <typename Data>
  friend io_factory & operator>>(io_factory& iof, Data& a_data) {
    iof.load(a_data);
    return iof;
  }


  virtual void tree_dump (std::ostream& out = std::cerr,
                          const std::string& title = "",
                          const std::string& indent = "",
                          bool inherit = false) const;

  void dump(std::ostream& out) const;


 private:
  int            _mode_;
  std::istream * _in_;
  std::ostream * _out_;

  boost::iostreams::filtering_istream * _in_fs_;
  boost::iostreams::filtering_ostream * _out_fs_;
  std::ifstream  * _fin_;
  std::ofstream  * _fout_;
  std::locale    * _default_locale_;
  std::locale    * _locale_;

  bool                                _read_archive_is_initialized_;
  bool                                _write_archive_is_initialized_;
  boost::archive::text_iarchive     * _itar_ptr_;
  boost::archive::text_oarchive     * _otar_ptr_;
  boost::archive::xml_iarchive      * _ixar_ptr_;
  boost::archive::xml_oarchive      * _oxar_ptr_;
  //boost::archive::portable_binary_iarchive * _ibar_ptr_;
  //boost::archive::portable_binary_oarchive * _obar_ptr_;
  eos::portable_iarchive * _ibar_ptr_;
  eos::portable_oarchive * _obar_ptr_;
}; // end of class io_factory ?


//----------------------------------------------------------------------
// Reader factory tag type
//
class io_reader : public io_factory {
 public:
  io_reader(int mode_ = io_factory::MODE_DEFAULT);

  io_reader(const std::string & stream_name_ ,
            int mode_ = io_factory::MODE_DEFAULT);

  virtual ~io_reader();
};


//----------------------------------------------------------------------
// Writer factory tag type
//
class io_writer : public io_factory {
 public:
  io_writer(int mode_ = io_factory::MODE_DEFAULT);
  io_writer(const std::string & stream_name_ ,
            int mode_ = io_factory::MODE_DEFAULT);

  virtual ~io_writer ();
};

//----------------------------------------------------------------------
// user friendly constants used in ctors
// for data_reader/data_writer:
static const bool using_multi_archives = true;
static const bool using_multiple_archives = using_multi_archives;
static const bool using_single_archive = false;
static const bool append_mode          = true;
static const bool no_append_mode       = false;

//----------------------------------------------------------------------
// data_reader class
//
class data_reader {
 public:
  enum status_t {
    STATUS_OK    = 0,
    STATUS_ERROR = 1
  };

  static const std::string EMPTY_RECORD_TAG;

 public:
  // ctor
  data_reader();

  data_reader(const std::string& a_filename,
              bool a_multiple_archives = using_single_archive);

  data_reader(const std::string& a_filename, int a_mode);

  // dtor
  virtual ~data_reader();

 public:
  bool is_error() const;

  const std::string& get_record_tag() const;

  bool has_record_tag() const;

  bool record_tag_is(const std::string& a_tag) const;

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

  void init(const std::string& a_filename,
            bool a_multiple_archives = using_single_archive);

  void init_multi(const std::string& a_filename) {
    this->init(a_filename, using_multiple_archives);
  }

  void init_single(const std::string& a_filename) {
    this->init(a_filename, using_single_archive);
  }

  void init(const std::string& a_filename, int a_mode);

  void dump(std::ostream& a_out = std::clog) const;

 public:
  template <typename Data>
  void load(const std::string& a_tag, Data& a_data) {
    if (!this->has_record_tag()) {
      throw std::logic_error("data_reader::load(...): no more record tag!");
    }

    if (this->get_record_tag() != a_tag) {
      std::ostringstream message;
      message << "data_reader::load(...): unexpected tag ('"
              << get_record_tag ()
              << " != " << a_tag << "')!" ;
      throw std::logic_error(message.str());
    }

    this->_basic_load(a_data);

    if (_reader_->is_multi_archives()) _reader_->stop_archive();

    _read_next_tag_();
  }


  template <typename Data>
  void load(Data& a_data) {
    // Huh? If it's a reference then just try get_serial_tag method??
    datatools::serialization::i_serializable& i_ser = static_cast<datatools::serialization::i_serializable&>(a_data);

    this->load(i_ser.get_serial_tag(), a_data);
  }


 protected:
  template <typename Data>
  void _basic_load(Data& a_data) {
    if (_reader_ == 0) {
      throw std::logic_error("data_reader::_basic_load(...): not initialized!");
    }

    try {
      _reader_->load(a_data);
    }
    catch (std::exception& x) {
      bool warn = io_factory::g_warning;

      //>>> 2008-11-13 FM: skip EOF message printing
      std::string msg = x.what();
      if (msg.find("EOF") != msg.npos) {
        warn = false;
      }
      if (warn) {
        std::cerr << "WARNING: data_reader::_basic_load(...): "
                  << "cannot read data: exception="
                  << x.what () << " !"
                  << std::endl;
      }
      //<<<

      _status_   = STATUS_ERROR;
      _next_tag_ = EMPTY_RECORD_TAG;
      throw std::logic_error(x.what());
    }
    catch (...) {
      std::cerr << "WARNING: data_reader::_basic_load(...): "
                << "cannot read data: "
                << "unexpected exception" << '!'
                << std::endl;

      _status_   = STATUS_ERROR;
      _next_tag_ = EMPTY_RECORD_TAG;
      throw std::logic_error("data_reader::_basic_load: unexpected error!");
    }
  }


 private:
  void _read_next_tag_();

  void _init_reader_(const std::string& a_filename, int a_mode);

  void _reset_reader_();

 private:
  int         _status_;
  io_reader * _reader_;
  std::string _next_tag_;
};


//----------------------------------------------------------------------
// data_writer class
//
class data_writer {
 public:
  // ctor
  data_writer();

  data_writer(const std::string & a_filename,
              bool a_multiple_archives = using_single_archive,
              bool a_append_mode = no_append_mode);

  data_writer(const std::string & a_filename,
              int a_mode);

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

  void init(const std::string& a_filename,
            bool a_multiple_archives = using_single_archive,
            bool a_append_mode = no_append_mode);

  void init_multi(const std::string& a_filename);

  void init_single(const std::string& a_filename);

  void init(const std::string& a_filename, int a_mode);

 public:
  template <typename Data>
  void store(const std::string& a_tag, const Data& a_data) {
    if (_writer_->is_multi_archives()) {
      _writer_->start_archive();
    }

    this->_basic_store<std::string>(a_tag);
    this->_basic_store<Data>(a_data);
  
    if (_writer_->is_multi_archives()) {
      _writer_->stop_archive();
    }
  }

  template <typename Data>
  void store(const Data& a_data) {
    const datatools::serialization::i_serializable& i_ser =
      static_cast<const datatools::serialization::i_serializable&>(a_data);
    this->store<Data>(i_ser.get_serial_tag(), a_data);
  }

 protected:
  template <typename Data>
  void _basic_store(const Data& a_data) {
    if (_writer_ == 0) {
      throw std::logic_error("data_writer::_basic_store(...): not initialized!");
    }
  
    _writer_->store<Data>(a_data);
  }

 private:
  void _init_writer_ (const std::string & a_filename, int a_mode);

  void _reset_writer_ ();

 private:
  io_writer* _writer_;
};

} // end of namespace serialization
} // end of namespace datatools

#endif // DATATOOLS_SERIALIZATION_IO_FACTORY_H_

