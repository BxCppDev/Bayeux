/// \file brio/writer.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2019-05-21
 *
 * License: GPL3
 *
 * Description:
 *
 *   A Boost archive to ROOT file writer
 *
 */

#ifndef BRIO_WRITER_H
#define BRIO_WRITER_H 1

// Standard Library:
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third Party:
// - Boost:
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/filesystem.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/archive/text_oarchive.hpp>
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

  //! \brief The brio generic writer class
  class writer
    : public detail::base_io
  {
  public:
    //! Default constructor
    writer();

    //! Constructor
    writer(const std::string & filename_,
           datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Constructor
    writer(const std::string & filename_,
           const std::string & format_str_,
           datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Destructor
    ~writer() override;

    //! Lock the writer
    void lock();

    //! Unlock the writer
    void unlock();

    //! Check if the writer is locked
    bool is_locked() const;

    //! Check if mixed types are allowed in any given store
    bool is_allow_mixed_types_in_stores() const;

    //! Check if automatic store is allowed
    bool is_allow_automatic_store() const;

    //! Check if protection against file overwriting exists
    bool is_existing_file_protected() const;

    /** Allow to serialize different types of objects within stores.
     *  Usually, each store is setup in order to accept only one
     *  kind of serializable object (potato) , in such a way one cannot mix
     *  several object's types (ex: potato and carot) within the same branch
     *  Default at construction is false.
     */
    void set_allow_mixed_types_in_stores(bool new_value_ = true);

    /** Enables the use of some automatic store (with
     *  standard label == store_info::AUTOMATIC_STORE_LABEL)
     *  within the writer.
     *  Default at construction is true.
     */
    void set_allow_automatic_store(bool new_value_ = true);

    //! Set the protection against file overwriting
    void set_existing_file_protected(bool new_value_ = true);

    /** Add a new store with label 'label_'
     *  to store objects with a dedicated serialization tag 'serial_tag_'
     */
    int add_store(const std::string & label_,
                  const std::string & serial_tag_,
                  size_t buffer_size_ = 256000);

    /** Add a new store with label 'label_'
     *  to store objects with a dedicated serialization tag 'serial_tag_'
     */
    int add_store(const std::string & label_,
                  size_t buffer_size_ = 256000);

    /** Add a new store with label 'label_'
     *  to store objects with arbitrary serialization tags
     */
    int add_mixed_store(const std::string & label_,
                        size_t buffer_size_ = 0);

    // Store template method
    template <typename T>
    int store(const T & data_, const std::string & label_ = "");

    //! Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    //! Print
    void print_info(std::ostream & out_ = std::clog) const;

  protected:

    void _set_default();

    store_info * _add_store(const std::string & label_,
                            const std::string & serial_tag_,
                            size_t buffer_size_);

    template <typename T>
    int _at_store(const T & dat, store_info * store_info_);

    void _at_open(const std::string & filename_) override;

  private:

    bool _locked_;                      ///< Flag to lock the writer storage structure
    bool _allow_mixed_types_in_stores_; ///< Flag to allow stores with mixed types
    bool _allow_automatic_store_;       ///< Flag to allow an default automatic store
    bool _existing_file_protected_;     ///< Flag to protect existing output data file
    store_info * _automatic_store_ = nullptr; ///< A handle to the automatic store (if any)

  };

} // end of namespace brio

#include <brio/writer-inl.h>

#endif // BRIO_WRITER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
