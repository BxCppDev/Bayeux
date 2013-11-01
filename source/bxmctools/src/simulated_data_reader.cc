// -*- mode: c++ ; -*-
/* simulated_data_reader.cc
 */

#include <mctools/simulated_data_reader.h>

#include <datatools/exception.h>
#include <datatools/io_factory.h>
#include <datatools/properties.h>

#include <brio/reader.h>

#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// Special serialization code:
#include <datatools/archives_instantiation.h>
#include <datatools/properties.ipp>
#include <mctools/simulated_data.ipp>

namespace mctools {

  void simulated_data_reader::set_max_files (int a_max_files)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Reader is already initialized !");
    DT_THROW_IF(a_max_files < 0,
                std::domain_error,
                "Invalid value '" << a_max_files << "' !");
    _max_files_ = a_max_files;
    return;
  }

  void simulated_data_reader::set_max_record_total (int a_max_record_total)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Reader is already initialized !");
    DT_THROW_IF(a_max_record_total < 0,
                std::domain_error,
                "Invalid value '" << a_max_record_total << "' !");
    _max_record_total_ = a_max_record_total;
    return;
  }

  void simulated_data_reader::set_max_record_per_file (int a_max_record_per_file)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Reader is already initialized !");
    DT_THROW_IF(a_max_record_per_file < 0,
                std::domain_error,
                "Invalid value '" << a_max_record_per_file << "' !");
    _max_record_per_file_ = a_max_record_per_file;
    return;
  }

  void simulated_data_reader::set_filenames (const datatools::properties & a_setup)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Reader is already initialized !");
    if (_filenames_.is_valid ()) {
      _filenames_.reset ();
    }
    _filenames_.initialize(a_setup);
    DT_THROW_IF(! _filenames_.is_valid (),
                std::logic_error,
                "Invalid list of filenames !");
    return;
  }

  bool simulated_data_reader::is_terminated () const
  {
    return _terminated_;
  }

  bool simulated_data_reader::file_has_changed() const
  {
    return _file_has_changed_;
  }

  int simulated_data_reader::get_max_record_per_file() const
  {
    return _max_record_per_file_;
  }

  int simulated_data_reader::get_max_record_total() const
  {
    return _max_record_total_;
  }

  int simulated_data_reader::get_max_files() const
  {
    return _max_files_;
  }

  int simulated_data_reader::get_file_index() const
  {
    return _file_index_;
  }

  int simulated_data_reader::get_file_record_counter() const
  {
    return _file_record_counter_;
  }

  int simulated_data_reader::get_record_counter() const
  {
    return _record_counter_;
  }

  void simulated_data_reader::_set_defaults ()
  {
    _max_record_per_file_ = 0;
    _max_record_total_ = 0;
    _max_files_ = -1;
    _terminated_ = false;
    _file_record_counter_ = 0;
    _record_counter_ = 0;
    _file_index_ = -1;
    _file_has_changed_ = false;
    if (_bio_reader_) _bio_reader_.reset(0);
    if (_brio_reader_) _brio_reader_.reset(0);
    return;
  }

  datatools::logger::priority simulated_data_reader::get_logging_priority() const
  {
    return _logging_;
  }

  void simulated_data_reader::set_logging_priority(datatools::logger::priority logging_)
  {
    _logging_ = logging_;
  }

  bool simulated_data_reader::has_run_header() const
  {
    return _run_header_;
  }

  const datatools::properties & simulated_data_reader::get_run_header() const
  {
    DT_THROW_IF(! has_run_header(), std::logic_error, "No available run header !");
    return *_run_header_.get();
  }

  bool simulated_data_reader::has_run_footer() const
  {
    return _run_footer_;
  }

  const datatools::properties & simulated_data_reader::get_run_footer() const
  {
    DT_THROW_IF(! has_run_footer(), std::logic_error, "No available run footer !");
    return *_run_footer_.get();
  }

  simulated_data_reader::simulated_data_reader(datatools::logger::priority logging_)
  {
    _initialized_ = false;
    _logging_ = logging_;
    _set_defaults();
  }

  /// Destructor
  simulated_data_reader::~simulated_data_reader()
  {
    if (is_initialized()) {
      reset();
    }
  }

  /// Check if reader is initialized
  bool simulated_data_reader::is_initialized() const
  {
    return _initialized_;
  }

  /// Initialize the reader from an unique filename
  void simulated_data_reader::initialize(const std::string & filename_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Reader is already initialized !");
    datatools::smart_filename::make_single(_filenames_, filename_);
    _initialize();
    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  /// Initialize the reader from a collection of filenames
  void simulated_data_reader::initialize(const std::vector<std::string> & filenames_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Reader is already initialized !");
    datatools::smart_filename::make_list(_filenames_);
    for (size_t i = 0; i < filenames_.size(); i++) {
      _filenames_.add_to_list(filenames_[i]);
    }
    _initialize();
    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  /// Initialize the reader from a set of incremental filenames
  void simulated_data_reader::initialize(const std::string & path_,
                                         const std::string & prefix_,
                                         const std::string & extension_,
                                         unsigned int stop_,
                                         unsigned int start_,
                                         int increment_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Reader is already initialized !");
    datatools::smart_filename::make_incremental(_filenames_,
                                                path_,
                                                prefix_,
                                                extension_,
                                                stop_,
                                                start_,
                                                increment_
                                                );
    _initialize();
    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  /// Initialize the reader from a list of parameters
  void simulated_data_reader::initialize(const datatools::properties & setup_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Reader is already initialized !");

    {
      datatools::logger::priority p
        = datatools::logger::extract_logging_configuration(setup_,
                                                           datatools::logger::PRIO_FATAL,
                                                           false);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority property !");
      _logging_ = p;
    }

    if (! _filenames_.is_valid ()) {
      datatools::properties input_filenames_setup;
      setup_.export_and_rename_starting_with(input_filenames_setup, "files.", "");
      set_filenames (input_filenames_setup);
    }

    if (setup_.has_key ("max_files")) {
      int the_max_files = setup_.fetch_integer ("max_files");
      DT_THROW_IF(the_max_files < 1,
                  std::logic_error,
                  "Invalid maximum number of files !");
      set_max_files (the_max_files);
    }

    if (setup_.has_key ("max_record_total")) {
      int the_max_record_total = setup_.fetch_integer ("max_record_total");
      DT_THROW_IF(the_max_record_total < 1,
                  std::logic_error,
                  "Invalid maximum total number of data records !");
      set_max_record_total (the_max_record_total);
    }

    if (setup_.has_key ("max_record_per_file")) {
      int the_max_record_per_file = setup_.fetch_integer ("max_record_per_file");
      DT_THROW_IF(the_max_record_per_file < 1,
                  std::logic_error,
                  "Invalid maximum number of data records per file !");
      set_max_record_per_file (the_max_record_per_file);
    }
    _initialize();
    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  void simulated_data_reader::_initialize()
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    _check_input();
    _initialized_ = true;
    if (_logging_ >= datatools::logger::PRIO_TRACE) {
      this->tree_dump(std::cerr, "Reader's status: ");
    }
    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
   }

  /// Reset the reader
  void simulated_data_reader::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Reader is not initialized !");
    _initialized_ = false;
    _run_header_.reset(0);
    _run_footer_.reset(0);
    if (_bio_reader_) _bio_reader_.reset(0);
    if (_brio_reader_) _brio_reader_.reset(0);
    _filenames_.reset();
    _set_defaults ();
    return;
  }

  /// Check if the reader has next record
  bool simulated_data_reader::has_next() const
  {
    DT_THROW_IF(!is_initialized(),
                std::logic_error,
                "Reader is not initialized !");
    return ! is_terminated();
  }

  /// Load next reader
  int simulated_data_reader::load_next(simulated_data & sd_)
  {
    DT_THROW_IF(!is_initialized(),
                std::logic_error,
                "Reader is not initialized !");
    DT_THROW_IF(is_terminated(),
                std::logic_error,
                "Reader is terminated !");
    return _load(sd_);
  }

  int simulated_data_reader::_check_input ()
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    if (_terminated_) {
      return 1;
    }
    _file_has_changed_ = false;
    // Attempt to open a source of event records :
    while (!_bio_reader_ && !_brio_reader_) {
      _file_record_counter_ = 0;
      _file_index_++;
      DT_LOG_TRACE(_logging_, "_file_index_ = " << _file_index_);
      if (_file_index_ >= (int)_filenames_.size ()) {
        DT_LOG_NOTICE (_logging_, "No more available input data file ! Reader is terminated.");
        _terminated_ = true;
        break;
      }
      std::string source_label = _filenames_[_file_index_];
      int mode_guess = 0;
      bool brio_format = false;
      if (brio::store_info::guess_mode_from_filename (source_label, mode_guess)
          == brio::store_info::SUCCESS) {
        brio_format = true;
      } else if (datatools::io_factory::guess_mode_from_filename (source_label, mode_guess)
                 == datatools::io_factory::SUCCESS) {
        brio_format = false;
      } else {
        DT_THROW_IF(true, std::logic_error,"Cannot guess mode for input data file '" << source_label << "' !");
      }
      if (brio_format) {
        // BRIO format :
        _run_header_.reset(0);
        _run_footer_.reset(0);
        _brio_reader_.reset(new brio::reader(source_label));
        _file_has_changed_ = true;
        // Load optional run header and footer:
        if (_brio_reader_.get()->has_store_with_serial_tag(io_utils::GENERAL_INFO_STORE,
                                                           datatools::properties::SERIAL_TAG)) {
          _brio_reader_.get()->select_store(io_utils::GENERAL_INFO_STORE);
          if (_brio_reader_.get()->has_next()) {
            _run_header_.reset(new datatools::properties);
            DT_LOG_NOTICE(_logging_, "Loading run header from BRIO file '" << source_label << "'...");
            _brio_reader_.get()->load(*_run_header_.get());
          }
          // if (_brio_reader_.get()->has_next()) {
          //   _run_footer_.reset(new datatools::properties);
          //   DT_LOG_NOTICE(_logging_, "Loading run footer from BRIO file '" << source_label << "'...");
          //   _brio_reader_.get()->load(*_run_footer_.get());
          // }
        }
        // Check for simulated data input:
        if (! _brio_reader_.get()->has_store_with_serial_tag(io_utils::PLAIN_SIMULATED_DATA_STORE,
                                                             simulated_data::SERIAL_TAG)) {
          DT_THROW_IF(true, std::logic_error, "Missing '" << io_utils::PLAIN_SIMULATED_DATA_STORE
                      << "' store from input BRIO file '" << source_label << "' !");
        }
        _brio_reader_.get()->select_store(io_utils::PLAIN_SIMULATED_DATA_STORE);
        if (! _brio_reader_.get()->has_next ()) {
          DT_LOG_TRACE(_logging_, "No next simulated data record.");
          _brio_reader_.reset (0);
        }
      } else {
        // BIO format :
        _run_header_.reset(0);
        _run_footer_.reset(0);
        _bio_reader_.reset(new datatools::data_reader(source_label, datatools::using_multi_archives));
        _file_has_changed_ = true;
        if (_bio_reader_.get()->has_record_tag()) {
          DT_LOG_NOTICE(_logging_, "BIO record tag is '" << _bio_reader_.get()->get_record_tag() << "'");
          if (_bio_reader_.get()->record_tag_is(datatools::properties::SERIAL_TAG)) {
            _run_header_.reset(new datatools::properties);
            DT_LOG_NOTICE(_logging_, "Loading run header from BIO file '" << source_label << "'...");
            datatools::properties & rh = *_run_header_.get();
            _bio_reader_.get()->load(rh);
            if (_logging_ >= datatools::logger::PRIO_NOTICE) {
              rh.tree_dump(std::cerr, "Run header:");
            }
          }
        }
        if (! _bio_reader_.get()->has_record_tag()) {
          DT_LOG_TRACE(_logging_, "No next simulated data record.");
          _bio_reader_.reset (0);
        } else {
          if (_bio_reader_.get()->record_tag_is(datatools::properties::SERIAL_TAG)) {
            _run_footer_.reset(new datatools::properties);
            DT_LOG_NOTICE(_logging_, "Loading run footer from BIO file '" << source_label << "'...");
            datatools::properties & rf = *_run_footer_.get();
            _bio_reader_.get()->load(rf);
            _bio_reader_.reset (0);
            DT_LOG_NOTICE(_logging_, "End of BIO file '" << source_label << "'.");
          } else if (! _bio_reader_.get()->record_tag_is(simulated_data::SERIAL_TAG)) {
            DT_THROW_IF(true, std::logic_error, "Unrecognized serial tag '" << _bio_reader_.get()->get_record_tag()
                        << "' from input BIO file '" << source_label << "' !");
          }
        }
      }
    } // while.
    if (! _terminated_) {
      DT_LOG_TRACE(_logging_, "Reader is open.");
    }
    DT_LOG_TRACE(_logging_, "Exiting.");
    return 0;
  }

  int simulated_data_reader::_load (simulated_data & sd_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_LOG_TRACE(_logging_, "_filenames_.size () = " << _filenames_.size ());
    if (_terminated_) {
      return 1;
    }

    std::string source_label = _filenames_[_file_index_];

    // Force loading of the current event record :
    bool load_it = LOAD_ERROR;
    // Load action :
    if (load_it) {
      sd_.clear ();
      if (_brio_reader_) {
        DT_THROW_IF(! _brio_reader_.get()->has_next (),
                    std::logic_error,
                    "No more available simulated data record from BRIO input file '" << source_label << "' ! !");
        _brio_reader_.get()->load_next (sd_);
      } else if (_bio_reader_) {
        DT_THROW_IF(! _bio_reader_.get()->has_record_tag (),
                    std::logic_error,
                    "No more available simulated data record from BIO input file '" << source_label << "' !");
        DT_LOG_TRACE(_logging_, "BIO record tag is '" << _bio_reader_.get()->get_record_tag() << "'");
        if (_bio_reader_.get()->record_tag_is(datatools::properties::SERIAL_TAG)) {
          // Load run header/footer :
          if (! _run_header_) {
            _run_header_.reset(new datatools::properties);
            DT_LOG_TRACE(_logging_, "Loading run header from BIO file '" << source_label << "'...");
            _bio_reader_.get()->load(*_run_header_.get());
          } else if (! _run_footer_) {
            _run_footer_.reset(new datatools::properties);
            DT_LOG_TRACE(_logging_, "Loading run footer from BIO file '" << source_label << "'...");
            _bio_reader_.get()->load(*_run_footer_.get());
          } else {
            DT_THROW_IF(true, std::logic_error, "Run header and footer have already been loaded ! "
                        << "Probable bad input BIO file '" << source_label << "' !");
          }
        }  else if (_bio_reader_.get()->record_tag_is(simulated_data::SERIAL_TAG)) {
          // Load simulated data :
          DT_LOG_TRACE(_logging_, "Loading simulated data from BIO file '" << source_label << "'...");
          _bio_reader_.get()->load(sd_);
          DT_LOG_TRACE(_logging_, "Simulated data has been loaded from BIO file '" << source_label << "'.");
        }  else {
          // Error:
          DT_THROW_IF(true, std::logic_error, "Cannot find any known serial tag ! "
                      << "Probable bad input BIO file '" << source_label << "' !");
        }
      }
      _file_record_counter_++;
      _record_counter_++;
    } // load_it

    DT_LOG_TRACE(_logging_, "Checking reader limits...");

    bool stop_file  = false;
    bool stop_input = false;
    if (_max_record_total_ > 0) {
      if (_record_counter_ >= _max_record_total_) {
        stop_input = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging_,
                      "Reader has reached the maximum number "
                      << "of simulated data records from the input data source (" << _max_record_total_ << ") !");
      }
    }
    if (_max_record_per_file_ > 0) {
      if (_file_record_counter_ >= _max_record_per_file_) {
        stop_file = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging_,
                      "Reader has reached the maximum number "
                      << "of simulated data records from the current input file (" << _max_record_per_file_ << ") !");
      }
    }
    // Check the end of the input file :
    if (_brio_reader_) {
      if (! _brio_reader_.get()->has_next ()) {
        stop_file = true;
      }
    } else if (_bio_reader_) {
      if (! _bio_reader_.get()->has_record_tag ()) {
        stop_file = true;
      } else if (_bio_reader_.get()->record_tag_is(datatools::properties::SERIAL_TAG)) {
        _run_footer_.reset(new datatools::properties);
        DT_LOG_TRACE(_logging_, "Loading run footer from BIO file '" << source_label << "'...");
        _bio_reader_.get()->load(*_run_footer_.get());
        stop_file = true;
      }
    }

    // Manage the end of the input file :
    if (stop_file) {
      DT_LOG_TRACE(_logging_, "This is the end of the current input file '" << source_label << "'...");
      if (_brio_reader_) {
        _brio_reader_.reset(0);
      } else if (_bio_reader_) {
        _bio_reader_.reset(0);
      }
      _file_record_counter_ = 0;
      int effective_max_files = _max_files_;
      if (_max_files_ < 0) {
        if (_filenames_.is_ranged()) {
          effective_max_files = _filenames_.size ();
          DT_LOG_TRACE(_logging_, "effective_max_files = " << effective_max_files);
        }
      }
      if (effective_max_files > 0) {
        if ((_file_index_ + 1) >= effective_max_files) {
          stop_input = true;
          DT_LOG_NOTICE(_logging_,
                        "Reader has reached the requested maximum number of input file(s) ("
                        << effective_max_files << ") !");
        }
      }
      if ((_file_index_ + 1) >= (int)_filenames_.size ()) {
        stop_input = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging_,
                      "Reader has loaded the last available input file (total is "
                      << _filenames_.size () << " file" << (_filenames_.size ()>1?"":"s") << ")!");
      }
    }
    if (stop_input) {
      _terminated_ = true;
    }
    if (! _terminated_) {
      _check_input();
    }
    DT_LOG_TRACE(_logging_, "Reader limits checked. Terminated = " << _terminated_);
    DT_LOG_TRACE(_logging_, "Exiting.");
    return LOAD_OK;
  }

  void simulated_data_reader::tree_dump (std::ostream & a_out ,
                                         const std::string & a_title,
                                         const std::string & a_indent,
                                         bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty ()) {
      indent = a_indent;
    }
    if ( !a_title.empty () ) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Initialized           : " << _initialized_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max record per file   : " << _max_record_per_file_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max record total      : " << _max_record_total_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max files             : " << _max_files_ << std::endl;

    if (_filenames_.is_initialized()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Number of filenames   : " << _filenames_.size () << std::endl;
    }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Terminated            : " << _terminated_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current file record counter   : " << _file_record_counter_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current record counter        : " << _record_counter_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current file index            : " << _file_index_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current file has changed      : " << _file_has_changed_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Current Source                : ";
    if (_bio_reader_) {
      a_out << "BIO reader (@ " << _bio_reader_.get() << ")";
    } else if (_brio_reader_) {
      a_out << "BRIO reader (@ " << _brio_reader_.get() << ")";
    } else {
      a_out << "<none>";
    }
    a_out << std::endl;

    return;
  }

} // end of namespace mctools


