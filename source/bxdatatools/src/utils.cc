// utils.cc

// Ourselves:
#include <datatools/utils.h>

// Standard Library:
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <wordexp.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
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

namespace datatools {

  const std::string & empty_string()
  {
    static const std::string _e;
    return _e;
  }
  
  const std::string & none_label()
   {
     static const std::string _l("none");
    return _l;
  }
 
  const std::string & yes_label()
    {
    static const std::string _l("yes");
    return _l;
  }

  const std::string & no_label()
  {
    static const std::string _l("no");
    return _l;
  }

  // const boost::property_tree::ptree & empty_options()
  // {
  //   static const boost::property_tree::ptree _empty;
  //   return _empty;
  // }

  /* Single precision utility functions */

  void invalidate(float& x) {
    x = std::numeric_limits<float>::quiet_NaN();
  }

  float invalid_real_single()
  {
    return std::numeric_limits<float>::quiet_NaN();
  }

  bool is_valid(float x) {
    return x == x;
  }

  bool is_infinity(float x) {
    return std::isinf(x);
  }

  bool is_plus_infinity(float x)
  {
    return is_infinity(x) && x > 0.0F;
  }

  bool is_minus_infinity(float x)
  {
    return is_infinity(x) && x < 0.0F;
  }

  void plus_infinity(float& x) {
    x = std::numeric_limits<float>::infinity();
  }

  void minus_infinity(float& x) {
    x = -std::numeric_limits<float>::infinity();
  }

  void infinity(float& x) {
    plus_infinity(x);
  }

  /* Double precision utility functions */

  void invalidate(double& x) {
    x = std::numeric_limits<double>::quiet_NaN();
  }

