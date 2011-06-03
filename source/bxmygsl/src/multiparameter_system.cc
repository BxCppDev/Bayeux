// mygsl::multiparameter_system.cc 

#include <mygsl/multiparameter_system.h>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>

#include <mygsl/param_entry.h>


namespace mygsl {

  void multiparameter_system::load_params (const std::string & filename_)
  {
    load_parameters (filename_);
    return;
  }

  void multiparameter_system::store_params (const std::string & filename_) const
  {
    store_parameters (filename_);
    return;
  }

  bool multiparameter_system::is_lock_params () const
  {
    return __lock_params;
  }
  
  size_t multiparameter_system::get_number_of_params () const
  {
    return __params.size ();
  }
  
  size_t multiparameter_system::get_number_of_free_params () const
  {
    return __free_params.size ();
  }
  
  size_t multiparameter_system::get_number_of_auto_params () const
  {
    return __auto_params.size ();
  }
  
  size_t multiparameter_system::get_number_of_const_params () const
  {
    return __const_params.size ();
  }

  void multiparameter_system::init ()
  {
    lock_params ();
    for (int i = 0; i < __auto_params.size (); i++)
      {
	/*
	cerr << "DEVEL: multiparameter_system::init: " 
	     << "check wheter automatic parameter '" 
	     << __auto_params[i]->get_name ()
	     << "' has been computed..." << endl;
	*/
	if (! __auto_params[i]->is_auto_computed ())
	  {
	    ostringstream message;
	    message << "multiparameter_system::init: "
		    << "automatic parameter '" << __auto_params[i]->get_name ()
		    << "' has not been computed! Please do it in the overloaded 'compute_auto_params' method!";
	    throw runtime_error (message.str ());
	  }
      }
  }

  void multiparameter_system::reset ()
  {
    bool devel = false;
    if (devel)
      {
	clog << "DEVEL: multiparameter_system::reset: "
	     << "entering..."
	     << endl;
      }
    if (__lock_params) 
      {
	unlock_params ();
      }
    if (devel)
      {
	clog << "DEVEL: multiparameter_system::reset: "
	     << "clearing free/auto/const params arrays..."
	     << endl;
      }
    __free_params.clear ();
    __auto_params.clear ();
    __const_params.clear ();
    if (devel)
      {
	clog << "DEVEL: multiparameter_system::reset: "
	     << "done."
	     << endl;
	
	clog << "DEVEL: multiparameter_system::reset: "
	     << "deleting params..."
	     << endl;
      }
    for (int i = 0; i < __params.size (); i++)
      {
	if (__params[i] != 0)
	  {
	    delete __params[i];
	  }
	__params[i] = 0;
      }    
    if (devel)
      {
	clog << "DEVEL: multiparameter_system::reset: "
	     << "done."
	     << endl;
      }
    __params.clear ();
  }

  void multiparameter_system::lock_params ()
  {
    if (__lock_params) 
      {
	unlock_params ();
      }	
    compute_automatic_params ();
    __lock_params = true;	
  }

  void multiparameter_system::unlock_params ()
  {
    __lock_params = false;
  }
  
  bool multiparameter_system::has_param_with_name (const string & name_) const
  {
    return find_if (__params.begin (), 
		    __params.end (), 
		    param_ptr_has_name (name_)) != __params.end ();
  }

  const param_entry & multiparameter_system::get_param_by_name (const string & name_) const
  {
    params_col::const_iterator found = find_if (__params.begin (), 
				__params.end (), 
				param_ptr_has_name (name_));
    if (found == __params.end ())
      {
	ostringstream message;
	message << "multiparameter_system::get_param_by_name: No parameter named '" << name_ << "'!"; 
	throw runtime_error (message.str ());
      }
    return **found;
  }

  param_entry & multiparameter_system::get_param_by_name (const string & name_)
  {
    params_col::iterator found = find_if (__params.begin (), 
				__params.end (), 
				param_ptr_has_name (name_));
    if (found == __params.end ())
      {
	ostringstream message;
	message << "multiparameter_system::get_param_by_name: No parameter named '" << name_ << "'!"; 
	throw runtime_error (message.str ());
      }
    return **found;
  }

  const param_entry & multiparameter_system::get_param (int i_) const
  {
    return *(__params[i_]);
  }

