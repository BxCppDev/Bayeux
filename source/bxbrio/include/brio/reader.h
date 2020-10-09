/// \file brio/reader.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2019-05-21
 *
 * License: GPL3
 *
 * Description:
 *
 *   A Boost archive from ROOT file reader
 *
 */

#ifndef BRIO_READER_H
#define BRIO_READER_H 1

// Standard Library:
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

// Third Party:
// - Boost:
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

// Supress known unused parameter warning from Boost's smart_cast header
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/archive/text_iarchive.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// - Bayeux/datatools:
#include <datatools/portable_archives_support.h>
#include <datatools/logger.h>
#include <datatools/exception.h>

// This Project:
#include <brio/detail/base_io.h>

namespace brio {
  
  //! \brief The brio generic reader class
  class reader
    : public detail::base_io
  {
  public:
    //! Default constructor
    reader();

    //! Constructor
    reader(const std::string & filename_,
           datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Constructor
    reader(const std::string & filename_,
           const std::string & format_str_,
           datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Destructor
    ~reader() override;

    /** By default, serialization tag are checked each time
     *  an object is deserialized.
     *  This behaviour can be changed:
     *
     *  brio::reader a_reader;
     *  a_reader.open("file.root");
     *  a_reader.set_check_serial_tag(false);
     *
     */
    void set_check_serial_tag(bool new_value_ = true);

    bool is_check_serial_tag() const;

    /** Position current entry of store 'label' just before the first
     *  serialized object
     */
    void rewind_store(const std::string & label_ = "");

    /** Position current entry of store 'label' just after the last
     *  serialized object
     */
    void unwind_store(const std::string& label_ = "");

    /** Check if some previous entry exists relatively to the current entry
     */
    bool has_previous(const std::string & label_ = "") const;

    /** Check if some next entry exists relatively to the current entry
     */
    bool has_next(const std::string & label_ = "") const;

    //! Load template method for next entry
    template<typename T>
    int load_next(T & data_, const std::string & label_ = "");

    //! Load template method for previous entry
    template<typename T>
    int load_previous(T & data_, const std::string & label_ = "");

    //! Load template method for arbitrary entry
    template<typename T>
    int load(T & data_, int64_t nentry_ = -1);

    //! Load template method for arbitrary store and entry
    template<typename T>
    int load(T & data_, const std::string & label_, int64_t nentry_ = -1);

    //! Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    //! Print
    void print_info(std::ostream& out_ = std::clog) const;

  protected:
    void _at_open(const std::string & filename_) override;

    template<class T>
    int _at_load(T & data_, store_info * ptr_si_, int64_t nentry_);

    void _set_default();

  private:
    
    //! Return pointer to store with name label. Throw if file is not
    //! open or store does not exist
    const store_info * get_store_or_throw(const std::string & label_) const;
    store_info * get_store_or_throw(const std::string & label_);

  private:
    bool _allow_mixed_types_in_stores_; ///< Flag to allow stores with mixed types
    bool _allow_automatic_store_;       ///< Flag to allow an default automatic store
    bool _check_serial_tag_;            ///< Flag to automatically check coherence between the store's serialization tag and the stored objects serialization tag
    store_info * _automatic_store_ = nullptr; ///< A handle to the automatic store (if any)
  };
  
} // end of namespace brio

#include <brio/reader-inl.h>

#endif // BRIO_READER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
