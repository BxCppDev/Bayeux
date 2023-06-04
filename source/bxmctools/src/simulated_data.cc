// -*- mode: c++ ; -*-
// simulated_data.cc

// Ourselves:
#include <mctools/simulated_data.h>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace mctools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(simulated_data,"mctools::simulated_data")

  void simulated_data::_set_defaults()
  {
    geomtools::invalidate(_vertex_);
    datatools::invalidate(_time_);
    _collection_type_ = HANDLE_HIT_COLLECTION_TYPE;
    return;
  }

  simulated_data::simulated_data()
  {
    _set_defaults();
    return;
  }

  simulated_data::simulated_data(int a_collection_type)
  {
    _set_defaults();
    set_collection_type(a_collection_type);
    return;
  }

  simulated_data::~simulated_data()
  {
    reset();
    return;
  }

  void simulated_data::reset_collection_type()
  {
    DT_THROW_IF(_step_hits_dict_.size() > 0 || _plain_step_hits_dict_.size() > 0,
                std::logic_error,
                "Cannot reset the collection type "
                << "since the dictionary of hit collections is not empty !");
    _collection_type_ = INVALID_HIT_COLLECTION_TYPE;
    return;
  }

  void simulated_data::set_collection_type(int a_collection_type)
  {
    DT_THROW_IF(_step_hits_dict_.size() > 0 || _plain_step_hits_dict_.size() > 0,
                std::logic_error,
                "Cannot set the collection type "
                << "since the dictionary of hit collections is not empty !");
    DT_THROW_IF(_collection_type_ != INVALID_HIT_COLLECTION_TYPE,
                std::logic_error,
                "Cannot change the collection type !");
    DT_THROW_IF((a_collection_type != PLAIN_HIT_COLLECTION_TYPE)
                && (a_collection_type != HANDLE_HIT_COLLECTION_TYPE),
                std::logic_error,
                "Invalid collection type '" << a_collection_type << "' !");
    /*
      if (a_collection_type == PLAIN_HIT_COLLECTION_TYPE)
      {
      ostringstream message;
      message << "mctools::simulated_data::set_collection_type: "
      << "Collection type '" << "plain" << "' is not supported anymore !";
      throw logic_error(message.str());
      }
    */
    _collection_type_ = a_collection_type;
    return;
  }

  bool simulated_data::use_plain_hit_collection() const
  {
    return _collection_type_ == PLAIN_HIT_COLLECTION_TYPE;
  }

  bool simulated_data::use_handle_hit_collection() const
  {
    return _collection_type_ == HANDLE_HIT_COLLECTION_TYPE;
  }

  bool simulated_data::has_data() const
  {
    return has_vertex();
  }

  bool simulated_data::has_vertex() const
  {
    return geomtools::is_valid(_vertex_);
  }

  const geomtools::vector_3d & simulated_data::get_vertex() const
  {
    return _vertex_;
  }

  geomtools::vector_3d & simulated_data::grab_vertex()
  {
    return _vertex_;
  }

  void simulated_data::set_vertex(const geomtools::vector_3d & a_v)
  {
    _vertex_ = a_v;
    return;
  }

  bool simulated_data::has_time() const
  {
    return datatools::is_valid(_time_);
  }

  double simulated_data::get_time() const
  {
    return _time_;
  }

  void simulated_data::set_time(double t_)
  {
    _time_ = t_;
    return;
  }

  const simulated_data::primary_event_type & simulated_data::get_primary_event() const
  {
    return _primary_event_;
  }

  simulated_data::primary_event_type & simulated_data::grab_primary_event()
  {
    return _primary_event_;
  }

  void simulated_data::set_primary_event(const simulated_data::primary_event_type & a_pe)
  {
    _primary_event_ = a_pe;
    return;
  }

  void simulated_data::set_properties(const datatools::properties & p)
  {
    _properties_ = p;
  }

  const datatools::properties & simulated_data::get_properties() const
  {
    return _properties_;
  }

  datatools::properties & simulated_data::grab_properties()
  {
    return _properties_;
  }

  simulated_data::step_hits_dict_type &
  simulated_data::grab_step_hits_dict()
  {
    return _step_hits_dict_;
  }

  const simulated_data::step_hits_dict_type &
  simulated_data::get_step_hits_dict() const
  {
    return _step_hits_dict_;
  }

  simulated_data::plain_step_hits_dict_type &
  simulated_data::grab_plain_step_hits_dict()
  {
    return _plain_step_hits_dict_;
  }

  const simulated_data::plain_step_hits_dict_type &
  simulated_data::get_plain_step_hits_dict() const
  {
    return _plain_step_hits_dict_;
  }

  void simulated_data::get_step_hits_categories(std::vector<std::string> & the_categories,
                                                unsigned int a_mode,
                                                const std::string & a_prefix) const
  {
    the_categories.clear();
    if (a_mode == HIT_CATEGORY_TYPE_ALL) {
      if (use_handle_hit_collection()) {
        the_categories.assign(_step_hits_dict_.size(), "");
        int count = 0;
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
             i != _step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          the_categories[count] = category;
          ++count;
        }
      }
      if (use_plain_hit_collection()) {
        the_categories.assign(_plain_step_hits_dict_.size(), "");
        int count = 0;
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
             i != _plain_step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          the_categories[count] = category;
          ++count;
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PUBLIC) {
      if (use_handle_hit_collection()) {
        the_categories.reserve(_step_hits_dict_.size());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
             i != _step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (! boost::starts_with(category, "__")) {
            the_categories.push_back(category);
          }
        }
      }
      if (use_plain_hit_collection()) {
        the_categories.reserve(_plain_step_hits_dict_.size());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
             i != _plain_step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (! boost::starts_with(category, "__")) {
            the_categories.push_back(category);
          }
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PRIVATE) {
      if (use_handle_hit_collection()) {
        the_categories.reserve(_step_hits_dict_.size());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
             i != _step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (boost::starts_with(category, "__")) {
            the_categories.push_back(category);
          }
        }
      }
      if (use_plain_hit_collection()) {
        the_categories.reserve(_plain_step_hits_dict_.size());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
             i != _plain_step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (boost::starts_with(category, "__")) {
            the_categories.push_back(category);
          }
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PREFIX && ! a_prefix.empty()) {
      if (use_handle_hit_collection()) {
        the_categories.reserve(_step_hits_dict_.size());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
             i != _step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (boost::starts_with(category, a_prefix)) {
            the_categories.push_back(category);
          }
        }
      }
      if (use_plain_hit_collection()) {
        the_categories.reserve(_plain_step_hits_dict_.size());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
             i != _plain_step_hits_dict_.end();
             i++) {
          const std::string & category = i->first;
          if (boost::starts_with(category, a_prefix)) {
            the_categories.push_back(category);
          }
        }
      }
    }
    else {
      DT_THROW_IF(true, std::logic_error, "Invalid mode !");
    }
    return;
  }

  simulated_data & simulated_data::add_step_hits(const std::string & a_category, size_t a_capacity)
  {
    if (use_handle_hit_collection()) {
      if (has_step_hits(a_category)) {
        return *this;
      }
      hit_handle_collection_type dummy;
      _step_hits_dict_[a_category] = dummy;
      if (a_capacity > 0) {
        _step_hits_dict_[a_category].reserve(a_capacity);
      }
    } else if (use_plain_hit_collection()) {
      if (has_step_hits(a_category)) {
        return *this;
      }
      hit_collection_type dummy;
      _plain_step_hits_dict_[a_category] = dummy;
      if (a_capacity > 0) {
        _plain_step_hits_dict_[a_category].reserve(a_capacity);
      }
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return *this;
  }

  simulated_data & simulated_data::remove_step_hits(const std::string & a_category)
  {
    if (use_handle_hit_collection()) {
      step_hits_dict_type::iterator found = _step_hits_dict_.find(a_category);
      if (found == _step_hits_dict_.end()) {
        return *this;
      }
      _step_hits_dict_.erase(found);
    } else if (use_plain_hit_collection()) {
      plain_step_hits_dict_type::iterator found = _plain_step_hits_dict_.find(a_category);
      if (found == _plain_step_hits_dict_.end()) {
        return *this;
      }
      _plain_step_hits_dict_.erase(found);
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return *this;
  }

  base_step_hit &
  simulated_data::add_step_hit(const std::string & a_category)
  {
    base_step_hit * bsh = nullptr;
    if (use_handle_hit_collection()) {
      step_hits_dict_type::iterator found
        = _step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of hits with category '" << a_category << "' !");
      // found->second.push_back(hit_handle_type(new base_step_hit));
      found->second.push_back(datatools::make_handle<base_step_hit>());
      bsh = &found->second.back().grab();
    } else if (use_plain_hit_collection()) {
      plain_step_hits_dict_type::iterator found
        = _plain_step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of hits with category '" << a_category << "' !");
      base_step_hit dummy;
      found->second.push_back(dummy);
      bsh = &found->second.back();
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return *bsh;
  }

  size_t simulated_data::get_number_of_step_hits(const std::string & a_category) const
  {
    size_t nbr_step_hits = 0;
    if (use_handle_hit_collection()) {
      step_hits_dict_type::const_iterator found = _step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of hits with category '" << a_category << "' !");
      nbr_step_hits = found->second.size();
    } else if (use_plain_hit_collection()) {
      plain_step_hits_dict_type::const_iterator found
        = _plain_step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of hits with category '" << a_category << "' !");
      nbr_step_hits = found->second.size();
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return nbr_step_hits;
  }

  bool simulated_data::has_hit(const std::string & category_, const int index_) const
  {
    if (use_handle_hit_collection()) {
      step_hits_dict_type::const_iterator found = _step_hits_dict_.find(category_);
      if (found == _step_hits_dict_.end()) return false;
      if (index_ < 0 || index_ >=(int)found->second.size()) return false;
      if (! found->second[index_].has_data()) return false;
      return true;
    } else {
      plain_step_hits_dict_type::const_iterator found
        = _plain_step_hits_dict_.find(category_);
      if (found == _plain_step_hits_dict_.end()) return false;
      if (index_ < 0 || index_ >=(int)found->second.size()) return false;
      return true;
    }
    return false;
  }

  const base_step_hit &
  simulated_data::get_step_hit(const std::string & a_category, int a_hit_index) const
  {
    const base_step_hit * bsh = nullptr;
    if (use_handle_hit_collection()) {
      step_hits_dict_type::const_iterator found
        = _step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of handles of hits with category '" << a_category << "' !");
      DT_THROW_IF(a_hit_index < 0 || a_hit_index >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << a_category << "' !");
      DT_THROW_IF(! found->second[a_hit_index].has_data(),
                  std::logic_error,
                  "null handle at index " << a_hit_index << " in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index].get();
    } else if (use_plain_hit_collection()) {
      plain_step_hits_dict_type::const_iterator found
        = _plain_step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of plain hits with category '" << a_category << "' !");
      DT_THROW_IF(a_hit_index < 0 || a_hit_index >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index];
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return *bsh;
  }

  base_step_hit &
  simulated_data::grab_step_hit(const std::string & a_category, int a_hit_index)
  {
    base_step_hit * bsh = 0;
    if (use_handle_hit_collection()) {
      step_hits_dict_type::iterator found
        = _step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of handles of hits with category '" << a_category << "' !");
      DT_THROW_IF(a_hit_index < 0 || a_hit_index >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << a_category << "' !");
      DT_THROW_IF(! found->second[a_hit_index].has_data(),
                  std::logic_error,
                  "Null hit handle at index " << a_hit_index << " in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index].grab();
    } else if (use_plain_hit_collection()) {
      plain_step_hits_dict_type::iterator found
        = _plain_step_hits_dict_.find(a_category);
      DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of plain hits with category '" << a_category << "' !");
      DT_THROW_IF(a_hit_index < 0 || a_hit_index >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index];
    } else {
      DT_THROW(std::logic_error, "Simulation must either use 'handle' or 'plain' hit collection!");
    }
    return *bsh;
  }

  bool simulated_data::has_step_hits(const std::string & a_category) const
  {
    if (use_handle_hit_collection()) {
      return _step_hits_dict_.find(a_category) != _step_hits_dict_.end();
    }
    if (use_plain_hit_collection()) {
      return _plain_step_hits_dict_.find(a_category) != _plain_step_hits_dict_.end();
    }
    return false;
  }

  simulated_data::hit_handle_collection_type &
  simulated_data::grab_step_hits(const std::string & a_category)
  {
    DT_THROW_IF(! use_handle_hit_collection(),
                std::logic_error,
                "Invalid mode !");
    step_hits_dict_type::iterator found = _step_hits_dict_.find(a_category);
    DT_THROW_IF(found == _step_hits_dict_.end(),
                std::logic_error,
                "No collection of hits with category '" << a_category << "' !");
    return found->second;
  }

  const simulated_data::hit_handle_collection_type &
  simulated_data::get_step_hits(const std::string & a_category) const
  {
    DT_THROW_IF(! use_handle_hit_collection(),
                std::logic_error,
                "Invalid mode !");
    step_hits_dict_type::const_iterator found = _step_hits_dict_.find(a_category);
    DT_THROW_IF(found == _step_hits_dict_.end(),
                std::logic_error,
                "No collection of hits with category '" << a_category << "' !");
    return found->second;
  }


  simulated_data::hit_collection_type &
  simulated_data::grab_plain_step_hits(const std::string & a_category)
  {
    DT_THROW_IF(! use_plain_hit_collection(),
                std::logic_error,
                "Invalid mode !");
    plain_step_hits_dict_type::iterator found = _plain_step_hits_dict_.find(a_category);
    DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                std::logic_error,
                "No collection of plain hits with category '" << a_category << "' !");
    return found->second;
  }

  const simulated_data::hit_collection_type &
  simulated_data::get_plain_step_hits(const std::string & a_category) const
  {
    DT_THROW_IF(! use_plain_hit_collection(),
                std::logic_error,
                "Invalid mode !");
    plain_step_hits_dict_type::const_iterator found = _plain_step_hits_dict_.find(a_category);
    DT_THROW_IF(found == _plain_step_hits_dict_.end(),
                std::logic_error,
                "No collection of plain hits with category '" << a_category << "' !");
    return found->second;
  }

  void simulated_data::reset()
  {
    if (use_handle_hit_collection()) {
      _step_hits_dict_.clear();
    }
    if (use_plain_hit_collection()) {
      _plain_step_hits_dict_.clear();
    }
    _properties_.clear();
    _primary_event_.reset();
    _set_defaults();
    return;
  }

  simulated_data & simulated_data::reset(bool a_reset_collection_type)
  {
    geomtools::invalidate(_vertex_);
    datatools::invalidate(_time_);
    _primary_event_.reset();
    _properties_.clear();
    if (use_handle_hit_collection()) {
      _step_hits_dict_.clear();
    }
    if (use_plain_hit_collection()) {
      _plain_step_hits_dict_.clear();
    }
    if (a_reset_collection_type) {
      _collection_type_ = HANDLE_HIT_COLLECTION_TYPE; //INVALID_HIT_COLLECTION_TYPE;
    }
    return *this;
  }

  void simulated_data::clear()
  {
    reset(false);
    return;
  }

  void simulated_data::print_tree(std::ostream & out_,
                                  const boost::property_tree::ptree & options_) const
  {
    datatools::i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    bool hit_list  = options_.get<bool>("list_hits", false);
    bool primary_particle_list  = options_.get<bool>("list_primary_particles", false);
    if (! popts.title.empty ()) {
      out_ << popts.indent << popts.title << std::endl;
    }
   
    // Properties:
    {
      out_ << popts.indent << datatools::i_tree_dumpable::tag
            << "Properties : ";
      if (_properties_.size() == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump(out_, "", indent_oss.str());
      }
    }

    // Collection type :
    out_ << popts.indent << datatools::i_tree_dumpable::tag
         << "Collection type : ";
    if (_collection_type_ == PLAIN_HIT_COLLECTION_TYPE) {
      out_ << "'plain'";
    } else if (_collection_type_ == HANDLE_HIT_COLLECTION_TYPE) {
      out_ << "'handle'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    // Step hits collections(handle type):
    if (use_handle_hit_collection()) {
      out_ << popts.indent << datatools::i_tree_dumpable::tag
            << "Collections of step hit handles : ";
      if (_step_hits_dict_.size() == 0) {
        out_ << "<none>";
      } else {
        out_ << '[' << _step_hits_dict_.size() << ']';
      }
      out_ << std::endl;
      for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
           i != _step_hits_dict_.end();
           i++) {
        const std::string & category = i->first;
        size_t no_hits = i->second.size();
        step_hits_dict_type::const_iterator j = i;
        j++;
        out_ << popts.indent << skip_tag;
        if (j == _step_hits_dict_.end()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Category '" << category << "' has "
              << no_hits << " hit(s)"  << " [capacity="
              << i->second.capacity() << ']' << std::endl;
        if (hit_list) {
          for (size_t ihit = 0; ihit < no_hits; ihit++) {
            std::ostringstream indent_oss;
            out_       << popts.indent << skip_tag;
            indent_oss << popts.indent << skip_tag;
             if (j == _step_hits_dict_.end()) {
              out_       << last_skip_tag;
              indent_oss << last_skip_tag;
            } else {
              out_       << skip_tag;
              indent_oss << skip_tag;
            }
            if (ihit + 1 == no_hits) {
              out_       << last_tag;
              indent_oss << last_skip_tag;
            } else {
              out_       << tag;
              indent_oss << skip_tag;
            }
            out_ << "Hit #" << ihit << " : " << std::endl;
            const base_step_hit & hit = get_step_hit(category, ihit);
            boost::property_tree::ptree pr_options;
            pr_options.put(i_tree_dumpable::base_print_options::indent_key(),
                           indent_oss.str());
            hit.print_tree(out_, pr_options);
          }
        }
      }
    }

    // Step hits collections(plain type):
    if (use_plain_hit_collection()) {
      out_ << popts.indent << datatools::i_tree_dumpable::tag
            << "Collections of plain step hits : ";
      if (_plain_step_hits_dict_.size() == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
           i != _plain_step_hits_dict_.end();
           i++) {
        const std::string & category = i->first;
        size_t no_hits = i->second.size();
        plain_step_hits_dict_type::const_iterator j = i;
        j++;
        out_ << popts.indent << datatools::i_tree_dumpable::skip_tag;
        if (j == _plain_step_hits_dict_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "Category '" << category << "' has "
              << no_hits << " hit(s)"  << " [capacity="
              << i->second.capacity() << ']' << std::endl;
      }
    }
   
    // Primary event:
    {
      out_ << popts.indent << datatools::i_tree_dumpable::tag
	   << "Primary event : "
	   << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
	boost::property_tree::ptree pe_options;
	pe_options.put("indent", indent_oss.str());
	pe_options.put("list_particles", primary_particle_list);
        _primary_event_.print_tree(out_, pe_options);
      }
    }

    // Time:
    {
      out_ << popts.indent << tag
            << "Time : ";
      if (has_time()) {
        out_ << _time_ / CLHEP::nanosecond << " ns";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
    }

    // Vertex:
    {
      out_ << popts.indent << inherit_tag(popts.inherit)
            << "Vertex : ";
      if (has_vertex()) {
        out_ << _vertex_ / CLHEP::mm  << " mm";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
    }

    return;
  }

  void simulated_data::tree_dump(std::ostream & a_out,
                                 const std::string & a_title,
                                 const std::string & a_indent,
                                 bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if ( !a_title.empty() ) {
      a_out << indent << a_title << std::endl;
    }

    // Properties:
    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Properties : ";
      if (_properties_.size() == 0) {
        a_out << "<empty>";
      }
      a_out << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump(a_out, "", indent_oss.str());
      }
    }

    // Collection type :
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Collection type : " <<(int) _collection_type_ << std::endl;

    // Step hits collections(handle type):
    if (use_handle_hit_collection()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Collections of step hit handles : ";
      if (_step_hits_dict_.size() == 0) {
        a_out << "<none>";
      }
      a_out << std::endl;
      for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin();
           i != _step_hits_dict_.end();
           i++) {
        const std::string & category = i->first;
        size_t no_hits = i->second.size();
        step_hits_dict_type::const_iterator j = i;
        j++;
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        if (j == _step_hits_dict_.end()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
        a_out << "Category '" << category << "' has "
              << no_hits << " hit(s)"  << " [capacity="
              << i->second.capacity() << ']' << std::endl;
      }
    }

    // Step hits collections(plain type):
    if (use_plain_hit_collection()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Collections of plain step hits : ";
      if (_plain_step_hits_dict_.size() == 0) {
        a_out << "<none>";
      }
      a_out << std::endl;
      for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin();
           i != _plain_step_hits_dict_.end();
           i++) {
        const std::string & category = i->first;
        size_t no_hits = i->second.size();
        plain_step_hits_dict_type::const_iterator j = i;
        j++;
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        if (j == _plain_step_hits_dict_.end()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        }
        else {
          a_out << datatools::i_tree_dumpable::tag;
        }
        a_out << "Category '" << category << "' has "
              << no_hits << " hit(s)"  << " [capacity="
              << i->second.capacity() << ']' << std::endl;
      }
    }

    // Primary event:
    {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Primary event : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _primary_event_.tree_dump(a_out, "", indent_oss.str());
      }
    }

    // Time:
    {
      a_out << indent << tag
            << "Time : ";
      if (has_time()) {
        a_out << _time_ / CLHEP::nanosecond << " ns";
      } else {
        a_out << "<none>";
      }
      a_out << std::endl;
    }

    // Vertex:
    {
      a_out << indent << inherit_tag(a_inherit)
            << "Vertex : ";
      if (has_vertex()) {
        a_out << _vertex_ / CLHEP::mm  << " mm";
      } else {
        a_out << "<none>";
      }
      a_out << std::endl;
    }

    return;
  }

} // end of namespace mctools
