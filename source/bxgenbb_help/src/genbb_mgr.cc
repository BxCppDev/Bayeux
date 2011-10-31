// -*- mode: c++; -*- 
// genbb_mgr.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/genbb_mgr.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <datatools/utils/utils.h>
 
#include <geomtools/utils.h>

namespace genbb {

  using namespace std;

  const string genbb_mgr::FORMAT_GENBB_LABEL = "genbb";
  const string genbb_mgr::FORMAT_BOOST_LABEL = "boost";

  bool genbb_mgr::is_initialized () const
  {
    return _initialized_;
  }

   bool genbb_mgr::is_debug () const
    {
      return _debug_;
    }

    void genbb_mgr::set_debug (bool d_)
    {
      _debug_ = d_;
    }

    int genbb_mgr::get_format () const
    {
      return _format_;
    }

    bool genbb_mgr::is_format_genbb () const
    {
      return _format_ == FORMAT_GENBB;
    }

    bool genbb_mgr::is_format_boost () const
    {
      return _format_ == FORMAT_BOOST;
    }

  // ctor:
  genbb_mgr::genbb_mgr (int format_)
  {
    _debug_ = false;
    _initialized_ = false;
    _in_ = 0;
    _format_ = FORMAT_GENBB;
    set_format (format_);
    return;
  }
  
