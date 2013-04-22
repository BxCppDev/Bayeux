// -*- mode: c++ ; -*-
/* reader.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *
 *   A Boost archive from ROOT file reader
 *
 * History:
 *
 */

#ifndef BRIO_READER_H_
#define BRIO_READER_H_ 1

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <datatools/eos/portable_iarchive.hpp>

#include <brio/detail/base_io.h>

#include <TTree.h>

namespace brio {

  //! \brief The brio generic reader class
  class reader : public detail::base_io
  {

  public:

    // setters:

    /** By default, serialization tag are checked each time
     *  an object is deserialized.
     *  This behaviour can be changed:
     *
     *  brio::reader a_reader;
     *  a_reader.open ("file.root");
     *  a_reader.set_check_serial_tag (false);
     *
     */
    void set_check_serial_tag (bool new_value_ = true);

    bool is_check_serial_tag () const;

    /** Position current entry of store 'label' just before the first
     *  serialized object
     */
    void rewind_store (const std::string & label_ = "");

    /** Position current entry of store 'label' just after the last
     *  serialized object
     */
    void unwind_store (const std::string & label_ = "");

    /** Check if some previous entry exists relatively to the current entry
     */
    bool has_previous (const std::string & label_ = "") const;

    /** Check if some next entry exists relatively to the current entry
     */
    bool has_next (const std::string & label_ = "") const;

  protected:

    void _set_default ();

  public:

    //! Default constructor
    reader ();

    //! Constructor
    reader (const std::string & filename_,
            bool verbose_ = false,
            bool debug_ = false);

    //! Constructor
    reader (const std::string & filename_,
            const std::string & format_str_,
            bool verbose_ = false,
            bool debug_ = false);

    //! Destructor
    virtual ~reader ();

    //! Smart print
    virtual void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;

    //! Print
    void print_info (std::ostream & out_ = std::clog) const;

    //! Load template method for next entry
    template<class T>
    int load_next (T & data_,
                   const std::string & label_ = "")
    {
      int64_t entry = get_current_entry (label_);
      return load<T> (data_, label_, entry + 1);
    }

    //! Load template method for previous entry
    template<class T>
    int load_previous (T & data_,
                       const std::string & label_ = "")
    {
      int64_t entry = get_current_entry (label_);
      return load<T> (data_, label_, entry - 1);
    }

    //! Load template method for arbitrary entry
    template<class T>
    int load (T & data_, int64_t nentry_ = -1)
    {
      return load <T> (data_, "", nentry_);
    }

    //! Load template method for arbitrary store and entry
    template<class T>
    int load (T & data_, const std::string & label_, int64_t nentry_ = -1)
    {
      _only_if_opened ("brio::reader::load");
      store_info * ptr_si = _get_store_info (label_);
      if (ptr_si == 0)
        {
          if (label_.empty ())
            {
              // if we do not allow automatic store, this is a critical error:
              if (! _allow_automatic_store_)
                {
                  std::ostringstream message;
                  message << "brio::reader::load: "
                          << "No source store is selected nor default store is available !";
                  throw std::logic_error (message.str ());
                }
              else
                {
                  ptr_si = _automatic_store_;
                }
            }
          else
            {
              std::ostringstream message;
              message << "brio::reader::load: "
                      << "No source store with label '" << label_ << "' !";
              throw std::logic_error (message.str ());
            }
        }
      return _at_load<T> (data_, ptr_si, nentry_);
    }

  protected:

    virtual void _at_open (const std::string & filename_);

