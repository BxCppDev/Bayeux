// -*- mode: c++ ; -*- 
/* i_model.cc
 */

#include <geomtools/i_model.h>

namespace geomtools {
  
  using namespace std;  
  
  bool i_model::g_devel = false;
  
  const string i_model::LOGICAL_SUFFIX = "_log";
  
  const string i_model::PHYSICAL_SUFFIX = "_phys";
   
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

  void i_model::construct (const string & name_,
			   const datatools::utils::properties & setup_,
			   models_col_t * models_)
  {
    if (g_devel) clog << "DEVEL: i_model::construct: Entering..." << endl;
    if (__constructed)
      {
	throw runtime_error ("i_model::construct: Already constructed !");
      }
    _at_construct (name_, setup_, models_);
    __constructed = true;
    if (g_devel) clog << "DEVEL: i_model::construct: Exiting." << endl;
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
