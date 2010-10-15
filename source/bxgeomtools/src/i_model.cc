// -*- mode: c++ ; -*- 
/* i_model.cc
 */ 

#include <geomtools/i_model.h>

namespace geomtools {
  
  using namespace std;  
  
  bool i_model::g_devel = false;
  
  const string i_model::SOLID_SUFFIX = ".solid";
  
  const string i_model::LOGICAL_SUFFIX = ".log";
  
  const string i_model::PHYSICAL_SUFFIX = ".phys";
  
  const string i_model::PHANTOM_SOLID_FLAG = "phantom_solid";
  
  //const string i_model::DEFAULT_WORLD_NAME = "world";

  void i_model::assert_constructed (const string & where_, 
				    const string & what_) const
  {
    if (is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ())
      {
	message << where_ << ": ";
      }
    if (! what_.empty ())
      {
	message << what_;
      }
    else
      {
	message << "Operation not allowed ! Model has not been constructed yet";
      }
    message << " !";
    throw runtime_error (message.str ());
  }

  void i_model::assert_unconstructed (const string & where_, 
				      const string & what_) const
  {
    if (! is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ())
      {
	message << where_ << ": ";
      }
    if (! what_.empty ())
      {
	message << what_;
      }
    else
      {
	message << "Operation not allowed ! Model has already been constructed";
      }
    message << " !";
    throw runtime_error (message.str ());
  }

