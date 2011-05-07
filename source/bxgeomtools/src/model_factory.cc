// -*- mode: c++ ; -*- 
/* model_factory.cc
 */

#include <geomtools/model_factory.h>

namespace geomtools {

  using namespace std;

  const string model_factory::DEFAULT_WORLD_LABEL = "world";

  bool model_factory::g_devel = false;
  
  bool model_factory::is_locked () const
  {
    return __locked;
  }
  
  bool model_factory::is_debug () const
  {
    return __debug;
  }
  
  void model_factory::set_debug (bool new_value_)
  {
    __debug = new_value_;
  }
  
  const models_col_t & model_factory::get_models () const
  {
    return __models;
  }

  const logical_volume::dict_t & model_factory::get_logicals () const
  {
    return __logicals;
  }
  
  // ctor:
  model_factory::model_factory (bool debug_)
  {
    __debug = debug_;
    __locked = false;
    __mp.set_key_label ("name");
    __mp.set_meta_label ("type");
    __mp.set_description ("Geometry models setup");
    return;
  }
  
  // dtor:
  model_factory::~model_factory ()
  {
    if (__locked)
      {
	reset ();
      }
    return;
  }

  void model_factory::load (const string & mprop_file_)
  {
    bool devel = g_devel || __debug;
    if (__locked)
      {
	throw runtime_error ("model_factory::init: Factory is locked !");	
      }

    //cerr << "********** DEVEL: model_factory::load: Go !" << endl; 
    //__mp.set_debug (devel);
    __mp.read (mprop_file_);  
    //cerr << "********** DEVEL: model_factory::load: Done !" << endl;
    //usleep (200);

    if (devel)
      {
	__mp.dump (clog);
      }
   
    return;
  }

  void model_factory::__lock ()
  {
    if (__locked) 
      {
	throw runtime_error ("model_factory::init: Already locked !");
      }
    __construct ();
    __mp.reset ();
    return;
  }

  void model_factory::__unlock ()
  {
    return;
  }
  
  void model_factory::unlock ()
  {
    if (! __locked) return;
    __unlock ();
    __locked = false;
    return;
  }
  
  void model_factory::lock ()
  {
    if (__locked)
      {
	throw runtime_error ("model_factory::init: Already locked !");
      }
    __lock ();
    __locked = true;
    return;
  }

  void model_factory::reset ()
  {
    if (__locked)
      {
	unlock ();
      }
    __logicals.clear ();
    __models.clear ();
    __mp.reset ();
    return;
  }

  void model_factory::__construct ()
  {
    bool devel = g_devel;
    if (devel) 
      {
	clog << "DEVEL: model_factory::__construct: "
	     << "Entering..." << endl;
      }
    for (datatools::utils::multi_properties::entries_ordered_col_t::const_iterator i 
	   = __mp.ordered_entries ().begin ();
	 i != __mp.ordered_entries ().end ();
	 i++)
      {
	const datatools::utils::multi_properties::entry * ptr_entry = *i;
	const datatools::utils::multi_properties::entry & e = *ptr_entry;
	string name = e.get_key ();
	string type = e.get_meta ();

	model_creator_t & creator = i_model::get_model_db ().get_model (type);
	if (devel)  
	  {
	    clog << "DEVEL: model_factory::__construct: "
		 << "About to create a new model of type \"" << type 
		 << "\" with name \"" << name << "\"..." << endl;
	  }
	i_model * model = 0;
	model = creator (name, 
			 e.get_properties (),
			 &__models);
	if (model != 0)
	  {
	    e.get_properties ().export_starting_with (model->get_logical ().parameters (),
						      visibility::VISIBILITY_PREFIX);

	    __models[name] = model;
	    string log_name = model->get_logical ().get_name ();
	    __logicals[log_name] = &(model->get_logical ());
	    if (devel) model->tree_dump (clog, "New model is:", "DEVEL: model_factory::__construct: ");
	  }
	else
	  {
	    cerr << "ERROR: model_factory::__construct: "
		 << "Cannot create model of type \"" << type 
		 << "\" with name \"" << name << "\"..." << endl;
	  }
      }
    if (devel) 
      {
	clog << "DEVEL: model_factory::__construct: "
	     << "Exiting." << endl;
      }
    return;
  }

  void model_factory::tree_dump (ostream & out_, 
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
    /*
      out_ << indent << du::i_tree_dumpable::tag 
      << "Debug : " <<  __debug << endl;
    */

    out_ << indent << du::i_tree_dumpable::tag 
	 << "Locked  : " <<  (__locked? "Yes": "No") << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Multi-properties : ";
      if ( __mp.entries ().size () == 0) 
	{
	  out_ << "<empty>"; 
	}
      out_ << endl;
      {
	ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::skip_tag;
	__mp.tree_dump (out_, "", indent_oss.str ());
      }
    }
 
    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	   << "Models : ";
      if ( __models.size () == 0) 
	{
	  out_ << "<empty>"; 
	}
      else
	{
	  out_ << "[" << __models.size () << "]"; 
	}
      out_ << endl;
      for (models_col_t::const_iterator i = __models.begin ();
	   i != __models.end ();
	   i++)
	{
	  const string & key = i->first;
	  const i_model * a_model = i->second;
	  ostringstream indent_oss;
	  out_ << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	  indent_oss << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	  models_col_t::const_iterator j = i; 
	  j++;
	  if (j == __models.end ()) 
	    {
	      out_ << du::i_tree_dumpable::inherit_tag (inherit_);
	      indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	    }
	  else 
	    {
	      out_ << du::i_tree_dumpable::tag;
	      indent_oss << du::i_tree_dumpable::skip_tag;
	    }
	  out_ << "Model : " << '"' << key << '"' << endl;
	  a_model->tree_dump (out_, "", indent_oss.str ());
	}
    }
      
    return;
  }
       
} // end of namespace geomtools

// end of model_factory.cc
