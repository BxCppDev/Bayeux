// datatools/properties_config.cc

// Ourselves:
#include <datatools/multi_properties_config.h>

// Standard Library
#include <fstream>
#include <iomanip>

// Third Party:
// - Boost:
#include <boost/algorithm/string/trim.hpp>

// This Project
#include <datatools/configuration/io.h>
#include <datatools/ioutils.h>
#include <datatools/properties_config.h>
#include <datatools/units.h>

namespace {

  struct _format {
    static const char OPEN_CHAR    = '['; ///< Open section character
    static const char ASSIGN_CHAR  = '='; ///< Assignement key/meta label character
    static const char CLOSE_CHAR   = ']'; ///< Close section character
    static const char COMMENT_CHAR = '#'; ///< Comment character
    static const char QUOTES_CHAR  = '"'; ///< Quoting character
    static const char SPACE_CHAR   = ' '; ///< Space character
    static const char CONTINUATION_CHAR = '\\'; ///< Continuation character
  };

}

namespace datatools {
//----------------------------------------------------------------------
  // - Config inner class implementation
  multi_properties_config::multi_properties_config(uint32_t options_, const std::string & topic_)
  {
    _init_defaults();
    _skip_private_sections_ = (options_ & SKIP_PRIVATE_SECTIONS);
    _skip_private_properties_ = (options_ & SKIP_PRIVATE_PROPS);
    _forbid_variants_ = (options_ & FORBID_VARIANTS);
    _header_footer_ = (options_ & HEADER_FOOTER);

    if (options_ & LOG_MUTE) {
      _logging_ = datatools::logger::PRIO_FATAL;
    }
    if (options_ & LOG_DEBUG) {
      _logging_ = datatools::logger::PRIO_DEBUG;
    }
    if (options_ & LOG_TRACE) {
      _logging_ = datatools::logger::PRIO_TRACE;
    }
    if (options_ & RESOLVE_PATH) {
      _resolve_path_ = true;
    }
    if (options_ & START_WITHOUT_LABELS) {
      _start_without_labels_ = true;
    }
    if (options_ & WITHOUT_DECORATION) {
      _without_decoration_ = true;
    }
    if (options_ & FORBID_INCLUDE) {
      _forbid_include_ = true;
    }
    if (!topic_.empty()) {
      set_topic(topic_);
    }
    return;
  }

  void multi_properties_config::_init_defaults()
  {
    _logging_ = datatools::logger::PRIO_WARNING;
    _skip_private_sections_ = false;
    _skip_private_properties_ = false;
    _forbid_variants_ = false;
    _header_footer_ = false;
    _requested_topic_ = false;
    _resolve_path_ = false;
    _current_line_number_ = -1;
    _start_without_labels_ = false;
    _without_decoration_ = false;
    _forbid_include_ = false;
    return;
  }

  void multi_properties_config::reset()
  {
    _topic_.clear();
    _current_filename_.clear();
    _init_defaults();
    return;
  }

  void multi_properties_config::_set_current_filename(const std::string & filename_)
  {
    _current_filename_ = filename_;
    return;
  }

  void multi_properties_config::read(const std::string & filename_, multi_properties & target_)
  {
    std::string filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::runtime_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ifstream fin(filename.c_str());
    DT_THROW_IF(!fin,
                std::runtime_error,
                "Cannot open filename '" + filename_ + "' (resolved as '" + filename + "'!");
    _set_current_filename(filename);
    read(fin, target_);
    fin.close();
    _current_filename_.clear();
    _current_line_number_ = -1;
    return;
  }

  void multi_properties_config::read(std::istream & in_, multi_properties & target_)
  {
    _read(in_, target_);
    return;
  }

  void multi_properties_config::write(const std::string & filename_,
                                       const multi_properties & source_)
  {
    std::string filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::runtime_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ofstream fout(filename.c_str());
    DT_THROW_IF (!fout,
                 std::runtime_error,
                 "Cannot open filename '" + filename_ + "' (resolved as '" + filename + "'!");
    _set_current_filename(filename);
    _write(fout, source_);
    fout.close();
    _current_filename_.clear();
    _current_line_number_ = -1;
    return;
  }

