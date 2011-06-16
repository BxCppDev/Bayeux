// -*- mode: c++ ; -*- 
/* writer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2010-11-01
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

#ifndef __brio__writer_h
#define __brio__writer_h 1

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/portable_binary_oarchive.hpp>

#include <brio/detail/base_io.h>

#include <TTree.h>

namespace brio {

  using namespace std;

  class writer : public base_io
  {    
  private: 

    bool __locked;
    bool __allow_mixed_types_in_stores;
    bool __allow_automatic_store;
    bool __existing_file_protected;
    store_info * __automatic_store;

  public: 

    void lock ();
    void unlock ();

    // getters:

    bool is_locked () const;
    bool is_allow_mixed_types_in_stores () const;
    bool is_allow_automatic_store () const;
    bool is_existing_file_protected () const;
    
    // setters:

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

    void set_existing_file_protected (bool a_new_value = true);
  
    // File open/close operations:
    //void open (const string & filename_);

    //void close ();
  
  private:

    void __only_if_unlocked (const string & a_where) const;

    void __only_if_locked (const string & a_where) const;

  protected:

    void _set_default ();

  public: 

    // ctor:
    writer ();

    // ctor:
    writer (const string & a_filename, 
	    bool a_verbose = false, 
	    bool a_debug = false);


    // ctor:
    writer (const string & a_filename, 
	    const string & a_format_str,
	    bool a_verbose = false, 
	    bool a_debug = false);

    // dtor:
    virtual ~writer ();

  public:

    // Utilities:
    virtual void tree_dump (std::ostream & a_out = std::clog, 
			    const std::string & a_title = "", 
			    const std::string & a_indent = "", 
			    bool a_inherit = false) const;

    void print_info (ostream & a_out = clog) const;

    /** Add a new store with label 'label_'
     *  to store objects with a dedicated serialization tag 'serial_tag_':
     */
    int add_store (const string & a_label, 
		   const string & a_serial_tag,
		   size_t a_buffer_size = 256000);

    int add_store (const string & a_label, 
		   size_t a_buffer_size = 256000);

    int add_mixed_store (const string & a_label, 
			 size_t a_buffer_size = store_info::DEFAULT_STORE_BUFFER_SIZE);

  protected:

    store_info * _add_store (const string & a_label, 
			     const string & a_serial_tag,
			     size_t a_buffer_size);

    virtual void _at_open (const string & a_filename);

  public:

    // Main store template method:
    template<class T>
    int store (const T & a_data, const string & a_label = "")
    {
      _only_if_opened ("brio::writer::store");
      store_info * ptr_si = _get_store_info (a_label);
      if (ptr_si == 0)
	{
	  if (a_label.empty ())
	    {
	      // if we do not allow automatic store, this is a critical error:
	      if (! __allow_automatic_store)
		{
		  ostringstream message;
		  message << "brio::writer::store: "
			  << "No target store is selected nor target !";
		  throw runtime_error (message.str ());
		}
	      else
		{
		  ptr_si = _add_store (store_info::AUTOMATIC_STORE_LABEL, 
				       a_data.get_serial_tag (),
				       store_info::DEFAULT_STORE_BUFFER_SIZE);
		}
	    }
	  else
	    {
	      ptr_si =_add_store (a_label, 
				  a_data.get_serial_tag (),
				  store_info::DEFAULT_STORE_BUFFER_SIZE);
	    }
	}

      // Final check:
      if (ptr_si == 0)
	{
	  ostringstream message;
	  message << "brio::writer::store: "
		  << "Could not determine any store to save data !";
	  throw runtime_error (message.str ());
	}

      if (is_debug())
	{
	  cerr << "DEBUG: " << "brio::writer::_at_store: "
	       << "ptr_si = " << hex << ptr_si << dec << endl;
	  cerr << "DEBUG: " << "brio::writer::_at_store: "
	       << "Using store with label '" << ptr_si->label << "'..." << endl;
	  ptr_si->tree->Print ();
	}
 
      return _at_store<T> (a_data, ptr_si);
    }

  protected:

    template<class T>
    int _at_store (const T & a_data, store_info * a_store_info)
    {
      if (is_debug ())
	{
	  cerr << "DEBUG: " << "brio::writer::_at_store: "
	       << "Entering..." << endl;
	}
      store_info * ptr_si = a_store_info;

      // The first serialized object sets the serialization tag for this store:
      if (ptr_si->serialization_tag == store_info::POSTPONED_DEDICATED_SERIAL_TAG_LABEL)
	{
	  ptr_si->serialization_tag = a_data.get_serial_tag ();
	}
      // Else if the store has a dedicated serialization tag:
      else if (ptr_si->has_dedicated_serialization_tag ())
	{
	  // Check if the data serialization tag matches the one requested by the store:
	  if (a_data.get_serial_tag () != ptr_si->get_serialization_tag ())
	    {
	      ostringstream message;
	      message << "brio::writer::_at_store: "
		      << "Serialization tag mismatch ! "
		      << "Attempt to store an object with `" << a_data.get_serial_tag () 
		      << "' serialization tag "
		      << "in the store labelled '" << ptr_si->label << "' with dedicated `"
		      << ptr_si->get_serialization_tag () << "' serialization tag !";
	      throw runtime_error (message.str ());
	    }     
	}

      // Prepare the (vector<char>) buffer to host the binary archive as a sequence of bytes: 
      typedef std::vector<char> buffer_type;

      /* Clear the buffer of characters for streaming but 
       * intrinsic capacity is kept as is (this is a behaviour of vector 
       * class from the STL library):
       */
      if (! ptr_si->buffer.empty ())
	{
	  ptr_si->buffer.clear ();
	}
      else if (ptr_si->buffer.capacity () == 0)
	{
	  // Ensure minimum starting capacity of the buffer of characters for streaming
	  // in order to optimize possible memory reallocation:
	  ptr_si->buffer.reserve (store_info::DEFAULT_STREAM_BUFFER_SIZE);
	}

      // Archiving is redirected to the buffer:
      namespace io = boost::iostreams;
      io::stream<io::back_insert_device<buffer_type> > output_stream (ptr_si->buffer);
      // 2011-06-16 FM: restored 
      if (is_format_pba ())
	{
	  boost::archive::portable_binary_oarchive oa (output_stream);   
	  oa << a_data;
	}
      if (is_format_text ())
	{
	  output_stream.imbue (*_locale);
	  boost::archive::text_oarchive oa (output_stream);
	  oa << a_data;
	}
     output_stream.flush ();

      // Now the buffer contains the final sequence of bytes corresponding to 
      // the serialized output binary archive:
      if (is_debug ())
	{
	  cerr << "DEBUG: " <<  "brio::writer::_at_store: "
	       << "buffer size = " 
	       << ptr_si->buffer.size ()
	       << "   buffer capacity = " 
	       << ptr_si->buffer.capacity () << endl;
	}

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

      if (is_debug ())
	{
	  cerr << "DEBUG: " << "brio::writer::_at_store: "
	       << "Exiting." << endl;
	}
      return 0;
    }

  };

} // end of namespace brio

#endif // __brio__writer_h

// end of writer.h
