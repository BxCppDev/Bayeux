// datatools/properties_config.cc

// Ourselves:
#include <datatools/properties_config.h>

// Standard Library
#include <fstream>

// Third Party:
// - Boost:
#include <boost/algorithm/string/trim.hpp>

// This Project
#include <datatools/configuration/io.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>

// clang-format off
#define DT_PROP_CFG_READ_THROW_IF(Condition, ExceptionType,             \
                                  FileName,                             \
                                  SectionName, SectionLineNumber,       \
                                  LineNumber, Message)                  \
  {                                                                     \
    if (Condition) {                                                    \
      std::stringstream sDT_THROW_IF_ONLY;                              \
      sDT_THROW_IF_ONLY << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": "; \
      if (!FileName.empty()) {                                          \
        sDT_THROW_IF_ONLY << "in file '" << FileName << "': ";          \
      }                                                                 \
      if (SectionName.empty()) {                                        \
        if (LineNumber > 0) {                                           \
          sDT_THROW_IF_ONLY << "at line #" << LineNumber << ": ";       \
        }                                                               \
      } else {                                                          \
        sDT_THROW_IF_ONLY << "in section '" << SectionName << "'";      \
        if (SectionLineNumber > 0) {                                    \
          sDT_THROW_IF_ONLY << " starting at line #" << SectionLineNumber; \
        }                                                               \
        sDT_THROW_IF_ONLY << ": ";                                      \
      }                                                                 \
      sDT_THROW_IF_ONLY << Message << "]";                              \
      throw ExceptionType(sDT_THROW_IF_ONLY.str());                     \
    }                                                                   \
  }                                                                     \
  /**/

#define DT_PROP_CFG_READ_LOG_DEBUG(LogLevel,FileName,                   \
                                   SectionName, SectionLineNumber,      \
                                   LineNumber, Message)                 \
  {                                                                     \
    std::stringstream sDT_LOG;                                          \
    sDT_LOG << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": "; \
    if (!FileName.empty()) {                                            \
      sDT_LOG << "in file '" << FileName << "': ";                      \
    }                                                                   \
    if (SectionName.empty()) {                                          \
      if (LineNumber > 0) {                                             \
        sDT_LOG << "at line #" << LineNumber << ": ";                   \
      }                                                                 \
    } else {                                                            \
      sDT_LOG << "in section '" << SectionName << "'";                  \
      if (SectionLineNumber > 0) {                                      \
        sDT_LOG << " starting at line #" << SectionLineNumber;          \
      }                                                                 \
      sDT_LOG << ": ";                                                  \
    }                                                                   \
    sDT_LOG << Message << "]";                                          \
    DT_LOG_DEBUG(LogLevel, sDT_LOG.str());                              \
  }                                                                     \
// clang-format on

namespace {
  struct _format {
    static const char OPEN_VECTOR  = '['; ///< Open section character
    static const char ASSIGN_CHAR  = '='; ///< Assignement key/meta label character
    static const char DESC_CHAR    = ':'; ///< Type descriptor separator character
    static const char CLOSE_VECTOR = ']'; ///< Close section character
    static const char COMMENT_CHAR = '#'; ///< Comment character
    static const char METACOMMENT_CHAR = '@'; ///< Comment character
    static const char SPACE_CHAR   = ' '; ///< Space character
    static const char CONTINUATION_CHAR = '\\'; ///< Continuation character
  };
}


namespace datatools {
  //----------------------------------------------------------------------
  // properties_config class implementation
  // Contains the actual parsing of onput ASCII
  properties_config::properties_config(uint32_t options_,
                                       const std::string & topic_,
                                       const std::string & section_name_,
                                       int section_start_line_number_)
  {
    _init_defaults_();
    _write_public_only_ = (options_ & SKIP_PRIVATE);
    _forbid_variants_ = (options_ & FORBID_VARIANTS);
    _forbid_include_ = (options_ & FORBID_INCLUDE);

    if (options_ & LOG_MUTE) {
      _logging_ = datatools::logger::PRIO_FATAL;
    }
    if (options_ & LOG_DEBUG) {
      _logging_ = datatools::logger::PRIO_DEBUG;
    }
    if (options_ & LOG_TRACE) {
      _logging_ = datatools::logger::PRIO_TRACE;
    }
    if (options_ & LOG_WARNING) {
      _logging_ = datatools::logger::PRIO_WARNING;
    }

    _use_smart_modulo_ = (options_ & SMART_MODULO);

    if (options_ & HEADER_FOOTER) {
      _mode_ = MODE_HEADER_FOOTER;
    }

    _dont_clear_ = (options_ & DONT_CLEAR);

    _resolve_path_ = (options_ & RESOLVE_PATH);

    if (!topic_.empty()) {
      set_topic(topic_);
    }
    if (!section_name_.empty()) {
      set_section_info(section_name_, section_start_line_number_);
    }
    return;
  }