  double invalid_real()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }

  double invalid_real_double()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }

  bool is_valid(double x) {
    return x == x;
  }

  bool is_infinity(double x) {
    return std::isinf(x);
  }

  bool is_plus_infinity(double x)
  {
    return is_infinity(x) && x > 0.0;
  }

  bool is_minus_infinity(double x)
  {
    return is_infinity(x) && x < 0.0;
  }

  bool is_normal(double x) {
    return ! is_infinity(x) && is_valid(x);
  }

  void plus_infinity(double& x) {
    x = std::numeric_limits<double>::infinity();
  }

  void minus_infinity(double& x) {
    x = -std::numeric_limits<double>::infinity();
  }

  void infinity(double& x) {
    plus_infinity(x);
  }

  compare_result_type compare_real(double x1_, double x2_,
                                   double abs_eps_, double rel_eps_)
  {
    if (std::isunordered(x1_,x2_)) {
      // At least one id a NaN: cannot compare NaNs:
      return COMPARE_UNDEFINED;
    }

    if (is_infinity(x1_) && is_infinity(x2_)) {
      // Both numbers are infinite:
      if (is_minus_infinity(x1_) && is_minus_infinity(x2_)) {
        return COMPARE_UNDEFINED;
      }
      if (is_plus_infinity(x1_) && is_plus_infinity(x2_)) {
        return COMPARE_UNDEFINED;
      }
      if (is_minus_infinity(x1_) && is_plus_infinity(x2_)) {
        return COMPARE_LESS;
      }

      if (is_plus_infinity(x1_) && is_minus_infinity(x2_)) {
        return COMPARE_GREATER;
      }
    }

    // Only one number is infinite:
    if (is_minus_infinity(x1_)) {
      return COMPARE_LESS;
    }
    if (is_plus_infinity(x1_)) {
      return COMPARE_GREATER;
    }
    if (is_minus_infinity(x2_)) {
      return COMPARE_GREATER;
    }
    if (is_plus_infinity(x2_)) {
      return COMPARE_LESS;
    }

    // Both numbers are finite:

    // First we try to compare numbers using an absolute tolerance, if given
    double abseps = abs_eps_;
    if (is_valid(abseps) && abseps > 0.0) {
      // We compare them using an absolute tolerance:
      if (x1_ < (x2_ - abseps)) {
        return COMPARE_LESS;
      } else if (x1_ > (x2_ + abseps)) {
        return COMPARE_GREATER;
      } else {
        return COMPARE_EQUAL;
      }
    }

    // If no absolute tolerance is provided, try to use comparison
    // with respect to a relative error:
    double releps = rel_eps_;
    if (!is_valid(abseps) || abseps < 0.0) {
      releps = std::numeric_limits<double>::epsilon();
    }
    double diff = std::abs(x1_ - x2_);
    double ax1 = std::fabs(x1_);
    double ax2 = std::fabs(x2_);
    double largest = (ax1 > ax2) ? ax1 : ax2;
    if (diff <= largest * releps) {
      return COMPARE_EQUAL;
    } else if (x1_ < x2_) {
      return COMPARE_LESS;
    } else {
      return COMPARE_GREATER;
    }

    return COMPARE_UNDEFINED;
  }

  /* String utility functions */

  bool name_validation(const std::string & name_, uint32_t flags_ )
  {
    static const std::string _base_allowed_chars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.:-";
    static const std::string _upper_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const std::string _lower_chars =
      "abcdefghijklmnopqrstuvwxyz";
    if (name_.empty()) return false;
    bool forbid_colon  = flags_ & NV_NO_COLON;
    bool forbid_dot    = flags_ & NV_NO_DOT;
    bool forbid_hyphen = flags_ & NV_NO_HYPHEN;
    bool forbid_underscore = flags_ & NV_NO_UNDERSCORE;
    bool allow_leading_digit = flags_ & NV_LEADING_DIGIT;
    bool forbid_lower_case = flags_ & NV_NO_LOWER_CASE;
    bool forbid_upper_case = flags_ & NV_NO_UPPER_CASE;
    if (forbid_colon) {
      if (name_.find(':') != name_.npos) return false;
    }
    if (forbid_dot) {
      if (name_.find('.') != name_.npos) return false;
    }
    if (forbid_hyphen) {
      if (name_.find('-') != name_.npos) return false;
    }
    if (forbid_underscore) {
      if (name_.find('_') != name_.npos) return false;
    }
    if (forbid_lower_case) {
      if (name_.find_first_of(_lower_chars) != name_.npos) return false;
    }
    if (forbid_upper_case) {
      if (name_.find_first_of(_upper_chars) != name_.npos) return false;
    }
    if (name_.find_first_not_of(_base_allowed_chars) != name_.npos) return false;
    if (!allow_leading_digit) {
      if (name_.find_first_of("0123456789.") == 0) return false;
    }
    // Reject trailing dot:
    if (name_[name_.size()-1] == '.') return false;
    // Reject trailing colon:
    if (name_[name_.size()-1] == ':') return false;
    // Reject trailing hyphen:
    if (name_[name_.size()-1] == '-') return false;
    // // Colon must be followed by one other colon (example: "::")
    // unsigned int colon_first = name_.find(':');
    // if (colon_first != name_.npos) {
    //   if (name_[colon_first + 1] != ':') return false;
    // }
    return true;
  }

  bool is_quoted(const std::string & text_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    char c = q_;
    if (text_.size() > 1) {
      return (text_[0] == c) && (text_[text_.size() - 1] == c);
    }
    return false;
  }

  void add_quotes(const std::string & from_, std::string & to_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    if (! is_quoted(from_, q_)) {
      std::ostringstream oss;
      oss << q_ << from_ << q_;
      to_ = oss.str();
    } else {
      to_ = from_;
    }
    return;
  }

  void add_quotes(std::string & text_, char q_)
  {
    std::string t;
    add_quotes(text_, t, q_);
    text_ = t;
    return;
  }

  void remove_all_quotes(std::string & text_)
  {
    if (is_quoted(text_, '"')) {
      remove_quotes(text_, '"');
    } else if (is_quoted(text_, '\'')) {
      remove_quotes(text_, '\'');
    }
    return;
  }

  void remove_quotes(const std::string & from_, std::string & to_, char q_)
  {
    DT_THROW_IF(q_ != '\'' && q_ != '"',
                std::range_error,
                "Unsupported quoting character '" << q_ << "'!");
    if (is_quoted(from_, q_)) {
      to_ = from_.substr(1,from_.length()-2);
    } else {
      to_ = from_;
    }
    return;
  }

  void remove_quotes(std::string & text_, char q_)
  {
    std::string t;
    remove_quotes(text_, t, q_);
    text_ = t;
    return;
  }

  struct _gp {
    enum action_type {
      ACTION_GET = 1,
      ACTION_SET = 2,
      ACTION_RESET = 3
    };
    static const std::string & global_path(int action_, const std::string & gpath_ = "");
  };

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

  /// \brief A path parser enabling environment variable as well as datatools kernel's library info
  class fetch_path_processor {
  public:

    enum use_type {
      USE_NOTHING          = 0x0,
      USE_TRACE            = bit_mask::bit01, //!< Activate trace logging
      USE_GLOBAL_PATH      = bit_mask::bit02, //!< Undocumented
      USE_ENVIRON          = bit_mask::bit03, //!< Accept environment variables in path
      USE_KERNEL_LIBINFO   = bit_mask::bit04, //!< Accept Kernel library query resolution
      USE_KERNEL_URN_QUERY = bit_mask::bit05, //!< Accept URN query resolution
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

    void set_use_kernel_urn_query(bool);

    bool use_kernel_urn_query() const;

  private:
    void process_impl(std::string &);
    bool _trace_; //!< Activation of trace logging message
    bool _use_global_path_;
    bool _use_env_; //!< Accept environment variables in paths
    bool _use_kernel_libinfo_; //!< Accept library locator in paths
    bool _use_kernel_urn_query_; //!< Accept URN path resolution
    std::string _parent_path_;
  };

  bool resolve_library_info_path_keys(const std::string & library_topic_,
                                      std::string & install_path_key_,
                                      std::string & environ_path_key_)
  {
    typedef fetch_path_processor::lib_info_keys_dict_type dict_type;
    const dict_type & lik = fetch_path_processor::lib_info_keys();
    dict_type::const_iterator found = lik.find(library_topic_);
    if (found == lik.end()) {
      return false;
    }
    bool valid = false;
    if (! found->second.install_path_key.empty()) {
      install_path_key_ = found->second.install_path_key;
      valid = true;
    }
    if (! found->second.environ_path_key.empty()) {
      environ_path_key_ = found->second.environ_path_key;
      valid = true;
    }
    return valid;
  }

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

  void fetch_path_processor::set_use_kernel_urn_query(bool u_)
  {
    _use_kernel_urn_query_ = u_;
    return;
  }

  bool fetch_path_processor::use_kernel_urn_query() const
  {
    return _use_kernel_urn_query_;
  }

  fetch_path_processor::fetch_path_processor(uint32_t use_mode_,
                                             const std::string & parent_path_)
  {
    _trace_                = use_mode_ & USE_TRACE;
    _use_global_path_      = use_mode_ & USE_GLOBAL_PATH;
    _use_env_              = use_mode_ & USE_ENVIRON;
    _use_kernel_libinfo_   = use_mode_ & USE_KERNEL_LIBINFO;
    _use_kernel_urn_query_ = use_mode_ & USE_KERNEL_URN_QUERY;
    _parent_path_ = parent_path_;
    return;
  }

  fetch_path_processor::fetch_path_processor(const std::string & parent_path_,
                                             bool use_global_path_,
                                             bool use_env_,
                                             bool use_kernel_libinfo_,
                                             bool use_kernel_urn_query_) {
    _trace_ = false;
    _parent_path_ = parent_path_;
    _use_global_path_ = use_global_path_;
    _use_env_ = use_env_;
    _use_kernel_libinfo_ = use_kernel_libinfo_;
    _use_kernel_urn_query_ = use_kernel_urn_query_;
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
        e.install_path_key = library_info::keys().install_prefix();
        e.environ_path_key = library_info::keys().env_prefix();
        keys["prefix"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_resource_dir();
        e.environ_path_key = library_info::keys().env_resource_dir();
        keys["resources"] = e;
        keys[""] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_lib_dir();
        e.environ_path_key = library_info::keys().env_lib_dir();
        keys["libraries"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_bin_dir();
        e.environ_path_key = library_info::keys().env_bin_dir();
        keys["binaries"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_plugin_lib_dir();
        e.environ_path_key = library_info::keys().env_plugin_lib_dir();
        keys["plugins"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_data_dir();
        e.environ_path_key = library_info::keys().env_data_dir();
        keys["data"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_include_dir();
        e.environ_path_key = library_info::keys().env_include_dir();
        keys["includes"] = e;
      }

      {
        lib_info_keys_entry_type e;
        e.install_path_key = library_info::keys().install_doc_dir();
        e.environ_path_key = library_info::keys().env_doc_dir();
        keys["docs"] = e;
      }

    }
    return *_lik;
  }

  // Hmmm probably this method needs refactoring... (with Boost/Spirit?)
  // Well, Boost Spirit is probably expensive for such a simple parsing
  // (compilation time and size of code explodes with many levels of templatization...)
  void fetch_path_processor::process_impl(std::string& path) {
    bool trace = _trace_;
    {
      // Special environment variable to trace the resolution of path
      char * env = getenv("DATATOOLS_FETCH_PATH_TRACE");
      if (env != NULL) {
        std::string str_env = env;
        if (str_env == "1") {
          trace = true;
          DT_LOG_TRACE(datatools::logger::PRIO_TRACE,
                       "Activating the TRACE logging level thanks to the 'DATATOOLS_FETCH_PATH_TRACE' environment variable for path '" << path << "'...");
        }
      }
    }
    if (trace) {
      DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
    }
    // working buffer:
    std::string text = path;
    // First we search for an URN scheme in the case the filename has been registered
    // in some URN system repository:
    if ( boost::starts_with(text, urn::urn_scheme() + urn::urn_separator())) {
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
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering '@' mode for text='" << text << "'");
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
      if (trace) {
        DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "text='" << text << "'");
      }
      std::string resolved_path;
      std::string errmsg;
      if (!lib_info_reg.resolve_path(text, resolved_path, errmsg)) {
        DT_THROW(std::logic_error,
                 "Cannot resolve path from mount point : " << errmsg << "!");
      } else {
        if (trace) {
          DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "resolved_path='"<< resolved_path << "'");
        }
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
      if (!error_message.empty()) {
        DT_THROW_IF(true,
                    std::logic_error,
                    "Shell expansion error: " << error_message);
      }
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
          parent_path = get_global_path();
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

  /* From:
     Data Structures in C++ Using the STL
     by Timothy A. Budd
     published by Addison Wesley Longman
     ISBN 0-201-31659-5
     http://web.engr.oregonstate.edu/~budd/Books/stl/info/ReadMe.html
  */
  void split_string(const std::string& text, const std::string& separators,
                    std::list<std::string>& words) {
    // split a string into a list of words
    // 'text_' and 'separators_' are input,
    // list of 'words_' is output
    int text_len = text.length();

    // find first non-separator character
    int start = text.find_first_not_of(separators, 0);

    // loop as long as we have a non-separator character
    while ((start >= 0) && (start < text_len)) {
      // find end of current word
      int stop = text.find_first_of(separators, start);

      if ((stop < 0) || (stop > text_len)) stop = text_len;

      // add word to list of words
      words.push_back(text.substr(start, stop - start));

      // find start of next word
      start = text.find_first_not_of(separators, stop+1);
    }
  }

} // namespace datatools
