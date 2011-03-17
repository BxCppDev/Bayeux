// -*- mode: c++ ; -*- 
/* base_io.cc
 */

#include <brio/detail/base_io.h>
#include <boost/archive/codecvt_null.hpp>

namespace brio {

  using namespace std;
  
  bool base_io::g_devel = false;

  const string base_io::QPBA_LABEL = "qpba";
  const string base_io::TEXT_LABEL = "text";
   
  // static : 
  int base_io::get_format (const string & format_str_)
  {
    if (format_str_ == QPBA_LABEL) return FORMAT_QPBA;
    if (format_str_ == TEXT_LABEL) return FORMAT_TEXT;
    ostringstream message;
    message << "brio::base_io::get_format: "
	    << "Invalid format label '" << format_str_ << "' !";
    throw logic_error (message.str ());
  }

  int base_io::get_format () const
  {
    return __format;
  }

  void base_io::set_format (int format_)
  {
    _only_if_not_opened ("brio::base_io::set_format");
    __format = format_;
    if (is_format_text ())
      {
	if (_default_locale == 0)
	  {
	    _default_locale = new std::locale (std::locale::classic (), 
					       new boost::archive::codecvt_null<char>); 
	  }
	if (_locale == 0)
	  {
	    if (_rw == RW_READ)
	      {
		_locale = new std::locale (*_default_locale, 
					   new boost::math::nonfinite_num_get<char>);
	      }
	    if (_rw == RW_WRITE)
	      {
		_locale = new std::locale (*_default_locale, 
					   new boost::math::nonfinite_num_put<char>);
	      }
	  }

      }
    return;
  }

  void base_io::set_format (const string & format_str_)
  {
    set_format (base_io::get_format (format_str_));
    return;
  }

  void base_io::_only_if_not_opened (const string & where_) const
  {
    if (is_opened ())
      {
	ostringstream message;
	message << "brio::base_io::_only_if_not_opened: ";
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation prohibited; file is opened !";
	throw runtime_error (message.str ());
      }
    return;
  }

  void base_io::_only_if_opened (const string & where_) const
  {
    if (! is_opened ())
      {
	ostringstream message;
	message << "brio::base_io::_only_if_opened: ";
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation prohibited; file is not opened !";
	throw runtime_error (message.str ());
      }
    return;
  }
 
  bool base_io::is_debug () const
  {
    return __debug;
  }
  
  void base_io::set_debug (bool new_value_)
  {
    __debug = new_value_; 
    return;
  }

  bool base_io::is_verbose () const
  {
    return __verbose;
  }
  
  void base_io::set_verbose (bool new_value_)
  {
    __verbose = new_value_; 
    return;
  }

  bool base_io::is_reading () const
  {
    return _rw == RW_READ;
  }

  bool base_io::is_writing () const
  {
    return _rw == RW_WRITE;
  }

  bool base_io::is_format_qpba () const
  {
    return __format == FORMAT_QPBA;
  }

  bool base_io::is_format_text () const
  {
    return __format == FORMAT_TEXT;
  }

  bool base_io::is_opened () const
  {
    return (_file != 0) && _file->IsOpen ();
  }

  void base_io::close ()
  { 
    if (is_debug ())  
      {
	cerr << "DEBUG: " << "brio::base_io::close: "
	     << "Entering..." << endl;
      }
    if (! is_opened ()) 
      {
	throw runtime_error ("brio::base_io::close: Not opened !");
      }
     if (_file != 0)
      {
 	_file->cd ();
	if (is_writing ()) _file->Write ();
	_file->Close ();
      }
    base_io::_reset ();
    if (is_debug ())
      {
	cerr << "DEBUG: " << "brio::base_io::close: "
	     << "Exiting." << endl;
      }
    return;
  }
  
  void base_io::open (const string & filename_)
  {
    if (is_debug ())
      {
	cerr << "DEBUG: " << "brio::base_io::open: "
	     << "Entering with filename '" << filename_ << "'" << endl;
      }
    if (is_opened ()) 
      {
	throw runtime_error ("brio::base_io::open: Already opened !");
      }
    cerr << "DEVEL: " << "brio::base_io::open: "
	 << "format = '" << __format << "'" << endl;
    if (__format == FORMAT_UNDEFINED)
      {
	clog << "NOTICE: brio::base_io::open: "
	     << "Guessing the archive format from the filename !" << endl; 
	string file_extension = boost::filesystem::extension (filename_);
	if (file_extension == store_info::TRIO_FILE_EXTENSION)
	  {
	    set_format (FORMAT_TEXT);
	    clog << "NOTICE: brio::base_io::open: "
		 << "Using '" << TEXT_LABEL << "' archive format !" << endl; 
	  }
	else
	  {
	    set_format (FORMAT_QPBA);
	    clog << "NOTICE: brio::base_io::open: "
		 << "Using '" << QPBA_LABEL << "' archive format !" << endl; 
	  }
      }
    _at_open (filename_);
    if (is_debug ())
      {
	cerr << "DEBUG: " << "brio::base_io::open: "
	     << "Exiting." << endl;
      }
   return;
  }