  void multi_properties_config::write(std::ostream & out_,
                                       const multi_properties & source_)
  {
    _write(out_, source_);
    return;
  }

  bool multi_properties_config::has_topic() const
  {
    return !_topic_.empty();
  }

  void multi_properties_config::set_topic(const std::string & topic_)
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

  const std::string & multi_properties_config::get_topic() const
  {
    return _topic_;
  }

  const file_include & multi_properties_config::get_fi() const
  {
    return _fi_;
  }

  file_include & multi_properties_config::grab_fi()
  {
    return _fi_;
  }

  void multi_properties_config::set_fi(const file_include & fi_)
  {
    _fi_ = fi_;
    if (!_fi_.is_initialized()) {
      _fi_.initialize();
    }
    return;
  }


  void multi_properties_config::_write(std::ostream & out_, const multi_properties & target_)
  {
    uint32_t pcfg_options = 0;
    pcfg_options |= properties_config::SMART_MODULO;
    pcfg_options |= properties_config::RESOLVE_PATH;
    if (_skip_private_properties_) {
      pcfg_options |= properties_config::SKIP_PRIVATE;
    }

    if (!_without_decoration_) {
      out_ << "# -*- mode: conf-unix; -*-" << std::endl;
      out_ << std::endl;
    }

    properties_config pcfg(pcfg_options);
    if (_header_footer_) {
      out_ << _format::COMMENT_CHAR << _format::SPACE_CHAR
            << "List of sections of configuration properties (datatools::multi_properties)"
            << std::endl;
      out_ << std::endl;
    }

    if (has_topic() && _requested_topic_) {
      out_ << "#@topic" << _format::SPACE_CHAR << get_topic() << std::endl;
    }
    if (target_.has_description()) {
      out_ << "#@description" << _format::SPACE_CHAR << target_.get_description() << std::endl;
    }
    out_ << "#@key_label" << _format::SPACE_CHAR << _format::QUOTES_CHAR << target_.get_key_label() << _format::QUOTES_CHAR
          << std::endl;
    out_ << "#@meta_label" << _format::SPACE_CHAR << _format::QUOTES_CHAR << target_.get_meta_label() << _format::QUOTES_CHAR
          << std::endl;
    out_ << std::endl;

    for (const std::string& k : target_.ordered_keys()) {
      const multi_properties::entry& pentry = target_.get(k);
    //for (const entry * pentry : target_._ordered_entries_) {
      const std::string & name = pentry.get_key();

      // Apply criterion to skip the section:
      if (_skip_private_sections_ && properties::key_is_private(name)) {
        continue;
      }

      std::ostringstream entry_head_oss;
      entry_head_oss << _format::OPEN_CHAR
            << target_.get_key_label() << _format::ASSIGN_CHAR
            << _format::QUOTES_CHAR << name << _format::QUOTES_CHAR;
      if (pentry.has_meta()) {
        entry_head_oss << _format::SPACE_CHAR
              << target_.get_meta_label() << _format::ASSIGN_CHAR
              << _format::QUOTES_CHAR << pentry.get_meta() << _format::QUOTES_CHAR;
      }
      entry_head_oss << _format::CLOSE_CHAR;

      if (!_without_decoration_) {
        std::size_t len = entry_head_oss.str().length();
        out_ << std::setfill('#') << std::setw(len) << "" << std::endl;
      }
      out_ << entry_head_oss.str() << std::endl << std::endl;

      pcfg.write(out_, pentry.get_properties());
      out_ << std::endl;
    }

    if (_header_footer_) {
      out_ << _format::COMMENT_CHAR << _format::SPACE_CHAR << "End of list of sections of configuration properties (datatools::multi_properties)"
            << std::endl;
    }
    return;
  }

