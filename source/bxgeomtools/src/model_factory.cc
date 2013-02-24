// -*- mode: c++ ; -*-
/* model_factory.cc
 */

#include <geomtools/model_factory.h>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <datatools/utils.h>

#include <geomtools/detail/model_tools.h>
#include <geomtools/i_model.h>
#include <geomtools/logical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  const string model_factory::DEFAULT_WORLD_LABEL = "world";

  bool model_factory::g_devel = false;

  bool model_factory::is_locked () const
  {
    return _locked_;
  }

  bool model_factory::is_debug () const
  {
    return _debug_;
  }

  void model_factory::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  const models_col_type & model_factory::get_models () const
  {
    return _models_;
  }

  const logical_volume::dict_type & model_factory::get_logicals () const
  {
    return _logicals_;
  }

  // Constructor:
  model_factory::model_factory (bool debug_, bool verbose_)
    : _factory_register_ ("geomtools::i_model/model_factory",
                          verbose_ ? i_model::factory_register_type::verbose : 0)

  {
    _debug_ = debug_;
    _locked_ = false;
    _mp_.set_key_label ("name");
    _mp_.set_meta_label ("type");
    _mp_.set_description ("Geometry models setup");
    bool preload = true;
    if (preload)
      {
        _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::geomtools::i_model));
      }
    return;
  }

  // Destructor:
  model_factory::~model_factory ()
  {
    if (_locked_)
      {
        reset ();
      }
    return;
  }
  
  // 2012-05-25 FM : add support for loading a file that contains a list of geometry filenames :
  void model_factory::load_geom_list (const std::string & geom_list_file_)
  {
    string geom_lis_filename = geom_list_file_;
    datatools::fetch_path_with_env (geom_lis_filename);

    ifstream finlist (geom_lis_filename.c_str ());
    if (! finlist)
      {
        ostringstream message;
        message << "geomtools::model_factory::load_geom_list: "
                << "Cannot open file '" << geom_lis_filename << "' !";
        throw logic_error (message.str ());
      }
    while (finlist)
      {
        string line;
        std::getline (finlist, line);
        if (! finlist)
          {
            ostringstream message;
            message << "geomtools::model_factory::load_geom_list: "
                    << "I/O error while reading file '" << geom_lis_filename << "' !";
            throw logic_error (message.str ());
          }
        string word;
        istringstream line_iss (line);
        line_iss >> word;
        if (word.length () < 1)
          {
            // skip blank line
            continue;
          }
        if (word[0] == '#')
          {
            continue;
          }
        string geom_filename = word;
        datatools::fetch_path_with_env (geom_filename);
        load (geom_filename);

        finlist >> ws;
        if (finlist.eof ())
          {
            break;
          }
      }
   
  }

  void model_factory::load (const string & mprop_file_)
  {
    bool devel = g_devel || _debug_;
    if (_locked_)
      {
        throw logic_error ("geomtools::model_factory::load: Factory is locked !");
      }
    _mp_.read (mprop_file_);
    if (devel)
      {
        _mp_.dump (clog);
      }
    return;
  }

  void model_factory::_lock_ ()
  {
    if (_locked_)
      {
        throw logic_error ("geomtools::model_factory::_lock_: Already locked !");
      }
    _construct_ ();
    _mp_.reset ();
    return;
  }

  void model_factory::_unlock_ ()
  {
    return;
  }

  void model_factory::unlock ()
  {
    if (! _locked_) return;
    _unlock_ ();
    _locked_ = false;
    return;
  }

  void model_factory::lock ()
  {
    if (_locked_)
      {
        throw logic_error ("geomtools::model_factory::lock: Already locked !");
      }
    _lock_ ();
    _locked_ = true;
    return;
  }

  void model_factory::reset ()
  {
    if (_locked_)
      {
        unlock ();
      }
    // The container of logicals does not have ownership of the pointers :
    _logicals_.clear ();
    // Memory leak to be fixed:
    for (models_col_type::iterator i = _models_.begin ();
         i != _models_.end();
         i++)
      {
        const string & model_name = i->first;
        i_model * model_ptr = i->second;
        if (model_ptr != 0)
          {
            if (_debug_)
              {
                clog << "DEBUG: " << "geomtools::model_factory::reset: "
                     << "Deleting registered model '" << model_name << "'"
                     << endl;
              }
            delete model_ptr;
          }
      }
    _models_.clear ();
    _mp_.reset ();
    return;
  }

  /// Add a property prefix to be preserved in logicals
  void model_factory::add_property_prefix(const std::string & prefix_, int level_)
  {
    if (_property_prefixes_.find (prefix_) != _property_prefixes_.end())
      {
        std::ostringstream message;
        message << "geomtools::model_factory::add_property_prefix: " 
                << "Property prefix to be preserved '" << prefix_<< "' already exists !";
        throw std::logic_error(message.str());
       }
    _property_prefixes_[prefix_] = level_;
    return;
  }

  void model_factory::_construct_ ()
  {
    bool devel = g_devel;
    if (devel)
      {
        clog << "DEVEL: geomtools::model_factory::_construct_: "
             << "Entering..." << endl;
      }
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = _mp_.ordered_entries ().begin ();
         i != _mp_.ordered_entries ().end ();
         i++)
      {
        const datatools::multi_properties::entry * ptr_entry = *i;
        const datatools::multi_properties::entry & e = *ptr_entry;
        string model_name = e.get_key ();
        string model_type = e.get_meta ();

        if (! _factory_register_.has (model_type))
          {
            ostringstream message;
            message << "No registered class with ID '"
                    << model_type << "' for model named '" << model_name << " !";
            cerr << "geomtools::model_factory::_construct: "
                 << message.str ()
                 << endl;
            continue;
          }
        const i_model::factory_register_type::factory_type & the_factory 
          = _factory_register_.get (model_type);
        if (devel)
          {
            clog << "DEVEL: geomtools::model_factory::_construct_: "
                 << "About to create a new model of type \"" << model_type
                 << "\" with name \"" << model_name << "\"..." << endl;
          }
        i_model * model = the_factory ();
        model->construct (model_name, e.get_properties (), &_models_);
        /*
        e.get_properties ().export_starting_with (model->get_logical ().parameters (),
                                                  visibility::constants::instance().VISIBILITY_PREFIX);
        */
        for (std::map<std::string,int>::const_iterator i = _property_prefixes_.begin ();
             i != _property_prefixes_.end ();
             i++)
          {
            e.get_properties ().export_starting_with (model->get_logical ().parameters (),
                                                      i->first);
          }

        _models_[model_name] = model;
        if (is_debug ())
          {
            clog << "DEBUG: geomtools::model_factory::_construct_: "
                 << "Adding model '" << model_name << "'..." << endl;
          }
        string log_name = model->get_logical ().get_name ();
        _logicals_[log_name] = &(model->get_logical ());
        if (is_debug ()) model->tree_dump (clog, 
                                           "New model is:", 
                                           "DEBUG: geomtools::model_factory::_construct_: ");
      }
    if (devel)
      {
        clog << "DEVEL: geomtools::model_factory::_construct_: "
             << "Exiting." << endl;
      }
    return;
  }

  void model_factory::tree_dump (ostream & out_,
                                 const string & title_,
                                 const string & indent_,
                                 bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }
    /*
      out_ << indent << datatools::i_tree_dumpable::tag
      << "Debug : " <<  _debug_ << endl;
    */

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Locked  : " <<  (_locked_? "Yes": "No") << endl;

    // Configurations: 
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Multi-properties : ";
      if ( _mp_.entries ().size () == 0)
        {
          out_ << "<empty>";
        }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mp_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    // Logicals: 
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logicals : ";
      if ( _logicals_.size () == 0)
        {
          out_ << "<empty>";
        }
      else
        {
          out_ << "[" << _logicals_.size () << "]";
        }
      out_ << endl;
      for (logical_volume::dict_type::const_iterator i = _logicals_.begin ();
           i != _logicals_.end ();
           i++)
        {
         const string & key = i->first;
         const logical_volume * a_logical = i->second;
        }
    }
 
    // Models: 
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Models : ";
      if ( _models_.size () == 0)
        {
          out_ << "<empty>";
        }
      else
        {
          out_ << "[" << _models_.size () << "]";
        }
      out_ << endl;
      for (models_col_type::const_iterator i = _models_.begin ();
           i != _models_.end ();
           i++)
        {
          const string & key = i->first;
          const i_model * a_model = i->second;
          ostringstream indent_oss;
          out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          models_col_type::const_iterator j = i;
          j++;
          if (j == _models_.end ())
            {
              out_ << datatools::i_tree_dumpable::inherit_tag (inherit_);
              indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
            }
          else
            {
              out_ << datatools::i_tree_dumpable::tag;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
            }
          out_ << "Model : " << '"' << key << '"' << endl;
          a_model->tree_dump (out_, "", indent_oss.str ());
        }
    }

    return;
  }

} // end of namespace geomtools

// end of model_factory.cc