    template<class T>
    int _at_load (T & data_,
                  store_info * ptr_si_,
                  int64_t nentry_)
    {
      if (is_debug ())
        {
          std::cerr << "DEBUG: " << "brio::reader::_at_load: "
                    << "Entering..." << std::endl;
        }
      store_info & si = *ptr_si_;

      if (_check_serial_tag_)
        {
          // We check if the serialization tag from the store matches the data's one:
          if (si.has_dedicated_serialization_tag ())
            {
              // 2013-02-19 FM : change the way we check the serial tag :
              //if (data_.get_serial_tag () != si.get_serialization_tag ())
              if (! datatools::check_serial_tag<T>(si.get_serialization_tag ()))
                {
                  std::ostringstream message;
                  message << "brio::reader::_at_load: "
                          << "Data serialization tag '" << data_.get_serial_tag ()
                          << "' does not match source store's serialization tag '" << si.get_serialization_tag () << "' !";
                  throw std::logic_error (message.str ());
                }
            }
        }

      if (si.number_of_entries == 0)
        {
          std::ostringstream message;
          message << "brio::reader::_at_load: "
                  << "Source store '" << si.label << "' has no entry !";
          throw std::logic_error (message.str ());
        }

      int64_t nentry = nentry_;
      if (nentry >= 0)
        {
          // check overflow:
          if (nentry_ >= si.number_of_entries)
            {
              std::cerr << "DEVEL: brio::reader::_at_load: nentry=" << nentry_ << std::endl;
              std::cerr << "DEVEL: brio::reader::_at_load: si.number_of_entries=" << si.number_of_entries << std::endl;
              std::ostringstream message;
              message << "brio::reader::_at_load: "
                      << "Source store '" << si.label << "' has "
                      << "no serialized entry at index '" << nentry_ << "' !";
              throw std::logic_error (message.str ());
            }
        }
      else
        {
          // if nentry_ < 0: use entry index relative to the current entry position
          if (si.current_entry < 0) // at rewind position
            {
              // start with first entry:
              nentry = 0;
            }
          else
            {
              // try next entry:
              nentry = si.current_entry + 1;
            }
        }

      // read this tree entry in the ROOT I/O system:
      si.record.reset ();
      int ret = si.tree->GetEntry (nentry, 1); // -> 1 for all branches
      if (ret == 0)
        {
          std::ostringstream message;
          message << "brio::reader::_at_load: "
                  << "No entry '"
                  << nentry << "' at entry # " << nentry
                  << " in source store labelled '" << si.label.c_str ()
                  << "' from  file '" << _filename << "' !";
          throw std::logic_error (message.str ());
        }
      else if (ret == -1)
        {
          std::ostringstream message;
          message << "brio::reader::_at_load: "
                  << "An I/O error occurs from entry '"
                  << nentry << "' in source store labelled '" << si.label.c_str ()
                  << "' from  file '" << _filename << "' !";
          throw std::runtime_error (message.str ());
        }
      else
        {
          si.current_entry = nentry;
        }

      if (_check_serial_tag_)
        {
          /* We may be confused with stores without dedicated serialization tag.
           * Here we test if data and the entry's serial tags match:
           */
          if (! si.has_dedicated_serialization_tag ())
            {
              // check serial tag associated to the buffered binary archive:
              std::string serial_tag = si.record.fSerialTag.Data ();
              // 2013-02-19 FM : change the way we check
              // if (data_.get_serial_tag () != serial_tag)
              if (! datatools::check_serial_tag<T>(serial_tag))
                {
                  std::ostringstream message;
                  message << "brio::reader::_at_load: "
                          << "Entry '"
                          << nentry << "' with serial tag '" << serial_tag << "' in (mixed) source store labelled '" << si.label.c_str ()
                          << "' from  file '" << _filename << "' does not match the requested '"
                          << data_.get_serial_tag () << "' data type !";
                  throw std::logic_error (message.str ());
                }
            }
        }

      // Deserialize from the archive:
      boost::iostreams::stream<boost::iostreams::array_source> input_stream (si.record.fDataBuffer.fArray,
                                                                             si.record.fDataBuffer.fN);
      // 2011-06-16 FM: restored
      if (is_format_pba ())
        {
          //boost::archive::portable_binary_iarchive ia (input_stream);
          eos::portable_iarchive ia (input_stream);
          ia >> data_;
        }
      if (is_format_text ())
        {
          input_stream.imbue (*_locale);
          boost::archive::text_iarchive ia (input_stream);
          ia >> data_;
        }

      _current_store = &si;
      if (is_debug ())
        {
          std::cerr << "DEBUG: " << "brio::reader::_at_load: "
                    << "Exiting." << std::endl;
        }
      return 0;
    }

  private:

    bool _allow_mixed_types_in_stores_; /// Flag to allow stores with mixed types
    bool _allow_automatic_store_;       /// Flag to allow an default automatic store
    bool _check_serial_tag_;            /// Flag to automatically check coherence between the store's serialization tag and the stored objects serialization tag
    store_info * _automatic_store_;     /// A handle to the automatic store (if any)

  };

} // end of namespace brio

#endif // BRIO_READER_H_

// end of reader.h
