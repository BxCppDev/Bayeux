// -*- mode: c++ ; -*- 
/* writer.cc
 */

#include <brio/writer.h>

namespace brio {

  using namespace std;

  void writer::lock ()
  {
    __locked = true;
  }

  void writer::unlock ()
  {
    __locked = false;
  }

  bool writer::is_locked () const
  {
    return __locked;
  }

  void writer::__only_if_unlocked (const string & where_) const
  {
    if (is_locked ())
      {
	ostringstream message;
	message << "brio::writer::__only_if_unlocked: ";
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation prohibited; writer is locked !";
	throw runtime_error (message.str ());
      }
    return;
  }

  void writer::__only_if_locked (const string & where_) const
  {
    if (! is_locked ())
      {
	ostringstream message;
	message << "brio::writer::__only_if_locked: ";
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation prohibited; writer is locked !";
	throw runtime_error (message.str ());
      }
    return;
  }

  bool writer::is_existing_file_protected () const
  {
    return __existing_file_protected;
  }

  bool writer::is_allow_mixed_types_in_stores () const
  {
    return __allow_mixed_types_in_stores;
  }

  bool writer::is_allow_automatic_store () const
  {
    return __allow_automatic_store;
  }
 
  void writer::set_allow_mixed_types_in_stores (bool new_value_)
  {
    _only_if_not_opened ("set_allow_mixed_types_in_stores");
    __allow_mixed_types_in_stores = new_value_; 
    return;
  }
 
  void writer::set_allow_automatic_store (bool new_value_)
  {
    _only_if_not_opened ("set_allow_automatic_store");
    __allow_automatic_store = new_value_; 
    return;
  }

  void writer::set_existing_file_protected (bool new_value_)
  {
    _only_if_not_opened ("set_existing_file_protected");
    __existing_file_protected = new_value_; 
    return;
  }
 
  void writer::_set_default ()
  {
    __locked = false;
    __allow_mixed_types_in_stores = false;
    __allow_automatic_store = true;
    __existing_file_protected = false;
    __automatic_store = 0;
    return;
  }
 