  param_entry & multiparameter_system::get_param (int i_)
  {
    return *(__params[i_]);
  }

  const param_entry & multiparameter_system::get_free_param (int i_) const
  {
    return * (__free_params[i_]);
  }

  param_entry & multiparameter_system::get_free_param (int i_)
  {
    return *__free_params.at (i_);
  }

  const param_entry & multiparameter_system::get_auto_param (int i_) const
  {
    return * (__auto_params[i_]);
  }

  param_entry & multiparameter_system::get_auto_param (int i_)
  {
    return *__auto_params.at (i_);
  }

  void multiparameter_system::add_param (const param_entry & pe_, const string & comment_)
  {
    bool devel = false;
    if (is_lock_params ()) 
      {
	std::ostringstream message;
	message << "multiparameter_system::add_param: ";
	message << "system is locked!";
	throw std::runtime_error (message.str ());            
      }
    if (std::find_if (__params.begin (), 
		      __params.end (), 
		      param_ptr_has_name (pe_.get_name ())) != __params.end ()) 
      {
	std::ostringstream message;
	message << "multiparameter_system::add_param: ";
	message << "parameter '" << pe_.get_name () << "' already exist!";
	throw std::runtime_error (message.str ());       
      }

    if (devel) pe_.print (clog, "multiparameter_system::add_param: New param", "DEVEL: ");

    param_entry * new_pe = new param_entry (pe_);
    if (devel) new_pe->print (clog, "multiparameter_system::add_param: New entry", "DEVEL: ");
    __params.push_back (new_pe);
    if (! comment_.empty ())new_pe->set_comment (comment_);
    params_col::reverse_iterator last = __params.rbegin ();
    //clog << "DEVEL: last_pe= " << last  << endl;

    if (devel) 
      {
	for (int i = 0; i < __params.size (); i++)
	  {
	    __params[i]->print (clog, "multiparameter_system::add_param: List of params", "DEVEL: ");
	  }
      }
    param_entry * last_pe = *last; 
    if (devel) 
      {
	last_pe->print (clog, "multiparameter_system::add_param: Last entry: ", "DEVEL: ");
	clog << "DEVEL: last_pe= " << last_pe << endl;
	clog << "DEVEL: parameter '" << last_pe->get_name () << "' pushed!" << endl;
	last_pe->print (clog, "multiparameter_system::add_param", "DEVEL: ");
      }
    if (last_pe->is_free ())
      {
	__free_params.push_back (last_pe);
	clog << "DEVEL: free '" << last_pe->get_name () << "' pushed!" << endl;
      }
    else if (last_pe->is_auto ())
      {
	__auto_params.push_back (last_pe);
	clog << "DEVEL: auto '" << last_pe->get_name () << "' pushed!" << endl;
      }
    else if (last_pe->is_const ())
      {
	__const_params.push_back (last_pe);
	clog << "DEVEL: const '" << last_pe->get_name () << "' pushed!" << endl;
      }
  }

  void multiparameter_system::compute_automatic_params ()
  {
    if (get_number_of_auto_params () > 0)
      {
	throw runtime_error ("multiparameter_system::compute_automatic_params: Not implemented! Please overload this virtual method for your own inherited class!");
      }      
  }

  // ctor:
  multiparameter_system::multiparameter_system ()
  {
    __lock_params = false;
  }

  // dtor:
  multiparameter_system::~multiparameter_system ()
  {
    reset ();
  }

  void multiparameter_system::print_line (std::ostream & out_) const
  {
    for (int i = 0; i < __params.size (); i++)
      {
	out_ << __params[i]->get_value () << ' ';
      }
  }
      