  void multi_properties_config::_read(std::istream & in_, multi_properties & target_)
  {
    std::string line_in;
    std::string mprop_description;
    std::string mprop_key_label;
    std::string mprop_meta_label;
    std::string mprop_format;
    int closed_section_first_line_number = -1;
    int new_section_first_line_number = -1;
    if (_start_without_labels_) {
      target_.clear_key_label();
      target_.clear_meta_label();
    }
    bool line_goon = false;
    int line_count = 0;

    /* Current block to be processed:
     *
     * [key_label="current_key" meta_label="current_meta"]
     *
     * current_block_oss.str()
     *
     */
    std::ostringstream current_block_oss;
    std::string current_key = "";
    std::string current_meta = "";

    _current_line_number_ = 0;
    // File inclusion support:
    bool enable_file_inclusion = true;
    // By default, file inclusion rules are propagated to included files:
    bool include_file_propagate = true;
    logger::priority fi_debug = logger::PRIO_FATAL;
    if (_forbid_include_) {
      enable_file_inclusion = false;
    }    bool allow_fi_setup = true;
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
      if (datatools::logger::is_trace(_logging_)) {
        variant_trace = true;
      }
    }
    std::string variant_section_only;
    bool can_variant_section_only = false;
    unsigned int vpp_flags = 0;
    if (variant_trace) {
      // Special trace print:
      vpp_flags |= configuration::variant_preprocessor::FLAG_TRACE;
    }
    configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> paths_to_be_included;

