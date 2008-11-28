// properties_commander.cc
  
#include <datatools/utils/properties_commander.h>

namespace datatools {

  namespace utils {

    bool 
    properties_commander_i::is_debug () const
    {
      return __debug;
    }
    
    void 
    properties_commander_i::set_debug (bool debug_)
    {
      __debug = debug_;
    }

    properties *
    properties_commander_i::get_prop ()
    {
      return __prop;
    }

    const properties *
    properties_commander_i::get_prop () const
    {
      return __prop;
    }

    void 
    properties_commander_i::reset ()
    {
      __prop = 0;
    }

    void 
    properties_commander_i::set_properties (properties & prop_)
    {
      /*
      std::clog << "DEVEL: properties_commander_i::set_properties: "
		<< &prop_ << std::endl;
      */
      __prop = & prop_;
      /*
      std::clog << "DEVEL: properties_commander_i::set_properties: "
		<< __prop << " done." << std::endl;
      */
    }

    properties_commander_i::properties_commander_i (bool debug_)
    {
      __prop = 0;
      set_debug (debug_);
    }
    
    properties_commander_i::properties_commander_i (properties & prop_, 
						    bool debug_)
    {
      set_properties (prop_);
      set_debug (debug_);
    }
    
    void 
    properties_commander_i::do_invoke (command & command_)
    {
      using namespace std;
      //clog << "DEVEL: properties_commander_i::do_invoke: entering... " << endl;
      if (__prop == 0)
	{
	  throw runtime_error ("No 'properties' target!");
	}

      std::string command_name = command_.get_name ();
      size_t      nargs        = command_.get_number_of_arguments ();
      size_t      nopts        = command_.get_number_of_options ();
      command_.reset_output ();

      bool lock_option = false;

      if (command_name == "dump")
	{
	  //string key = command_.get_argument (0);
	  //bool hk = __prop->has_key (key);
	  string title = "Current properties";
	  __prop->tree_dump (std::clog, title);
	  //clog << "DEVEL: properties_commander_i::do_invoke: exiting." << endl;
	  return;
	}

      if (command_name == "has")
	{
	  
	  if (nargs < 1) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's key!");
	    }
	  string key = command_.get_argument (0);
	  bool hk = __prop->has_key (key);
	  command_.set_returns (hk? "1": "0");
	  return;
	}

      if (command_name == "flag")
	{
	  if (nargs < 1) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's key!");
	    }
	  string key  = command_.get_argument (0);
	  string desc = command_.get_argument (1);
	  __prop->store_flag (key, desc, lock_option);
	  //clog << "DEVEL: properties_commander_i::do_invoke: exiting. " << endl;
	  return;
	}