  void multiparameter_system::print (std::ostream & out_, 
				     const string & title_, 
				     const string & indent_) const
  {
    string indent = indent_;
    string tag    = "|-- ";
    string ltag   = "`-- ";
    string stag   = "|   ";
    string sltag  = "    ";
    if (! title_.empty ())
      {
	out_ << indent << title_ << ":" << endl;
      }
    out_ << indent << tag << "Locked: " << __lock_params << endl;
    out_ << indent << tag << "Number of parameters : " << __params.size () << endl;
    out_ << indent << stag << tag << "Number of free parameters  : " << __free_params.size () << endl;
    out_ << indent << stag << tag << "Number of const parameters : " << __const_params.size () << endl;
    out_ << indent << stag << ltag << "Number of auto parameters  : " << __auto_params.size () << endl;
    out_ << indent << ltag << "Parameters: " << endl;
    for (int i = 0; i < __params.size (); i++)
      {
	string atag = tag;
	string atag2 = stag;
	if (i == (__params.size () - 1)) 
	  {
	    atag  = ltag;
	    atag2 = sltag;
	  }
	ostringstream title_ss, indent_ss;
	title_ss << atag << "Parameter '" << __params[i]->get_name () << "'";
	indent_ss << indent << sltag << atag2; // << sltag << atag2;
	out_ << indent << sltag << title_ss.str () << ":" << endl;
	__params[i]->print (out_, "", indent_ss.str ()); 
      }
  }

  void multiparameter_system::print_status (ostream & out_) const
  {
    out_ << "Multi-parameter system status: " << endl;
    for (int i = 0; i < get_number_of_params (); i++)
      {
	get_param (i).print_status (out_);
      }
    out_ << endl;
  }

  void multiparameter_system::load_parameters (const std::string & filename_)
  {
    std::ifstream f (filename_.c_str ());
    if (! f)
      {
	std::ostringstream message;
	message << "multiparameter_system::load_parameters: Cannot open file '"
		<< filename_ << "'!";
	throw std::runtime_error (message.str ());
      }
    load_parameters (f);
    f.close ();
  }
 
