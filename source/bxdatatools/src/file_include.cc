// datatools/file_include.cc

// Ourselves:
#include <datatools/file_include.h>

// Standard Library:
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <vector>

// Third parties:
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

// This project:
#include <datatools/utils.h>

namespace datatools {
  
  file_include::file_include()
  {
    return;
  }

  datatools::logger::priority file_include::get_logging() const
  {
    return _logging_;
  }

  void file_include::set_logging(const datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }
  
  bool file_include::allows_current_directory() const
  {
    return _allow_current_directory_;
  }

  void file_include::set_allows_current_directory(bool a_)
  {
    _allow_current_directory_ = a_;
    _has_changed_ = true;
    return;
  }

  void file_include::add_explicit_include_path(const std::string & path_)
  {
    prepend_explicit_include_path(path_);
    return;
  }
  
  void file_include::append_explicit_include_path(const std::string & path_)
  {
    _explicit_include_paths_.push_back(path_);
    _has_changed_ = true;
    return;
  }

  void file_include::prepend_explicit_include_path(const std::string & path_)
  {
    _explicit_include_paths_.push_front(path_);
    _has_changed_ = true;
    return;
  }

  void file_include::remove_explicit_include_path(const std::string & path_)
  {
    _explicit_include_paths_.remove(path_);
    _has_changed_ = true;
    return;
  }  

  bool file_include::has_explicit_include_path(const std::string & path_) const
  {
    return std::find(_explicit_include_paths_.begin(),
                     _explicit_include_paths_.end(),
                     path_) != _explicit_include_paths_.end();
  }

  bool file_include::has_include_path(const std::string & path_) const
  {
    return std::find(_include_paths_.begin(),
                     _include_paths_.end(),
                     path_) != _include_paths_.end();
  }

  file_include::ev_strategy file_include::get_include_path_env_strategy() const
  {
    return _include_path_env_strategy_;
  }

  void file_include::set_include_path_env_strategy(const ev_strategy s_)
  {
    _include_path_env_strategy_ = s_;
    _has_changed_ = true;
    return;
  }
  
  const std::string & file_include::get_include_path_env_name() const
  {
    return _include_path_env_name_;
  }

  void file_include::set_include_path_env_name(const std::string & ev_name_)
  {
    uint32_t nv_flags =
      datatools::NV_INSTANCE |
      datatools::NV_NO_DOT |
      datatools::NV_NO_LOWER_CASE;
    DT_THROW_IF(!name_validation(ev_name_, nv_flags),
                std::logic_error,
                "Invalid file inclusion directory list environment variable '"
                << ev_name_ << "'!");
    _include_path_env_name_ = ev_name_;
    _has_changed_ = true;
    return;
  }

  void file_include::reset_include_path_env_name()
  {
    _include_path_env_name_.clear();
    _has_changed_ = true;
    return;
  }

  bool file_include::has_include_path_env_name() const
  {
    return !_include_path_env_name_.empty();
  }

  std::tuple<bool, std::string>
  file_include::resolve_err(const std::string & to_be_included_path_) const noexcept
  {
    std::tuple<bool, std::string> result = std::make_tuple(false, "");
    try {
      std::string resolved_path = _resolve_(to_be_included_path_);
      result = std::make_tuple(true, resolved_path);
    } catch (std::exception & error) {
      result = std::make_tuple(false, error.what());
    } catch (...) {
      result = std::make_tuple(false, "unexpected error");
    } 
    return result;
  }

  bool file_include::resolve(const std::string & to_be_included_path_,
                             std::string & resolved_path_) const noexcept
  {
    std::tuple<bool, std::string> res = resolve_err(to_be_included_path_);
    if (std::get<0>(res)) {
      resolved_path_ = std::get<1>(res);
      return true;
    }
    resolved_path_.clear();
    DT_LOG_ERROR(_logging_, std::get<1>(res));
    return false;
  }

  std::string file_include::resolve(const std::string & to_be_included_path_) const noexcept(false)
  {
    return _resolve_(to_be_included_path_);
  }