  // ctor:
  writer::writer () : base_io ()
  {
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::writer::writer (1): "
	     << "Entering..." << endl;
      }
    _set_default ();
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::writer::writer (1): "
	     << "Exiting." << endl;
      }
    return;
  }
 
  // ctor:
  writer::writer (const string & filename_, bool verbose_, bool debug_) : base_io (verbose_, debug_)
  {
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::writer::writer (2): "
	     << "Entering..." << endl;
      }
    _set_default ();
    set_debug (debug_);
    set_verbose (verbose_);
    open (filename_);
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::writer::writer (2): "
	     << "Exiting." << endl;
      }
    return;
  }
  
  // dtor:
  writer::~writer ()
  {
    if (is_opened ()) 
      {
	close ();
      }
    this->base_io::_reset ();
    return;
  }

  void writer::print_info (ostream & out_) const
  {
    this->tree_dump (out_, "brio::writer:");
    return;
  }

  void writer::tree_dump (std::ostream & out_, 
			   const std::string & title_, 
			   const std::string & indent_, 
			   bool inherit_) const
  {
    using namespace datatools::utils;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    base_io::tree_dump (out_, title_, indent_, true);
    
    out_ <<  indent << i_tree_dumpable::tag 
	 << "Allow automatic store: " << __allow_automatic_store << endl;

    if (__automatic_store != 0)
      {
	out_ <<  indent << i_tree_dumpable::tag 
	     << "Automatic store: '" << __automatic_store->label << "'" << endl;
      }

    out_ <<  indent << i_tree_dumpable::tag 
	 << "Allow mixed types in stores: " << __allow_mixed_types_in_stores << endl;

    out_ <<  indent << i_tree_dumpable::inherit_tag (inherit_)  
	 << "Locked: " << __locked << endl;
   
    return;
  }

  int writer::add_store (const string & label_, const string & serial_tag_, size_t buffer_size_)
  {
    if (serial_tag_.empty ())
      {
	ostringstream message;
	message << "brio::writer::add_store: "
		<< "Missing serialization tag for store with label '"
		<< label_ << "' !";
	throw runtime_error (message.str ());	
      }
    store_info * si = _add_store (label_, serial_tag_, buffer_size_);
    if (si == 0)
      {
	return 1;
      }
    return 0;
  }

  int writer::add_store (const string & label_, size_t buffer_size_)
  {
    store_info * si = _add_store (label_, store_info::POSTPONED_DEDICATED_SERIAL_TAG_LABEL, buffer_size_);
    if (si == 0)
      {
	return 1;
      }
    return 0;
  }

  int writer::add_mixed_store (const string & label_, size_t buffer_size_)
  {
    if (! __allow_mixed_types_in_stores)
      {
	ostringstream message;
	  message << "brio::writer::add_mixed_store: "
		  << "Stores with mixed data types are not allowed !";
	  throw runtime_error (message.str ());
      }
    store_info * si = _add_store (label_, store_info::NO_DEDICATED_SERIAL_TAG_LABEL, buffer_size_);
    if (si == 0)
      {
	return 1;
      }
    return 0;
  }

  store_info * writer::_add_store (const string & label_,
				   const string & serial_tag_,
				   size_t buffer_size_)
  {
    _only_if_opened ("brio::writer::_add_store");
    __only_if_unlocked ("brio::writer::_add_store");
    if (label_.empty ())
      {
	ostringstream message;
	message << "brio::writer::_add_store: "
		<< "Empty label !";
	throw runtime_error (message.str ());
      }  
    if (! __allow_automatic_store 
	&& (label_ == store_info::AUTOMATIC_STORE_LABEL))
      {
	ostringstream message;
	message << "brio::writer::_add_store: "
		<< "Label '" << label_ << "' for a conventional automatic store is not allowed !";
	throw runtime_error (message.str ());	
      }

    if (is_verbose ())
      {
	cerr << "NOTICE: " << "brio::writer::_add_store: "
	     << "Create a new store with label '" << label_ << "'..." << endl;
      }
    const store_info * ptr_si = _get_store_info (label_);
    if (ptr_si != 0)    
      {
	ostringstream message;
	message << "brio::writer::_add_store: "
		<< "Store with label '" << label_ << "' already exists !";
	throw runtime_error (message.str ());
      }

    // create a new store:
    {
      store_info si;
      // pointer within store_info::ctor must be NULL for
      // no copy constructor is provided.
      _store_infos [label_] = si;
    }
    store_info & the_si = _store_infos.find (label_)->second;
    the_si.label = label_;
    // store may have a dedicated serialization tag:
    the_si.serialization_tag = serial_tag_;
    the_si.bufsize = buffer_size_;
    the_si.number_of_entries = 0;
    the_si.current_entry = -1;

    ostringstream tree_title;
    tree_title << label_.c_str () << " tree for binary archive serialization";
    _file->cd ();
    the_si.tree = new TTree (label_.c_str (), tree_title.str ().c_str ());
    the_si.p_record = &the_si.record;
    int splitlevel = 0; // no branch-splitting of the streamed 'brio_record' instance
    const string * branch_name = &the_si.serialization_tag;
    if (the_si.serialization_tag.empty () || the_si.serialization_tag == store_info::NO_DEDICATED_SERIAL_TAG_LABEL)
      {
	branch_name = &store_info::NO_DEDICATED_SERIAL_TAG_LABEL;
      }
    the_si.tree->Branch (branch_name->c_str (),
			 "brio_record",
			 &(the_si.p_record), 
			 the_si.bufsize, 
			 splitlevel);
    _current_store = &the_si;
    if (label_ == store_info::AUTOMATIC_STORE_LABEL)
      {
	__automatic_store = _current_store;
      }
    return _current_store;
  } 

  void writer::_at_open (const string & filename_)
  {
    _filename = filename_;
    datatools::utils::fetch_path_with_env (_filename);
    
    if (is_existing_file_protected ())
      {
	if (boost::filesystem::exists (_filename))
	  {
	    ostringstream message;
	    message << "brio::writer::_at_open: "
		    << "File '" << _filename << "' already exists ! Abort !";
	    throw runtime_error (message.str ());	
	  }
      }

    string default_extension = store_info::DEFAULT_FILE_EXTENSION;
    static size_t test_extension_size 
      = store_info::DEFAULT_FILE_EXTENSION.length ();
    string extension = _filename.substr (_filename.length () - test_extension_size,
					  test_extension_size);
    if (is_debug ())
      {
 	cerr << "DEBUG: "
	     << "brio::writer::_at_open: "
	     << "Extension is `" << extension << "' !" 
	     << endl;
     }
    if (extension != store_info::DEFAULT_FILE_EXTENSION)
      {
	cerr << "WARNING: "
	     << "brio::writer::_at_open: "
	     << "Using extension different from `" 
	     << store_info::DEFAULT_FILE_EXTENSION << "' is not recommended !" 
	     << endl;
      }
    string mode = "RECREATE";
    _file = new TFile (_filename.c_str (), 
			mode.c_str (), 
			"ROOT file from the `brio' library");
    if (_file == 0)
      {
	ostringstream message;
	message << "brio::writer::_at_open: "
		<< "Cannot open file '" << _filename << "' !";
	throw runtime_error (message.str ());	
      }
    if (_file->IsOpen ())
      {
	if (is_verbose ())
	  {
	    cerr << "NOTICE: " <<  "brio::writer::_at_open: "
		 << "File '" << _filename << "' has been successfully opened !"
		 << endl;
	  }
      }
    else
      {
	ostringstream message;
	message << "brio::writer::_at_open: "
		<< "File '" << _filename << "' is not opened !";
	throw runtime_error (message.str ());	
      }
    _file->cd ();
    return;
  }

  void writer::_at_close ()
  {
    if (_file != 0)
      {
	_file->cd ();
	_file->Write ();
	_file->Close ();
      }
    base_io::_reset ();
    return;
  }
  
} // end of namespace brio

// end of writer.cc