  void multiparameter_system::load_parameters (std::istream & in_ )
  {
    bool local_debug = false; 
    //local_debug = true; 
    if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: entering..." << endl; 
    this->unlock_params ();
    while (in_) 
      { 
	string line; 
	getline (in_, line);
	if (in_.eof ()) 
	  { 
	    clog << "NOTICE: multiparameter_system::load_parameters: " 
		 << "End of stream has been reached." << endl;
	    break;
	  } 
	{
	  istringstream line_iss (line); 
	  string word;
	  line_iss >> word;
	  if (word.empty ())
	    { 
	      if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: skipping blank line..." << endl;
	      continue; 
	    }
	  if (word[0] == '#') 
	    { 
	      if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: skipping commented line..." << endl;
	      continue;
	    }
	} 
	if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: parsing line '" << line << "'" << endl;
	
	
	istringstream line_iss (line); 
	string name, type, comment;
	double min, max, value, step;
       
	string token;
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> name;
	    if (! token_iss)
	      {
		throw runtime_error (" multiparameter_system::load_parameters: Invalid format!");
	      }
	  }
	
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> type;
	    if (! token_iss)
	      {
		throw runtime_error (" multiparameter_system::load_parameters: Invalid format!");
	      }
	    if ((type != param_entry::FREE_LABEL) 
		&& (type != param_entry::AUTO_LABEL) 
		&& (type != param_entry::CONST_LABEL))
	      {
		throw runtime_error (" multiparameter_system::load_parameters: Invalid type!");
	      }
	  }
	
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> min;
	    if (! token_iss)
	      {
		min = param_entry::NO_MIN_VALUE;
	      }
	  }
	
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> max;
	    if (! token_iss)
	      {
		max = param_entry::NO_MAX_VALUE;
	      }
	  }
	
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> value;
	    if (! token_iss)
	      {
		value = param_entry::NO_VALUE;
	      }
	  }
	
	getline (line_iss, token, ':');
	if (line_iss)
	  {
	    istringstream token_iss (token);
	    token_iss >> step;
	    if (! token_iss)
	      {
		step = param_entry::AUTO_STEP;
	      }
	  }
	
	getline (line_iss, comment);
	if (! line_iss)
	  {
	    comment = "";
	  }

	if (local_debug) 
	  {
	    clog << "DEVEL: multiparameter_system::load_parameters: " << endl;
	    clog << "|-- name:    " << name    << endl;
	    clog << "|-- type:    " << type    << endl;
	    clog << "|-- min:     " << min     << endl;
	    clog << "|-- max:     " << max     << endl;
	    clog << "|-- value:   " << value   << endl;
	    clog << "|-- step:    " << step    << endl;
	    clog << "`-- comment: " << comment << endl;
	  }

	typedef mygsl::param_entry pe;

	if (type == param_entry::CONST_LABEL)
	  {
		add_param (pe::make_const (name, 
					   value),
			   comment);
	  }
	
	if (type == param_entry::FREE_LABEL)
	  {
	    if (isfinite (min) && isfinite (max))
	      {
		add_param (pe::make_free_range (name, 
						min, 
						max, 
						value, 
						step),
			   comment);

	      } 
	    else if (isfinite (min) && ! isfinite (max))
	      {
		add_param (pe::make_free_min (name, 
					      min, 
					      value, 
					      step),
			   comment);

	      } 
	    else if (! isfinite (min) && isfinite (max))
	      {
		add_param (pe::make_free_min (name, 
					      max, 
					      value, 
					      step),
			   comment);

	      }
	    else 
	      {
		add_param (pe::make_free (name, 
					  value, 
					  step),
			   comment);
	      }
	  }

	if (type == param_entry::AUTO_LABEL)
	  {
	    if (isfinite (min) && isfinite (max))
	      {
		add_param (pe::make_auto_range (name, 
						min, 
						max, 
						step),
			   comment);

	      } 
	    else if (isfinite (min) && ! isfinite (max))
	      {
		add_param (pe::make_auto_min (name, 
					      min, 
					      step),
			   comment);

	      } 
	    else if (! isfinite (min) && isfinite (max))
	      {
		add_param (pe::make_auto_min (name, 
					      max, 
					      step),
			   comment);

	      }
	    else 
	      {
		add_param (pe::make_auto (name, 
					  step),
			   comment);
	      }
	  }
       } 
    this->lock_params ();
    if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: exiting." << endl;
  } 

  void multiparameter_system::store_parameters (const std::string & filename_) const
  { 
    std::ofstream f (filename_.c_str ());
    if (! f)
      {
	std::ostringstream message;
	message << "multiparameter_system::store_parameters: Cannot open file '"
		<< filename_ << "'!";
	throw std::runtime_error (message.str ());
      }
    f << "# mygsl::multiparameter_system: list of parameters: " << endl;
    f.width (12);
    f.setf (ios::left);
    f << "# Name";
      
    f << " : " ;

    f.width (6);
    f << "Type";

    f << " : " ;

    f.width (10);
    f << "Minimum";

    f << " : " ;
    
    f.width (10);
    f << "Maximum";

    f << " : " ;
    
    f.width (10);
    f << "Value";

    f << " : " ;
    
    f.width (10);
    f << "Step";

    f << " : " ;
    
    f.width (10);
    f << "Comment";
    
    f << endl;
    store_parameters (f);
    f << "# mygsl::multiparameter_system: end of list." << endl;
    f.close ();
  }
      
  void multiparameter_system::store_parameters (std::ostream & out_ ) const
  {

    for (int i = 0; i < get_number_of_params (); i++)
      {
	// print name:
	out_.width (12);
	out_.setf (ios::left);
	out_ << get_param (i).get_name ();

	out_ << " : " ;

	// print type (free/auto/const):
	out_.width (6);
	out_ <<  (get_param (i).is_free () ? param_entry::FREE_LABEL : (get_param (i).is_const () ? param_entry::CONST_LABEL : param_entry::AUTO_LABEL));

	out_ << " : " ;

	// print minimum value:
	out_.width (10);
	if (get_param (i).has_min ())
	  {
	    out_ << get_param (i).get_min ();
	  }
	else
	  {
	    out_ << ' ';
	  }

	out_ << " : ";

	// print maximum value:
	out_.width (10);
	if (get_param (i).has_max ())
	  {
	    out_ << get_param (i).get_max ();
	  }
	else
	  {
	    out_ << ' ';
	  }

	out_ << " : ";

	out_.width (10);
	// print effective value:
	if (get_param (i).has_value ())
	  {
	    out_ << get_param (i).get_value ();
	  }
	else
	  {
	    out_ << ' ';
	  }

	out_ << " : ";

	// print step size:
	out_.width (10);
	if (get_param (i).has_step ())
	  {
	    out_ << get_param (i).get_step ();
	  }
	else
	  {
	    out_ << ' ';
	  }

	out_ << " : ";

	// print comment:
	out_ << get_param (i).get_comment ();

	out_ << std::endl;
      }
   
  }

}

// end of mygsl::multiparameter_system.cc
