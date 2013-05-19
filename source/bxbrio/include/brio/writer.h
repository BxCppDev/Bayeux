// -*- mode: c++ ; -*-
/* writer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *
 *   A Boost archive to ROOT file writer
 *
 * History:
 *
 */

#ifndef BRIO_WRITER_H_
#define BRIO_WRITER_H_ 1

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <datatools/eos/portable_oarchive.hpp>
#include <datatools/logger.h>
#include <datatools/exception.h>

#include <brio/detail/base_io.h>

#include <TTree.h>

namespace brio {

  //! \brief The brio generic writer class
  class writer : public detail::base_io
  {
  public:

    //! Lock the writer
    void lock ();

    //! Unlock the writer
    void unlock ();

    // getters:

    //! Check if the writer is locked
    bool is_locked () const;

    //! Check if mixed types are allowed in any given store
    bool is_allow_mixed_types_in_stores () const;

    //! Check if automatic store is allowed
    bool is_allow_automatic_store () const;

    //! Check if protection against file overwriting exists
    bool is_existing_file_protected () const;

    /** Allow to serialize different types of objects within stores.
     *  Usually, each store is setup in order to accept only one
     *  kind of serializable object (potato) , in such a way one cannot mix
     *  several object's types (ex: potato and carot) within the same branch
     *  Default at construction is false.
     */
    void set_allow_mixed_types_in_stores (bool a_new_value = true);

    /** Enables the use of some automatic store (with
     *  standard label == store_info::AUTOMATIC_STORE_LABEL)
     *  within the writer.
     *  Default at construction is true.
     */
    void set_allow_automatic_store (bool a_new_value = true);

    //! Set the protection against file overwriting
    void set_existing_file_protected (bool a_new_value = true);

  public:

    //! Default constructor
    writer ();

