// -*- mode: c++; -*- 
/* genbb.cc
 */

#include <genbb_help/genbb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace genbb {
 
  using namespace std;

  bool genbb::is_debug () const
  {
    return __debug;
  }

  void genbb::__clean ()
  {
    if (__genbb_in != 0)
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::__clean: "
		 << "Deleting current GENBB data input file stream '"
		 << __fin << "'..."
		 << endl;
	  }
	delete __genbb_in;
	__genbb_in = 0;
      }
   if (! __fin.empty () && boost::filesystem::exists (__fin.c_str ()))
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::__clean: "
		 << "Removing previous GENBB data file '" << __fin << "'..."
		 << endl;
	  }
 	unlink (__fin.c_str ());
	__fin = "";
      }
    __fin = "";
    return;
  }
  
  // ctor:
  genbb::genbb ()
  {
    __debug = false;
    __test = false;
    __buffer_size = DEFAULT_BUFFER_SIZE;
    __buffer_item = 0;
    __genbb_in = 0;
    __event_count = 0;
    __tmp_base_dir = "/tmp/${USER}";
    __decay_type = DECAY_TYPE_DBD;
    __decay_isotope = "Se82";  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;
    __seed = 0;
    __tmp_dir[0] = 0;
    return;
  }
  
  // dtor:
  genbb::~genbb ()
  {
    __clean ();
  }

  void genbb::reset ()
  {
    __clean ();
    __test = false;
    __buffer_size = DEFAULT_BUFFER_SIZE;
    __buffer_item = 0;
    __genbb_in = 0;
    __event_count = 0;
    __tmp_base_dir = "/tmp/${USER}";

    __decay_type = DECAY_TYPE_DBD;
    __decay_isotope = "Se82";  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;

    __tmp_dir[0] = 0;

    __seed = 0;
    __random.reset ();
    __genbb_conf = "";
    __genbb_conf_file.close ();
    __genbb_data = "";
    return;
  }

  void genbb::initialize (const datatools::utils::properties & config_)
  {

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
    __random.init ("mt19937", __seed);
    //clog << "DEVEL: genbb::initialize: Done." << endl;
    
    /***************/
    datatools::utils::fetch_path_with_env (__tmp_base_dir);

    {
      if (! boost::filesystem::is_directory (__tmp_base_dir))
	{
	  ostringstream message;
	  message << "genbb::initialize: Base temporary directory '"
		  << __tmp_base_dir << "' does not exist !";
	  __clean ();
	  throw runtime_error (message.str());
	}
    }

    {
      ostringstream oss;
      oss << __tmp_base_dir << '/' << "genbb_help.XXXXXX";
      int i;
      for (i = 0; i < oss.str ().size (); i++)
	{
	  __tmp_dir[i] = oss.str ()[i];
	}
      __tmp_dir[i] = 0;

      //clog << "DEVEL: __tmp_dir='" << __tmp_dir << "'" << endl;
      char * ret = mkdtemp (__tmp_dir);
      if (ret == NULL)
	{
	  ostringstream message;
	  message << "genbb::initialize: Cannot create temporary directory in '"
		<< __tmp_base_dir << "' !";
	  throw runtime_error (message.str());
	}
      //clog << "DEVEL: genbb::initialize: temp. dir.='" << __tmp_dir << "'" << endl;

      {
	ostringstream oss;
	oss << __tmp_dir << '/' << "genbb.conf";
	__genbb_conf = oss.str ();
      }

      {
	ostringstream oss;
	oss << __tmp_dir << '/' << "data.genbb";
	__genbb_data = oss.str ();
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
    return;
  }

  bool genbb::has_next ()
  {
    return true;
  }

  void genbb::load_next (primary_event & event_, 
			 bool compute_classification_)
  {
    if (__debug)
      {
	clog << "debug: " << "genbb::load_next: "
	     << "Entering..."
	     << endl;
      }
    event_.reset ();

    if (__buffer_item == __buffer_size)
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::load_next: "
		 << "Buffer is full !"
		 << endl;
	  }
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
    if (__debug)
      {
	clog << "debug: " << "genbb::load_next: "
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

    __clean ();

    // reset counter:
    __buffer_item = 0;
    __fin = __genbb_data;
    datatools::utils::fetch_path_with_env (__fin);
 
    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Generate a new GENBB buffer data file '" << __fin << "'..."
	     << endl;
      }
   
    bool test = __test;
    if (test)
      {
	if (__debug)
	  {
	    clog << "debug: " << "genbb::__init: "
		 << "Generate fake GENBB buffer data file '"
		 << __fin << "'..."
		 << endl;
	  }
	// generate fake GENBB buffer data:
	ofstream ofs (__fin.c_str ());
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
	genbb_log_file_ss << __tmp_dir << '/' << "genbb.log";
	string genbb_log_file = genbb_log_file_ss.str ();
	genbb_cmd << genbb_script << " "
		  << "--temp-directory" << " "
		  << __tmp_dir << " "
		  << __genbb_conf << " " 
		  << genbb_seed << " "
		  << __buffer_size << " "
		  << __genbb_data << " "
		  << " > " << genbb_log_file << " 2>&1 ";
	int ret = system (genbb_cmd.str ().c_str ());
	if (ret != 0)
	  {
	    ostringstream message;
	    message << "genbb::__init: genbb command failed ! Check log file '"
		    << genbb_log_file << "' !";
	    __clean ();
	    throw runtime_error (message.str());
	  }
      }

    if (__debug)
      {
	clog << "debug: " << "genbb::__init: "
	     << "Opening GENBB buffer data file '"
	     << __fin << "'..."
	     << endl;
      }
    __genbb_in = new ifstream ();
    __genbb_in->open (__fin.c_str ());
    if (! *__genbb_in)
      {
	ostringstream message;
	message << "genbb::__init: Cannot open file '"
		<< __fin << "' !";
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