  bool base_io::has_automatic_store () const
  {
    return has_store (store_info::AUTOMATIC_STORE_LABEL);
  }

  void base_io::select_automatic_store ()
  {
    if (! has_automatic_store ())
      {
	ostringstream message;
	message << "brio::base_io::get_serialization_tag: "
		<< "No automatic store (with label '"
		<< store_info::AUTOMATIC_STORE_LABEL << "') is available !";
	throw runtime_error (message.str ());	
      }
    select_store (store_info::AUTOMATIC_STORE_LABEL);
    return;
  }

  bool base_io::has_store (const string & label_) const
  {
    // accelerator:
    if (_current_store != 0)
      {
	if (_current_store->label == label_) return true;
      }
    // else search the dictionnary:
    store_info_dict_t::const_iterator found = _store_infos.find (label_);
    return found != _store_infos.end ();
  }

  const string & base_io::get_serialization_tag (const string & label_) const
  {
    _only_if_opened ("brio::base_io::get_serialization_tag");
    const store_info * si = _get_store_info (label_);
    if (si == 0)
      {
	ostringstream message;
	message << "brio::base_io::get_serialization_tag: "
		<< "No store with label '"
		<< label_ << "' !";
	throw runtime_error (message.str ());
      }
    return si->serialization_tag;
  }

  int64_t base_io::get_number_of_entries (const string & label_) const
  {
    _only_if_opened ("brio::base_io::get_number_of_entries");
    const store_info * si = _get_store_info (label_);
    if (si == 0)
      {
	ostringstream message;
	message << "brio::base_io::get_number_of_entries: "
		<< "No store with label '"
		<< label_ << "' !";
	throw runtime_error (message.str ());
      }
    return si->number_of_entries;
  }

  int64_t base_io::get_current_entry (const string & label_) const
  {
    _only_if_opened ("brio::base_io::get_current_entry");
    const store_info * si = _get_store_info (label_);
    if (si == 0)
      {
	ostringstream message;
	message << "brio::base_io::get_current_entry: "
		<< "No store with label '"
		<< label_ << "' !";
	throw runtime_error (message.str ());
      }
    return si->current_entry;
  }

  void base_io::unselect_store ()
  { 
    _current_store = 0;
  }

  void base_io::select_store (const string & label_)
  {
    _only_if_opened ("brio::base_io::get_number_of_entries");
    store_info * si = _get_store_info (label_);
    if (si != 0)
      {
	_current_store = si;
      }
    else
      {
	ostringstream message;
	message << "brio::base_io::select_store: "
		<< "No store with label '"
		<< label_ << "' nor default store available !";
	throw runtime_error (message.str ());
      }
    return;
  }
  
  void base_io::_set_default ()
  {
    __debug = false;
    __verbose = false;
    __format = FORMAT_UNDEFINED;
    if (g_devel)
      {
	__debug = true;
	__verbose = true;
      }
    _file = 0;
    _filename= "";
    _current_store = 0;
    _locale = 0;
    _default_locale = 0;
    return;
  }

