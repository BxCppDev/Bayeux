// -*- mode: c++ ; -*- 
/* i_cut.cc
 */

#include <cuts/i_cut.h>

namespace cuts {

  bool i_cut::g_debug = false;

  i_cut::scoped_cut_creator_db_t i_cut::g__cut_creator_db (new cut_creator_db (true));

  const bool i_cut::ACCEPT = true;
  const bool i_cut::REJECT = false;

  // ctor:
  i_cut::i_cut ()
  {
    __user_data = 0;
  }
  
  // dtor:
  i_cut::~i_cut ()
  {
  }
  
  bool i_cut::has_user_data () const
  {
    return __user_data != 0;
  }

  void * i_cut::_get_user_data ()
  {
    return __user_data;
  }

  void i_cut::set_user_data (void * user_data_)
  {
    __user_data = user_data_;
  }

  void i_cut::unset_user_data ()
  {
    __user_data = 0;
  }

  bool i_cut::operator() ()
  {
    return this->accept () == ACCEPT;
  }

  void i_cut::_prepare_cut ()
  {
  }

  void i_cut::_finish_cut ()
  {
  }

  bool i_cut::accept ()
  {
    _prepare_cut ();
    bool ok = _accept ();
    _finish_cut ();
    return ok;
  }

  /***************************************************/

  i_cut::cut_creator_db::cut_creator_db (bool test_)
  {
    bool debug = g_debug;
    //debug = true;
    if (debug)
      {
	clog << "DEVEL: i_cut::cut_creator_db::cut_creator_db: "
	     << "Entering: " 
	     << (test_? "TEST": "-") << endl;
      }
    return;    
  }

  i_cut::cut_creator_db::~cut_creator_db ()
  {
    if (g_debug)
      {
	clog << "DEVEL: i_cut::cut_creator_db::~cut_creator_db: Entering... " 
	     << endl;
      }
    return;
  }
 
  bool i_cut::cut_creator_db::has_cut_creator (const string & cut_id_) const
  {
    return __dict.find (cut_id_) != __dict.end ();
  }

  cut_creator_t & 
  i_cut::cut_creator_db::get_cut_creator (const string & cut_id_)
  {
    cut_creator_dict_t::iterator found = __dict.find (cut_id_);
    if (found == __dict.end ())
      {
	ostringstream message;
	message << "i_cut::cut_creator_db::get_cut_creator: "
		<< "No cut creator with ID='" << cut_id_ << "'!";
	throw runtime_error (message.str ());
      }
    return (found->second);
  }

  void i_cut::cut_creator_db::register_cut_creator (cut_creator_t cut_creator_, 
						    const string & cut_id_)
  {
    bool devel = g_debug;
    //devel = true;
    using namespace std;
    string cut_id = cut_id_;
    if (devel)
      {
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator: "
	     << "cut_id='" << cut_id << "'"
	     << endl;
      }
    if (has_cut_creator (cut_id))
      {
	ostringstream message;
	message << "i_cut::cut_creator_db::register_cut_creator: " 
		<< "Cut creator ID '" << cut_id_ << "' is already used "
		<< "within the cut factory dictionnary !";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator: "
	     << "new '" << cut_id << "' cut creator  ID !"
	     << endl;
      }
    
    if (cut_id.empty ())
      {
	ostringstream message;
	message << "i_cut::cut_creator_db::register_cut_creator: " 
		<< "Empty cut creator ID !";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator:  "
	     << "insert cut creator ID='" << cut_id << "'!"
	     << endl;
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator: "
	     << "with creator address='" << hex 
	     << (void *) cut_creator_ << dec << "'"
	     << endl;
      }
    pair<string, cut_creator_t> a_pair;
    a_pair.first = cut_id;
    a_pair.second = cut_creator_;
    __dict.insert (a_pair);
    size_t sz = get_dict ().size ();
    if (devel)
      {
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator: size="
	     << sz << " element" << (sz > 1? "s": "") << endl;
	clog << "DEVEL: i_cut::cut_creator_db::register_cut_creator: "
	     << "done."
	     << endl;
      }
    return;
  }

  void i_cut::cut_creator_db::dump_cut_creators (ostream & out_)
  {
    out_ << "List of cut creators in 'cut_creator_db::__dict': ";
    size_t sz = get_dict ().size ();
    out_ << sz << " element(s)" << endl;
    size_t count = 0; 
    for (cut_creator_dict_t::const_iterator it = get_dict ().begin ();
	 it != get_dict ().end ();
	 it++) 
      {
	count++;
	if (count == sz) out_ << "`-- "; 
	else out_ << "|-- ";
	out_ << it->first << ": " 
	     << hex << (void *) it->second 
	     << dec << endl;
      }
    out_ << "end." << endl;
    return;
  }
     
  i_cut::cut_creator_db & i_cut::get_cut_creator_db ()
  {
    if (! g__cut_creator_db) 
      {
	throw runtime_error ("i_cut::get_cut_creator_db: Library has a critical bug !");
      }
    return *(g__cut_creator_db.get ());
  }
  
} // end of namespace cuts

// end of dummy_cuts.cc