  void properties_config::_init_defaults_()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    _mode_ = MODE_BARE;
    _dont_clear_ = false;
    _use_smart_modulo_ = false;
    _write_public_only_ = false;
    _current_line_number_ = 0;
    _forbid_variants_ = false;
    _forbid_include_ = false;
    _requested_topic_ = false;
    _resolve_path_    = false;
    _allow_key_override_ = false;
    _section_start_line_number_ = -1;
    return;
  }

  bool properties_config::has_section_info() const
  {
    return !_section_name_.empty();
  }

  void properties_config::set_section_info(const std::string & section_name_,
                                            int section_start_line_number_)
  {
    _section_name_ = section_name_;
    _section_start_line_number_ = section_start_line_number_;
    return;
  }

  void properties_config::reset_section_info()
  {
    _section_name_.clear();
    _section_start_line_number_ = -1;
    return;
  }

  const std::string & properties_config::get_section_name() const
  {
    return _section_name_;
  }

  int properties_config::get_section_start_line_number() const
  {
    return _section_start_line_number_;
  }

  bool properties_config::has_topic() const
  {
    return !_topic_.empty();
  }

  void properties_config::set_topic(const std::string & topic_)
  {
    if (!topic_.empty()) {
      uint32_t validation_flags = 0;
      // validation_flags != NV_NO_HYPHEN
      DT_THROW_IF(!name_validation(topic_, validation_flags),
                  std::logic_error,
                  "Keyword/topic '" << topic_ << "' contains forbidden characters!");
      _requested_topic_ = true;
    } else {
      _requested_topic_ = false;
    }
    _topic_ = topic_;
    return;
  }

  const std::string & properties_config::get_topic() const
  {
    return _topic_;
  }

  // static
  const std::string & properties_config::lock_decorator()
  {
    static const std::string _label("const");
    return _label;
  }

  // static
  const std::string & properties_config::as_directive()
  {
    static const std::string _label("as");
    return _label;
  }

  // static
  const std::string & properties_config::in_directive()
  {
    static const std::string _label("in");
    return _label;
  }

  // static
  const std::string & properties_config::path_decorator()
  {
    static const std::string _label("path");
    return _label;
  }

  // static
  const std::string & properties_config::metacomment_prefix()
  {
    static const std::string _label("#@");
    return _label;
  }

  void properties_config::write_data(std::ostream & out_,
                                      const std::string & prop_key_,
                                      const properties::data & data_,
                                      const std::string & /* unit_symbol_ */,
                                      const std::string & /* unit_label_ */,
                                      const std::string & comment_)
  {
    std::string eol = "\n";
    if (!comment_.empty()) {
      DT_THROW_IF(comment_[0] != _format::COMMENT_CHAR, std::logic_error,
                  "Comment line prefix does not start with '" << _format::COMMENT_CHAR << "'!");
      eol += comment_;
    }

    bool real_with_unit = false;
    std::string unit_symbol;
    std::string unit_label;
    double      unit_value = 1.0;
    if (data_.is_real()) {
      if (data_.has_unit_symbol()) {
        real_with_unit = true;
        unit_symbol = data_.get_unit_symbol();
      }
      if (!unit_symbol.empty()) {
        std::string unit_label2;
        DT_THROW_IF(!units::find_unit(unit_symbol, unit_value, unit_label2),
                    std::logic_error,
                    "Invalid unit symbol '" << unit_symbol << "'!");
      }
    }
    if (real_with_unit) {
      write_metacomment(out_, "enable_real_with_unit", "", comment_);
    }

    if (! data_.get_description().empty()) {
      write_metacomment(out_, "description", data_.get_description(), comment_);
    }

    // name:
    out_ << prop_key_ << _format::SPACE_CHAR << _format::DESC_CHAR << _format::SPACE_CHAR;

    if (data_.is_locked()) {
      out_ << lock_decorator() << _format::SPACE_CHAR;
    }

    // type:
    int size = properties::data::SCALAR_SIZE;
    out_ << data_.get_type_label();

    if (data_.is_vector()) {
      size = data_.get_size();
      out_ << _format::OPEN_VECTOR << data_.get_size() << _format::CLOSE_VECTOR;
    }

    // Real :
    if (data_.is_real()) {
      // Vectors of real with the same explicit unit sybol applied to all items :
      if (data_.is_vector()) {
        if (! unit_symbol.empty()) {
          out_ << _format::SPACE_CHAR << in_directive() << _format::SPACE_CHAR << unit_symbol << _format::SPACE_CHAR;
        }
      }
      // Scalar real with the explicit unit label applied to all items :
      else {
        if (! unit_label.empty()) {
          out_ << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << unit_label << _format::SPACE_CHAR;
        }
      }
    }

    // String :
    if (data_.is_string() && data_.is_explicit_path()) {
      out_ << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << path_decorator() << _format::SPACE_CHAR;
    }
    out_ << _format::SPACE_CHAR;
    out_ << _format::ASSIGN_CHAR;

    int modulo = 1; // Default modulo (for real and string)

    if (data_.is_boolean()) modulo = 10;

    if (data_.is_integer()) modulo = 5;

    if (_use_smart_modulo_) {
      if ((size > 1) && (size > modulo)) {
        out_ << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << std::endl;
      }
    }

    // Values:
    // For scalar or vector/array :
    for (int i = 0; i < size; i++) {
      out_ << _format::SPACE_CHAR;
      if (data_.is_boolean()) datatools::io::write_boolean(out_, data_.get_boolean_value(i));

      if (data_.is_integer()) datatools::io::write_integer(out_, data_.get_integer_value(i));

      if (data_.is_real()) {
        double val = data_.get_real_value(i);
        val /= unit_value;
        datatools::io::write_real_number(out_, val, datatools::io::REAL_PRECISION);
        if (data_.is_scalar() && ! unit_symbol.empty()) {
          out_ << _format::SPACE_CHAR << unit_symbol;
        }
      }

      if (data_.is_string()) {
        datatools::io::write_quoted_string(out_, data_.get_string_value(i));
      }

      if (_use_smart_modulo_) {
        if ((i < (size - 1)) && ((i + 1) % modulo) == 0) {
          out_ << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << std::endl;
        }
      }
    }
    out_ << std::endl;

    if (real_with_unit) {
      write_metacomment(out_, "disable_real_with_unit", "", comment_);
    }
    return;
  }

  void properties_config::write_metacomment(std::ostream & out_,
                                             const std::string & tag_,
                                             const std::string & value_,
                                             const std::string & comment_)
  {
    std::string eol = "\n";
    if (!comment_.empty()) eol += comment_;
    out_ << eol;

    out_ << metacomment_prefix() << tag_;
    if (!value_.empty()) {
      out_ << _format::SPACE_CHAR << value_;
    }
    out_ << eol;
    return;
  }

  void properties_config::write(const std::string & filename_,
                                 const properties & props_)
  {
    std::string the_filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(the_filename),
                  std::logic_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ofstream fout(the_filename.c_str());
    DT_THROW_IF(!fout,
                std::logic_error,
                "Cannot open filename '" + filename_ + "' (resolved as '" + the_filename + "'!");
    write(fout, props_);
    fout.close();
    return;
  }

  void properties_config::write(std::ostream & out_,
                                 const properties & props_)
  {
    this->_write_(out_, props_);
    return;
  }

  void properties_config::_write_(std::ostream & out_,
                                   const properties & props_)
  {
    if (_mode_ == MODE_HEADER_FOOTER) {
      out_ << "# List of configuration properties (datatools::properties)"
           << std::endl << std::endl;
    }

    if (!props_.get_description().empty()) {
      out_ << "#@configuration" << _format::SPACE_CHAR << props_.get_description() << std::endl;
      out_ << std::endl;
    }

    if (has_topic() and _requested_topic_) {
      out_ << "#@topic" << _format::SPACE_CHAR << get_topic() << std::endl;
    }

    for (const auto & k : props_.keys()) {
      if (_write_public_only_) {
        if (properties::key_is_private(k)) continue;
      }

      write_data(out_, k, props_.get(k), "", "", "");
      out_ << std::endl;
    }

    if (_mode_ == MODE_HEADER_FOOTER) {
      out_ << "# End of list of configuration properties (datatools::properties)"
           << std::endl;
    }
    return;
  }

  const file_include & properties_config::get_fi() const
  {
    return _fi_;
  }

  file_include & properties_config::grab_fi()
  {
    return _fi_;
  }

  void properties_config::set_fi(const file_include & fi_)
  {
    _fi_ = fi_;
    if (!_fi_.is_initialized()) {
      _fi_.initialize();
    }
    return;
  }

  void properties_config::read(const std::string & filename_, properties & props_)
  {
    std::string filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::logic_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ifstream fin(filename.c_str());
    DT_THROW_IF(!fin,
                std::logic_error,
                "Cannot open filename '" + filename_ + "' (resolved as '" + filename + "'!");
    set_reader_input(filename, -1);
    _read_(fin, props_);
    fin.close();
    _current_filename_.clear();
    _current_line_number_ = 0;
    return;
  }

  void properties_config::reset()
  {
    _current_filename_.clear();
    _topic_.clear();
    _section_name_.clear();
    _init_defaults_();
    return;
  }

  void properties_config::set_reader_input(const std::string & filename_, int line_count_)
  {
    _current_filename_ = filename_;
    if (line_count_ >= 0) {
      _current_line_number_ = line_count_;
    }
    return;
  }

  int properties_config::get_current_line_number() const
  {
    return _current_line_number_;
  }

  void properties_config::read(std::istream & in_, properties & props_)
  {
    this->_read_(in_, props_);
    return;
  }

  void properties_config::_read_(std::istream & in_, properties & props_)
  {
    if (!_dont_clear_) {
      props_.clear();
    }
    datatools::logger::priority logging = _logging_;
    DT_LOG_TRACE(logging, "Parsing a properties file...");
    std::string line_in;
    std::string prop_topic;
    std::string prop_config;
    std::string prop_description;
    bool property_parsing_started = false;
    bool allow_key_override = _allow_key_override_;
    // bool allow_array_override_index = true;
    bool line_goon = false;
    // 2013-04-05 FM : default is to allow unit directives for real numbers
    bool enable_real_with_unit = true;
    // File inclusion support:
    bool enable_file_inclusion = true;
    // By default, file inclusion rules are propagated to included files:
    bool include_file_propagate = true;
    bool include_file_allow_override = true;
    logger::priority fi_debug = logger::PRIO_FATAL;
    if (_forbid_include_) {
      enable_file_inclusion = false;
    }
    bool allow_fi_setup = true;
    if (_fi_.is_initialized()) {
      allow_fi_setup = false;
    }
    if (allow_fi_setup) {
      _fi_.set_include_path_env_strategy(file_include::EV_PREPEND);
    }
    bool enable_variants = true;
    bool variant_trace = false;
    if (_forbid_variants_) {
      enable_variants = false;
    } else {
      if (datatools::logger::is_trace(logging)) {
        variant_trace = true;
      }
    }

    /* The variant_if directive must be placed before the property line:
     *
     *  Examples:
     *
     *
     *  #@variant_if trigger:trigger_mode/coincidence
     *
     *  #@description The submodules to be triggered in coincidence
     *  trigger.coincidence.submodules    : string[2] = "calorimeter" "tracker"
     *
     *  #@description The coincidence time gate between submodules
     *  trigger.coincidence.time_gate     : real as time = 1260 us
     *
     *  #@description The number of neighbouring sectors to be considered
     *  trigger.coincidence.sector_spread : integer = 3
     *
     *  #@variant_endif
     *
     */
    std::list<std::string> variant_if_blocks;

    /* The variant_only directive must be placed before the property line:
     *
     *  Examples:
     *
     *  #@variant_only trigger:trigger_mode/if_multiplicity
     *  #@description The multiplicity threshold of the trigger system (when the trigger runs multiplicity mode)
     *  trigger.multiplicity.threshold : integer = 3
     *
     *  #@variant_only !trigger:trigger_mode/if_multiplicity
     *  #@description The ADC threshold of the trigger system (when the trigger runs NO multiplicity mode)
     *  trigger.adc.threshold : real = 15 mV
     *
     */
    std::string variant_only;
    // The variant preprocessor:
    // for now this preprocessor uses the datatools' kernel variant repository,
    // if it has been instanciated and properly feed by user (at application startup for example):

    unsigned int vpp_flags = 0;
    if (variant_trace) {
      // Special trace print:
      vpp_flags |= configuration::variant_preprocessor::FLAG_TRACE;
    }
    configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> paths_to_be_included;
    bool do_break = false;
    while (in_) {
      DT_LOG_TRACE(logging, "Loop on input stream...");
      std::string line_get;
      std::getline(in_, line_get);
      _current_line_number_++;
      DT_LOG_TRACE(logging, "Raw line = '" << line_get << "'");
      // Check if line has a continuation mark at the end:
      bool line_continue = false;
      int sz = line_get.size();
      if (sz > 0 && line_get[sz-1] == _format::CONTINUATION_CHAR) {
        line_continue = true;
        line_get = line_get.substr(0, sz - 1);
        DT_LOG_TRACE(logging, "line_get='" << line_get << "'");
      }
      if (line_goon) {
        // Append to previous line:
        line_in += line_get;
        DT_LOG_TRACE(logging, "Append to line='"<< line_in<< "'" );
      } else {
        // A new line
        line_in = line_get;
        DT_LOG_TRACE(logging, "New full line = '" << line_in << "'");
      }
      line_goon = false;
      // Check if line should go on:
      if (line_continue) line_goon = true;
      // bool allow_include = ! _forbid_include_;
      if (!line_goon) {
        bool skip_line = false;
        std::string & line = line_in;
        DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " size is " << line.size());
        // Check if line is blank:
        std::istringstream check_iss(line_in);
        std::string check_word;
        check_iss >> std::ws >> check_word;
        if (check_word.empty()) {
          DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is blank");
          skip_line = true;
        }
        DT_LOG_TRACE(logging, "skip_line #" << _current_line_number_ << " = " << std::boolalpha << skip_line);
        // Check if line is a comment:
        if (!skip_line) {
          DT_LOG_TRACE(logging, "Processing line #" << _current_line_number_ << " ...");
          std::istringstream iss(line);
          char c = 0;
          iss >> c;
          if (c == _format::COMMENT_CHAR) {
            // Handle comments:
            DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is a comment.");
            iss >> std::ws;
            std::string token;
            iss >> token;
            if (token.size() > 0 && token[0] == _format::METACOMMENT_CHAR) {
              DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is a meta-comment.");
              // Parse meta-comments:
              if (token == "@end") {
                DT_LOG_TRACE(logging, "Break the parsing loop");
                do_break = true;
                break;
              } else if (token == "@verbose_parsing") {
                DT_LOG_TRACE(logging, "Activate parsing verbosity");
                logging = datatools::logger::PRIO_TRACE;
              } else if (token == "@mute_parsing") {
                DT_LOG_TRACE(logging, "Mute parsing verbosity");
                logging = datatools::logger::PRIO_FATAL;
              } else if (token == "@enable_variants") {
                DT_LOG_TRACE(logging, "Enabling variants");
                DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Variant directives are forbidden!");
                enable_variants = true;
              } else if (token == "@disable_variants") {
                DT_LOG_TRACE(logging, "Disabling variants");
                enable_variants = false;
              } else if (token.substr(0, 9) == "@variant_") {
                DT_LOG_TRACE(logging, "Entering variant support");
                // Variant support :
                DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Variant directives are forbidden!");
                if (token == "@variant_devel") {
                  DT_LOG_TRACE(logging, "Activate variant support verbosity");
                  vpp.set_logging(logger::PRIO_TRACE);
                } else if (token == "@variant_no_devel") {
                  DT_LOG_TRACE(logging, "Mute variant support verbosity");
                  vpp.set_logging(logger::PRIO_FATAL);
                } else if (token == "@variant_remove_quotes") {
                  DT_LOG_TRACE(logging, "Set variant 'remove quotes' mode");
                  vpp.set_remove_quotes(true);
                } else if (token == "@variant_preserve_quotes") {
                  DT_LOG_TRACE(logging, "Set variant 'preserve quotes' mode");
                  vpp.set_remove_quotes(false);
                } else if (token == "@variant_only") {
                  DT_LOG_TRACE(logging, "Set a 'variant-only' conditional directive");
                  // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                  std::string variant_path_rule;
                  iss >> std::ws >> variant_path_rule;
                  variant_only = variant_path_rule;
                  property_parsing_started = true;
                  DT_LOG_TRACE(logging, "Next parameter is active only with variant '" << variant_only << "'.");
                } else if (token == "@variant_if") {
                  DT_LOG_TRACE(logging, "Set a 'variant-if' conditional block");
                  // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                  std::string variant_path_rule;
                  iss >> std::ws >> variant_path_rule;
                  variant_if_blocks.push_back(variant_path_rule);
                  property_parsing_started = true;
                  DT_LOG_TRACE(logging, "Open a variant if block with variant '" << variant_if_blocks.back() << "'.");
                } else if (token == "@variant_endif") {
                  DT_PROP_CFG_READ_THROW_IF(!enable_variants,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Variants are not supported!");
                  DT_PROP_CFG_READ_THROW_IF(variant_if_blocks.size() == 0,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "No variant conditional block is currently set!");
                  std::string variant_path;
                  iss >> std::ws >> variant_path;
                  if (!variant_path.empty()) {
                    std::string vibr_current = variant_if_blocks.back();
                    const size_t npipe = vibr_current.find('|');
                    std::string vib_path = vibr_current;
                    if (npipe != vibr_current.npos) {
                      vib_path = vibr_current.substr(0, npipe);
                    }
                    DT_PROP_CFG_READ_THROW_IF(variant_path != vib_path,
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Unmatching closing variant conditional block '"
                                              << variant_path << "'! "
                                              << "Expected variant path is '" << vib_path << "' !");
                  }
                  DT_LOG_TRACE(logging, "Close a 'variant if block' with variant '" << variant_if_blocks.back() << "'.");
                  variant_if_blocks.pop_back();
                  if (variant_if_blocks.size()) {
                    DT_LOG_TRACE(logging, "Current variant conditional block is '"
                                 << variant_if_blocks.back() << "' (was '" << variant_path << "')");;
                  }
                }
                DT_LOG_TRACE(logging, "End of a variant support");
              } else if (token == "@topic") {
                DT_LOG_TRACE(logging, "Topic directive");
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '@topic' is not allowed after first property record!");
                iss >> std::ws;
                std::string topic_desc;
                if (!prop_topic.empty()) {
                  // Warn if more than one '@topic' directive is set...
                  DT_LOG_WARNING(logging, "Duplicated '@topic' directive; "
                                 << "Configuration topic '" << get_topic() << "' already loaded!");
                }
                std::getline(iss, topic_desc);
                boost::trim(topic_desc);
                remove_quotes(topic_desc);
                if (topic_desc.empty()) {
                  DT_LOG_WARNING(_logging_, "Found an empty '@topic' directive! Ignore!");
                } else {
                  if (has_topic()) {
                    DT_PROP_CFG_READ_THROW_IF(topic_desc != get_topic(),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Parsed topic '" << topic_desc << "' does not match expected topic '" << get_topic() << "'!");
                  }
                }
              } else if (token == "@allow_key_override") {
                bool flag_value = true;
                iss >> std::ws;
                std::string flag_repr;
                std::getline(iss, flag_repr);
                if (flag_repr == "0"
                    or flag_repr == "false"
                    or flag_repr == "no") {
                  flag_value = false;
                }
                DT_LOG_TRACE(logging, "Force allow key override to : " << std::boolalpha << flag_value);
                allow_key_override = flag_value;
              } else if (token == "@forbid_key_override") {
                bool flag_value = true;
                iss >> std::ws;
                std::string flag_repr;
                std::getline(iss, flag_repr);
                if (flag_repr == "0"
                    or flag_repr == "false"
                    or flag_repr == "no") {
                  flag_value = false;
                }
                DT_LOG_TRACE(logging, "Force forbid key override to : " << std::boolalpha << flag_value);
                allow_key_override = !flag_value;
                if (! allow_key_override) {
                  // Specific case when 'key overriding' is explicitely forbiden,
                  // it is also for file inclusion mechanism:
                  include_file_allow_override = false;
                }
              } else if (token == "@config" or token == "@configuration") {
                // Warn if more than one '@config' directive is set...
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '@config[uration]' is not allowed after first property record!");
                iss >> std::ws;
                std::string config_desc;
                if (!prop_config.empty()) {
                  DT_LOG_WARNING(logging, "Duplicated '@config[uration]' directive; "
                                 << "Configuration description '" << props_.get_description() << "' already loaded!");
                }
                std::getline(iss, config_desc);
                boost::trim(config_desc);
                if (!config_desc.empty()) {
                  prop_config = config_desc;
                  remove_quotes(config_desc);
                  props_.set_description(config_desc);
                }
              } else if (token == "@forbid_include") {
                DT_LOG_TRACE(logging, "Inhibit file inclusion");
                enable_file_inclusion = false;
              } else if (token == "@include_debug") {
                DT_LOG_TRACE(logging, "Activate file inclusion verbosity");
                fi_debug = datatools::logger::PRIO_DEBUG;
                _fi_.set_logging(fi_debug);
              } else if (token == "@include_no_propagate") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                include_file_propagate = false;
              } else if (token == "@include_dir") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                if (!allow_fi_setup) {
                  DT_LOG_TRACE(logging, "Ignoring directive '" << token << "'");
                } else {
                  DT_LOG_TRACE(logging, "Specify a file inclusion search directory");
                  DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_dir_path;
                  uint32_t reader_flags = 0;
                  DT_THROW_IF(!io::read_quoted_string(iss, fi_dir_path, reader_flags),
                              std::logic_error,
                              (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                              "Line " << _current_line_number_ << ": "
                              << "Unquoted value for '@include_dir'");
                  DT_LOG_DEBUG(fi_debug, "Append include path '" << fi_dir_path << "'");
                  _fi_.append_explicit_include_path(fi_dir_path);
                }
              } else if (token == "@include_path_env") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                if (!allow_fi_setup) {
                  DT_LOG_TRACE(logging, "Ignoring directive '" << token << "'");
                } else {
                  DT_LOG_TRACE(logging, "Specify a file inclusion search directory environment variable");
                  DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_env_name;
                  uint32_t reader_flags = 0;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, fi_env_name, reader_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Unquoted value for '@include_path_env'");
                  DT_LOG_DEBUG(fi_debug, "Set include path env '" << fi_env_name << "'");
                  _fi_.set_include_path_env_name(fi_env_name);
                }
              } else if (token == "@include") {
                DT_LOG_TRACE(logging, "Specify a file to be included");
                DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                DT_LOG_DEBUG(logging, "Using '@include' directive from line <<<" << line << ">>>");
                iss >> std::ws;
                std::string include_desc;
                std::getline(iss, include_desc);
                boost::trim(include_desc);
                if (!include_desc.empty()) {
                  std::string include_config_file;
                  if (datatools::is_quoted(include_desc, '\'')) {
                    datatools::remove_quotes(include_desc, include_config_file, '\'');
                  } else if (datatools::is_quoted(include_desc, '"')) {
                    datatools::remove_quotes(include_desc, include_config_file, '"');
                  } else {
                    include_config_file = include_desc;
                  }
                  std::tuple<bool, std::string> fi_result = _fi_.resolve_err(include_config_file);
                  DT_PROP_CFG_READ_THROW_IF(! std::get<0>(fi_result),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot resolve include file path for '" << include_config_file << "'!");
                  std::string resolved_path = std::get<1>(fi_result);
                  DT_LOG_DEBUG(logging, "Set resolved include path '" << resolved_path << "'");
                  paths_to_be_included.push_back(resolved_path);
                  // Any include directive forbid to change the settings of the file inclusion mechanism:
                  property_parsing_started = true;
                }
              } else if (token == "@include_try") {
                DT_LOG_TRACE(logging, "Specify a file to be included if it exists");
                DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                DT_LOG_DEBUG(logging, "Using '@include' directive from line <<<" << line << ">>>");
                iss >> std::ws;
                std::string include_desc;
                std::getline(iss, include_desc);
                boost::trim(include_desc);
                if (!include_desc.empty()) {
                  std::string include_config_file;
                  if (datatools::is_quoted(include_desc, '\'')) {
                    datatools::remove_quotes(include_desc, include_config_file, '\'');
                  } else if (datatools::is_quoted(include_desc, '"')) {
                    datatools::remove_quotes(include_desc, include_config_file, '"');
                  } else {
                    include_config_file = include_desc;
                  }
                  std::tuple<bool, std::string> fi_result = _fi_.resolve_err(include_config_file);
                  if(std::get<0>(fi_result)) {
                    std::string resolved_path = std::get<1>(fi_result);
                    DT_LOG_DEBUG(logging, "Set resolved include path '" << resolved_path << "'");
                    paths_to_be_included.push_back(resolved_path);
                  } else {
                    DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                                   "Could not resolve file inclusion '" << include_config_file << "'!");
                  }
                  // Any include directive forbid to change the settings of the file inclusion mechanism:
                  property_parsing_started = true;
                }
              } else if (token == "@enable_real_with_unit") {
                DT_LOG_TRACE(logging, "Enable unit support for real properties");
                enable_real_with_unit = true;
              } else if (token == "@disable_real_with_unit") {
                DT_LOG_TRACE(logging, "Disable unit support for real properties");
                enable_real_with_unit = false;
              } else if (token == "@description" or token == "@parameter") {
                DT_LOG_TRACE(logging, "Set the description of the next property to be parsed");
                iss >> std::ws;
                std::string desc;
                std::getline(iss, desc);
                if (!desc.empty()) {
                  prop_description = desc;
                } else {
                  DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Empty property description !");
                }
                property_parsing_started = true;
              } else {
                DT_PROP_CFG_READ_THROW_IF(true,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Unsupported meta-comment token '" << token << "'!");
              }
            } // end of metacomments '@'
            skip_line = true;
          } // end of comments
        } // if (! skip_line)

        DT_LOG_TRACE(logging, "skip_line = " << std::boolalpha << skip_line);

        // Manage breaks:
        if (do_break) {
          DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                    std::logic_error,
                                    _current_filename_,
                                    _section_name_,
                                    _section_start_line_number_,
                                    _current_line_number_,
                                    "Enforced break while parsing of a property has started!");
        }

        // Manage included files:
        if (paths_to_be_included.size()) {
          DT_LOG_DEBUG(_logging_, "Before inclusion : ");
          props_.print_tree(std::cerr);
          // Trigger the inclusion of external file:
          for (const std::string & inc_path : paths_to_be_included) {
            DT_LOG_DEBUG(_logging_, "About to include file with path '" << inc_path << "'");
            uint32_t reader_options = properties_config::RESOLVE_PATH;
            if (datatools::logger::is_debug(_logging_)) {
              reader_options |= properties_config::LOG_DEBUG;
            }
            properties_config reader(reader_options);
            if (include_file_propagate) {
              // Propagate the resolving rules of the local file inclusion mechanism:
              reader.set_fi(_fi_);
            }
            properties inc_conf;
            reader.read(inc_path, inc_conf);
            DT_LOG_DEBUG(_logging_, " -> Included config : ");
            if (datatools::logger::is_debug(_logging_)) {
              inc_conf.print_tree(std::cerr);
            }
            props_.merge_with(inc_conf, include_file_allow_override);
          }
          paths_to_be_included.clear();
        }

        // Parse line:
        // if (!skip_line && parsing ) {
        if (!skip_line) {
          DT_LOG_TRACE(logging, "Parsing payload line #" << _current_line_number_ << " : '" << line << "'");
          std::string line_parsing = line;
          std::size_t flag_pos = line_parsing.find_first_of(_format::ASSIGN_CHAR);
          DT_PROP_CFG_READ_THROW_IF(flag_pos == line_parsing.npos,
                                    std::logic_error,
                                    _current_filename_,
                                    _section_name_,
                                    _section_start_line_number_,
                                    _current_line_number_,
                                    "Cannot find assign symbol '" << _format::ASSIGN_CHAR << "'!");
          property_parsing_started = true;
          bool array_append = false;
          bool array_override_item = false;
          int array_override_index = -1;
          if (flag_pos > 0) {
            if (line_parsing.substr(flag_pos - 1, 2) == "+=") {
              array_append = true;
            }
          }
          // Parse property desc:
          std::string property_desc_str = line_parsing.substr(0, flag_pos - (array_append ? 1 : 0));
          DT_LOG_TRACE(logging, "property_desc_str='" << property_desc_str << "'");
          // Parse property value:
          std::string property_value_str = line_parsing.substr(flag_pos+1);
          DT_LOG_TRACE(logging, "property_value_str='" << property_value_str << "'");
          bool scalar = true;
          bool locked = false;
          int  vsize  = -1;
          char type   =  0;
          std::string prop_key;
          std::size_t desc_pos = property_desc_str.find_first_of(_format::DESC_CHAR);
          if (desc_pos == property_desc_str.npos) {
            std::istringstream key_ss(property_desc_str);
            key_ss >> std::ws >> prop_key;
            type = properties::data::TYPE_STRING_SYMBOL;
          } else {
            std::string key_str = property_desc_str.substr(0, desc_pos);
            std::istringstream key_ss(key_str);
            key_ss >> std::ws >> prop_key;
            std::string type_str = property_desc_str.substr(desc_pos + 1);
            std::string type_str2;
            std::string type_str3;
            DT_LOG_TRACE(logging, "type_str='" << type_str << "'");
            std::size_t vec_pos = type_str.find_first_of(_format::OPEN_VECTOR);
            if (vec_pos != type_str.npos) {
              scalar = false;
              type_str2 = type_str.substr(0, vec_pos);
              DT_LOG_TRACE(logging, "type_str2='" << type_str2 << "'");
              std::string vec_str = type_str.substr(vec_pos + 1);
              std::istringstream vec_ss(vec_str);
              vec_ss >> vsize;
              DT_PROP_CFG_READ_THROW_IF(!vec_ss,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Cannot find vector size for key '" << prop_key << "' at line '" << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(vsize < 0,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid vector size !");
              char c = 0;
              // DT_PROP_CFG_READ_THROW_IF(!vec_ss,
              //                                std::logic_error,
              //                                _current_filename_,
              //                                _section_name_,
              //                                _section_start_line_number_,
              //                                _current_line_number_,
              //                                "Cannot find expected vector size closing symbol for key '"
              //                                << prop_key
              //                                << "' at line '"
              //                                << line << "'  !");
              vec_ss >> c;
              DT_PROP_CFG_READ_THROW_IF(c !=_format:: CLOSE_VECTOR,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid vector size closing symbol for key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              std::getline(vec_ss, type_str3);
            } else {
              scalar = true;
              type_str2 = type_str;
            }

            {
              // Search for syntax like : 'array_foo[42]'
              int open_index_pos = -1;
              //std::size_t close_index_pos = -1;
              open_index_pos = prop_key.find('[');
              if (open_index_pos >= 0) {
                DT_PROP_CFG_READ_THROW_IF(prop_key.back() != ']',
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Invalid array item format for key '"
                                          << prop_key
                                          << "' at line '"
                                          << line << "' !");
                std::string array_prop_key = prop_key.substr(0, open_index_pos);
                std::string array_index_token = prop_key.substr(open_index_pos + 1,
                                                                prop_key.length() - array_prop_key.length() - 2);
                // std::cerr << "******** DEVEL ***" << "array_prop_key = '" << array_prop_key << "'" << std::endl;
                // std::cerr << "******** DEVEL ***" << "array_index_token = '" << array_index_token << "'" << std::endl;
                std::istringstream array_index_ss(array_index_token);
                int index = -1;
                array_index_ss >> index;
                DT_PROP_CFG_READ_THROW_IF(!array_index_ss or index < 0,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Invalid array item format from '" << array_index_token << "' for key '"
                                          << prop_key
                                          << "' at line '"
                                          << line << "' !");
                // std::cerr << "******** DEVEL ***" << "index = [" << index<< "]" << std::endl;
                array_override_item = true;
                array_override_index = index;
                prop_key = array_prop_key;
                // std::cerr << "******** DEVEL ***" << "array_override_item  = " << std::boolalpha << array_override_item << std::endl;
                // std::cerr << "******** DEVEL ***" << "array_override_index = [" << array_override_index << "]" << std::endl;
                // std::cerr << "******** DEVEL ***" << "prop_key_token       = '" << prop_key << "'" << std::endl;
              }
            }
            if (array_override_item) {
              DT_PROP_CFG_READ_THROW_IF(! allow_key_override,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Key override mode is not allowed for property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(! props_.is_vector(prop_key),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Key override mode is only supported for array property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(array_override_index < 0 or static_cast<size_t>(array_override_index) >= props_.key_size(prop_key),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid array index [" << std::to_string(array_override_index) << "] for key override mode for array property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
            }
            DT_PROP_CFG_READ_THROW_IF(array_append and scalar,
                                      std::logic_error,
                                      _current_filename_,
                                      _section_name_,
                                      _section_start_line_number_,
                                      _current_line_number_,
                                      "Append mode is only supported for array property with key '"
                                      << prop_key
                                      << "' at line '"
                                      << line << "' !");
            std::istringstream type_ss(type_str2);
            std::string type_token;
            type_ss >> std::ws >> type_token >> std::ws;
            if (type_token == lock_decorator()) {
              locked = true;
              type_token.clear();
              type_ss >> std::ws >> type_token >> std::ws;
            }
            if (type_token == properties::data::format::boolean_label()) {
              type = properties::data::TYPE_BOOLEAN_SYMBOL;
            } else if (type_token == properties::data::format::integer_label()) {
              type = properties::data::TYPE_INTEGER_SYMBOL;
            } else if (type_token == properties::data::format::real_label()) {
              type = properties::data::TYPE_REAL_SYMBOL;
            } else if (type_token == properties::data::format::string_label()) {
              type = properties::data::TYPE_STRING_SYMBOL;
            } else {
              DT_PROP_CFG_READ_THROW_IF(true,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid type specifier '"
                                        << type_token << "' "
                                        << "at key '"
                                        << prop_key << "' !");
            }
            DT_PROP_CFG_READ_THROW_IF(array_override_item and props_.get_type_label(prop_key) != type_token,
                                      std::logic_error,
                                      _current_filename_,
                                      _section_name_,
                                      _section_start_line_number_,
                                      _current_line_number_,
                                      "Type mismatch '" << type_token << "' vs '" << props_.get_type_label(prop_key) << "' for array item override mode for key '"
                                      << prop_key
                                      << "' at line '"
                                      << line << "' !");
            if (! scalar && ! type_str3.empty()) {
              std::istringstream iss3(type_str3);
              std::string dummy;
              iss3 >> std::ws >> dummy;
              if (! dummy.empty()) {
                type_ss.clear();
                type_ss.str(type_str3);
              }
            }

            bool with_explicit_path = false;
            if (type == properties::data::TYPE_STRING_SYMBOL) {
              std::string token;
              type_ss >> std::ws >> token >> std::ws;
              if (! token.empty()) {
                if (token == as_directive()) {
                  std::string token2;
                  type_ss >> token2;
                  DT_PROP_CFG_READ_THROW_IF(token2.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing directive (as) for string value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_PROP_CFG_READ_THROW_IF(token2 != "path",
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Invalid token '" << token2 << "' for string value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  with_explicit_path = true;
                }
              }
            }

            /// 2013-03-07 FM : add support for embedded unit directives in real property :
            std::string requested_unit_label;
            std::string requested_unit_symbol;
            double      requested_unit_value;
            datatools::invalidate(requested_unit_value);
            if (type == properties::data::TYPE_REAL_SYMBOL) {
              std::string token;
              type_ss >> std::ws >> token >> std::ws;
              if (! token.empty()) {
                if (token == as_directive()) {
                  type_ss >> std::ws >> requested_unit_label;
                  DT_PROP_CFG_READ_THROW_IF(requested_unit_label.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing unit label (ex: 'as length') for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_PROP_CFG_READ_THROW_IF(! units::is_unit_label_valid(requested_unit_label),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Invalid unit label '" << requested_unit_label
                                            << "' for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_LOG_TRACE(logging, "Unit label '"<< requested_unit_label << "' is valid !");
                } else if (token == in_directive()) {
                  type_ss >> std::ws >> requested_unit_symbol;
                  DT_PROP_CFG_READ_THROW_IF(requested_unit_symbol.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing unit symbol (ex: 'in millimeter') for real value with key '"
                                            << prop_key << "' at line '" << line << "' !");
                  if (! scalar) {
                    // For vectors of real:
                    DT_PROP_CFG_READ_THROW_IF(!units::find_unit(requested_unit_symbol,
                                                                requested_unit_value,
                                                                requested_unit_label),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Invalid unit symbol '" << requested_unit_symbol << "' for real value with key '"
                                              << prop_key << "' at line '" << line << "' !");
                    DT_LOG_TRACE(logging, "Unit symbol '" << requested_unit_symbol << "' is valid !");
                  } else {
                    DT_PROP_CFG_READ_THROW_IF(true,
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Directive '" << in_directive() << "' " << requested_unit_symbol
                                              << "' is not supported for scalar real value with key '"
                                              << prop_key
                                              << "' at line '"
                                              << line << "' !");
                  }
                } else {
                  DT_PROP_CFG_READ_THROW_IF(true,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Unknown directive '" << token
                                            << "' for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                }
              } // Token not emtpy

              if (! enable_real_with_unit) {
                DT_PROP_CFG_READ_THROW_IF(! requested_unit_label.empty()
                                          || ! requested_unit_symbol.empty(),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "The use of unit directives '" << token
                                          << "' is not allowed for real value with key '"
                                          << prop_key << "' at line '" << line << "' !");
              }
            } // End of REAL

            {
              std::string trailing_token;
              type_ss >> std::ws >> trailing_token >> std::ws;
              DT_PROP_CFG_READ_THROW_IF(! trailing_token.empty(),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Unsupported trailing token '" << trailing_token
                                        << "' for key '"
                                        << prop_key << "' at line '" << line << "' !");
            }

            DT_LOG_TRACE(logging, "type='"<< type << "'");
            DT_LOG_TRACE(logging, "locked='" << locked << "'");
            DT_LOG_TRACE(logging, "vsize='" << vsize << "'");
            DT_LOG_TRACE(logging, "prop_description='" << prop_description << "'");
            bool        a_boolean = false;
            int         a_integer = 0;
            double      a_real    = 0.0;
            std::string a_string  = "";
            std::vector<bool>        v_booleans;
            std::vector<int>         v_integers;
            std::vector<double>      v_reals;
            std::vector<std::string> v_strings;
            if (!scalar) {
              if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                if (vsize > 0) {
                  v_booleans.assign(vsize,
                                    properties::data::defaults::boolean_value());
                }
              }
              if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                if (vsize > 0) {
                  v_integers.assign(vsize,
                                    properties::data::defaults::integer_value());
                }
              }
              if (type == properties::data::TYPE_REAL_SYMBOL) {
                if (vsize > 0) {
                  v_reals.assign(vsize,
                                 properties::data::defaults::real_value());
                }
              }
              if (type == properties::data::TYPE_STRING_SYMBOL) {
                if (vsize > 0) {
                  v_strings.assign(vsize,
                                   properties::data::defaults::string_value());
                }
              }
            }
            // Property is enabled by default:
            bool exhibit_property = true;
            if (variant_if_blocks.size()) {
              for (std::list<std::string>::const_iterator ivib = variant_if_blocks.begin();
                   ivib != variant_if_blocks.end();
                   ivib++) {
                bool variant_if_active = false;
                bool variant_if_reverse = false;
                const std::string & variant_if_rule = *ivib;
                command::returned_info cri = vpp.resolve_variant(variant_if_rule,
                                                                 variant_if_active,
                                                                 variant_if_reverse);
                DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot preprocess variant if block directive from '"
                                          << variant_if_rule << "' : "
                                          << cri.get_error_message());
                exhibit_property = variant_if_active;
                if (variant_if_reverse) {
                  exhibit_property = !variant_if_active;
                }
                if (!exhibit_property) {
                  // We break at first inhibited conditional block:
                  break;
                }
              }
            }
            bool variant_only_active = false;
            /* The variant_only_reverse flag reverses the condition:
             *
             *  Example:
             *
             *  #@variant_only !trigger:trigger_mode/if_multiplicity_mode
             *  #@description The ADC threshold of the trigger system (when the trigger runs NO multiplicity mode)
             *  trigger.adc.threshold : real = 15 mV
             *
             */
            bool variant_only_reverse = false;
            // Process the variant_only directive if it exists:
            if (exhibit_property && !variant_only.empty()) {
              command::returned_info cri = vpp.resolve_variant(variant_only,
                                                               variant_only_active,
                                                               variant_only_reverse);
              DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Cannot preprocess variant only directive from '"
                                        << variant_only << "' : "
                                        << cri.get_error_message());
              DT_LOG_TRACE(logging, "VPP ==> variant_only_active='" << variant_only_active << "'");
              DT_LOG_TRACE(logging, "VPP ==> variant_only_reverse='" << variant_only_reverse << "'");
            }
            // Process the line or not:
            bool process_line = exhibit_property;
            if (process_line) {
              // variant_only may inhibit the property:
              if (! variant_only.empty()) {
                process_line = variant_only_active;
                if (variant_only_reverse) {
                  process_line = !variant_only_active;
                }
              }
            }
            // Process the current line:
            if (!process_line) {
              DT_PROP_CFG_READ_LOG_DEBUG(logging,
                                         _current_filename_,
                                         _section_name_,
                                         _section_start_line_number_,
                                         _current_line_number_,
                                         "Do not process property value repr '"
                                         << property_value_str << "' for property '"
                                         << prop_key << "'...");
            } else {
              std::string pv_str = property_value_str;
              boost::trim(pv_str);
              DT_LOG_TRACE(logging, "pv_str='" << pv_str << "'");
              std::string effective_property_values_line = property_value_str;
              {
                std::string effective_line;
                command::returned_info cri = vpp.preprocess(pv_str, effective_line);
                DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot preprocess variant in '" << pv_str << "' : "
                                          << cri.get_error_message());
                DT_LOG_TRACE(logging, "VPP ==> effective_line='" << effective_line << "'");
                effective_property_values_line = effective_line;
              }

              DT_LOG_TRACE(logging, "--> effective_property_values_line='" << effective_property_values_line << "'");
              std::istringstream iss(effective_property_values_line);
              /***************
               *   BOOLEAN   *
               ***************/
              if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                DT_LOG_TRACE(logging, "boolean: effective_property_values_line='" << effective_property_values_line << "'");
                if (scalar) {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, a_boolean, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read boolean value from '"
                                            << effective_property_values_line << "' for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  for (int i = 0; i < vsize; ++i) {
                    bool boolean_value;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, boolean_value, read_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector boolean value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    v_booleans[i] = boolean_value;
                  }
                }
              }
              /***************
               *   INTEGER   *
               ***************/
              if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                if (scalar) {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_integer(iss, a_integer, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read integer value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  for (int i = 0; i < vsize; ++i) {
                    int int_value = 0;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_integer(iss, int_value, read_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector integer value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    v_integers[i] = int_value;
                  }
                }
              }
              /**************
               *    REAL    *
               **************/
              bool with_explicit_unit = false;
              std::string unit_symbol;
              double      unit_value = 1.0;
              std::string unit_label;
              if (type == properties::data::TYPE_REAL_SYMBOL) {
                if (scalar) {
                  // Scalar real :
                  // Special mode to accept unit symbol after the real value :
                  bool normal_value = false;
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, a_real, normal_value, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read vector real value for key '"
                                            << prop_key << "' at line '" << line << "' (" << props_.get_description() << ") !");
                  iss >> std::ws;
                  if (! iss.eof())  {
                    std::string unit_word;
                    iss >> unit_word >> std::ws;
                    if (! unit_word.empty()) {
                      if (unit_word[0] == _format::COMMENT_CHAR) {
                        // Eat the end of the line:
                        std::string trailing_bits;
                        std::getline(iss, trailing_bits);
                      } else {
                        // Try to parse a unit symbol:
                        DT_PROP_CFG_READ_THROW_IF(! enable_real_with_unit,
                                                  std::logic_error,
                                                  _current_filename_,
                                                  _section_name_,
                                                  _section_start_line_number_,
                                                  _current_line_number_,
                                                  "Trailing token '" << unit_word << "' is not allowed for real value with key '"
                                                  << prop_key << "' at line '" << line
                                                  << "'; you should activate the 'real with unit' feature to support this syntax !");
                        bool found_unit = datatools::units::find_unit(unit_word,
                                                                      unit_value,
                                                                      unit_label);
                        DT_PROP_CFG_READ_THROW_IF(! found_unit,
                                                  std::logic_error,
                                                  _current_filename_,
                                                  _section_name_,
                                                  _section_start_line_number_,
                                                  _current_line_number_,
                                                  "Invalid unit symbol for key '"
                                                  << prop_key
                                                  << "' in line '"
                                                  << line
                                                  << "' !");
                        DT_PROP_CFG_READ_THROW_IF(! requested_unit_label.empty()
                                                  && unit_label != requested_unit_label,
                                                  std::logic_error,
                                                  _current_filename_,
                                                  _section_name_,
                                                  _section_start_line_number_,
                                                  _current_line_number_,
                                                  "Unit symbol '" << unit_word << "' in not compatible with requested unit label '"
                                                  << requested_unit_label << "' for real scalar property with key '"
                                                  << prop_key << "' at line '" << line << "' !");
                        a_real *= unit_value;
                        with_explicit_unit = true;
                        unit_symbol = unit_word;
                      }
                    } // End of unit symbol parsing.
                  }
                } else {
                  // Vector real:
                  for (int i = 0; i < vsize; ++i) {
                    double real_value;
                    bool normal_value = false;
                    uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                    // No unit is allowed after the real value in vector mode:
                    DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, real_value, normal_value, reader_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector real value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    if (enable_real_with_unit && datatools::is_valid(requested_unit_value)) {
                      real_value *= requested_unit_value;
                      with_explicit_unit = true;
                      unit_symbol = requested_unit_symbol;
                    }
                    v_reals[i] = real_value;
                  }
                }
              } // Real
              /**************
               *   STRING   *
               **************/
              if (type == properties::data::TYPE_STRING_SYMBOL) {
                if (scalar) {
                  uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, a_string, reader_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read string value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  for (int i = 0; i < vsize; ++i) {
                    uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                    std::string str;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, str, reader_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read string value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    v_strings[i] = str;
                  }
                }
              }
              {
                // Analyse the end of the line and search for spurious trailing bits:
                std::string trailing_bits;
                std::getline(iss, trailing_bits);
                boost::trim(trailing_bits);
                if (trailing_bits.length() > 0) {
                  if (trailing_bits[0] != _format::COMMENT_CHAR) {
                    std::ostringstream message_oss;
                    if (!_current_filename_.empty()) {
                      message_oss << "File '" << _current_filename_ << "': ";
                    }
                    if (_section_name_.empty()) {
                      if (_current_line_number_ > 0) {
                        message_oss << "Line #" << _current_line_number_ << ": ";
                      }
                    } else {
                      message_oss << "Section '" << _section_name_ << "'";
                      if (_section_start_line_number_ > 0) {
                        message_oss << " starting at line #" << _section_start_line_number_;
                      }
                      message_oss << ": ";
                    }
                    message_oss << "There are unprocessed trailing characters '"
                                << trailing_bits << "' after value record of property '"
                                << prop_key << "'!";
                    DT_LOG_WARNING(_logging_,message_oss.str());
                  }
                }
              }
              bool store_it = true;
              // bool store_it = exhibit_property;
              // if (store_it) {
              //        if (! variant_only.empty()) {
              //          store_it = variant_only_active;
              //          if (variant_only_reverse) {
              //            store_it = !variant_only_active;
              //          }
              //        }
              // }
              if (not scalar and array_append) {
                DT_PROP_CFG_READ_THROW_IF(! allow_key_override,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Property override/append is not allowed for array with key '" << prop_key << "'!");
                DT_PROP_CFG_READ_THROW_IF(! props_.has_key(prop_key),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Key '" << prop_key << "' is not used and no append action can be done on it!");
                DT_PROP_CFG_READ_THROW_IF(! props_.is_vector(prop_key),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Key '" << prop_key << "' is not a vector property and no append action can be done on it!");
              } else {
                if (props_.has_key(prop_key)) {
                  if (!allow_key_override) {
                    DT_PROP_CFG_READ_THROW_IF(! array_override_item,
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Key '" << prop_key << "' is already used and property override is not allowed!");
                  }
                  DT_LOG_WARNING(_logging_, "Overriden property with key '" << prop_key << "' at line #" <<
                                 std::to_string(_current_line_number_) << " : '" << line << "'");
                  bool clean_key = true;
                  if (scalar and array_override_item) {
                    // std::cerr << "***** DEVEL ***" << "Clean existing array property '" << prop_key << "' ************" << std::endl;
                    clean_key = false;
                  }
                  if (clean_key) {
                    props_.clean(prop_key);
                  }
               }
              }
              // std::cerr << "***** DEVEL ***" << "About to store property '" << prop_key << "' ************" << std::endl;

              if (store_it) {
                // std::cerr << "***** DEVEL ***" << "Store property '" << prop_key << "' ************" << std::endl;
                if (type != properties::data::TYPE_REAL_SYMBOL) {
                  DT_PROP_CFG_READ_THROW_IF(with_explicit_unit or !unit_symbol.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Non-real property with key '" << prop_key << "' does not support explicit unit!");
                }
                if (type != properties::data::TYPE_STRING_SYMBOL) {
                  DT_PROP_CFG_READ_THROW_IF(with_explicit_path,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Non-string property with key '" << prop_key << "' does not support explicit path!");
                }
                if (scalar) {
                  // std::cerr << "***** DEVEL ***" << "Scalar property '" << prop_key << "' ************" << std::endl;
                  if (! array_override_item) {
                    // std::cerr << "***** DEVEL ***" << "Store normal scalar property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.store(prop_key, a_boolean, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.store(prop_key, a_integer, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      props_.store(prop_key, a_real, prop_description, locked);
                      if (with_explicit_unit) {
                        props_.set_explicit_unit(prop_key, true);
                      }
                      if (!unit_symbol.empty()) {
                        props_.set_unit_symbol(prop_key, unit_symbol);
                      }
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      props_.store(prop_key, a_string, prop_description, locked);
                      if (with_explicit_path) {
                        props_.set_explicit_path(prop_key, true);
                      }
                    }
                  } else {
                    // Array override item:
                    // std::cerr << "***** DEVEL ***" << "Array override item for key '" << prop_key << "'************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.change_boolean_vector(prop_key, a_boolean, array_override_index);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.change_integer_vector(prop_key, a_integer, array_override_index);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(with_explicit_unit != props_.has_explicit_unit(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Overriden item for real vector key '" << prop_key << "' has unmatching explicit unit ith original vector!");
                      DT_PROP_CFG_READ_THROW_IF(props_.get_unit_symbol(prop_key) != unit_symbol,
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Overriden item for real vector key '" << prop_key
                                                << "' has unmatching explicit unit '" << props_.get_unit_symbol(prop_key)
                                                << "' with respect to original value with unit '" << unit_symbol << "'!");
                      props_.change_real_vector(prop_key, a_real, array_override_index);
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(with_explicit_path != props_.is_explicit_path(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original string vector key '" << prop_key << "' has unmatching explicit path with respect to overriden vector item!");
                      props_.change_string_vector(prop_key, a_string, array_override_index);
                    }
                  }

                } else {
                  // std::cerr << "***** DEVEL ***" << "Vector property '" << prop_key << "' ************" << std::endl;
                  if (not array_append) {
                    // std::cerr << "***** DEVEL ***" << "Normal store Vector property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.store(prop_key, v_booleans, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.store(prop_key, v_integers, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      props_.store(prop_key, v_reals, prop_description, locked);
                      if (with_explicit_unit) {
                        props_.set_explicit_unit(prop_key, true);
                      }
                      if (!unit_symbol.empty()) {
                        props_.set_unit_symbol(prop_key, unit_symbol);
                      }
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      props_.store(prop_key, v_strings, prop_description, locked);
                      if (with_explicit_path) {
                        props_.set_explicit_path(prop_key, true);
                      }
                    }
                  } else {
                    // std::cerr << "***** DEVEL ***" << "Append mode vector property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(not props_.is_boolean(prop_key) and not props_.is_vector(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original property with key '" << prop_key << "' is not a boolean vector property!");

                      properties::data::vbool v_booleans_appended;
                      props_.fetch(prop_key, v_booleans_appended);
                      for (auto val : v_booleans) {
                        v_booleans_appended.push_back(val);
                      }
                      props_.update(prop_key, v_booleans_appended);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(not props_.is_integer(prop_key) and not props_.is_vector(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original property with key '" << prop_key << "' is not an integer vector property!");

                      properties::data::vint v_integers_appended;
                      props_.fetch(prop_key, v_integers_appended);
                      for (auto val : v_integers) {
                        v_integers_appended.push_back(val);
                      }
                      props_.update(prop_key, v_integers_appended);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(not props_.is_real(prop_key) and not props_.is_vector(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original property with key '" << prop_key << "' is not a real vector property!");
                      properties::data::vdouble v_reals_appended;
                      props_.fetch(prop_key, v_reals_appended);
                      for (auto val : v_reals) {
                        v_reals_appended.push_back(val);
                      }
                      if (with_explicit_unit) {
                        DT_PROP_CFG_READ_THROW_IF(not props_.has_explicit_unit(prop_key),
                                                  std::logic_error,
                                                  _current_filename_,
                                                  _section_name_,
                                                  _section_start_line_number_,
                                                  _current_line_number_,
                                                  "Original real vector key '" << prop_key << "' has no explicit unit!");
                        DT_PROP_CFG_READ_THROW_IF(props_.has_unit_symbol(prop_key)
                                                  and props_.get_unit_symbol(prop_key) != unit_symbol,
                                                  std::logic_error,
                                                  _current_filename_,
                                                  _section_name_,
                                                  _section_start_line_number_,
                                                  _current_line_number_,
                                                  "Original real vector key '" << prop_key << "' has unmatching explicit unit '" << props_.get_unit_symbol(prop_key) << "' with respect to appended values with unit '" << unit_symbol << "'!");
                      }
                      props_.update(prop_key, v_reals_appended);
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(not props_.is_string(prop_key) and not props_.is_vector(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original property with key '" << prop_key << "' is not a string vector property!");
                      DT_PROP_CFG_READ_THROW_IF(with_explicit_path != props_.is_explicit_path(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original string vector key '" << prop_key << "' has unmatching explicit path with respect to appended vector!");
                      properties::data::vstring v_strings_appended;
                      props_.fetch(prop_key, v_strings_appended);
                      for (auto val : v_strings) {
                        v_strings_appended.push_back(val);
                      }
                      props_.update(prop_key, v_strings_appended);
                    }
                  }
                }
                prop_description.clear();
              } // if (store_it)
            } // if (process_line)
            // Clear current requested variant only :
            if (! variant_only.empty()) {
              variant_only.clear();
            }
            variant_only_reverse = false;
          }
        } // !skip_line
      } // if (! line_goon)
    } // while (*_in)
    DT_PROP_CFG_READ_THROW_IF(variant_if_blocks.size() > 0,
                              std::logic_error,
                              _current_filename_,
                              _section_name_,
                              _section_start_line_number_,
                              _current_line_number_,
                              "Unclosed variant conditional block '" << variant_if_blocks.back() << "'!");
    return;
  }

} // namespace datatools