  // ctor:
  base_io::base_io (int rw_)
  {
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (1): "
	     << "Entering..." << endl;
      }
    _rw = rw_;
    __format = FORMAT_UNDEFINED;
    base_io::_set_default ();
    _default_locale = 0;
    _locale = 0;
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (1): "
	     << "Exiting." << endl;
      }
    return;
  }

  // ctor:
  base_io::base_io (int rw_, bool verbose_, bool debug_)
  {
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (2): "
	     << "Entering..." << endl;
      }
    _rw = rw_;
    __format = FORMAT_UNDEFINED;
    base_io::_set_default ();
    __verbose = verbose_;
    __debug = debug_;
    _default_locale = 0;
    _locale = 0;
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (2): "
	     << "Exiting." << endl;
      }
    return;
  }

  // ctor:
  base_io::base_io (int rw_, int format_ , bool verbose_, bool debug_)
  {
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (2): "
	     << "Entering..." << endl;
      }
    _rw = rw_;
    __format = FORMAT_UNDEFINED;
    base_io::_set_default ();
    __verbose = verbose_;
    __debug = debug_;
    _default_locale = 0;
    _locale = 0;
    set_format (format_);
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::base_io (2): "
	     << "Exiting." << endl;
      }
    return;
  }
  
  // dtor:
  base_io::~base_io ()
  {
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::~base_io: "
	     << "Entering..." << endl;
      }
    if (_locale != 0)
      {
	delete _locale;
	_locale = 0;
      }
    if (_default_locale != 0)
      {
	delete _default_locale;
	_default_locale = 0;
      }
    base_io::_reset ();
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::~base_io: "
	     << "Exiting." << endl;
      }
    return;
  }

  void base_io::_reset ()
  {
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::_reset: "
	     << "Entering..." << endl;
      }
     if (_file != 0)
      {
	if (_file->IsOpen ())
	  {
	    _file->Close ();
	  }
	_file = 0;
      }
    if (! _filename.empty ())
      {
	_filename.clear ();
      }
    base_io::_set_default ();
    if (g_devel)
      {
	cerr << "DEVEL: " << "brio::base_io::_reset: "
	     << "Exiting." << endl;
      }
    return;
  }  
  

  void base_io::get_list_of_stores (list<string> & list_) const
  {
    list_.clear ();
    for (store_info_dict_t::const_iterator i = _store_infos.begin ();
	 i != _store_infos.end ();
	 i++)
      {
	list_.push_back (i->first);
      }
    return;
  }
  
  store_info * base_io::_get_store_info (const string & label_)
  {
    if (label_.empty ())
      {
	// if label is not specified, use the current store if it exists:
	if (_current_store != 0)
	  {
	    return _current_store;
	  }
      }
    else
      {
	// first we check the current store:
	if ((_current_store != 0) && (_current_store->label == label_))
	  {
	    return _current_store;
	  }
	// then we search from the stores' dictionnary :
	store_info_dict_t::iterator found = _store_infos.find (label_);
	if (found != _store_infos.end ())
	  {
	    return &(found->second);
	  }
      }
    return 0;
  }

  const store_info * base_io::_get_store_info (const string & label_) const
  {
    const store_info * c_ptr_si = 0;
    base_io * ptr_io = const_cast<base_io *> (this);
    store_info * ptr_si = ptr_io->_get_store_info (label_);
    c_ptr_si = const_cast<store_info *> (ptr_si);
    return c_ptr_si; 
  }

  void base_io::tree_dump (std::ostream & out_, 
			   const std::string & title_, 
			   const std::string & indent_, 
			   bool inherit_) const
  {
    using namespace datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }

    out_ << indent << i_tree_dumpable::tag << "Debug : " << __debug  << endl;

    out_ << indent << i_tree_dumpable::tag << "Verbose : " << __verbose  << endl;

    out_ << indent << i_tree_dumpable::tag << "Format: '" 
	 << (__format == FORMAT_QPBA ? base_io::QPBA_LABEL : base_io::TEXT_LABEL) 
	 << "'" << endl;

    out_ << indent << i_tree_dumpable::tag
	 << "Opened: " << is_opened () << endl;
    if (is_opened ())
      {
	out_ << indent << i_tree_dumpable::skip_tag 
	     << i_tree_dumpable::tag 
	     << "Filename: '" << _filename << "'" << endl;
	out_ << indent << i_tree_dumpable::skip_tag 
	     << i_tree_dumpable::last_tag 
	     << "File: " << hex << _file << dec << " (ROOT TFile)" << endl;
      }
 
    out_ << indent << i_tree_dumpable::tag << "Stores: ";
    if (_store_infos.size () > 0) 
      {
	out_ << _store_infos.size ();
      }
    else
      {
	out_ << "<none>";
      }
    out_ << endl;
    for (store_info_dict_t::const_iterator i = _store_infos.begin ();
	 i != _store_infos.end ();
	 i++)
      {
	const string & label = i->first;
	const store_info & si = i->second;
	store_info_dict_t::const_iterator j = i;
	j++;
	string tag = "|-- ";
	string skip_tag = "|   ";
	
	if (j ==  _store_infos.end ())
	  {
	    tag = "`-- ";
	    skip_tag = "    ";
	  }
	out_ << indent << "|   " << tag << "Store label: '" << si.label << "' : " << endl;
	out_ << indent << "|   " << skip_tag << "|-- " << "Serialization tag = ";
	if (si.has_dedicated_serialization_tag ())
	  {
	    out_ << indent << "'" << si.get_serialization_tag () << "'";
	  }
	else
	  {
	    out_ << "<mixed>";
	  }
	out_ << endl;
	out_ << indent << "|   " << skip_tag << "|-- " << "Number of entries = " << si.number_of_entries << " " << endl;
	out_ << indent << "|   " << skip_tag << "`-- " << "Current entry     = ";
	if (si.current_entry < 0) 
	  {
	    out_ << "<rewind>";
	  }
	else if (si.current_entry >= si.number_of_entries) 
	  {
	    out_ << "<unwind>";
	  }
	else
	  {
	    out_ << '#' << si.current_entry << " in [ 0 : " << (si.number_of_entries - 1) << " ]"; 
	  }
	out_ << endl;
      }

    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) << "Current store: ";
      if (_current_store == 0)
	{
	  out_ << "<none>";
	}
      else
	{
	  out_ << "'" << _current_store->label << "'";
	}
      out_ << endl;
    }

    return;
  }
    
} // end of namespace brio

// end of base_io.cc
