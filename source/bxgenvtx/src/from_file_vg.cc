// -*- mode: c++ ; -*- 
/* from_file_vg.cc
 */

#include <genvtx/from_file_vg.h>

namespace genvtx {

  using namespace std;

  bool from_file_vg::is_open  () const
  {
    return __open;
  }

  const string & from_file_vg::get_filename () const
  {
    return __filename;
  }
  
  void from_file_vg::set_filename (const string & filename_)
  {
    if (__open)
      {
	throw runtime_error ("from_file_vg::set_filename: Cannot change source file name as source file is already opened !");
      }
    __filename = filename_;
  }

  void from_file_vg::_open_source ()
  {
    if (__open)
      {
	throw runtime_error ("from_file_vg::_open_source: Source file is already opened !");
      }
    if (__filename.empty ())
      {
	throw runtime_error ("from_file_vg::_open_source: Missing source file name !");
      } 

    datatools::utils::fetch_path_with_env (__filename);

    if (! boost::filesystem::exists (__filename)) 
      {
	ostringstream message;
	message << "Source file '" << __filename << "' does not exist !";
	throw runtime_error (message.str ());
      }
    __source.open (__filename.c_str ());
    if (! __source)
      {
	ostringstream message;
	message << "Cannot open source file '" << __filename << "' !";
	throw runtime_error (message.str ());
      }
    __open = true;
    _read_next ();
  }

  void from_file_vg::_close_source ()
  {
    if (! __open)
      {
	throw runtime_error ("from_file_vg::_close_source: Source file is not opened !");
      }
    __source.close ();
    __filename = "";
    geomtools::invalidate (__next);
    __open = true;
  }
  
  // ctor:
  from_file_vg::from_file_vg ()
  {
    __filename = "";
    __open = false;
    geomtools::invalidate (__next);
  }
  
  from_file_vg::from_file_vg (const string & filename_)
  {
    __filename = "";
    __open = false;
    geomtools::invalidate (__next);
    set_filename (filename_);
    _open_source ();
  }
  
  // dtor:
  from_file_vg::~from_file_vg ()
  {
    if (__open)
      {
	_close_source ();
      }
  }

  void from_file_vg::_read_next ()
  {
    if (! __open)
      {
	throw runtime_error ("from_file_vg::_read_next: Source file is not opened !");
	//_open_source ();
      }
    double x, y, z;
    geomtools::invalidate (__next);

    bool goon = true;
    while (goon)
      {
	goon = false;
	string line;
	std::getline (__source, line);
	// detect special cases:
	{
	  istringstream iss (line);
	  string word;
	  iss >> word;
	  //skip blank lines:
	  if (word.empty ())
	    {
	      goon = true;
	    }
	  //skip commented line:
	  else if (word[0] == '#')
	    {
	      goon = true;
	      continue;
	    }
	}
	if (! goon) 
	  {
	  istringstream iss (line);
	  iss >> x >> y >> z >> ws;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "'x y z' format error at invalid line '" << line << "' !";
	      throw runtime_error (message.str ());
	    }
	  __next.set (x, y, z);
	  break;
	}
	if (__source.eof ())
	  {
	    break;
	  }
      }
  }

  bool from_file_vg::_has_next ()
  {
    return geomtools::is_valid (__next);
  }
 
  void from_file_vg::_shoot_vertex (mygsl::rng & random_, 
				    geomtools::vector_3d & vertex_)
  {
    vertex_ = __next;
    _read_next ();
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  from_file_vg::create (const properties & configuration_)
  {
    cerr << "DEVEL: from_file_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "from_file_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the cut:
    string filename;

    if (configuration_.has_key ("filename"))
      {
	filename = configuration_.fetch_string ("filename");
      }

    // create a new parameterized 'from_file_vg' instance:
    from_file_vg * ptr = new from_file_vg (filename);
    return ptr;	
  }

  string from_file_vg::vg_id () const
  {
    return "genvtx::from_file_vg";
  }

  vg_creator_t from_file_vg::vg_creator () const
  {
    return from_file_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<from_file_vg> from_file_vg::__CR;
  
} // end of namespace genvtx

// end of from_file_vg.cc