  std::string file_include::_resolve_(const std::string & to_be_included_path_) const noexcept(false)
  {
    if (_has_changed_) {
      const_cast<file_include*>(this)->_compute_internals_();
    }
    DT_THROW_IF(to_be_included_path_.empty(),
                std::logic_error,
                "Missing path for the file to be included!");
    DT_LOG_DEBUG(_logging_, "=== resolving path : '" << to_be_included_path_ << "'");
    boost::filesystem::path bfullpath;
    if (bfullpath.empty()) {
      // Check if the file path is an absolute path:
      std::string to_be_included_path(to_be_included_path_);
      datatools::fetch_path_with_env(to_be_included_path);
      if (to_be_included_path[0] == '/') {
        DT_LOG_DEBUG(_logging_, "Found an absolute path : '" << to_be_included_path << "'");
        if (boost::filesystem::is_regular_file(to_be_included_path)) {
          bfullpath = to_be_included_path;
        } 
      } else {
        DT_LOG_DEBUG(_logging_, "Not an absolute path");
      }
    }
    if (bfullpath.empty()) {
      for (const std::string & incpath : _include_paths_) {
        // Now search the file in some include dir:
        std::string rincpath = incpath;
        datatools::fetch_path_with_env(rincpath);
        DT_LOG_DEBUG(_logging_, "Candidate dir path '" << rincpath << "'");
        boost::filesystem::path bincpath = rincpath;
        DT_THROW_IF(! boost::filesystem::is_directory(bincpath),
                    std::runtime_error,
                    "Include dir path '" << bincpath << "' does not correspond to a directory!");
        boost::filesystem::path btestpath = bincpath / to_be_included_path_;
        if (boost::filesystem::is_regular_file(btestpath)) {
          bfullpath = btestpath;
          break;
        } 
      }
    }
    if (bfullpath.empty()) {
      DT_THROW(std::logic_error, "File was not found!");
    } else {
      DT_LOG_DEBUG(_logging_, "Found path " << bfullpath);
    }
    return bfullpath.string();
  }

  void file_include::_compute_internals_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    _include_paths_ = _explicit_include_paths_;
    std::list<std::string> _ev_include_paths_;
    if (has_include_path_env_name()) {
      const char * c_include_path_env = getenv(_include_path_env_name_.c_str());
      if (c_include_path_env != nullptr) {
        std::string include_path_env = c_include_path_env;
        DT_LOG_DEBUG(_logging_, "include_path_env = '" << include_path_env << "'");
        if (_include_path_env_strategy_ == EV_CLEAR) {
          _include_paths_.clear();
        }
        std::vector<std::string> split_results;
        boost::split(split_results, include_path_env, boost::is_any_of(":"));
        if (_include_path_env_strategy_ == EV_CLEAR or _include_path_env_strategy_ == EV_APPEND) {
          // [EV0]:[EV1]
          // [IC0]:[IC1]:[IC2]:[EV0]:[EV1]
          for (const auto & sr : split_results) {
            _include_paths_.push_back(sr);
          }
        } else {
          for (std::vector<std::string>::const_reverse_iterator it = split_results.rbegin();
               it != split_results.rend();
               it++) {
            // [EV0]:[EV1]:[IC0]:[IC1]:[IC2]
            _include_paths_.push_front(*it);
          }
        }
      } else {
        DT_LOG_DEBUG(_logging_, "no include path env variable '" << _include_path_env_name_ << "' was found.");
      }
    }
    if (_allow_current_directory_) {
      // We search for the file to be included in the current dir first
      _include_paths_.push_front(".");
    }
    _has_changed_ = false;
    if (datatools::logger::is_debug(_logging_)) {
      DT_LOG_DEBUG(_logging_, "File include : ");
      {
        boost::property_tree::ptree popts2;
        std::ostringstream indentss;
        indentss << "[debug] ";
        popts2.put("indent", indentss.str());
        this->print_tree(std::cerr, popts2);
      }
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void file_include::_reset_internals_()
  {
    _include_paths_.clear();    
    return;
  }
  
  bool file_include::is_initialized() const
  {
    return _include_paths_.size();
  }

  void file_include::initialize()
  {
    if (!is_initialized() or _has_changed_) {
      _compute_internals_(); 
    }
    return;
  }

  void file_include::reset()
  {
    _reset_internals_();
    _allow_current_directory_ = false;
    _include_path_env_strategy_ = EV_PREPEND;
    _include_path_env_name_.clear();
    _explicit_include_paths_.clear();
    _has_changed_ = false;
    return;
  }

  void file_include::print_tree(std::ostream & out_,
                                const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    std::ostringstream outs;
    if (! popts.title.empty ()) {
      outs << popts.indent << popts.title << std::endl;
    }

    outs << popts.indent << tag << "Allow current directory  : "
         << std::boolalpha << _allow_current_directory_ << "\n";
    
    outs << popts.indent << tag << "Include path environment strategy : ";
    switch (_include_path_env_strategy_) {
    case EV_PREPEND : 
      outs << "'prepend'";
      break;
    case EV_APPEND : 
      outs << "'append'";
      break;
    case EV_CLEAR : 
      outs << "'clear'";
    }
    outs << "\n";

    outs << popts.indent << tag << "Include path environment variable: '" << _include_path_env_name_ << "'\n";

    outs << popts.indent << tag << "Include paths : " << _include_paths_.size() << std::endl;
    {
      unsigned int counter = 0;
      for (const std::string & p : _include_paths_) {
        outs << popts.indent << skip_tag;
        if (++counter == _include_paths_.size()) {
          outs << last_tag;
        } else {
          outs << tag;
        }
        outs << counter << " : '" << p << "'" << std::endl;
      }
    }
    
    outs << popts.indent << inherit_tag(popts.inherit) << "Has changed : "
         << std::boolalpha << _has_changed_ << std::endl;
    
    out_ << outs.str();
    return;
  }

} // end of namespace datatools