      if (command_name == "unflag")
	{
	  if (nargs < 1) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's key!");
	    }
	  string key  = command_.get_argument (0);
	  __prop->clean (key);
	  //clog << "DEVEL: properties_commander_i::do_invoke: exiting. " << endl;
	  return;
	}

      if (command_name == "unset")
	{
	  if (nargs < 1) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's key!");
	    }
	  string key  = command_.get_argument (0);
	  __prop->clean (key);
	  //clog << "DEVEL: properties_commander_i::do_invoke: exiting. " << endl;
	  return;
	}

      if (command_name == "set")
	{
	  //clog << "DEVEL: properties_commander_i::do_invoke: set..." << endl;
	  if (nargs < 1) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's key!");
	    }
	  
	  bool is_public  = true;
	  bool is_bool    = false;
	  bool is_integer = false;
	  bool is_real    = false;
	  bool is_string  = true; // default type
	  bool is_vector  = false;
	  bool is_locked  = false;
	  int size = 1;

	  for (int i = 0; i < nopts; i++)
	    {
	      string opt = command_.get_option (i);

	      string prefix = "-v=";

	      if ((opt.size () >= prefix.size ()) 
		  && (opt.substr(0,3) == prefix))
		{
		  istringstream iss (opt.substr(3));
		  int sz;
		  iss >> sz;
		  if (! iss)
		    {
		      throw datatools::utils::invalid_argument ("Invalid size format!");      
		    }
		  if (sz < 0)
		    {
		      throw datatools::utils::invalid_argument ("Invalid size value!");      
		    }
		  size = sz;
		  is_vector = true;
		}
	    }

	  if (nopts > 0)
	    {
	      if (command_.has_option ("-l")) is_locked = true;
	      if (command_.has_option ("-p")) is_public = false;
	      if (command_.has_option ("-b")) 
		{
		  is_bool = true;
		  is_string = false;
		}
	      if (command_.has_option ("-i")) 
		{
		  is_integer = true;
		  is_string = false;
		}
	      if (command_.has_option ("-r")) 
		{
		  is_real = true;
		  is_string = false;
		}
	      if (command_.has_option ("-s")) 
		{
		  is_string = true;
		}
	    }

	  string key  = command_.get_argument (0);
	  if (! is_public)
	    {
	      key = properties::make_private_key (key);
	    }
	  
	  vector<bool>    bvals;
	  vector<int32_t> ivals;
	  vector<double>  rvals;
	  vector<string>  svals;
	  if (is_bool)    bvals.assign (size, false);
	  if (is_integer) ivals.assign (size, 0);
	  if (is_real)    rvals.assign (size, 0);
	  if (is_string)  svals.assign (size, "");

	  int val_count = 0;
	  for (int iarg = 0; iarg < size; iarg++)
	    { 
	      string valstr = command_.get_argument (iarg + 1);
	      if (valstr.empty ()) break;
	      val_count++;
	      if (is_bool)
		{
		  istringstream iss (valstr);
		  bool b;
		  iss >> b;
		  if (! iss ) 
		    {
		      throw datatools::utils::invalid_argument ("Invalid format for boolean value!");
		    }
		  bvals[iarg] = b;
		}

	      if (is_integer)
		{
		  istringstream iss (valstr);
		  int i;
		  iss >> i;
		  if (! iss ) 
		    {
		      throw datatools::utils::invalid_argument ("Invalid format for integer value!");
		    }
		  ivals[iarg] = i;
		}

	      if (is_real)
		{
		  istringstream iss (valstr);
		  double d;
		  iss >> d;
		  if (! iss ) 
		    {
		      throw datatools::utils::invalid_argument ("Invalid format for real value!");
		    }
		  rvals[iarg] = d;
		}

	      if (is_string)
		{
		  svals[iarg] = valstr;
		}

	    }

	  // check the number of values:
	  if (val_count < size) 
	    {
	      throw datatools::utils::missing_argument ("Missing property's value!");
	    }

	  // parse property description (if any):
	  string desc;
	  /*
	  ostringstream oss;
	  for (int iarg = 2; iarg < (int) nargs; iarg++)
	    {
	      oss << command_.get_argument (iarg);
	    }
	  desc = oss.str ();
	  */

	  if (! is_vector)
	    {
	      //clog << "DEVEL: ! is_vector..." << endl;
	      if (is_bool)
		{
		  __prop->store (key, bvals[0], desc, is_locked);
		}
	      else if (is_integer)
		{
		  __prop->store (key, ivals[0], desc, is_locked);
		}
	      else if (is_real)
		{
		  __prop->store (key, rvals[0], desc, is_locked);
		}
	      else if (is_string)
		{
		  __prop->store (key, svals[0], desc, is_locked);
		}
	    }
	  else
	    {
	      //clog << "DEVEL: is_vector..." << endl;
	      if (is_bool)
		{
		  __prop->store (key, bvals, desc, is_locked);
		}
	      else if (is_integer)
		{
		  __prop->store (key, ivals, desc, is_locked);
		}
	      else if (is_real)
		{
		  __prop->store (key, rvals, desc, is_locked);
		}
	      else if (is_string)
		{
		  __prop->store (key, svals, desc, is_locked);
		}
	    }
	  //clog << "DEVEL: properties_commander_i::do_invoke: exiting set." << endl;
	  return;
	}

 
      //clog << "DEVEL: properties_commander_i::do_invoke: exiting
      //with `throw`." << endl;
      throw datatools::utils::command_not_found (command_name);
    }


    /***********************************************/

    bool 
    properties_commander::allows_map_commands () const
    {
      return __allow_map_commands;
    }

    void 
    properties_commander::set_allows_map_commands (bool amc_)
    {
      __allow_map_commands = amc_;
    }

    bool 
    properties_commander::is_debug () const
    {
      return __debug;
    }
    
    void 
    properties_commander::set_debug (bool debug_)
    {
      __debug = debug_;
      __i.set_debug(__debug);
    }

    properties_commander::properties_commander (bool debug_)
    {
      __debug = debug_;
    }

    properties_commander::properties_commander (properties & prop_, 
						bool debug_)
    {
      set_debug (debug_);
      set_current_target (prop_);
    }

    int 
    properties_commander::run (properties & prop_, 
			       command & command_)
    {
      if (__debug) std::clog << "DEVEL: properties_commander::run (properties, command): "
			     << "entering..." << std::endl;
      set_current_target (prop_);
      return this->run (command_);
    }

    void 
    properties_commander::set_current_target (properties & prop_)
    {
      __i.set_properties (prop_);      
    }

    properties_commander::~properties_commander ()
    {
      __reset ();
    }

    int 
    properties_commander::switch_target (std::string name_)
    {
      pmap_t::iterator found = __targets.find (name_);
      if (found == __targets.end ())
	{
	  std::cerr << "properties_commander::switch_target: "
		    << "no target named '" << name_ << "'!"
		    << std::endl;
	  return command::ERROR;
	}
      const pentry_t & pe = found->second;
      properties & prop = *pe.second;
      /*
      std::clog << "DEVEL: properties_commander::switch_target: "
		<< & prop << std::endl;
      */
      set_current_target (prop);
      return command::SUCCESS;
    }

    int 
    properties_commander::remove_target (std::string name_)
    {
      pmap_t::iterator found = __targets.find (name_);
      if (found != __targets.end ())
	{
	  std::string target_name = found->first;
	  pentry_t & pe = found->second;
	  if (pe.first)
	    {
	      if (pe.second != 0)
		{
		  delete pe.second;
		}
	    }
	  pe.second = 0;
	  __targets.erase (found);
	  return command::SUCCESS;
	}
      std::cerr << "properties_commander::remove_target: "
		<< "a target with name '" << name_ << "' does not exist!"
		<< std::endl;
      return command::ERROR;      
    }

    int 
    properties_commander::create_target (std::string name_)
    {
      pmap_t::iterator found = __targets.find (name_);
      if (found != __targets.end ())
	{
	  std::cerr << "properties_commander::create_target: "
		    << "a target named '" << name_ << "' already exists!"
		    << std::endl;
	  return command::ERROR;
	}
      pentry_t prop_entry;
      // allocate a new properties object:
      prop_entry.first  = true; // memory management flag
      prop_entry.second = new properties (name_);
      __targets[name_]  = prop_entry;
      return command::SUCCESS;
    }

    int 
    properties_commander::add_target (std::string name_, 
				      properties & prop_, 
				      bool current_)
    {
      pmap_t::iterator found = __targets.find (name_);
      if (found != __targets.end ())
	{
	  std::cerr << "properties_commander::add_target: "
		    << "name '" << name_ << "' already used!"
		    << std::endl;
	  return command::ERROR;
	}
      pentry_t prop_entry;
      prop_entry.first  = false;
      prop_entry.second = &prop_;
      __targets[name_] = prop_entry;
      if (current_)
	{
	  set_current_target (prop_);
	}
      return command::SUCCESS;
    }

    int 
    properties_commander::run (std::string name_, command & command_)
    {
      if (__debug) std::clog << "DEVEL: properties_commander::run (string, command): "
			     << "entering..." << std::endl;
      pmap_t::iterator found = __targets.find (name_);
      if (found != __targets.end ())
	{
	  properties * ptr = found->second.second;
	  if (ptr != 0)
	    {
	      set_current_target (*ptr);
	      return this->run (command_);
	    }
	}
      std::cerr << "properties_commander::run: target '" 
		<< name_ << "' not found!" << std::endl;
      return command::ERROR;
    }

    int 
    properties_commander::run (command & command_)
    {
      if (__debug) std::clog << "DEVEL: properties_commander::run (command): "
			     << "entering..." << std::endl;
      return this->__run (command_);
    }

    void 
    properties_commander::list_targets (std::ostream & out_) const
    {
      out_ << "List of targets: ";
      if (__targets.size () == 0) 
	{
	  out_ << "<empty>";
	}
      else 
	{
	  out_ << __targets.size () << " item(s)";
	}
      out_ << std::endl;
      for (pmap_t::const_iterator i = __targets.begin ();
	   i !=  __targets.end ();
	   i++)
	{
	  std::string target_name = i->first;
	  const pentry_t & pe = i->second;
	  const properties & prop = *pe.second;
	  bool current = false;
	  current = (&prop == __i.get_prop());
	  bool memory_handle = pe.first;
	  //const properties & prop = *pe.second;
	  out_ << "  " << target_name << " : ";
	  out_ << (memory_handle ? "internal": "external") << " ";
	  out_ << "[" << &prop << "] ";
	  out_ << (current ? "(current)": "");
	  out_ << std::endl;
	}
    }

    void 
    properties_commander::print_help (std::ostream & out_) const
    {
      using namespace std;
      out_ << "`properties_commander` online help" << endl;
      out_ << endl;
      out_ << "Commander actions:" << endl;
      out_ << " .help          : print this help" << endl;
      out_ << " .debug         : switch on the debug mode" << endl;
      out_ << " .nodebug       : switch off the debug mode" << endl;
      out_ << " .list          : list targets" << endl;
      out_ << " .switch <name> : switch target <name>" << endl;
      out_ << " .clear         : clear all targets" << endl;
      out_ << " .create <name> : new target <name>" << endl;
      out_ << " .remove <name> : remove target <name>" << endl;
      out_ << endl;
      out_ << "Properties manipulation actions:" << endl;
      out_ << endl;
      out_ << " has <key>    : check if the current properties object \n"
	   << "                owns a property named '<key>'"<< endl;
      out_ << " flag <key>   : add a flag named '<key>' in the \n"
	   << "                current properties object"<< endl;
      out_ << " unflag <key> : remove a flag named '<key>' from \n"
	   << "                the current properties object" << endl;
      out_ << " set [-l] [-v=<size>] [-b|-i|-r|-s] <key> <value> [<desc>]: \n"
	   << "                add a property named '<key>' in the \n"
	   << "                current properties object" << endl;
      out_ << "              Options:  " << endl;
      out_ << "                -l : locked  property" << endl;
      out_ << "                -p : private property" << endl;
      out_ << "                -b : boolean property" << endl;
      out_ << "                -i : integer property" << endl;
      out_ << "                -r : real    property" << endl;
      out_ << "                -s : string  property (default)" << endl;
      out_ << "                -v=<size> : vector of size <size>" << endl;
      out_ << " unset <key>  : remove a property named '<key>' from \n"
	   << "                the current properties object" << endl;
      out_ << " dump :         dump the current properties object" << endl;
      out_ << endl;
    }

    int 
    properties_commander::_local_run (command & command_)
    {
      using namespace std;
      if (__debug) clog << "DEVEL: properties_commander::_local_run: "
			     << "entering..." << endl;
      command_.reset_output ();

      std::string command_name = command_.get_name ();
      size_t      nargs        = command_.get_number_of_arguments ();
      //size_t      nopts        = command_.get_number_of_options ();

      if (command_name == ".help")
	{
	  print_help (std::clog);
	  return command::SUCCESS;
	}

      if (command_name == ".list")
	{
	  list_targets (std::clog);
	  return command::SUCCESS;
	}

      if (command_name == ".clear")
	{
	  if (! allows_map_commands ())
	    {
	      command_.set_error ();
	      command_.set_error_message ("Command not allowed!");
	      return command_.get_error_code ();	      
	    }
	  clear_targets ();
	  return command::SUCCESS;
	}

      if (command_name == ".switch")
	{
	  if (nargs < 1)
	    {
	      command_.set_error ();
	      command_.set_error_message ("Missing target's name!");
	    }
	  else
	    {
	      string target_name = command_.get_argument (0); 
	      int err = switch_target (target_name);
	      if (err != command::SUCCESS)
		{
		  command_.set_error ();
		  command_.set_error_message ("");
		}
	    }
	  return command_.get_error_code ();
	}

      if (command_name == ".create")
	{
	  if (! allows_map_commands ())
	    {
	      command_.set_error ();
	      command_.set_error_message ("Command not allowed!");
	      return command_.get_error_code ();	      
	    }
	  if (nargs < 1)
	    {
	      command_.set_error ();
	      command_.set_error_message ("Missing target's name!");
	    }
	  else
	    {
	      string target_name = command_.get_argument (0); 
	      int err = create_target (target_name);
	      if (err != command::SUCCESS)
		{
		  command_.set_error ();
		  command_.set_error_message ("");
		}
	    }
	  return command_.get_error_code ();
	}

      if (command_name == ".remove")
	{
	  if (! allows_map_commands ())
	    {
	      command_.set_error ();
	      command_.set_error_message ("Command not allowed!");
	      return command_.get_error_code ();	      
	    }
	  if (nargs < 1)
	    {
	      command_.set_error ();
	      command_.set_error_message ("Missing target's name!");
	    }
	  else
	    {
	      string target_name = command_.get_argument (0); 
	      int err = remove_target (target_name);
	      if (err != command::SUCCESS)
		{
		  command_.set_error ();
		  command_.set_error_message ("");
		}
	    }
	  return command_.get_error_code ();
	}

      if (command_name == ".debug")
	{
	  set_debug (true);
	  return command::SUCCESS;
	}

      if (command_name == ".nodebug")
	{
	  set_debug (false);
	  return command::SUCCESS;
	}

      command_.set_pass ();
      command_.set_error_message ("unknown local command!");

      return command_.get_error_code ();
    }

    int 
    properties_commander::__run (command & command_)
    {
      if (__debug) std::clog << "DEVEL: properties_commander::__run: "
			     << "entering..." << std::endl;
      int err = command::SUCCESS;
      err = _local_run (command_);
      if (command::is_pass (err))
	{
	  err = command::SUCCESS;
	  command_.reset_output ();
	  if (! command_.is_with_options ())
	    {
	      command_.force_with_options ();
	    }
	  err = i_invokable::invoke (__i, command_);
	  /*
	    std::clog << "DEVEL: properties_commander::__run: err=" 
	    << err << std::endl;
	  */
	}
      if (__debug) std::clog << "DEVEL: properties_commander::__run: "
			     << "exiting." << std::endl;
      return err;
    }

    int 
    properties_commander::clear_targets ()
    {
      __reset_targets ();
      return command::SUCCESS;
    }

    void 
    properties_commander::__reset_targets ()
    {
      // remove 'properties' targets in the '__targets' dictionary:
      for (pmap_t::iterator i = __targets.begin ();
	   i !=  __targets.end ();
	   i++)
	{
	  std::string target_name = i->first;
	  pentry_t &  pe = i->second;
	  /* if the commander manage the dynamic allocated memory,
	   * we *delete* the object; 
	   * otherwise just set the pointer == 0: 
	   */
	  if (pe.first)
	    {
	      if (pe.second != 0)
		{
		  delete pe.second;
		}
	    }
	  pe.second = 0;
	}
      __targets.clear ();
    }

    void 
    properties_commander::__reset ()
    {
      __i.reset ();
      __reset_targets ();
    }

  } // end of namespace utils 

} // end of namespace datatools

// end of properties_commander.cc