// OCD support for class 'mctools::simulated_data_reader' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::simulated_data_reader,ocd_)
{
  ocd_.set_class_name ("mctools::simulated_data_reader");
  ocd_.set_class_library ("mctools");
  ocd_.set_class_description ("A reader to load Monte-Carlo events from a set of data files produced by the ``g4_production`` executable.");


  ocd_.set_configuration_hints ("A ``mctools::simulated_data_reader`` object can be setup with the    \n"
                                "following syntax in a ``datatools::properties`` configuration        \n"
                                "file.                                                                \n"
                                "                                                                     \n"
                                "Example::                                                            \n"
                                "                                                                     \n"
                                "  #@config A simulated data reader                                   \n"
                                "                                                                     \n"
                                "  #@description The logging priority                                 \n"
                                "  logging.priority : string = \"fatal\"                              \n"
                                "                                                                     \n"
                                "  #@description Input max number of files                            \n"
                                "  max_files : integer =  10                                          \n"
                                "                                                                     \n"
                                "  #@description Input max number of records                          \n"
                                "  max_record_total : integer =  100000                               \n"
                                "                                                                     \n"
                                "  #@description Input max number of records per file                 \n"
                                "  max_record_per_file : integer =  10000                             \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  files.mode : string = \"single\"                                   \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  files.single.filename : string = \"sd.xml\"                        \n"
                                "                                                                     \n"
                                "or::                                                                 \n"
                                "                                                                     \n"
                                "  #@description Input file mode                                      \n"
                                "  files.mode : string = \"list\"                                     \n"
                                "                                                                     \n"
                                "  #@description List of input files                                  \n"
                                "  files.list.filenames : string[2] = \"sd0.xml\" \"sd1.xml\"         \n"
                                "                                                                     \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::simulated_data_reader,"mctools::simulated_data_reader")


// end of simulated_data_reader.cc
