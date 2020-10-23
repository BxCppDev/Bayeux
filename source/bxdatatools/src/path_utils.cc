// path_utils.cc

// Ourselves
#include <datatools/path_utils.h>


// Standard Library:
#include <cstdlib>
#include <unistd.h>
#include <wordexp.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <memory>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/ioutils.h>
#include <datatools/library_info.h>
#include <datatools/properties.h>
#include <datatools/kernel.h>
#include <datatools/bit_mask.h>
#include <datatools/urn.h>
#include <datatools/urn_query_service.h>

namespace {

  //! Private implementation of the global path
  struct _gp {
    enum action_type {
      ACTION_GET = 1,
      ACTION_SET = 2,
      ACTION_RESET = 3
    };
    static const std::string & global_path(int action_, const std::string & gpath_ = "");
  };

  // Private Implementation of path processing/expansion
  /// \brief A path parser enabling environment variable as well as datatools kernel's library info
  class fetch_path_processor {
  public:
    enum use_type {
      USE_NOTHING          = 0x0,
      USE_TRACE            = datatools::bit_mask::bit01, //!< Activate trace logging
      USE_GLOBAL_PATH      = datatools::bit_mask::bit02, //!< Undocumented
      USE_ENVIRON          = datatools::bit_mask::bit03, //!< Accept environment variables in path
      USE_KERNEL_LIBINFO   = datatools::bit_mask::bit04, //!< Accept Kernel library query resolution
      USE_KERNEL_URN_QUERY = datatools::bit_mask::bit05, //!< Accept URN query resolution
      USE_DEFAULTS         = USE_ENVIRON | USE_KERNEL_LIBINFO| USE_KERNEL_URN_QUERY
    };

    struct lib_info_keys_entry_type
    {
      std::string install_path_key;
      std::string environ_path_key;
    };
    typedef std::map<std::string, lib_info_keys_entry_type> lib_info_keys_dict_type;

    static const lib_info_keys_dict_type & lib_info_keys();

    fetch_path_processor(const std::string & parent_path_ = "",
                         bool use_global_path_ = false,
                         bool use_env_ = true,
                         bool use_kernel_libinfo_ = true,
                         bool use_kernel_urn_query_ = true);

    explicit fetch_path_processor(uint32_t use_mode,
                                  const std::string & parent_path_ = "");

    bool process(std::string&);

    bool process(std::string& path_, std::string & error_msg_);

    void set_use_global_path(bool);

    bool use_global_path() const;

    void set_use_kernel_libinfo(bool);

    bool use_kernel_libinfo() const;

  private:
    void process_impl(std::string &);
    bool _trace_; //!< Activation of trace logging message
    bool _use_global_path_;
    bool _use_env_; //!< Accept environment variables in paths
    bool _use_kernel_libinfo_; //!< Accept library locator in paths
    bool _use_kernel_urn_query_; //!< Accept URN path resolution
    std::string _parent_path_;
  };

  fetch_path_processor::fetch_path_processor(uint32_t use_mode_,
                                             const std::string & parent_path_)
      // Use () initialization to allow implicit int -> bool
      : _trace_(use_mode_ & USE_TRACE),
        _use_global_path_(use_mode_ & USE_GLOBAL_PATH),
        _use_env_(use_mode_ & USE_ENVIRON),
        _use_kernel_libinfo_(use_mode_ & USE_KERNEL_LIBINFO),
        _use_kernel_urn_query_(use_mode_ & USE_KERNEL_URN_QUERY),
        _parent_path_{parent_path_}
  {
    return;
  }

  fetch_path_processor::fetch_path_processor(const std::string & parent_path_,
                                             bool use_global_path_,
                                             bool use_env_,
                                             bool use_kernel_libinfo_,
                                             bool use_kernel_urn_query_)
      : _trace_{false},
        _use_global_path_{use_global_path_},
        _use_env_{use_env_},
        _use_kernel_libinfo_{use_kernel_libinfo_},
        _use_kernel_urn_query_{use_kernel_urn_query_},
        _parent_path_{parent_path_}
  {
    return;
  }

  void fetch_path_processor::set_use_global_path(bool ugp_)
  {
    _use_global_path_ = ugp_;
  }

  bool fetch_path_processor::use_global_path() const
  {
    return _use_global_path_;
  }

  void fetch_path_processor::set_use_kernel_libinfo(bool ukl_)
  {
    _use_kernel_libinfo_ = ukl_;
    return;
  }

  bool fetch_path_processor::use_kernel_libinfo() const
  {
    return _use_kernel_libinfo_;
  }

