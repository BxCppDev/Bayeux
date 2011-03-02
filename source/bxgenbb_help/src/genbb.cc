// -*- mode: c++; -*- 
// genbb.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */
 
#include <genbb_help/genbb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace genbb {
 
  using namespace std;

  bool genbb::is_initialized () const
  {
    return __initialized;
  }

  bool genbb::is_debug () const
  {
    return __debug;
  }
 
  void genbb::set_debug (bool d_)
  {
    __debug = d_;
    return;
  }

  void genbb::set_delete_conf_file (bool d_)
  {
    __delete_conf_file = d_;
    return;
  }

  void genbb::set_delete_log_files (bool d_)
  {
    __delete_log_files = d_;
    return;
  }

  void genbb::set_delete_data_files (bool d_)
  {
    __delete_data_files = d_;
    return;
  }

  void genbb::set_delete_tmp_dir (bool d_)
  {
    __delete_tmp_dir = d_;
    return;
  }

  const mygsl::rng & genbb::get_random () const
  {
    return __random;
  }

  mygsl::rng & genbb::get_random ()
  {
    return __random;
  }

  void genbb::__clean ()
  {
    if (__genbb_in != 0)
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::__clean: "
		 << "Deleting current GENBB data input file stream instance..." << endl;
	  }
	delete __genbb_in;
	__genbb_in = 0;
      }
    if (! __genbb_data.empty () && boost::filesystem::exists (__genbb_data.c_str ()))
      {
	if (__delete_data_files)
	  {
	    if (__debug)
	      {
		clog << "debug: " << "genbb::__clean: "
		     << "Removing previous GENBB data file '" << __genbb_data << "'..."
		     << endl;
	      }
	    unlink (__genbb_data.c_str ());
	  }
	__genbb_data = "";
      }
    if (! __genbb_log.empty () && boost::filesystem::exists (__genbb_log.c_str ()))
      {
	if (__delete_log_files)
	  {
	    if (__debug)
	      {
		clog << "debug: " << "genbb::__clean: "
		     << "Removing previous GENBB log file '" << __genbb_log << "'..."
		     << endl;
	      }
	    unlink (__genbb_log.c_str ());
	  }
	__genbb_log = "";
      }
    __genbb_data = "";
    __genbb_log = "";
    __buffer_count++;
    return;
  }
  
  // ctor:
  genbb::genbb () : i_genbb ()
  {
    __initialized = false;
    __debug = false;
    __test = false;

    __buffer_size = DEFAULT_BUFFER_SIZE;
    __buffer_item = 0;
    __event_count = 0;
    __delete_conf_file = false;
    __delete_log_files  = false;
    __delete_data_files = true;
    __delete_tmp_dir = false;

    __genbb_in = 0;
    __genbb_data = "";
    __genbb_conf = "";
    __genbb_log = "";
    __tmp_base_dir = "/tmp/${USER}";

    __decay_type = DECAY_TYPE_DBD;
    __decay_isotope = "Se82";  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;

    __seed = 0;
    __tmp_dir[0] = 0;
    __buffer_count = 0;
    return;
  }
  
  // dtor:
  genbb::~genbb ()
  {
    __clean ();
    if (__initialized)
      {
	reset ();
      }
    return;
  }

  void genbb::set_tmp_dir (const string & td_)
  {
    if (__initialized)
      {
	throw logic_error ("genbb::set_tmp_base_dir: Operation prohibited ! Object is locked !");
      }
    __forced_tmp_dir = td_;
    return;
  }

  void genbb::set_tmp_base_dir (const string & tbd_)
  {
    if (__initialized)
      {
	throw logic_error ("genbb::set_tmp_base_dir: Operation prohibited ! Object is locked !");
      }
    __tmp_base_dir = tbd_;
    return;
  }

  const string & genbb::get_tmp_base_dir () const
  {
    return __tmp_base_dir;
  }

  string genbb::get_tmp_dir () const
  {
    if (__forced_tmp_dir.empty ())
      {
	return string(__tmp_dir);
      }
    return __forced_tmp_dir;
  }

  void genbb::reset ()
  {
    if (! __initialized)
      {
	throw logic_error ("genbb::reset: Object is not initialized !");
      }
    __clean ();
    __test = false;
    __buffer_size = DEFAULT_BUFFER_SIZE;
    __buffer_item = 0;
    __buffer_count = 0;
    if (__genbb_in != 0)
      {
	delete __genbb_in;
      }
    __genbb_in = 0;
    __event_count = 0;

    if (! __genbb_conf.empty () && boost::filesystem::exists (__genbb_conf.c_str ()))
      {
	if (__delete_conf_file)
	  {
	    unlink (__genbb_conf.c_str ());
	  }
      }
    if (__delete_tmp_dir)
      {
	clog << "WARNING: genbb::reset: "
	     << "Temporary working directory deletion not implemented yet !"
	     << endl;
      }
    __tmp_dir[0] = 0;
    __forced_tmp_dir;

    __tmp_base_dir = "/tmp/${USER}";
    __decay_type = DECAY_TYPE_DBD;
    __decay_isotope = "Se82";  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;


    __seed = 0;
    __random.reset ();
    __genbb_conf = "";
    __genbb_conf_file.close ();
    __genbb_data = "";
    __genbb_log = "";
    __delete_conf_file = false;
    __delete_log_files  = false;
    __delete_data_files = true;
    __delete_tmp_dir = false;

    __initialized = false;
    return;
  }

  void genbb::initialize (const datatools::utils::properties & config_)
  {
    if (__initialized)
      {
	throw logic_error ("genbb::reset: Object is already initialized !");
      }

    if (config_.has_flag ("debug"))
      {
	__debug = true;
      }

    if (config_.has_key ("seed"))
      {
	long seed = config_.fetch_integer ("seed");
	if (seed < 0)
	  {
	    throw runtime_error ("genbb::initialize: Invalid seed value (>=0) !");
	  }
	__seed = seed;
      }

    if (config_.has_flag ("test"))
      {
	__test = true;
      }

    if (config_.has_key ("buffer_size"))
      {
	int bs = config_.fetch_integer ("buffer_size");
	if (bs <= 0)
	  {
	    throw runtime_error ("genbb::initialize: Invalid buffer size property !");
	  }
	if (bs >= MAX_BUFFER_SIZE)
	  {
	    throw runtime_error ("genbb::initialize: Buffer size is too large !");
	  }
	__buffer_size = bs;
      }

    if (config_.has_key ("tmp_base_dir"))
      {
	__tmp_base_dir = config_.fetch_string ("tmp_base_dir");
      }

    if (config_.has_flag ("delete_conf_file"))
      {
	set_delete_conf_file (true);
      }

    if (config_.has_flag ("preserve_conf_file"))
      {
	set_delete_conf_file (false);
      }

    if (config_.has_flag ("delete_log_file"))
      {
	set_delete_log_file (true);
      }

    if (config_.has_flag ("preserve_log_file"))
      {
	set_delete_log_file (false);
      }

    if (config_.has_flag ("delete_data_file"))
      {
	set_delete_log_file (true);
      }

    if (config_.has_flag ("preserve_data_file"))
      {
	set_delete_log_file (false);
      }

    if (config_.has_key ("decay_type"))
      {
	string tmp = config_.fetch_string ("decay_type");
	if ((tmp != "DBD") && (tmp != "background"))
	  {
	    ostringstream message;
	    message << "genbb::initialize: Invalid decay type '"
		    << tmp << "' !";
	    throw runtime_error (message.str());
	  }
	if (tmp == "background") 
	  {
	    __decay_type = DECAY_TYPE_BACKGROUND;
	  }

	if (tmp == "DBD") 
	  {
	    __decay_type = DECAY_TYPE_DBD;

	    if (! config_.has_key ("decay_dbd_level"))
	      {
		ostringstream message;
		message << "genbb::initialize: Missing DBD decay level !";
		throw runtime_error (message.str());
	      }
	    __decay_dbd_level = config_.fetch_integer ("decay_dbd_level");

	    if (! config_.has_key ("decay_dbd_mode"))
	      {
		ostringstream message;
		message << "genbb::initialize: Missing DBD decay mode !";
		throw runtime_error (message.str());
	      }
	    __decay_dbd_mode = config_.fetch_integer ("decay_dbd_mode");
	  }

     }
 
    //clog << "DEVEL: genbb::initialize: Random init..." << endl;
    __random.init ("taus2", __seed);
    //clog << "DEVEL: genbb::initialize: Done." << endl;
    
    /***************/

    // Temporary directory setup :
    {
      if (__forced_tmp_dir.empty ())
	{
	  if (__tmp_base_dir.empty ())
	    {
	      ostringstream message;
	      message << "genbb::initialize: Missing base temporary directory ! ";
	      throw runtime_error (message.str());
	    }
	  datatools::utils::fetch_path_with_env (__tmp_base_dir);
	  {
	    if (! boost::filesystem::is_directory (__tmp_base_dir))
	      {
		ostringstream message;
		message << "genbb::initialize: Base temporary directory '"
			<< __tmp_base_dir << "' does not exist ! you should first create it !";
		__clean ();
		throw runtime_error (message.str());
	      }
	  }
	
	  ostringstream oss;
	  oss << __tmp_base_dir << '/' << "genbb_help.XXXXXX";
	  int i;
	  for (i = 0; i < oss.str ().size (); i++)
	    {
	      __tmp_dir[i] = oss.str ()[i];
	    }
	  __tmp_dir[i] = 0;
	
	  char * ret = mkdtemp (__tmp_dir);
	  if (ret == NULL)
	    {
	      ostringstream message;
	      message << "genbb::initialize: Cannot create temporary directory in '"
		      << __tmp_base_dir << "' !";
	      throw runtime_error (message.str());
	    }
	}
      else
	{
	  datatools::utils::fetch_path_with_env (__forced_tmp_dir);
	  if (__forced_tmp_dir.length () > (TMP_DIR_BUFSZ - 1))
	    {
	      ostringstream message;
	      message << "genbb::initialize: Temporary directory name is too long (<" << TMP_DIR_BUFSZ << ")!";
	      throw runtime_error (message.str());
	      
	    }
	  if (! boost::filesystem::is_directory (__forced_tmp_dir))
	    {
	      ostringstream message;
	      message << "genbb::initialize: Temporary directory '"
		      << __forced_tmp_dir << "' does not exist ! You should first create it !";
	      __clean ();
	      throw runtime_error (message.str());
	    }
	  int i;
	  for (i = 0; i < __forced_tmp_dir.size (); i++)
	    {
	      __tmp_dir[i] = __forced_tmp_dir[i];
	    }
	  __tmp_dir[i] = 0;
	}

      {
	ostringstream oss;
	oss << __tmp_dir << '/' << "genbb.conf";
	__genbb_conf = oss.str ();
      }
 
      {
	__genbb_conf_file.open (__genbb_conf.c_str ());
	if (! __genbb_conf_file)
	  {
	    ostringstream message;
	    message << "genbb::initialize: Cannot create GENBB config file in '"
		    << __tmp_dir << "' !";
	    throw runtime_error (message.str());
	  }
	__genbb_conf_file << "type=" << __decay_type << endl;
	if (__decay_type == DECAY_TYPE_BACKGROUND)
	  {
	    __genbb_conf_file << "nucleide=" << __decay_isotope << endl;
	  }
	else if (__decay_type == DECAY_TYPE_DBD)
	  {
	    __genbb_conf_file << "betabeta-nucleide=" << __decay_isotope << endl;
	    __genbb_conf_file << "betabeta-level=" << __decay_dbd_level << endl;
	    __genbb_conf_file << "betabeta-mode=" << __decay_dbd_mode << endl;
	  }
	__genbb_conf_file << "number-of-events=" << __buffer_size << endl;
	__genbb_conf_file << "first-event=1" << endl;
	__genbb_conf_file << "write-file=y" << endl;
	__genbb_conf_file << "output-file=" << "out.genbb" << endl;

      }
      
    }

    __init ();

    __initialized = true;
    return;
  }

  bool genbb::has_next ()
  {
    return true;
  }

  void genbb::_load_next (primary_event & event_, 
			 bool compute_classification_)
  {
    bool local_debug = __debug;
    local_debug = false;
    if (local_debug)
      {
	clog << "debug: " << "genbb::_load_next: "
	     << "Entering..."
	     << endl;
      }
    event_.reset ();

    if (__buffer_item == __buffer_size)
      {
	if (local_debug)
	  {
	    clog << "debug: " << "genbb::_load_next: "
		 << "Buffer is full !"
		 << endl;
	  }
	__clean ();
	__init ();
      }

    int evnum;
    double time;
    int npart;
    *__genbb_in >> ws >> evnum >> time >> npart;
    event_.time = time * CLHEP::second;
    double part_time = 0.0;
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	int part_type;
	double px, py ,pz, time_shift;
	*__genbb_in >> std::ws >> part_type >> px >> py >> pz >> time_shift;
	part_time += time_shift; 
	pp.set_type (part_type);
	pp.set_time (part_time * CLHEP::second); // GENBB unit is s
	geomtools::vector_3d p (px, py, pz);
	p *= CLHEP::MeV; // GENBB unit is MeV/c
	pp.set_momentum (p);
	event_.add_particle (pp);
      }
    *__genbb_in >> ws;
    __buffer_item++;

    if (compute_classification_)
      {
	event_.compute_classification ();
      }
    __event_count++;
    if (local_debug)
      {
	clog << "debug: " << "genbb::_load_next: "
	     << "Exiting."
	     << endl;
      }
    return;
  }

  size_t genbb::get_buffer_size () const
  {
    return __buffer_size;
  }

  size_t genbb::get_event_count () const
  {
    return __event_count;
  }

  void genbb::__init ()
  {
    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Entering..."
	     << endl;
      }

    // build the Genbb/Decay0 output data file name : 
    {
      ostringstream oss;
      oss << __tmp_dir << '/' << "data_" << __buffer_count << ".genbb";
      __genbb_data = oss.str ();
    }

    // reset counter:
    __buffer_item = 0; 
    datatools::utils::fetch_path_with_env (__genbb_data);
 
    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Generate a new GENBB buffer data file '" << __genbb_data << "'..."
	     << endl;
      }
   
    bool test = __test;
    if (test)
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::__init: "
		 << "Generate fake GENBB buffer data file '"
		 << __genbb_data << "'..."
		 << endl;
	  }
	// generate fake GENBB buffer data:
	ofstream ofs (__genbb_data.c_str ());
	for (int i = 0; i < __buffer_size; i++)
	  {
	    ofs <<  "0  0.00000       2" << endl;
	    ofs <<  "3  0.918076E-01 -0.958966      0.390933      0.00000  " << endl;
	    ofs <<  "3   2.03066      -1.14024       1.57686      0.00000  " << endl;
	  }
      }
    else
      {
	ostringstream genbb_cmd;
	string genbb_script = "${GENBB_HELP_ROOT}/scripts/genbb";
	datatools::utils::fetch_path_with_env (genbb_script);
	
	unsigned long genbb_seed = __random.uniform_int (0xFFFFFFF);

	ostringstream genbb_log_file_ss;
	genbb_log_file_ss << __tmp_dir << '/' << "genbb_" << __buffer_count << ".log";
	__genbb_log = genbb_log_file_ss.str ();
	genbb_cmd << genbb_script << " "
		  << "--temp-directory" << " "
		  << __tmp_dir << " "
		  << __genbb_conf << " " 
		  << genbb_seed << " "
		  << __buffer_size << " "
		  << __genbb_data << " "
		  << " > " << __genbb_log << " 2>&1 ";
	int ret = system (genbb_cmd.str ().c_str ());
	if (ret != 0)
	  {
	    ostringstream message;
	    message << "genbb::__init: genbb command failed ! Check log file '"
		    << __genbb_log << "' !";
	    __clean ();
	    throw runtime_error (message.str());
	  }
      }

    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Opening GENBB buffer data file '"
	     << __genbb_data << "'..."
	     << endl;
      }
    __genbb_in = 0;
    __genbb_in = new ifstream ();
    __genbb_in->open (__genbb_data.c_str ());
    if (! *__genbb_in)
      {
	ostringstream message;
	message << "genbb::__init: Cannot open file '"
		<< __genbb_data << "' !";
	if (__genbb_in != 0)
	  {
	    delete __genbb_in;
	    __genbb_in = 0;
	  }
	__genbb_data = "";
	throw runtime_error (message.str());
      }
    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Exiting."
	     << endl;
      }
    return;
  }

} // end of namespace genbb

// end of genbb.cc