  string i_model::make_solid_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::SOLID_SUFFIX;
  }

  string i_model::make_logical_volume_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::LOGICAL_SUFFIX;
    return oss.str ();
  }
  
  string i_model::make_physical_volume_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::PHYSICAL_SUFFIX;
    return oss.str ();
  }

  string i_model::make_physical_volume_name (const string & basename_,
					     int nitems_)
  {
    ostringstream oss;
    oss << basename_ << i_model::PHYSICAL_SUFFIX
	<< "." << "__" << 0 << ".." << (nitems_ - 1) << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name_per_item (const string & basename_,
						      int item_)
  {
    ostringstream oss;
    oss << basename_ << i_model::PHYSICAL_SUFFIX
	<< "." << "__" << item_ << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name_per_item (const string & basename_,
						      int i_, int j_)
  {
    ostringstream oss;
    oss << basename_ << i_model::PHYSICAL_SUFFIX
	<< "." << "__" << i_ << "__"
	<< "__" << j_ << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name (const string & basename_,
					     int ncols_,
					     int nrows_)
  {
    ostringstream oss;
    oss << basename_ << i_model::PHYSICAL_SUFFIX
	<< "." << "__" << 0 << ".." << (ncols_ - 1) << "__"
	<< "__" << 0 << ".." << (nrows_ - 1) << "__";
    return oss.str ();
  }

  string i_model::extract_label_from_physical_volume_name (const string & physical_volume_name_)
  {
    size_t pos = physical_volume_name_.rfind (i_model::PHYSICAL_SUFFIX);
    if (pos == physical_volume_name_.npos)
      {
	ostringstream message;
	message << "i_model::extract_label_from_physical_volume_name: "
		<< "Do not recognize a physical volume name !";
	throw runtime_error (message.str ());
      }
    return physical_volume_name_.substr (0, pos);
  }
   
  /***************************************************/
  
  // static:
  i_model::scoped_model_db_t i_model::g__model_db (new model_db (true));
     
  // static:
  i_model::model_db & i_model::get_model_db ()
  {
    if (! g__model_db) 
      {
	throw runtime_error ("i_model::get_model_db: Library build critical bug !");
      }
    return *(g__model_db.get ());
  }

  i_model::model_db::model_db (bool test_)
  {
    if (g_devel)
      {
	clog << "DEVEL: i_model::model_db::model_db: Entering..." << endl; 
      }
    if (g_devel)
      {
	clog << "DEVEL: i_model::model_db::model_db: Exiting. " << endl;
      }
  }

  i_model::model_db::~model_db ()
  {
    if (g_devel)
      {
	clog << "DEVEL: i_model::model_db::~model_db: Entering..." 
	     << endl;
	clog << "DEVEL: i_model::model_db::~model_db: Clearing the dictionary of model creators..." 
	     << endl;
      }
    __dict.clear ();
    if (g_devel)
      {
	clog << "DEVEL: i_model::model_db::~model_db: Exiting." 
	     << endl;
      }
  }
 
  bool i_model::model_db::has_model (const string & model_id_) const
  {
    return __dict.find (model_id_) != __dict.end ();
  }

  model_creator_t & 
  i_model::model_db::get_model (const string & model_id_)
  {
    model_creator_dict_t::iterator found = __dict.find (model_id_);
    if (found == __dict.end ())
      {
	ostringstream message;
	message << "i_model::model_db::get: "
		<< "No model creator with ID='" << model_id_ << "'!";
	throw runtime_error (message.str ());
      }
    return (found->second);
  }
  
  void i_model::model_db::register_model (model_creator_t creator_, 
					  const string & model_id_)
  {
    bool devel = g_devel;
    //devel = true;
    using namespace std;
    string model_id = model_id_;
    if (devel)
      {
	clog << "DEVEL: i_model::model_db::register_model: "
	     << "model_id='" << model_id << "'"
	     << endl;
      }
    if (has_model (model_id))
      {
	ostringstream message;
	message << "i_model::model_db::register_model: " 
		<< "Model ID '" << model_id_ << "' is already used "
		<< "within the model factory dictionnary!";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_model::model_db::register_model: "
	     << "new '" << model_id << "' model ID!"
	     << endl;
      }
    
    if (model_id.empty ())
      {
	ostringstream message;
	message << "i_model::model_db::register_model: " 
		<< "Empty model ID!";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_model::model_db::register_model: "
	     << "insert model ID='" << model_id << "'!"
	     << endl;
	clog << "DEVEL: i_model::model_db::register_model: "
	     << "with creator address='" << hex 
	     << (void *) creator_ << dec << "'"
	     << endl;
      }
    pair<string, model_creator_t> a_pair;
    a_pair.first = model_id;
    a_pair.second = creator_;
    __dict.insert (a_pair);
    //__dict[model_id] = creator_;
    size_t sz = get_dict ().size ();
    if (devel)
      {
	clog << "DEVEL: i_model::model_db::register_model: size="
	     << sz << " element" << (sz > 1? "s": "") << endl;
	clog << "DEVEL: i_model::model_db::register_model: "
	     << "done."
	     << endl;
      }
  }
  
  void i_model::model_db::dump_models (ostream & out_)
  {
    out_ << "List of model creators in 'model_db::__dict': ";
    size_t sz = get_dict ().size ();
    out_ << sz << " element(s)" << endl;
    size_t count = 0; 
    for (model_creator_dict_t::const_iterator it = get_dict ().begin ();
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
  }
  
  /************************************************/

  bool i_model::is_constructed () const
  {
    return __constructed;
  }
 
  bool i_model::is_debug () const
  {
    return __debug;
  }
  
  void i_model::set_debug (bool new_value_)
  {
    __debug = new_value_;
  }
   
  const string & i_model::get_name () const
  {
    return __name;
  }
 
  void i_model::set_name (const string & name_)
  {
    __name = name_;
  }
 
  const datatools::utils::properties & 
  i_model::parameters () const
  {
    return __parameters;
  }

  datatools::utils::properties & 
  i_model::parameters ()
  {
    return __parameters;
  }

  // ctor:
  i_model::i_model (const string & name_)
  {
    __constructed = false;
    __debug = false;
    set_name (name_);
  }

  // dtor:
  i_model::~i_model ()
  {
    if (g_devel) clog << "DEVEL: i_model::~i_model: Entering..." << endl;
    __parameters.clear ();
    if (g_devel) clog << "DEVEL: i_model::~i_model: Exiting." << endl;
  }
 
  const geomtools::logical_volume & i_model::get_logical () const
  {
    return _logical;
  }
 
  geomtools::logical_volume & i_model::get_logical ()
  {
    return _logical;
  }
 
  void i_model::_at_construct (const string & name_,
			       const datatools::utils::properties & setup_,
			       models_col_t * models_)
  {
    if (g_devel) clog << "DEVEL: i_model::_at_construct: Entering..." << endl;

    throw runtime_error ("i_model::_at_construct: This method MUST be overridden !");

    if (g_devel) clog << "DEVEL: i_model::_at_construct: Exiting." << endl;
    return;      
  }

  void i_model::_pre_construct (datatools::utils::properties & setup_)
  {
  }

  void i_model::_post_construct (datatools::utils::properties & setup_)
  {
    visibility::extract (setup_, get_logical ().parameters ());
    material::extract (setup_, get_logical ().parameters ());
    sensitive::extract (setup_, get_logical ().parameters ());
    mapping_utils::extract (setup_, get_logical ().parameters ());
    return;
  }

  void i_model::construct (const string & name_,
			   const datatools::utils::properties & setup_,
			   models_col_t * models_)
  {
    if (g_devel) clog << "DEVEL: i_model::construct: Entering..." << endl;
    bool devel_track_name = false;

    if (setup_.has_flag ("geomtools::i_model::devel_track_name"))
      {
	devel_track_name = true;
      }
    if (devel_track_name) clog << "DEVEL: i_model::construct: Constructing name='" << name_ << "'..." << endl;
    if (__constructed)
      {
	throw runtime_error ("i_model::construct: Already constructed !");
      }
    datatools::utils::properties & setup = const_cast<datatools::utils::properties &> (setup_);
    _pre_construct (setup);
    _at_construct (name_, setup_, models_);
    _post_construct (setup);
    __constructed = true;
    if (g_devel) clog << "DEVEL: i_model::construct: Exiting." << endl;
    if (devel_track_name) clog << "DEVEL: i_model::construct: Constructed name='" << name_ << "'." << endl;
    return;      
  }

  void i_model::tree_dump (ostream & out_, 
			   const string & title_, 
			   const string & indent_, 
			   bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
	out_ << indent << title_ << endl;
      }

    out_ << indent << du::i_tree_dumpable::tag 
	 << "Name        : \"" << __name << "\"" << endl;

    out_ << indent << du::i_tree_dumpable::tag 
	 << "Model ID    : \"" << get_model_id () << "\"" << endl;
 
    /*
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Debug       : " << __debug << endl;
    */

    out_ << indent << du::i_tree_dumpable::tag 
	 << "Constructed : " << __constructed << endl;
      
    {
      out_ << indent << i_tree_dumpable::tag 
	   << "Parameters  : ";
      /*
      if ( __parameters.size () == 0) 
	{
	  out_ << "<empty>"; 
	}
      */
      out_ << endl; 
      {
	ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::skip_tag;
	__parameters.tree_dump (out_,"",indent_oss.str ());
      }
    }
      
    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Logical  : " << endl;
      ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      _logical.tree_dump (out_, "", indent_oss.str ());
    }
      
  }

} // end of namespace snemo

// end of i_model.cc