  bool fetch_path_processor::process(std::string & path) {
    try {
      this->process_impl(path);
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot process path '" << path << "' : "
                   << error.what());
      return false;
    }
    return true;
  }

  bool fetch_path_processor::process(std::string & path, std::string & errmsg_) {
    try {
      errmsg_.clear();
      this->process_impl(path);
    } catch (std::exception & error) {
      errmsg_ = "Cannot process path '" + path + "' : " + error.what();
      return false;
    }
    return true;
  }

  // static
  const fetch_path_processor::lib_info_keys_dict_type & fetch_path_processor::lib_info_keys()
  {
    static std::unique_ptr<lib_info_keys_dict_type> _lik;
    if (! _lik) {
      _lik.reset(new lib_info_keys_dict_type);
      lib_info_keys_dict_type & keys = *_lik;

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_prefix();
        e.environ_path_key = datatools::library_info::keys().env_prefix();
        keys["prefix"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_resource_dir();
        e.environ_path_key = datatools::library_info::keys().env_resource_dir();
        keys["resources"] = e;
        keys[""] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_lib_dir();
        e.environ_path_key = datatools::library_info::keys().env_lib_dir();
        keys["libraries"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_bin_dir();
        e.environ_path_key = datatools::library_info::keys().env_bin_dir();
        keys["binaries"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_plugin_lib_dir();
        e.environ_path_key = datatools::library_info::keys().env_plugin_lib_dir();
        keys["plugins"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_data_dir();
        e.environ_path_key = datatools::library_info::keys().env_data_dir();
        keys["data"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_include_dir();
        e.environ_path_key = datatools::library_info::keys().env_include_dir();
        keys["includes"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = datatools::library_info::keys().install_doc_dir();
        e.environ_path_key = datatools::library_info::keys().env_doc_dir();
        keys["docs"] = e;
      }

    }
    return *_lik;
  }

  // Hmmm probably this method needs refactoring... (with Boost/Spirit?)
  // Well, Boost Spirit is probably expensive for such a simple parsing
  // (compilation time and size of code explodes with many levels of templatization...)
  void fetch_path_processor::process_impl(std::string& path) {
    // Special environment variable to trace the resolution of path
    bool trace = (getenv("DATATOOLS_FETCH_PATH_TRACE") != NULL) ? true : _trace_;

    if (trace) {
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
    }

    // working buffer:
    std::string text = path;
    // First we search for an URN scheme in the case the filename has been registered
    // in some URN system repository:
    if ( boost::starts_with(text, datatools::urn::urn_scheme() + datatools::urn::urn_separator())) {
      if (!_use_kernel_urn_query_) {
        // Unsupported URN scheme for path resolution
        DT_THROW(std::logic_error, "The path resolution from an URN tag is not allowed!");
      }
      // Found an URN scheme:
      // Examples:
      //   "urn:foo:bar"
      //   "urn:foo:bar:topic:setup:1.3.2-23"
      std::string urn_text = text;
      std::string urn_category;
      std::string urn_mime;
      std::string urn_path;
      // Search for a trailing requested category directive:
      // Examples:
      //   "urn:foo:bar:topic:setup:1.3.2-23@configuration"
      //   "urn:foo:bar:topic:result:1.3.2-23@data"
      //   "urn:foo:bar:topic:logo:small@image"
      std::size_t atpos = text.find('@');
      std::string tail;
      if (atpos != std::string::npos) {
        urn_text = text.substr(0, atpos);
        tail = text.substr(atpos + 1);
      }
      urn_category = tail;
      // We detected the signature of an URN which should be registered
      // in the kernel's URN query service (singleton)
      DT_THROW_IF(! datatools::kernel::is_instantiated(),
                  std::runtime_error,
                  "The Bayeux/datatools' kernel has not been instantiated !"
                  << "No support for 'urn:foo:bar[:baz[:...]][@CATEGORY]' syntax !");
      const datatools::kernel & dtk = datatools::kernel::instance();
      DT_THROW_IF(! dtk.has_urn_query(),
                  std::runtime_error,
                  "The Bayeux/datatools' kernel URN query service has not been activated !"
                  << "No support for 'urn:foo:bar[:baz[:...]][@CATEGORY]' syntax !");
      if (!dtk.get_urn_query().resolve_urn_to_path(urn_text,
                                                   urn_category,
                                                   urn_mime,
                                                   urn_path)) {
        DT_THROW(std::logic_error,
                 "Bayeux/datatools' kernel's URN query service failed to resolve path from URN '" << urn_text << "' "
                 << "in category '" << urn_category << "' with MIME type '" << urn_mime << "'!");
      }
      text = urn_path;
    }

    bool registered_lib_topic = false;
    if (text[0] == '@') {
      if (!_use_kernel_libinfo_) {
        // Unsupported mount point path resolution
        DT_THROW(std::logic_error, "The path resolution from a mount point is not allowed!");
      }
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering '@' mode...");
      }
      DT_THROW_IF(! datatools::kernel::is_instantiated(),
                  std::runtime_error,
                  "The Bayeux/datatools kernel has not been instantiated !"
                  << "No support for '@foo:bar.txt' syntax !");
      const datatools::kernel & dtk = datatools::kernel::instance();
      DT_THROW_IF(! dtk.has_library_info_register(),
                  std::runtime_error,
                  "The Bayeux/datatools kernel library info register has not been activated !"
                  << "No support for '@foo[.topic]:bar/blah.txt' syntax !");
      const datatools::library_info & lib_info_reg =
        datatools::kernel::instance().get_library_info_register();

      std::string resolved_path;
      std::string errmsg;
      if (!lib_info_reg.resolve_path(text, resolved_path, errmsg)) {
        DT_THROW(std::logic_error,
                 "Cannot resolve path from mount point : " << errmsg << "!");
      } else {
        text = resolved_path;
        registered_lib_topic = true;
      }
    } // end if (text[0] == '@')

    DT_THROW_IF(text.find('?') != text.npos || text.find('*') != text.npos,
                std::logic_error,
                "Wildcard characters found in path = " << text);

    if (_use_env_) {
      // Activate the parsing of embedded environment variables:
      wordexp_t p;
      // bool freed = false;
      std::string error_message;
      const char * word = text.c_str();
      try {
        int we_error = wordexp( word, &p, WRDE_NOCMD|WRDE_SHOWERR|WRDE_UNDEF);
        switch (we_error) {
        case WRDE_BADCHAR:
          DT_THROW_IF(true,
                      std::logic_error,
                      "wordexp error, code = " << we_error << " : Unquoted characters for input = '" << path << "'");
        case WRDE_BADVAL:
          DT_THROW_IF(true,
                      std::logic_error,
                      "wordexp error, code = " << we_error << " : Undefined shell variable for input = '" << path << "'");
        case WRDE_CMDSUB:
          DT_THROW_IF(true,
                      std::logic_error,
                      "wordexp error, code = " << we_error << " : Unauthorized command substitution for input = '" << path << "'");
        case WRDE_NOSPACE:
          // 2014-04-29 FM: Calling wordfree here at least (following the GNU example)
          wordfree( &p );
          // freed = true;
          DT_THROW_IF(true,
                      std::logic_error,
                      "wordexp error, code = " << we_error << " : Attempt to allocate memory failed for input = '" << path << "'");
        case WRDE_SYNTAX:
          DT_THROW_IF(true,
                      std::logic_error,
                      "wordexp error, code = " << we_error << " : Shell syntax error for input = '" << path << "'");
        default:
          if (p.we_wordc == 0) {
            DT_THROW_IF(true, std::logic_error, "Nothing to expand !");
          } else {
            char** w = p.we_wordv;
            if (p.we_wordc > 1) {
              std::ostringstream message;
              message << "wordexp expands to many tokens : ";
              for (size_t i = 0; i < p.we_wordc; i++) {
                message << " '" << w[i] << "'";
              }
              DT_THROW_IF(true, std::logic_error, message.str());
            } else {
              // std::ostringstream s;
              // s << w[0];
              text = w[0]; //s.str();
            }
          }
        } // switch (we_error)
      } catch (std::exception & we_x) {
        error_message = we_x.what();
      }
      // 2014-04-29 FM: Calling wordfree segfaults!!! (the manual is not clear about the need for wordfree)
      // if (! freed) {
      //   std::cerr << "FATAL ************* " << std::endl;
      //   usleep(100000);
      //   wordfree( &p );
      // }
      DT_THROW_IF(!error_message.empty(),
                  std::logic_error,
                  "Shell expansion error: " << error_message);
    } // if (_use_env_)
    path = text;

    if (! registered_lib_topic) {
      // If the path does not start with a "@libname[.XXX]:" prefix
      // we can search for an explicit parent path :
      // Check for an explicit parent path :
      std::string parent_path = _parent_path_;
      if (parent_path.empty()) {
        // Check for an implicit parent path :
        if (use_global_path() && datatools::has_global_path())  {
          parent_path = datatools::get_global_path();
        }
      }
      if (! parent_path.empty()
          && (path.substr(0, 1) != "/")
          && (path != ".")
          && (path.substr(0, 2) != "./")
          && (path.find(":/") == std::string::npos)) {
        // Process relative path with prepend parent path :
        text = parent_path + '/' + path;
      }
    }
    path = text;
    return;
  }


  // This member function uses fetch_path_processor, so must come after
  // its declaration...
  const std::string & _gp::global_path(int action_,
                                       const std::string & gpath_)
  {
    static std::unique_ptr<std::string> _gpath;
    if (_gpath.get() == nullptr) {
      _gpath.reset(new std::string);
    }
    std::string & gpath = *_gpath.get();
    {
      char * egp = ::getenv("BXDATATOOLS_GLOBAL_PATH");
      if (gpath.empty() && egp != 0) {
        std::clog << datatools::io::notice
                  << "datatools::_gp::global_path: "
                  << "Set the global path from the 'BXDATATOOLS_GLOBAL_PATH' environment variable."
                  << std::endl;
        gpath = egp;
      }
    }
    if (action_ == ACTION_RESET) {
      gpath.clear();
    } else if (action_ == ACTION_GET) {
    } else if (action_ == ACTION_SET) {
      if (! gpath_.empty()) {
        fetch_path_processor fpp;
        fpp.set_use_global_path(false);
        std::string p = gpath_;
        fpp.process(p);
        gpath = p;
      }
    }
    return gpath;
  }
}


namespace datatools {
  bool has_global_path()
  {
    return ! _gp::global_path(_gp::ACTION_GET).empty();
  }

  void set_global_path(const std::string & gpath_)
  {
    _gp::global_path(_gp::ACTION_SET, gpath_);
  }

  void reset_global_path()
  {
    _gp::global_path(_gp::ACTION_RESET);
  }

  const std::string & get_global_path()
  {
    return _gp::global_path(_gp::ACTION_GET);
  }

  //----------------------------------------------------------------------
  // Free functions
  bool resolve_library_info_path_keys(const std::string & library_topic_,
                                      std::string & install_path_key_,
                                      std::string & environ_path_key_)
  {
    const auto& lik = fetch_path_processor::lib_info_keys();
    auto found = lik.find(library_topic_);
    if (found == lik.end()) {
      return false;
    }
    if (! found->second.install_path_key.empty()) {
      install_path_key_ = found->second.install_path_key;
      return true;
    }
    if (! found->second.environ_path_key.empty()) {
      environ_path_key_ = found->second.environ_path_key;
      return true;
    }
    return false;
  }

  std::string fetch_path(const std::string& word) {
    std::string w = word; // Working buffer
    fetch_path_processor fpp("", true, true, true, true);
    bool r = fpp.process(w);
    DT_THROW_IF(!r, std::logic_error, "Cannot fetch path from '" << word << "' !");
    return w;
  }

  bool fetch_path(std::string& path, std::string & errmsg_) {
    fetch_path_processor fpp("", true, true, true, true);
    return fpp.process(path, errmsg_);
  }

  bool fetch_path_without_env(std::string& path) {
    fetch_path_processor fpp("", false, false, true, true);
    return fpp.process(path);
  }

  bool fetch_path_with_env(std::string& path) {
    fetch_path_processor fpp("", false, true, true, true);
    return fpp.process(path);
  }

  bool fetch_path_with_env(std::string& path, std::string & errmsg_) {
    fetch_path_processor fpp("", false, true, true, true);
    return fpp.process(path, errmsg_);
  }

  bool fetch_path_with_env_p(std::string& path,
                             const std::string & parent_path_) {
    fetch_path_processor fpp(parent_path_, false, true, true, true);
    return fpp.process(path);
  }

  bool fetch_path_with_env_g(std::string& path) {
    fetch_path_processor fpp("", true, true, true, true);
    return fpp.process(path);
  }

  bool fetch_path_with_env_pg(std::string& path,
                              const std::string & parent_path_) {
    fetch_path_processor fpp(parent_path_, true, true, true, true);
    return fpp.process(path);
  }

  std::string expand_path(const std::string& path) {
    std::string res("");
    if (path.empty()) return (res);

    const std::string allowed =
      ":~./_-${}0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int found = path.find_first_not_of(allowed);

    DT_THROW_IF(found > 0, std::runtime_error, "invalid character found");

    std::ostringstream sh_cmd;
    char dummy[256];
    sprintf(dummy, "tmp.XXXXXX");

    int tmpfile = mkstemp(dummy);
    DT_THROW_IF(tmpfile == -1, std::runtime_error, "Failed to create tmp file");

    sh_cmd << "echo \"" << path << "\" > " << dummy;

    int ret = system(sh_cmd.str().c_str());
    DT_THROW_IF(ret != 0, std::runtime_error, "Shell command " << sh_cmd.str() << " failed");

    std::ifstream input(dummy);
    DT_THROW_IF(!input, std::runtime_error, "Cannot open file " << dummy);

    std::string line_get;
    std::getline(input, line_get);
    DT_THROW_IF(!input, std::runtime_error, "Cannot read file " << dummy);

    input.close();
    close(tmpfile);
    ret = unlink(dummy);

    if (ret != 0) {
      std::cerr << "expand_path: warning: "
                << "Cannot delete temporary file '"
                << dummy << "'" << std::endl;
    }
    return (line_get);
  }

} // namespace datatools

