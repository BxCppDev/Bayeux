/// \file mctools/simulated_data_reader.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-06-22
 * Last modified: 2013-06-22
 *
 * License:
 *
 * Description:
 *   Simulation data reader.
 *
 */

#ifndef MCTOOLS_SIMULATED_DATA_READER_H
#define MCTOOLS_SIMULATED_DATA_READER_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost :
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools :
#include <datatools/logger.h>
#include <datatools/smart_filename.h>
#include <datatools/utils.h>

namespace datatools {
  class properties;
  class data_reader;
}

namespace brio {
  class reader;
}

namespace mctools {

  class simulated_data;

  /// Reader class for simulated data record
  class simulated_data_reader
    : public datatools::i_tree_dumpable
  {
  public:

    /// Loading errors
    enum load_status_type {
      LOAD_OK    = 0,
      LOAD_ERROR = 1,
      LOAD_FATAL = 2
    };

    /// Set the maximum number of files to be used
    void set_max_files (int max_files_);

    /// Set the maximum number of simulated data records to be used
    void set_max_record_total (int max_record_total_);

    /// Set the maximum number of simulated data records per file to be used
    void set_max_record_per_file (int max_record_per_file_);

    /// Get the maximum number of simulated data records per file to be used
    int get_max_record_per_file() const;

    /// Get the maximum number of simulated data records to be used
    int get_max_record_total() const;

   /// Get the maximum number of files to be used
    int get_max_files() const;

    /// Set the input filenames
    void set_filenames (const datatools::properties & setup_);

    /// Get logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set logging priority threshold
    void set_logging_priority(datatools::logger::priority logging_);

    /// Constructor
    simulated_data_reader(datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~simulated_data_reader() override;

    /// Check if reader is initialized
    bool is_initialized() const;

    /// Initialize the reader from an unique filename
    void initialize(const std::string & filename_);

    /// Initialize the reader from a collection of filenames
    void initialize(const std::vector<std::string> & filenames_);

    /// Initialize the reader from a set of incremental filenames
    void initialize(const std::string & path_,
                    const std::string & prefix_,
                    const std::string & extension_,
                    unsigned int stop_,
                    unsigned int start_ = 0,
                    int increment_ = 1);

    /// Initialize the reader from a list of parameters
    void initialize(const datatools::properties & setup_);

    /// Reset the reader
    void reset();

    /// Check if the reader is terminated
    bool is_terminated () const;

    /// Check if the reader has next record
    bool has_next() const;

    /// Load next reader
    int load_next(simulated_data & sd_);

    /// Check if run header exists
    bool has_run_header() const;

    /// Get run header
    const datatools::properties & get_run_header() const;

    /// Check if run footer exists
    bool has_run_footer() const;

    /// Get footer header
    const datatools::properties & get_run_footer() const;

    /// Check if the input file has changed after loading a simulated data record
    bool file_has_changed() const;

    /// Return the current input file index
    int get_file_index() const;

    /// Return the current input file record counter
    int get_file_record_counter() const;

    /// Return the record counter
    int get_record_counter() const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected:

    void _initialize();

    int _check_input();

    int _load(simulated_data & sd_);

    void _set_defaults();

  private:

    bool                        _initialized_; //!< Initialization flag
    datatools::logger::priority _logging_;     //!< Logging priority threshold
    datatools::smart_filename   _filenames_;   //!< Input filenames handler
    int _max_record_per_file_;                 //!< Maximum number of records per file
    int _max_record_total_;                    //!< Maximum number of records to be processed
    int _max_files_;                           //!< Maximum number of data files to be processed

    /* Running/dynamic attributes: */
    bool _terminated_;               //!< Termination flag
    int  _file_record_counter_;      //!< Event record counter in the current file
    int  _record_counter_;           //!< Total event record counter
    int  _file_index_;               //!< Index of the current datafile index
    bool _file_has_changed_;         //!< Flag a change of input file
    boost::scoped_ptr<datatools::data_reader> _bio_reader_;  //!< Datatools reader
    boost::scoped_ptr<brio::reader>           _brio_reader_; //!< Brio reader
    boost::scoped_ptr<datatools::properties>  _run_header_;
    boost::scoped_ptr<datatools::properties>  _run_footer_;
  };


} // end of namespace mctools

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::simulated_data_reader)

#endif // MCTOOLS_SIMULATED_DATA_READER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
