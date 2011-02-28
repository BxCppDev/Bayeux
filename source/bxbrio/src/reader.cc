// -*- mode: c++ ; -*- 
/* reader.cc
 */

#include <brio/reader.h>

namespace brio {

  using namespace std;
  
  void reader::set_check_serial_tag (bool new_value_)
  {
    //__only_if_not_opened ("brio::reader::checking_serial_tag");
    __check_serial_tag = new_value_; 
    return;
  }

  bool reader::is_check_serial_tag () const
  {
    return __check_serial_tag;
  }

  void reader::_set_default ()
  {
    base_io::_set_default ();
    __allow_mixed_types_in_stores = false;
    __allow_automatic_store = true;
    __check_serial_tag = true;
    __automatic_store = 0;
    return;
  }
 
  // ctor:
  reader::reader () : base_io ()
  {
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::reader: "
	     << "Entering..." << endl;
      }
    _set_default ();
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::reader: "
	     << "Exiting." << endl;
      }
    return;
  }
 
  // ctor:
  reader::reader (const string & filename_, bool verbose_, bool debug_) : base_io ()
  {
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::reader: "
	     << "Entering..." << endl;
      }
    _set_default ();
    set_debug (debug_);
    set_verbose (verbose_);
    open (filename_);
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::reader: "
	     << "Exiting." << endl;
      }
    return;
  }
  
  // dtor:
  reader::~reader ()
  {
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::~reader: "
	     << "Entering..." << endl;
      }
    if (is_opened ()) 
      {
	close ();
      }
    this->base_io::_reset ();
    if (base_io::g_devel)
      {
	cerr << "DEVEL: " << "brio::reader::~reader: "
	     << "Exiting." << endl;
      }
    return;
  }

  void reader::print_info (ostream & out_) const
  {
    this->tree_dump (out_, "brio::reader:");
    return;
  }

  void reader::tree_dump (std::ostream & out_, 
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
	 << "Check serial tag: " << __check_serial_tag << endl;
    
    return;
  }

  bool reader::has_previous (const string & label_) const
  {
    _only_if_opened ("brio::reader::has_previous");
    const store_info * ptr_si = _get_store_info (label_);
    if (ptr_si == 0)
      {
	ostringstream message;
	message << "brio::reader::has_previous: "
		<< "Missing explicit and valid store label !";
	throw runtime_error (message.str ());	    
      }
    if (ptr_si->number_of_entries == 0) 
      {
	return false;
      }
    else
      {
	if (ptr_si->current_entry > 0)
	  {
	    return true;
	  }
      }
    return false;
  }


  bool reader::has_next (const string & label_) const
  {
    _only_if_opened ("brio::reader::has_next");
    const store_info * ptr_si = _get_store_info (label_);
    if (ptr_si == 0)
      {
	ostringstream message;
	message << "brio::reader::has_next: "
		<< "Missing explicit and valid store label !";
	throw runtime_error (message.str ());	    
      }
    if (ptr_si->number_of_entries == 0) 
      {
	return false;
      }
    else
      {
	if (ptr_si->current_entry < (ptr_si->number_of_entries - 1))
	  {
	    return true;
	  }
      }
    return false;
  }


  void reader::rewind_store (const string & label_)
  {
    _only_if_opened ("brio::reader::rewind_store");
    store_info * ptr_si = _get_store_info (label_);
    if (ptr_si == 0)
      {
	ostringstream message;
	message << "brio::reader::rewind_store: "
		<< "Missing explicit and valid store label !";
	throw runtime_error (message.str ());	    
      }
    ptr_si->current_entry = -1;
    return;
  }
  
  void reader::unwind_store (const string & label_)
  {
    _only_if_opened ("brio::reader::unwind_store");
    store_info * ptr_si = _get_store_info (label_);
    if (ptr_si == 0)
      {
	ostringstream message;
	message << "brio::reader::unwind_store: "
		<< "Missing explicit and valid store label !";
	throw runtime_error (message.str ());	    
      }
    ptr_si->current_entry = ptr_si->number_of_entries;
    return;
  }

  void reader::_at_open (const string & filename_)
  {
    bool devel = false;
    devel = is_debug ();
    if (devel)
      {
	clog << "DEVEL: " << "brio::reader::_at_open: " 
	     << "Entering..." << endl;
      }
    _filename = filename_;
    datatools::utils::fetch_path_with_env (_filename);
    if (! boost::filesystem::exists (_filename))
      {
	ostringstream message;
	message << "brio::reader::_at_open: "
		<< "File '" << _filename << "' does not exist ! Abort !";
	throw runtime_error (message.str ());	
      }
    
    string default_extension = store_info::DEFAULT_FILE_EXTENSION;
    static size_t test_extension_size 
      = store_info::DEFAULT_FILE_EXTENSION.length ();
    string extension = _filename.substr (_filename.length () - test_extension_size,
					  test_extension_size);
    if (is_debug ())
      {
 	cerr << "DEBUG: "
	     << "brio::reader::_at_open: "
	     << "Extension is `" << extension << "' !" 
	     << endl;
     }
    if (extension != store_info::DEFAULT_FILE_EXTENSION)
      {
	cerr << "WARNING: "
	     << "brio::reader::_at_open: "
	     << "Using extension different from `" 
	     << store_info::DEFAULT_FILE_EXTENSION 
	     << "' is not recommended !" 
	     << endl;
      }
    string mode = "READ";
    _file = new TFile (_filename.c_str (), mode.c_str ());
    if (_file == 0)
      {
	ostringstream message;
	message << "brio::reader::_at_open: "
		<< "Cannot open file '" << _filename << "' !";
	throw runtime_error (message.str ());	
      }
    if (_file->IsOpen ())
      {
	if (is_verbose ())
	  {
	    cerr << "NOTICE: " <<  "brio::reader::_at_open: "
		 << "File '" << _filename << "' has been successfully opened !"
		 << endl;
	  }
      }
    else
      {
	ostringstream message;
	message << "brio::reader::_at_open: "
		<< "File '" << _filename << "' is not opened !";
	throw runtime_error (message.str ());	
      }
    _file->cd ();

    // extract the stores' informations:
    TList * list_of_keys = _file->GetListOfKeys ();
    if (devel)
      {
	clog << "DEVEL: " << "brio::reader::_at_open: " 
	     << "# entries: " << list_of_keys->GetEntries () << endl; 
      }
    TIter next (list_of_keys);
    while (TObject * obj_key = next ())
      {
	TKey * key = dynamic_cast<TKey *> (obj_key);
	string class_name = key->ClassName ();
	string store_label = key->GetName ();
	if (devel)
	  {
	    clog << "DEVEL: Class_name: `" << class_name << "'" << endl;
	    clog << "DEVEL: Store label is: `" << store_label << "'" << endl;
	  }
	// This test is probably not useful:
	if (class_name != "TKey")
	  {
	    continue;
	  }
	if (devel)
	  {	
	    key->Print ();
	  }
	// create a new store:
	{
	  store_info si;
	  _store_infos [store_label] = si;
	}
	store_info & the_si = _store_infos.find (store_label)->second;
	the_si.label = store_label;	
	the_si.serialization_tag = "";	
	the_si.tree = 0;
	the_si.p_record = &the_si.record;

	TTree * the_tree = 0;
	_file->cd ();
	_file->GetObject (store_label.c_str (), the_tree);
	if (the_tree == 0)
	  {
	    ostringstream message;
	    message << "brio::reader::_at_open: "
		    << "Cannot retrieve store's tree labelled '" << store_label.c_str () 
		    << "' from  file '" << _filename << "' !";
	    throw runtime_error (message.str ());  	    
	  }
	the_si.tree = the_tree;
	if (devel)
	  {	
	    the_si.tree->Print ();
	  }

	TObjArray * obj_array = the_si.tree->GetListOfBranches ();
	if (obj_array->GetEntries () != 1)
	  {
	    ostringstream message;
	    message << "brio::reader::_at_open: "
		    << "The store's tree labelled '" << store_label.c_str () 
		    << "' from  file '" << _filename << "' has not an unique branch ! Unsupported format !";
	    throw runtime_error (message.str ());  	    	    
	  }
	TObject * obj_branch = obj_array->At (0);
	TBranch * the_branch = dynamic_cast<TBranch *> (obj_branch);
	if (the_branch == 0)
	  {
	    ostringstream message;
	    message << "brio::reader::_at_open: "
		    << "Cannot retrieve a valid branch from the store's tree labelled '" << store_label.c_str () 
		    << "' from  file '" << _filename << "' !";
	    throw runtime_error (message.str ());  	    	    
	  }
	string branch_name = the_branch->GetName ();
	the_si.serialization_tag = branch_name;
	if (branch_name == store_info::NO_DEDICATED_SERIAL_TAG_LABEL)
	  {
	    __allow_mixed_types_in_stores = true;
	  }
	the_si.number_of_entries = the_si.tree->GetEntries ();
	the_si.current_entry = -1; // rewind entry  position
	the_si.tree->SetBranchAddress (branch_name.c_str (), &the_si.p_record);
	the_si.tree->SetBranchStatus (branch_name.c_str (), 1);	
	the_branch->SetAutoDelete (kFALSE);
	
	// tag if the automatic store label has been found:
	if (store_label == store_info::AUTOMATIC_STORE_LABEL)
	  {
	    __allow_automatic_store = true;
	    __automatic_store = &the_si;
	  }

	// The last found store is made the current selected one by default:
	_current_store = &the_si;

      } // while

 
    // If the conventional automatic store is found, make it the current selected one:
    if (_store_infos.size () >= 1)
      {
	if (__automatic_store != 0)
	  {
	    _current_store = __automatic_store;
	  }
      }

    if (devel)
      {	
	clog << "DEVEL: " << "brio::reader::_at_open: " 
	     << "Exiting." << endl;
      }
    return;
  }

  void reader::_at_close ()
  {
    if (_file != 0)
      {
	_file->cd ();
	_file->Close ();
      }
    base_io::_reset ();
    return;
  }
  
} // end of namespace brio

// end of reader.cc