    //! Constructor
    writer (const std::string & a_filename,
            datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Constructor
    writer (const std::string & a_filename,
            const std::string & a_format_str,
            datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

    //! Destructor
    virtual ~writer ();

    //! Smart print
    virtual void tree_dump (std::ostream & a_out = std::clog,
                            const std::string & a_title = "",
                            const std::string & a_indent = "",
                            bool a_inherit = false) const;

    //! Print
    void print_info (std::ostream & a_out = std::clog) const;

    /** Add a new store with label 'label_'
     *  to store objects with a dedicated serialization tag 'serial_tag_'
     */
    int add_store (const std::string & a_label,
                   const std::string & a_serial_tag,
                   size_t a_buffer_size = 256000);

    /** Add a new store with label 'label_'
     *  to store objects with a dedicated serialization tag 'serial_tag_'
     */
    int add_store (const std::string & a_label,
                   size_t a_buffer_size = 256000);

    /** Add a new store with label 'label_'
     *  to store objects with arbitrary serialization tags
     */
    int add_mixed_store (const std::string & a_label,
                         size_t a_buffer_size = store_info::DEFAULT_STORE_BUFFER_SIZE);


  public:

    // Store template method
    template<class T>
    int store (const T & a_data, const std::string & a_label = "")
    {
      DT_THROW_IF (! is_opened (),std::logic_error,"Operation prohibited; file is not opened !");
      store_info * ptr_si = _get_store_info (a_label);
      if (ptr_si == 0) {
        if (a_label.empty ()) {
          // if we do not allow automatic store, this is a critical error:
          DT_THROW_IF (! _allow_automatic_store_,
                       std::logic_error,
                       "No target store is selected nor target !");
          ptr_si = _add_store (store_info::AUTOMATIC_STORE_LABEL,
                               a_data.get_serial_tag (),
                               store_info::DEFAULT_STORE_BUFFER_SIZE);
        } else {
          ptr_si =_add_store (a_label,
                              a_data.get_serial_tag (),
                              store_info::DEFAULT_STORE_BUFFER_SIZE);
        }
      }
      // Final check:
      DT_THROW_IF (ptr_si == 0,std::logic_error,"Could not determine any store to save data !");
      DT_LOG_DEBUG (get_logging_priority(),"ptr_si = " << std::hex << ptr_si << std::dec);
      DT_LOG_DEBUG (get_logging_priority(),"Using store with label '" << ptr_si->label << "'...");
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) ptr_si->tree->Print ();
      return _at_store<T> (a_data, ptr_si);
    }

  protected:

    template<class T>
    int _at_store (const T & a_data, store_info * a_store_info)
    {
      DT_LOG_TRACE (get_logging_priority(),"Entering...");
      store_info * ptr_si = a_store_info;
      // The first serialized object sets the serialization tag for this store:
      if (ptr_si->serialization_tag == store_info::POSTPONED_DEDICATED_SERIAL_TAG_LABEL) {
        ptr_si->serialization_tag = a_data.get_serial_tag ();
      }
      // Else if the store has a dedicated serialization tag:
      else if (ptr_si->has_dedicated_serialization_tag ()) {
        // Check if the data serialization tag matches the one requested by the store:
        DT_THROW_IF (a_data.get_serial_tag () != ptr_si->get_serialization_tag (),
                     std::logic_error,
                     "Serialization tag mismatch ! "
                     << "Attempt to store an object with `" << a_data.get_serial_tag ()
                     << "' serialization tag "
                     << "in the store labelled '" << ptr_si->label << "' with dedicated `"
                     << ptr_si->get_serialization_tag () << "' serialization tag !");
      }

      // Prepare the (std::vector<char>) buffer to host the binary archive as a sequence of bytes:
      typedef std::vector<char> buffer_type;

      /* Clear the buffer of characters for streaming but
       * intrinsic capacity is kept as is (this is a behaviour of vector
       * class from the STL library):
       */
      if (! ptr_si->buffer.empty ()) {
        ptr_si->buffer.clear ();
      } else if (ptr_si->buffer.capacity () == 0) {
        // Ensure minimum starting capacity of the buffer of characters for streaming
        // in order to optimize possible memory reallocation:
        ptr_si->buffer.reserve (store_info::DEFAULT_STREAM_BUFFER_SIZE);
      }

      // Archiving is redirected to the buffer:
      namespace io = boost::iostreams;
      io::stream<io::back_insert_device<buffer_type> > output_stream (ptr_si->buffer);
      // 2011-06-16 FM: restored
      if (is_format_pba ()) {
        //boost::archive::portable_binary_oarchive oa (output_stream);
        eos::portable_oarchive oa (output_stream);
        oa << a_data;
      }
      if (is_format_text ()) {
        output_stream.imbue (*_locale);
        boost::archive::text_oarchive oa (output_stream);
        oa << a_data;
      }
      output_stream.flush ();

      // Now the buffer contains the final sequence of bytes corresponding to
      // the serialized output binary archive:
      DT_LOG_DEBUG(get_logging_priority(),
                   "Buffer size = "
                   << ptr_si->buffer.size ()
                   << "   buffer capacity = "
                   << ptr_si->buffer.capacity ());
      // Prepare the container interface to be streamed using the ROOT I/O system:
      ptr_si->record.fSerialTag  = a_data.get_serial_tag ().c_str ();
      ptr_si->record.fVersionTag = 0; // not used
      ptr_si->record.fDataBuffer.fN     = ptr_si->buffer.size ();
      ptr_si->record.fDataBuffer.fArray =  &(ptr_si->buffer[0]);
      // Store this tree entry in the ROOT I/O system:
      ptr_si->tree->Fill ();
      // Reset the ROOT data buffer:
      ptr_si->record.fDataBuffer.fN = 0;
      ptr_si->record.fDataBuffer.fArray = 0;
      // Update some counters:
      ptr_si->number_of_entries = ptr_si->tree->GetEntries ();
      ptr_si->current_entry = ptr_si->number_of_entries - 1;
      // Update the 'current store' pointer:
      _current_store = ptr_si;
      DT_LOG_TRACE(get_logging_priority(),"Exiting.");
      return 0;
    }

  protected:

    void _set_default ();

    store_info * _add_store (const std::string & a_label,
                             const std::string & a_serial_tag,
                             size_t a_buffer_size);

    virtual void _at_open (const std::string & a_filename);

  private:

    void __only_if_unlocked (const std::string & a_where) const;

    void __only_if_locked (const std::string & a_where) const;

  private:

    bool _locked_; /// Flag to lock the writer storage structure
    bool _allow_mixed_types_in_stores_; /// Flag to allow stores with mixed types
    bool _allow_automatic_store_;       /// Flag to allow an default automatic store
    bool _existing_file_protected_;     /// Flag to protect existing output data file
    store_info * _automatic_store_;     /// A handle to the automatic store (if any)

  };

} // end of namespace brio

#endif // BRIO_WRITER_H_

// end of writer.h