    bool blocks_started  = false;
    while (in_) {
      bool append_block_line = true;   // XXX
      // append_block_line = false;
      std::string line_get;
      std::getline(in_,line_get);
      _current_line_number_++;
      bool line_continue = false;
      int sz = line_get.size ();
      if (sz > 0 && line_get[sz-1] == _format::CONTINUATION_CHAR) {
        line_continue = true;
        line_get = line_get.substr(0, sz - 1);
      }
      if (line_goon) {
        line_in += line_get;
      } else {
        // A new line:
        line_in = line_get;
        line_count++;
      }
      line_goon = false;
      // Check if line should go on:
      if (line_continue) line_goon = true;
      // bool blocks_started  = false;
      bool process_block = false;
      std::string new_key = "";
      std::string new_meta = "";
      if (!line_goon) {
        bool skip_line = false;
        std::string & line = line_in;
        // Check if line is blank:
        std::istringstream check_iss(line_in);
        std::string check_word;
        check_iss >> std::ws >> check_word;
        if (check_word.empty()) skip_line = true;
        // Check if line is a comment:
        if (!skip_line) {
          std::istringstream iss(line);
          char c = 0;
          iss >> c;
          if (c == '#') {
            iss >> std::ws;
            std::string token;
            iss >> token;
            if (token.size() > 1 && token[0] == '@') {
              if (token == "@variant_section_only") {
                DT_THROW_IF(!enable_variants, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Variant directives are forbidden!");
                DT_THROW_IF(!can_variant_section_only, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "'@variant_section_only' directive can only be set at section start!");
                std::string variant_path_rule;
                iss >> std::ws >> variant_path_rule;
                variant_section_only = variant_path_rule;
                DT_LOG_DEBUG(_logging_,
                             (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                             "Line #" << _current_line_number_ << ": "
                             << "Next section is active only with variant '"
                             << variant_section_only << "'");
                append_block_line = false;
                can_variant_section_only = false;
                DT_LOG_TRACE(_logging_,
                             (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                             "Line #" << _current_line_number_ << ": "
                             << "'can_variant_section_only' is inhibited just after parsing a '@variant_section_only' directive.");
              } else if (token == "@format") {
                if (!blocks_started) {
                  DT_THROW_IF(!mprop_format.empty(), std::logic_error, "Directive '" << token << "' was already used!");
                  iss >> std::ws;
                  std::string format;
                  std::getline(iss, format);
                  if (!format.empty()) {
                    mprop_format = format;
                    DT_THROW_IF(mprop_format != "datatools::multi_properties",
                                std::logic_error,
                                (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                "Line #" << _current_line_number_ << ": "
                                << "The '@format' directive does not match the expected 'datatools::multi_properties'!");
                  } else {
                    DT_THROW(std::logic_error, "Empty format string!");
                  }
                  append_block_line = false;
                }
              } else if (token == "@description") {
                if (!blocks_started) {
                  if (mprop_description.empty()) {
                    iss >> std::ws;
                    std::string desc;
                    std::getline(iss, desc);
                    if (!desc.empty()) {
                      mprop_description = desc;
                      target_.set_description(mprop_description);
                    }
                    append_block_line = false;
                  } else {
                    DT_THROW(std::logic_error, "Directive '" << token << "' was already used!");
                  }
                } else {
                  append_block_line = true;
                }
              } else if (token == "@variant_devel") {
                DT_THROW_IF(!enable_variants, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Variant directives are forbidden!");
                if (!blocks_started) {
                  vpp.set_logging(logger::PRIO_TRACE);
                  append_block_line = false;
                } else {
                  append_block_line = true;
                }
              } else if (token == "@key_label") {
                if (!blocks_started) {
                  if (mprop_key_label.empty()) {
                    iss >> std::ws;
                    std::string key_label;
                    uint32_t reader_flags = 0;
                    DT_THROW_IF (!io::read_quoted_string(iss, key_label, reader_flags),
                                 std::logic_error,
                                 (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                 "Line #" << _current_line_number_ << ": "
                                 << "Unquoted value for '@key_label'");
                    std::string tmp;
                    std::getline(iss, tmp);
                    if (!key_label.empty()) {
                      mprop_key_label = key_label;
                      if (target_.get_key_label().empty()) {
                        target_.set_key_label(mprop_key_label);
                      } else {
                        DT_THROW_IF (target_.get_key_label() != mprop_key_label,
                                     std::logic_error,
                                     (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                     "Line #" << _current_line_number_ << ": "
                                     << "Incompatible key label '"
                                     << mprop_key_label
                                     << "' with required '"
                                     << target_.get_key_label() << "' !");
                      }
                    }
                    append_block_line = false;
                  } else {
                    DT_THROW(std::logic_error, "Directive '" << token << "' was already used!");
                  }
                } else {
                  DT_THROW(std::logic_error,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "Unsupported token '" << token << "' with required '" << target_.get_key_label() << "'!");
                }
              } else if (token == "@forbid_include") {
                DT_THROW_IF(blocks_started, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unsupported token '" << token << "' while parsing sections with required '" << target_.get_key_label() << "'!");
                enable_file_inclusion = false;
                append_block_line = false;
              } else if (token == "@include_no_propagate") {
                DT_THROW_IF(blocks_started, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unsupported token '" << token << "' while parsing sections with required '" << target_.get_key_label() << "'!");
                include_file_propagate = false;
              } else if (token == "@include_debug") {
                DT_THROW_IF(blocks_started, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unsupported token '" << token << "' while parsing sections with required '" << target_.get_key_label() << "'!");
                fi_debug = datatools::logger::PRIO_DEBUG;
                _fi_.set_logging(fi_debug);
                append_block_line = false;
              // } else if (token == "@include_path_env_strategy") {
              //   DT_THROW_IF(blocks_started, std::logic_error,
              //               (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
              //               "Line #" << _current_line_number_ << ": "
              //               << "Unsupported token '" << token << "' while parsing sections with required '" << target_._key_label_ << "'!");
              //   DT_THROW_IF(! enable_file_inclusion,
              //               std::logic_error,
              //               "Directive '" << token << "' is not supported!");
              //   iss >> std::ws;
              //   std::string fi_env_strategy;
              //   uint32_t reader_flags = 0;
              //   DT_THROW_IF(!io::read_quoted_string(iss, fi_env_strategy, reader_flags),
              //               std::logic_error,
              //               (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
              //               "Line #" << _current_line_number_ << ": "
              //               << "Unquoted value for '@include_path_env_strategy'!");
              //   file_include::ev_strategy evs = file_include::EV_PREPEND;
              //   if (fi_env_strategy == "prepend") {
              //   } else if (fi_env_strategy == "append") {
              //     evs = file_include::EV_APPEND;
              //   } else if (fi_env_strategy == "clear") {
              //     evs = file_include::EV_CLEAR;
              //   } else {
              //     DT_THROW(std::logic_error,
              //              (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
              //              "Line #" << _current_line_number_ << ": "
              //              << "Invalid include path env strategy '" << fi_env_strategy << "'!");
              //   }
              //   _fi_.set_include_path_env_strategy(evs);
              //   append_block_line = false;
              } else if (token == "@include_dir") {
                DT_THROW_IF(blocks_started, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unsupported token '" << token << "' while parsing sections with required '" << target_.get_key_label() << "'!");
                if (!allow_fi_setup) {
                  DT_LOG_DEBUG(fi_debug, "Ignoring directive '" << token << "'");
                } else {
                  DT_THROW_IF(! enable_file_inclusion,
                              std::logic_error,
                              "Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_dir_path;
                  uint32_t reader_flags = 0;
                  DT_THROW_IF(!io::read_quoted_string(iss, fi_dir_path, reader_flags),
                              std::logic_error,
                              (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                              "Line #" << _current_line_number_ << ": "
                              << "Unquoted value for '@include_dir'");
                  DT_LOG_DEBUG(fi_debug, "Append include path '" << fi_dir_path << "'");
                  _fi_.append_explicit_include_path(fi_dir_path);
                }
                append_block_line = false;
              } else if (token == "@include_path_env") {
                DT_THROW_IF(blocks_started, std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unsupported token '" << token << "' while parsing sections with required '" << target_.get_key_label() << "'!");
                if (!allow_fi_setup) {
                  DT_LOG_DEBUG(fi_debug, "Ignoring directive '" << token << "'");
                } else {
                  DT_THROW_IF(! enable_file_inclusion,
                              std::logic_error,
                              "Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_env_name;
                  uint32_t reader_flags = 0;
                  DT_THROW_IF(!io::read_quoted_string(iss, fi_env_name, reader_flags),
                              std::logic_error,
                              (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                              "Line #" << _current_line_number_ << ": "
                              << "Unquoted value for '@include_path_env'");
                  DT_LOG_DEBUG(fi_debug, "Set include path env '" << fi_env_name << "'");
                  _fi_.set_include_path_env_name(fi_env_name);
                }
                append_block_line = false;
              } else if (token == "@include_sections") {
                DT_THROW_IF(! enable_file_inclusion,
                            std::logic_error,
                            "Directive '" << token << "' is not supported!");
                iss >> std::ws;
                std::string fi_path;
                uint32_t reader_flags = 0;
                DT_THROW_IF(!io::read_quoted_string(iss, fi_path, reader_flags),
                            std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unquoted value for '@include_sections'");
                std::tuple<bool, std::string> fi_result = _fi_.resolve_err(fi_path);
                DT_THROW_IF(! std::get<0>(fi_result), std::logic_error,
                            "Cannot resolve include file path for '" << fi_path << "'!");
                std::string resolved_path = std::get<1>(fi_result);
                DT_LOG_DEBUG(fi_debug, "Set resolved include sections path '" << resolved_path << "'");
                paths_to_be_included.push_back(resolved_path);
                if (!current_key.empty()) {
                  // The current block must be processed before to include any file:
                  process_block = true;
                }
                append_block_line = false;
              } else if (token == "@include_sections_try") {
                DT_THROW_IF(! enable_file_inclusion,
                            std::logic_error,
                            "Directive '" << token << "' is not supported!");
                iss >> std::ws;
                std::string fi_path;
                uint32_t reader_flags = 0;
                DT_THROW_IF(!io::read_quoted_string(iss, fi_path, reader_flags),
                            std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                            "Line #" << _current_line_number_ << ": "
                            << "Unquoted value for '@include_sections_try'");
                std::tuple<bool, std::string> fi_result = _fi_.resolve_err(fi_path);
                if (std::get<0>(fi_result)) {
                  std::string resolved_path = std::get<1>(fi_result);
                  DT_LOG_DEBUG(fi_debug, "Set resolved include sections path '" << resolved_path << "'");
                  paths_to_be_included.push_back(resolved_path);
                } else {
                  DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                                 "Could not resolve file inclusion '" << fi_path << "'!");
                }
                if (!current_key.empty()) {
                  // The current block must be processed before to include any file:
                  process_block = true;
                }
                append_block_line = false;
              } else if (token == "@meta_label") {
                if (!blocks_started) {
                  DT_THROW_IF(!mprop_meta_label.empty(), std::logic_error, "Directive '" << token << "' was already used!");

                  iss >> std::ws;
                  std::string meta_label;
                  uint32_t reader_flags = 0;
                  DT_THROW_IF (!io::read_quoted_string(iss, meta_label, reader_flags),
                               std::logic_error,
                               (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                               "Line #" << _current_line_number_ << ": "
                               << "Unquoted value for '@meta_label'");
                  std::string tmp;
                  std::getline(iss, tmp);
                  if (meta_label.empty()) {
                    DT_THROW_IF (!target_.get_meta_label().empty(),
                                 std::logic_error,
                                 (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                 "Line #" << _current_line_number_ << ": "
                                 << "Missing meta label with setup '"
                                 << target_.get_meta_label() << "' !");
                  } else {
                    mprop_meta_label = meta_label;
                    if (target_.get_meta_label().empty()) {
                      target_.set_meta_label(mprop_meta_label);
                    } else {
                      DT_THROW_IF (target_.get_meta_label() != mprop_meta_label,
                                   std::logic_error,
                                   (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                   "Line #" << _current_line_number_ << ": "
                                   << "Incompatible meta label '"
                                   << mprop_meta_label
                                   << "' with required '"
                                   << target_.get_meta_label() << "' !");
                    }
                  }
                  append_block_line = false;
                } else {
                  append_block_line = true;
                  DT_LOG_TRACE(_logging_,
                               (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                               "Line #" << _current_line_number_ << ": "
                               << "Token '" << token << "' is not supported ! Passing it to the block parsing.");
                }
              }
            } // end of '#@xxxx'

            skip_line = true;
            {
              std::istringstream local_iss(line);
              std::string check;
              local_iss >> check;
              if (check.length() > 2 && check.substr(0,2) == "#@") {
                skip_line = false;
              }
            }
            if (skip_line) {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "Skip line  '" << line << "'");
            } else {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "Do not skip line  '" << line << "'");
            }
          } // if (c == '#')
        } // if ( ! skip_line )

        DT_LOG_TRACE(_logging_,
                     (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                     "Line '" << line << "'");
        DT_LOG_TRACE(_logging_,
                     (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                     "append_block_line=" << append_block_line);

        // Parse line which are not comments nor meta-comments:
        if (!skip_line)  {
          std::istringstream iss_line(line);
          char c = 0;
          iss_line >> c >> std::ws;
          // Search for 'key/meta' line:
          if (c == _format::OPEN_CHAR) {
            if (! blocks_started) {
              blocks_started = true;
              if (new_section_first_line_number > 0) {
                closed_section_first_line_number = new_section_first_line_number;
              }
              new_section_first_line_number = _current_line_number_;
            }
            // Parse 'key/meta' line:
            iss_line >> std::ws;
            std::string key_label;
            std::getline(iss_line, key_label, _format::ASSIGN_CHAR);
            DT_THROW_IF (key_label != target_.get_key_label(),
                         std::logic_error,
                         (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                         "Line #" << _current_line_number_ << ": "
                         << "Incompatible key label '"
                         << key_label
                         << "' with required '"
                         << target_.get_key_label() << "' !");
            new_key.clear();
            uint32_t key_reader_flags = datatools::io::reader_allow_trailing_characters;
            DT_THROW_IF (! io::read_quoted_string(iss_line, new_key, key_reader_flags),
                         std::logic_error,
                         (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                         "Line #" << _current_line_number_ << ": "
                         << "Cannot read quoted string key value from line '"
                         << line << "' !");
            iss_line >> std::ws;
            char dummy = iss_line.peek();
            new_meta.clear();
            std::string meta_label;
            if (dummy != _format::CLOSE_CHAR) {
              // Parsing meta directive:
              std::getline(iss_line, meta_label, _format::ASSIGN_CHAR);
              if (!meta_label.empty()) {
                DT_THROW_IF (meta_label != target_.get_meta_label(),
                             std::logic_error,
                             (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                             "Line #" << _current_line_number_ << ": "
                             << "Incompatible meta label '"
                             << meta_label
                             << "' with required '"
                             << target_.get_meta_label() << "' !");
                uint32_t meta_reader_flags = datatools::io::reader_allow_trailing_characters;
                DT_THROW_IF (!io::read_quoted_string(iss_line, new_meta, meta_reader_flags),
                             std::logic_error,
                             (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                             "Line #" << _current_line_number_ << ": "
                             << "Cannot read quoted string meta value from line '"
                             << line << "' !");
              }
            } else {
              // Case with missing meta directive:
              if (!target_.get_meta_label().empty()) {
                DT_LOG_WARNING(_logging_,
                               (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                               "Line #" << _current_line_number_ << ": "
                               << "Missing meta directive with meta label '"
                               << target_.get_meta_label() << "' for container described by '"
                               << mprop_description << "'!");
              }
              /*
              // 2014-02-26 FM: we now accept if the meta directive (meta_label="blah") is missing...
              DT_THROW_IF (!target_.get_meta_label().empty(),
              std::logic_error,
              "Expected meta record '"
              << target_.get_meta_label()
              << '='
              << "\"???\"" << "' is missing !");
              */
            }
            iss_line >> std::ws;
            c = 0;
            iss_line >> c;
            DT_THROW_IF (c != _format::CLOSE_CHAR, std::logic_error,
                         (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                         "Line #" << _current_line_number_ << ": "
                         << "Cannot read 'key/meta' closing symbol !");
            DT_LOG_TRACE(_logging_,
                         (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                         "Line #" << _current_line_number_ << ": "
                         << "Found a section header  = [" << key_label << "=" << '"' << new_key << "\"]");
            if (!meta_label.empty()) {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "  with meta label = [" << meta_label << "=" << '"' << new_meta << "\"]");
            }
            can_variant_section_only = true;
            DT_LOG_TRACE(_logging_,
                         (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                         "Line #" << _current_line_number_ << ": "
                         << "'can_variant_section_only' is enabled = " << can_variant_section_only);
            process_block = true;

            {
              // Detection of trailing bits after section definition :
              //  * Warn: '[key_label="new_key" meta_label="new_meta"] Blah-blah '
              //  * Pass: '[key_label="new_key" meta_label="new_meta"] # Blah-blah '
              std::string trailing_bits;
              iss_line >> trailing_bits;
              boost::trim(trailing_bits);
              if (trailing_bits.length() > 0) {
                if (trailing_bits[0] != _format::COMMENT_CHAR) {
                  DT_LOG_WARNING(_logging_,
                                 (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                                 "Line #" << _current_line_number_ << ": " <<
                                 "There are unprocessed trailing characters '" << trailing_bits
                                 << "' after definition of new section '" << new_key << "'!");
                }
              }
            }

          } else {
            // Append line to the current block stream:
            if (append_block_line) {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "appending block line = '" << line << "'");
              can_variant_section_only = false;
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                           "Line #" << _current_line_number_ << ": "
                           << "'can_variant_section_only' is inhibited because we have found a line to be appended to a section block.");
              current_block_oss << line << std::endl;
            }
          }
        } // !skip_line

      } // if ( ! line_goon )

      // in_ >> std::ws;

      // At the end of the stream, we should process the current block, if any:
      if (in_.eof()) {
        if (!current_key.empty()) {
          process_block = true;
        }
      }

      if (process_block) {
        // The current block is considered as closed so we parse it:
        if (datatools::logger::is_trace(_logging_)) {
          DT_LOG_TRACE(_logging_, "**************************************");
          DT_LOG_TRACE(_logging_, " BLOCK                                ");
          DT_LOG_TRACE(_logging_, "**************************************");
          std::cerr << current_block_oss.str() << std::endl;
          DT_LOG_TRACE(_logging_, "**************************************");
        }
        if (!current_key.empty()) {
          // By default we load the block:
          bool load_it = true;
          if (!variant_section_only.empty()) {
            bool variant_section_devel = logger::is_trace(vpp.get_logging());
            bool variant_section_only_found = false;
            bool variant_section_only_reverse = false;
            // But we first check if a 'variant_section_only' directive may disable the block:
            command::returned_info cri = vpp.resolve_variant(variant_section_only,
                                                             variant_section_only_found,
                                                             variant_section_only_reverse);
            DT_THROW_IF(cri.is_failure(), std::logic_error,
                        "Cannot preprocess variant section only directive from '"
                        << variant_section_only << "' with key='" << current_key << "' and meta='" << current_meta << "' : \n"
                        << cri.get_error_message() << "\n"
                        << "Block : \n" << current_block_oss.str() << '\n'
                        << "Hint : maybe you should add a missing default boolean value that will be used when no variant service is enabled.\n"
                        << "Example: #@variant_section_only " << variant_section_only << "|[true or false]");
            if (variant_section_devel) {
              DT_LOG_TRACE(_logging_, "VPP ==> variant_section_only_found='" << variant_section_only_found << "'");
              DT_LOG_TRACE(_logging_, "VPP ==> variant_section_only_reverse='" << variant_section_only_reverse << "'");
            }
            load_it = variant_section_only_found;
            if (variant_section_only_reverse) {
              load_it = !variant_section_only_found;
            }
            variant_section_only.clear();
          } // variant_section_only
          if (_skip_private_sections_) {
            // We check if private sections are parsed:
            if (properties::key_is_private(current_key)) {
              load_it = false;
            }
          }

          if (load_it) {
            DT_LOG_TRACE(_logging_, "load_it = [" << load_it << "]");
            DT_LOG_TRACE(_logging_, "load the current section '" << current_key << "' with meta='" << current_meta << "'");
            target_.add(current_key, current_meta);
            multi_properties::entry & e = target_.grab(current_key);
            uint32_t pcr_options = 0;
            pcr_options |= properties_config::RESOLVE_PATH;
            if (_skip_private_properties_) {
              pcr_options |= properties_config::SKIP_PRIVATE;
            }
            properties_config pcr(pcr_options);
            pcr.set_reader_input(_current_filename_);
            pcr.set_section_info(current_key, closed_section_first_line_number);
            if (include_file_propagate) {
              // Propagate the resolving rules of the local file inclusion mechanism:
              pcr.set_fi(_fi_);
            }
            std::istringstream block_iss(current_block_oss.str());
            pcr.read(block_iss, e.grab_properties());
            DT_LOG_TRACE(_logging_,"load_it = [" << load_it << "] is done.");
          }
          // Reset the current block stream:
          current_block_oss.str("");
          process_block = false;
        }
        // update new key/meta values:
        current_key = new_key;
        current_meta = new_meta;
      } // block processing

      if (paths_to_be_included.size()) {
        DT_LOG_DEBUG(_logging_, "Before inclusion : ");
        if (datatools::logger::is_debug(_logging_)) {
          target_.print_tree(std::cerr);
        }
        // Trigger the inclusion of external file:
        for (const std::string & inc_path : paths_to_be_included) {
          DT_LOG_DEBUG(_logging_, "About to include file with path '" << inc_path << "'");
          uint32_t reader_options = multi_properties_config::RESOLVE_PATH;
          if (datatools::logger::is_debug(_logging_)) {
            reader_options |= multi_properties_config::LOG_DEBUG;
          }
          multi_properties_config reader(reader_options);
          if (include_file_propagate) {
            // Propagate the resolving rules of the local file inclusion helper :
            reader.set_fi(_fi_);
          }
          multi_properties inc_mconf;
          reader.read(inc_path, inc_mconf);
          DT_LOG_DEBUG(_logging_, "About to include file with path '" << inc_path << "'");
          DT_LOG_DEBUG(_logging_, " -> Included config : ");
          if (datatools::logger::is_debug(_logging_)) {
            inc_mconf.print_tree(std::cerr);
          }
          bool allow_override_sections = true;
          bool allow_override_props = true;
          target_.merge_with(inc_mconf, allow_override_sections, allow_override_props);
        }
        paths_to_be_included.clear();
      }

    } // while ( *_in )
    if (mprop_key_label.empty()
        && mprop_meta_label.empty()
        && mprop_format.empty()
        && target_.size() == 0) {
      DT_LOG_WARNING(_logging_,
                     "Could not load any configuration section from the input stream !"
                     << "The input stream seems not to have the proper \"datatools::multi_properties\" format! "
                     << "Please check the input file/stream!");
    }
    return;

  } /* end of multi_properties_config::read */
}