  // dtor:
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
    return;
  }
  
  void genbb_mgr::set_format (const string & format_label_)
  {
    if (_initialized_)
      {
	throw logic_error ("genbb_mgr::set: Operation not allowed ! Manager is locked !");
      }
    int fmt = FORMAT_GENBB;
    if ((format_label_ != FORMAT_GENBB_LABEL) 
	&& (format_label_ != FORMAT_BOOST_LABEL))
      {
	ostringstream message;
	message << "genbb_mgr::set: Invalid format label '" 
		<< format_label_ << "' !";
	throw logic_error (message.str ());
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
    return;
  }
  
  void genbb_mgr::set_format (int format_)
  {
    if (_initialized_)
      {
	throw logic_error ("genbb_mgr::set: Operation not allowed ! Manager is locked !");
      }
    if ((format_ != FORMAT_GENBB) && (format_ != FORMAT_BOOST))
      {
	throw logic_error ("genbb_mgr::set: Invalid format !");
      }
    _format_ = format_;
    return;
  }

  void genbb_mgr::set (const string & filename_)
  {
    if (_initialized_)
      {
	throw logic_error ("genbb_mgr::set: Operation not allowed! Manager is locked!");
      }
    _filenames_.push_back (filename_);
    return;
  }

  bool genbb_mgr::has_next ()
  {
    return _current_.is_valid ();
  }
  
  void genbb_mgr::_load_next (primary_event & event_, 
			     bool compute_classification_)
  {
    //provide the preloadedcurrent event:
    event_ = _current_;
    if (compute_classification_) 
      {
	event_.compute_classification ();
      }
    // try to load the next event:
    _load_next_ ();
    return;
  }

  void genbb_mgr::_load_next_boost_ ()
  {
    if (! _reader_.is_initialized ())
      {
	string filename;

	if (_filenames_.size () == 0)
	  { 
	    if (is_debug ()) clog << "genbb_mgr::_load_next_boost_: no more filenames!" << endl;
	    return;
	  }

	filename = _filenames_.front ();
	_filenames_.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_boost_: filename = '" << filename << "'" << endl;
	    return;
	  }
	datatools::utils::fetch_path_with_env (filename);
	_reader_.init (filename, 
		       datatools::serialization::using_multi_archives);
	_current_filename_ = filename;
	return;
      }

    if (! _reader_.is_initialized ())
      {
	 throw logic_error ("genbb_mgr::_load_next_boost_: Reader is not initialized !");  
      }

    if (! _reader_.has_record_tag ())
      {
	 throw logic_error ("genbb_mgr::_load_next_boost_: Reader has no data !");  
      }
    if (_reader_.record_tag_is (primary_event::SERIAL_TAG)) 
      {
	_reader_.load (_current_);
      }
    if (! _reader_.has_record_tag ())
      {
	_reader_.reset ();
      }

    return;
  }

  void genbb_mgr::_load_next_ ()
  {
    _current_.reset ();
    if (_format_ == FORMAT_GENBB)
      {
	_load_next_genbb_ ();
      }
    if (_format_ == FORMAT_BOOST)
      {
	_load_next_boost_ ();
      }
    return;
  }
    
  void genbb_mgr::_load_next_genbb_ ()
  {
    if (_in_ == 0)
      {
	if (is_debug ()) clog << "genbb_mgr::_load_next_genbb_: no input stream!" << endl;
	string filename;

	if (_filenames_.size () == 0)
	  { 
	    if (is_debug ()) clog << "genbb_mgr::_load_next_genbb_: no more filenames!" << endl;
	    return;
	  }

	filename = _filenames_.front ();
	_filenames_.pop_front ();
	if (filename.empty ()) 
	  {
	    if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: filename = '" << filename << "'" << endl;
	    return;
	  }
	datatools::utils::fetch_path_with_env (filename);
	if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: filename = '" << filename << "'" << endl;
	_fin_.close ();
	_fin_.open (filename.c_str ());
	if (! _fin_)
	  {
	    ostringstream message;
	    message << "genbb_mgr::_load_next_genbb_: cannot open file '"
		    << filename << "'!";
	    throw runtime_error (message.str ());
	  }
	_in_ = &_fin_;
	_current_filename_ = filename;
      }
  
    if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: _in_ = " << _in_ << endl;

    if (! *_in_)
      {
	ostringstream message;
	message << "genbb_mgr::_load_next_genbb_: cannot load event!";
	throw runtime_error (message.str ());
      }
    int evnum;
    double time;
    int npart;
    *_in_ >> ws >> evnum >> time >> npart;
    if (! *_in_)
      {
	ostringstream message;
	message << "genbb_mgr::_load_next_genbb_: format error!";
	throw logic_error (message.str ());
      }
    _current_.time = time * CLHEP::second;
    double part_time = 0.0;
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	int part_type;
	double x, y ,z, time_shift;
	// 2009-07-14 FM: Vladimir Tretyak email about particles' time shifts: 
	//*_in_ >> ws >> pp.type >> x >> y >> z >> pp.time; 
	*_in_ >> ws >> part_type >> x >> y >> z >> time_shift;
	part_time += time_shift; 
	pp.set_type (part_type);
	pp.set_time (part_time * CLHEP::second); // GENBB unit is s
	
	if (! *_in_)
	  {
	    ostringstream message;
	    message << "genbb_mgr::_load_next_genbb_: format error!";
	    _fin_.close ();
	    _in_ = 0;
	    throw logic_error (message.str ());
	  }
	geomtools::vector_3d p (x, y, z);
	p *= CLHEP::MeV; // GENBB unit is MeV/c
	pp.set_momentum (p);
	_current_.add_particle (pp);
      }
    *_in_ >> ws;
    if (_fin_.eof ())
      {
	_fin_.close ();
	_in_ = 0;
      }
    return;
  }

  void genbb_mgr::dump (ostream & out_) const
  {
    out_ << "genbb_mgr::dump: " << endl;
    out_ << "|-- Debug : " << _debug_ << endl;
    out_ << "|-- Initialized : " << _initialized_ << endl;
    out_ << "|-- Format : " << (_format_ == FORMAT_GENBB? "GENBB": "Boost")<< endl;
    out_ << "|-- Current input file: '" << _current_filename_ << "'" << endl;
    out_ << "|-- List of remaining input files : " << endl;
    for (list<string>::const_iterator it = _filenames_.begin ();
	 it != _filenames_.end ();
	 it++)
      {
	list<string>::const_iterator jt = it;
	jt++;
	out_ << "|   ";
	if (jt == _filenames_.end ())
	  {
	    out_ << "`-- ";
	  }
	else
	  {
	    out_ << "|-- ";
	  }
	out_ << "Filename : '" << *it << "'" << endl; 
      } 
    if (_format_ == FORMAT_GENBB)
      {
	out_ << "|-- GENBB current file address : " << hex << _in_ 
	     << dec << endl;
      }
    if (_format_ == FORMAT_BOOST)
      {
	out_ << "|-- Current Boost reader : " 
	     << (_reader_.is_initialized ()? "Yes": "No") << endl;
      }
    out_ << "`-- Current event: " << endl;
    _current_.dump (out_, "    "); 
    return;
  }

  void genbb_mgr::init ()
  {
    if (_initialized_) return;
    _at_init_ ();
    _initialized_ = true;
    return;
  }

  void genbb_mgr::_at_init_ ()
  {
    _load_next_ ();
    return;
  }

  void genbb_mgr::initialize ()
  {
    this->init ();
    return;
  }

  void genbb_mgr::initialize (const datatools::utils::properties & config_)
  {
    if (_initialized_) 
      {
	throw logic_error ("genbb_mgr::initialize: Already initialized !");
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
	throw logic_error ("genbb_mgr::initialize: Missing 'format' of input files !");	
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
	    throw logic_error ("genbb_mgr::initialize: Input files pattern is empty !");	
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
	    throw logic_error (message.str());
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
	  if (_filenames_.empty ()) 
	  {
	    throw logic_error ("genbb_mgr::initialize: Missing list of input files !");	
	  }
	}
    }

    _at_init_ ();
    _initialized_ = true;
    return;
  }

  void genbb_mgr::reset ()
  {
    if (! _initialized_) return;
    _at_reset_ ();
    _initialized_ = false;
    return;
  }

  void genbb_mgr::_at_reset_ ()
  {
    _current_filename_ = "";
    _filenames_.clear ();

    // "genbb"
    if (_format_ == FORMAT_GENBB)
      {
	if (_in_ != 0)
	  {
	    _in_ = 0; 
	    _fin_.close ();
	  }
      }

    // "boost":
    if (_format_ == FORMAT_BOOST)
      {
	if (_reader_.is_initialized ())
	  {
	    _reader_.reset ();
	  }
      }
    _format_ = FORMAT_GENBB;

    return;
  }
  
} // end of namespace genbb

// end of genbb_mgr.cc
