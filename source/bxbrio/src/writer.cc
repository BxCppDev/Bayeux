/// writer.cc

// Ourselves:
#include <brio/writer.h>

// Standard Library:
#include <cstdlib>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
// - ROOT:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif
#include <TTree.h>
#include <TFile.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// - Bayeux/datatools:
#include <datatools/utils.h>

namespace brio {

  void writer::lock()
  {
    _locked_ = true;
    return;
  }

  void writer::unlock()
  {
    _locked_ = false;
    return;
  }

  bool writer::is_locked() const
  {
    return _locked_;
  }

  // void writer::_only_if_unlocked_(const std::string& where_) const {
  //   if (this->is_locked()) {
  //     std::ostringstream message;
  //     message << "brio::writer::__only_if_unlocked: ";
  //     if (!where_.empty()) message << where_ << ": ";
  //     message << "Operation prohibited; writer is locked !";
  //     throw std::logic_error(message.str());
  //   }
  // }

  // void writer::_only_if_locked_ (const std::string& where_) const {
  //   if (!this->is_locked()) {
  //     std::ostringstream message;
  //     message << "brio::writer::_only_if_locked_: ";
  //     if (!where_.empty()) message << where_ << ": ";
  //     message << "Operation prohibited; writer is locked !";
  //     throw std::logic_error(message.str());
  //   }
  // }

  bool writer::is_existing_file_protected() const
  {
    return _existing_file_protected_;
  }

  bool writer::is_allow_mixed_types_in_stores() const
  {
    return _allow_mixed_types_in_stores_;
  }

  bool writer::is_allow_automatic_store() const
  {
    return _allow_automatic_store_;
  }

  void writer::set_allow_mixed_types_in_stores(bool new_value_)
  {
    DT_THROW_IF(this->is_opened(),
                std::logic_error,
                "Operation prohibited; file is opened !");
    _allow_mixed_types_in_stores_ = new_value_;
    return;
  }

  void writer::set_allow_automatic_store(bool new_value_)
  {
    DT_THROW_IF(this->is_opened(),
                std::logic_error,
                "Operation prohibited; file is opened !");
    _allow_automatic_store_ = new_value_;
    return;
  }

  void writer::set_existing_file_protected(bool new_value_)
  {
    DT_THROW_IF(this->is_opened(),
                std::logic_error,
                "Operation prohibited; file is opened !");
    _existing_file_protected_ = new_value_;
    return;
  }

  void writer::_set_default()
  {
    _locked_ = false;
    _allow_mixed_types_in_stores_ = false;
    _allow_automatic_store_ = true;
    _existing_file_protected_ = false;
    _automatic_store_ = 0;
    return;
  }

  writer::writer() : detail::base_io(RW_WRITE)
  {
    writer::_set_default();
    return;
  }

  writer::writer(const std::string & filename_, datatools::logger::priority p_)
    : detail::base_io(RW_WRITE, p_)
  {
    writer::_set_default();
    std::string ext = boost::filesystem::extension(filename_);
    if (ext == store_info::constants::trio_file_extension()) {
      this->set_format(detail::base_io::text_label());
    } else {
      this->set_format(detail::base_io::pba_label());
    }
    this->open(filename_);
    return;
  }

  writer::writer(const std::string & filename_,
                 const std::string & format_str_,
                 datatools::logger::priority p_)
    : detail::base_io (RW_WRITE, p_)
  {
    writer::_set_default();
    this->set_format(format_str_);
    this->open(filename_);
    return;
  }

  writer::~writer()
  {
    if (this->is_opened()) this->close();
    this->detail::base_io::_reset();
    return;
  }

  void writer::print_info(std::ostream & out_) const
  {
    this->tree_dump(out_, "brio::writer:");
    return;
    return;
  }

  void writer::tree_dump(std::ostream & out_,
                         const std::string & title_,
                         const std::string & indent_,
                         bool inherit_) const
  {
    using namespace datatools;
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    detail::base_io::tree_dump(out_, title_, indent_, true);

    out_ <<  indent << i_tree_dumpable::tag
         << "Allow automatic store: " << _allow_automatic_store_
         << std::endl;

    if (_automatic_store_ != 0) {
      out_ <<  indent << i_tree_dumpable::tag
           << "Automatic store: '" << _automatic_store_->label << "'"
           << std::endl;
    }

    out_ <<  indent << i_tree_dumpable::tag
         << "Allow mixed types in stores: " << _allow_mixed_types_in_stores_
         << std::endl;

    out_ <<  indent << i_tree_dumpable::inherit_tag (inherit_)
         << "Locked: " << _locked_
         << std::endl;
    return;
  }

  int writer::add_store(const std::string& label_,
                        const std::string& serial_tag_,
                        size_t buffer_size_)
  {
    DT_THROW_IF(serial_tag_.empty(),
                std::logic_error,
                "Missing serialization tag for store with label '"
                << label_ << "' !");
    store_info *si = this->_add_store(label_, serial_tag_, buffer_size_);
    if (!si) {
      return 1;
    }
    return 0;
  }

