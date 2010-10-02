// -*- mode: c++; -*- 
/* genbb_mgr.cc
 */

#include <genbb_help/genbb_mgr.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace genbb {

  using namespace std;

  const string genbb_mgr::FORMAT_GENBB_LABEL = "genbb";
  const string genbb_mgr::FORMAT_BOOST_LABEL = "boost";

  bool genbb_mgr::is_initialized () const
  {
    return __initialized;
  }

  // ctor:
  genbb_mgr::genbb_mgr (int format_)
  {
    __debug = false;
    __initialized = false;
    __in = 0;
    __format = FORMAT_GENBB;
    set_format (format_);
  }
  
  // dtor:
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
  }
  
  void genbb_mgr::set_format (const string & format_label_)
  {
    if (__initialized)
      {
	throw runtime_error ("genbb_mgr::set: Operation not allowed ! Manager is locked !");
      }
    int fmt = FORMAT_GENBB;
    if ((format_label_ != FORMAT_GENBB_LABEL) 
	&& (format_label_ != FORMAT_BOOST_LABEL))
      {
	ostringstream message;
	message << "genbb_mgr::set: Invalid format label '" 
		<< format_label_ << "' !";
	throw runtime_error (message.str ());
      }
    if (format_label_ == FORMAT_GENBB_LABEL) 
      {
	fmt = FORMAT_GENBB;
      }
    if (format_label_ == FORMAT_BOOST_LABEL) 
      {
	fmt = FORMAT_BOOST;
      }
    this->set_format (fmt);
  }
  
  void genbb_mgr::set_format (int format_)
  {
    if (__initialized)
      {
	throw runtime_error ("genbb_mgr::set: Operation not allowed ! Manager is locked !");
      }
    if ((format_ != FORMAT_GENBB) && (format_ != FORMAT_BOOST))
      {
	throw runtime_error ("genbb_mgr::set: Invalid format !");
      }
    __format = format_;
  }

  void genbb_mgr::set (const string & filename_)
  {
    if (__initialized)
      {
	throw runtime_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    __filenames.push_back (filename_);
  }

  bool genbb_mgr::has_next ()
  {
    return __current.is_valid ();
  }
  
  void genbb_mgr::load_next (primary_event & event_, 
			     bool compute_classification_)
  {
    //provide the preloadedcurrent event:
    event_ = __current;
    if (compute_classification_) 
      {
	event_.compute_classification ();
      }
    // try to load the next event:
    __load_next ();
    return;
  }

  void genbb_mgr::__load_next_boost ()
  {
    if (! __reader.is_initialized ())
      {
	string filename;

	if (__filenames.size () == 0)
	  { 
	    if (is_debug ()) clog << "genbb_mgr::__load_next_boost: no more filenames!" << endl;
	    return;
	  }

	filename = __filenames.front ();
	__filenames.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) clog << "DEVEL: genbb_mgr::__load_next_boost: filename = '" << filename << "'" << endl;
	    return;
	  }
	__reader.init (filename, 
		       datatools::serialization::using_multi_archives);
	__current_filename = filename;
      }

    if (! __reader.is_initialized ())
      {
	 throw runtime_error ("genbb_mgr::__load_next_boost: Reader is not initialized !");  
      }

    if (! __reader.has_record_tag ())
      {
	 throw runtime_error ("genbb_mgr::__load_next_boost: Reader has no data !");  
      }
    if (__reader.record_tag_is (primary_event::SERIAL_TAG)) 
      {
	__reader.load (__current);
      }
    if (! __reader.has_record_tag ())
      {
	__reader.reset ();
      }

    return;
  }

  void genbb_mgr::__load_next ()
  {
    __current.reset ();
    if (__format == FORMAT_GENBB)
      {
	__load_next_genbb ();
      }
    if (__format == FORMAT_BOOST)
      {
	__load_next_boost ();
      }
  }
    
  void genbb_mgr::__load_next_genbb ()
  {
    if (__in == 0)
      {
	if (is_debug ()) clog << "genbb_mgr::__load_next: no input stream!" << endl;
	string filename;

	if (__filenames.size () == 0)
	  { 
	    if (is_debug ()) clog << "genbb_mgr::__load_next: no more filenames!" << endl;
	    return;
	  }

	filename = __filenames.front ();
	__filenames.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) clog << "DEVEL: genbb_mgr::__load_next: filename = '" << filename << "'" << endl;
	    return;
	  }

	if (is_debug ()) clog << "DEVEL: genbb_mgr::__load_next: filename = '" << filename << "'" << endl;
	__fin.close ();
	__fin.open (filename.c_str ());
	if (! __fin)
	  {
	    ostringstream message;
	    message << "genbb_mgr::__load_next: cannot open file '"
		    << filename << "'!";
	    throw runtime_error (message.str ());
	  }
	__in = &__fin;
	__current_filename = filename;
      }
  
    if (is_debug ()) clog << "DEVEL: genbb_mgr::__load_next: __in = " << __in << endl;

    if (! *__in)
      {
	ostringstream message;
	message << "genbb_mgr::__load_next: cannot load event!";
	throw runtime_error (message.str ());
      }
    int evnum;
    double time;
    int npart;
    *__in >> ws >> evnum >> time >> npart;
    if (! *__in)
      {
	ostringstream message;
	message << "genbb_mgr::__load_next: format error!";
	throw runtime_error (message.str ());
      }
    __current.time = time * CLHEP::second;
    double part_time = 0.0;
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	int part_type;
	double x, y ,z, time_shift;
	// 2009-07-14 FM: Vladimir Tretyak email about particles' time shifts: 
	//*__in >> ws >> pp.type >> x >> y >> z >> pp.time; 
	*__in >> ws >> part_type >> x >> y >> z >> time_shift;
	part_time += time_shift; 
	pp.set_type (part_type);
	pp.set_time (part_time * CLHEP::second); // GENBB unit is s
	
	if (! *__in)
	  {
	    ostringstream message;
	    message << "genbb_mgr::__load_next: format error!";
	    __fin.close ();
	    __in = 0;
	    throw runtime_error (message.str ());
	  }
	geomtools::vector_3d p (x, y, z);
	p *= CLHEP::MeV; // GENBB unit is MeV/c
	pp.set_momentum (p);
	__current.add_particle (pp);
      }
    *__in >> ws;
    if (__fin.eof ())
      {
	__fin.close ();
	__in = 0;
      }
  }

  void genbb_mgr::dump (ostream & out_) const
  {
    out_ << "genbb_mgr::dump: " << endl;
    out_ << "|-- Debug : " << __debug << endl;
    out_ << "|-- Initialized : " << __initialized << endl;
    out_ << "|-- Format : " << (__format == FORMAT_GENBB? "GENBB": "Boost")<< endl;
    out_ << "|-- Current input file: '" << __current_filename << "'" << endl;
    out_ << "|-- List of remaining input files : " << endl;
    for (list<string>::const_iterator it = __filenames.begin ();
	 it != __filenames.end ();
	 it++)
      {
	list<string>::const_iterator jt = it;
	jt++;
	out_ << "|   ";
	if (jt == __filenames.end ())
	  {
	    out_ << "`-- ";
	  }
	else
	  {
	    out_ << "|-- ";
	  }
	out_ << "Filename : '" << *it << "'" << endl; 
      } 
    if (__format == FORMAT_GENBB)
      {
	out_ << "|-- GENBB current file address : " << hex << __in 
	     << dec << endl;
      }
    if (__format == FORMAT_BOOST)
      {
	out_ << "|-- Current Boost reader : " 
	     << (__reader.is_initialized ()? "Yes": "No") << endl;
      }
    out_ << "`-- Current event: " << endl;
    __current.dump (out_, "    "); 
    return;
  }

  void genbb_mgr::init ()
  {
    if (__initialized) return;
    __at_init ();
    __initialized = true;
    return;
  }

  void genbb_mgr::__at_init ()
  {
    __load_next ();
    return;
  }

  void genbb_mgr::initialize ()
  {
    this->init ();
    return;
  }

  void genbb_mgr::initialize (const datatools::utils::properties & config_)
  {
    if (__initialized) 
      {
	throw runtime_error ("genbb_mgr::initialize: Already initialized !");
      }
    
    if (config_.has_flag ("debug"))
      {
	set_debug (true);
      }
    
    if (config_.has_key ("format"))
      {
	string format = config_.fetch_string ("format");
	set_format (format);
      }
    else
      {
	throw runtime_error ("genbb_mgr::initialize: Missing 'format' of input files !");	
      }

    // try to build a list of input filenames from a pattern:
    {
    string input_files_dir = ".";
    string input_files_pattern = "*.genbb";
    if (config_.has_key ("input_files.pattern"))
      {
	input_files_pattern = config_.fetch_string ("input_files.pattern");
	if (input_files_pattern.empty ())
	  {
	    throw runtime_error ("genbb_mgr::initialize: Input files pattern is empty !");	
	  }
	if (config_.has_key ("input_files.directory"))
	  {
	    input_files_dir = config_.fetch_string ("input_files.directory");
	  }
	
	char tmp_buffer[256];
	for (int i = 0; i < 256; i++)
	  {
	    tmp_buffer[i] = '\0';
	  }
	string tmp_dir = ".";
	sprintf (tmp_buffer,"%s/%s", tmp_dir.c_str (), "genbb_mgr_pattern.XXXXXX");
	int file_desc= mkstemp (tmp_buffer);
	if (file_desc == -1)
	  {
	    //cerr << "DEVEL: " << tmp_buffer << endl;
	    throw runtime_error ("genbb_mgr::initialize: Cannot create temporary file !");		    
	  }
	if (is_debug ()) clog << "DEBUG: genbb_mgr::initialize: Temporary file is: '" << tmp_buffer << "'" << endl;
	close (file_desc);
	string tmp_file = tmp_buffer;
	ostringstream sys_command;
	ostringstream genbb_mgr_log_file_ss;
	genbb_mgr_log_file_ss << tmp_dir << '/' << "genbb_mgr.log";
	string genbb_mgr_log_file = genbb_mgr_log_file_ss.str ();
	sys_command << "find " << input_files_dir 
		    << " -maxdepth 1" 
		    << " -name " << input_files_pattern
		    << " | sort > " << tmp_file
		    << " 2> " << genbb_mgr_log_file_ss.str ();
	if (is_debug ()) clog << "DEBUG: genbb_mgr::initialize: System command: '" << sys_command.str () << "'" << endl;
	int ret = system (sys_command.str ().c_str ());
	if (ret != 0)
	  {
	    ostringstream message;
	    message << "genbb_mgr::initialize: System command failed ! Check log file '"
		    << genbb_mgr_log_file << "' !";
	    throw runtime_error (message.str());
	  }
	
	ifstream fin (tmp_file.c_str ());
	if (! fin)
	  {
	    ostringstream message;
	    message << "genbb_mgr::initialize: Cannot open list of input files '"
		    << tmp_file << "' !";
	    throw runtime_error (message.str());
	  }
	while (fin)
	  {
	    string line;
	    getline (fin, line);
	    istringstream line_iss (line);
	    string filename;
	    line_iss >> filename;
	    if (! filename.empty ())
	      {
		this->set (filename);
	      }
	    fin >> ws;
	    if (fin.eof ()) break;
	  }
	unlink (tmp_file.c_str ());
	unlink (genbb_mgr_log_file.c_str ());
      }
    }

    // try to get a list of input filenames:
    {
      vector<string> input_files;
      if (config_.has_key ("input_files"))
	{
	  config_.fetch ("input_files", input_files);
	  for (int i = 0; i < input_files.size (); i++)
	    {
	      datatools::utils::fetch_path_with_env (input_files[i]);
	      set (input_files[i]);
	    }
	}
      else
	{
	  if (__filenames.empty ()) 
	  {
	    throw runtime_error ("genbb_mgr::initialize: Missing 'input_files' of input files !");	
	  }
	}
    }

    __at_init ();
    __initialized = true;
  }

  void genbb_mgr::reset ()
  {
    if (! __initialized) return;
    __at_reset ();
    __initialized = false;
    return;
  }

  void genbb_mgr::__at_reset ()
  {
    __current_filename = "";
    __filenames.clear ();

    // "genbb"
    if (__format == FORMAT_GENBB)
      {
	if (__in != 0)
	  {
	    __in = 0; 
	    __fin.close ();
	  }
      }

    // "boost":
    if (__format == FORMAT_BOOST)
      {
	if (__reader.is_initialized ())
	  {
	    __reader.reset ();
	  }
      }
    __format = FORMAT_GENBB;

    return;
  }
  
} // end of namespace genbb

// end of genbb_mgr.cc