  int writer::add_store(const std::string & label_, size_t buffer_size_)
  {
    store_info *si = this->_add_store(label_, store_info::constants::postponed_dedicated_serial_tag_label(), buffer_size_);
    if (si == 0) {
      return 1;
    }
    return 0;
  }

  int writer::add_mixed_store(const std::string & label_, size_t buffer_size_)
  {
    size_t buffer_size = buffer_size_;
    if (buffer_size == 0) {
      buffer_size = store_info::constants::default_store_buffer_size();
    }
    DT_THROW_IF(!_allow_mixed_types_in_stores_,
                std::logic_error,
                "Stores with mixed data types are not allowed !");
    store_info *si = this->_add_store(label_, store_info::constants::no_dedicated_serial_tag_label(), buffer_size);
    if (si == 0) {
      return 1;
    }
    return 0;
  }

  store_info* writer::_add_store(const std::string & label_,
                                 const std::string & serial_tag_,
                                 size_t buffer_size_)
  {
    DT_THROW_IF(!this->is_opened(),
                std::logic_error,
                "Operation prohibited; file for store label '" << label_
                << "' is not opened !");
    DT_THROW_IF(this->is_locked(),
                std::logic_error,
                "Operation prohibited; writer is locked !");
    DT_THROW_IF(label_.empty(), std::logic_error, "Empty label !");
    DT_THROW_IF(!_allow_automatic_store_
                && (label_ == store_info::constants::automatic_store_label()),
                std::logic_error,
                "Label '" << label_
                << "' for a conventional automatic store is not allowed !");
    DT_LOG_NOTICE(this->get_logging_priority(),
                  "Create a new store with label '" << label_ << "'...");
    const store_info *ptr_si = this->_get_store_info(label_);
    DT_THROW_IF(ptr_si != 0,
                std::logic_error,
                "Store with label '" << label_ << "' already exists !");
    // create a new store:
    {
      store_info si;
      // pointer within store_info::ctor must be NULL for
      // no copy constructor is provided.
      _store_infos[label_] = si;
    }
    store_info& the_si = _store_infos.find(label_)->second;
    the_si.label = label_;
    // store may have a dedicated serialization tag:
    the_si.serialization_tag = serial_tag_;
    the_si.bufsize = buffer_size_;
    the_si.number_of_entries = 0;
    the_si.current_entry = -1;

    std::ostringstream tree_title;
    tree_title << label_.c_str() << " tree for binary archive serialization";
    _file->cd();
    the_si.tree = new TTree(label_.c_str(), tree_title.str().c_str());
    // 2011-12-09 FM : Force affectation of the tree's current directory :
    the_si.tree->SetDirectory(_file);
    the_si.p_record = &the_si.record;
    int splitlevel = 0; // no branch-splitting of the streamed 'brio_record' instance
    const std::string* branch_name = &the_si.serialization_tag;
    if (the_si.serialization_tag.empty()
        || the_si.serialization_tag == store_info::constants::no_dedicated_serial_tag_label()) {
      branch_name = &store_info::constants::no_dedicated_serial_tag_label();
    }
    the_si.tree->Branch(branch_name->c_str(),
                        "brio_record",
                        &(the_si.p_record),
                        the_si.bufsize,
                        splitlevel);
    _current_store = &the_si;
    if (label_ == store_info::constants::automatic_store_label()) {
      _automatic_store_ = _current_store;
    }
    return _current_store;
  }

  void writer::_at_open (const std::string & filename_)
  {
    _filename = filename_;
    datatools::fetch_path_with_env(_filename);
    if (this->is_existing_file_protected()) {
      DT_THROW_IF(boost::filesystem::exists(_filename),
                  std::runtime_error,
                  "File '" << _filename << "' already exists !");
    }
    std::string default_extension = store_info::constants::default_file_extension();
    std::string extension = boost::filesystem::extension(_filename);
    DT_LOG_DEBUG(this->get_logging_priority(),
                 "Extension is `" << extension << "' !");
    std::string expected_extension = store_info::constants::default_file_extension();
    if (this->is_format_text()) {
      expected_extension = store_info::constants::trio_file_extension();
    }
    if (extension != expected_extension) {
      DT_LOG_WARNING(this->get_logging_priority(),
                     "Using extension different from `"
                     << expected_extension<< "' is not recommended !");
    }
    std::string mode = "RECREATE";
    _file = new TFile(_filename.c_str(), mode.c_str(), "ROOT file from the `brio' library");
    DT_THROW_IF(_file == 0,
                std::runtime_error,
                "Cannot open file '" << _filename << "' !");
    if (_file->IsOpen()) {
      DT_LOG_NOTICE(this->get_logging_priority(),
                    "File '" << _filename << "' has been successfully opened !");
    } else {
      DT_THROW_IF(true, std::runtime_error, "File '" << _filename << "' is not opened !");
    }
    _file->cd();
    return;
  }

} // end of namespace brio
