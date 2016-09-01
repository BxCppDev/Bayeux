// multi_properties.cc

// Ourselves:
#include <datatools/multi_properties.h>

// Standard Library:
#include <stdexcept>
#include <sstream>
#include <fstream>

// - Boost:
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/ioutils.h>
#include <datatools/kernel.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                      "datatools::multi_properties")

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                               "datatools::utils::multi_properties")

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

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(multi_properties,"datatools::multi_properties")

  const std::string & multi_properties::defaults::key_label()
  {
    static const std::string _label("name");
    return _label;
  }

  const std::string & multi_properties::defaults::meta_label()
  {
    static const std::string _label("type");
    return _label;
  }

  //----------------------------------------------------------------------
  // entry class impl
  //
  multi_properties::entry::entry(const std::string& a_key,
                                 const std::string& a_meta) {
    if (!a_key.empty()) this->set_key(a_key);
    this->set_meta(a_meta);
  }


  multi_properties::entry::~entry() {
    properties_.clear();
  }


  const properties& multi_properties::entry::get_properties() const {
    return properties_;
  }


  properties& multi_properties::entry::grab_properties() {
    return properties_;
  }


  const std::string& multi_properties::entry::get_key() const {
    return key_;
  }


  void multi_properties::entry::set_key(const std::string& a_key) {
    DT_THROW_IF (a_key.empty(),
                 std::logic_error,
                 "Empty key is not allowed !");
    key_ = a_key;
  }


  const std::string& multi_properties::entry::get_meta() const {
    return meta_;
  }


  void multi_properties::entry::set_meta(const std::string& a_meta) {
    meta_ = a_meta;
  }


  bool multi_properties::entry::has_meta() const {
    return (!meta_.empty());
  }


  void multi_properties::entry::tree_dump(std::ostream& a_out,
                                          const std::string & a_title,
                                          const std::string & a_indent,
                                          bool inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;
    if (!a_title.empty()) a_out << indent << a_title << std::endl;

    a_out << indent << i_tree_dumpable::tag
          << "Key        : \"" <<  key_ << "\"" << std::endl;

    a_out << indent << i_tree_dumpable::tag
          << "Meta       : \"" <<  meta_ << "\"" << std::endl;

    {
      a_out << indent << i_tree_dumpable::inherit_tag(inherit)
            << "Properties : ";
      if (properties_.size () == 0) {
        a_out << "<empty>";
      } else {
        a_out << '[' << properties_.size() << ']';
      }
      a_out << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << i_tree_dumpable::inherit_skip_tag(inherit);
        properties_.tree_dump(a_out, "", indent_oss.str());
      }
    }
  }


  //----------------------------------------------------------------------
  // multiproperties class impl
  //
  DATATOOLS_CLONEABLE_IMPLEMENTATION(multi_properties)

  bool multi_properties::is_debug() const {
    return debug_;
  }


  void multi_properties::set_debug(bool a_debug) {
    debug_ = a_debug;
    return;
  }

  bool multi_properties::has_description() const
  {
    return !description_.empty();
  }

  const std::string & multi_properties::get_description() const {
    return description_;
  }


  void multi_properties::set_description(const std::string& a_description) {
    description_ = a_description;
  }


  void multi_properties::set_key_label(const std::string& a_key_label) {
    DT_THROW_IF (a_key_label.empty(),
                 std::logic_error,
                 "Empty key is not allowed !");
    DT_THROW_IF (size(),
                 std::logic_error,
                 "Changing key label from '" << key_label_ << "' to '" << a_key_label << "' is not allowed in multi-properties '" << description_ << "'!");
    key_label_ = a_key_label;
  }


  const std::string & multi_properties::get_key_label() const {
    return key_label_;
  }


  void multi_properties::set_meta_label(const std::string& a_meta_label) {
    DT_THROW_IF (this->size(),
                 std::logic_error,
                 "Changing meta label from '" << meta_label_ << "' to '" << a_meta_label << "' is not allowed in multi-properties '" << description_ << "'!");
    meta_label_ = a_meta_label;
  }


  const std::string & multi_properties::get_meta_label() const {
    return meta_label_;
  }

  uint32_t multi_properties::size() const {
    DT_THROW_IF (ordered_entries_.size() != entries_.size(),
                 std::runtime_error,
                 "Internal containers are broken !");
    return entries_.size();
  }

  bool multi_properties::empty() const
  {
    return this->size() == 0;
  }

  void multi_properties::reset() {
    key_label_.clear();
    meta_label_.clear();
    this->clear();
  }

  void multi_properties::clear() {
    ordered_entries_.clear();
    entries_.clear();
  }

  const multi_properties::entries_col_type& multi_properties::entries() const {
    return entries_;
  }

  const multi_properties::entries_ordered_col_type&
  multi_properties::ordered_entries() const {
    return ordered_entries_;
  }

  void multi_properties::_init_(const std::string& a_key_label,
                                const std::string& a_meta_label,
                                const std::string& a_description,
                                bool a_debug) {
    debug_ = a_debug;
    if (key_label_.empty()) key_label_ = defaults::key_label();
    if (meta_label_.empty()) meta_label_ = defaults::meta_label();
    if (!a_key_label.empty()) this->set_key_label(a_key_label);
    this->set_meta_label(a_meta_label);
    this->set_description(a_description);
  }

  multi_properties::multi_properties() {
    debug_ = false;
    _init_("", "", "", false);
  }

  multi_properties::multi_properties(const std::string& a_key_label,
                                     const std::string& a_meta_label) {
    _init_(a_key_label,a_meta_label,"",false);
  }

  multi_properties::multi_properties(const std::string& a_key_label,
                                     const std::string& a_meta_label,
                                     const std::string& a_description,
                                     bool a_debug) {
    _init_(a_key_label,a_meta_label,a_description,a_debug);
  }

  multi_properties::~multi_properties() {
    ordered_entries_.clear();
    entries_.clear();
  }

  void multi_properties::_copy_impl_(const multi_properties & source_)
  {
    this->debug_       = source_.debug_;
    this->description_ = source_.description_;
    this->key_label_   = source_.key_label_;
    this->meta_label_  = source_.meta_label_;
    this->entries_     = source_.entries_;
    for (entries_ordered_col_type::const_iterator i = source_.ordered_entries_.begin();
         i != source_.ordered_entries_.end();
         i++) {
      const entry * source_entry = *i;
      const std::string & source_key = source_entry->get_key();
      entries_col_type::iterator found = this->entries_.find(source_key);
      this->ordered_entries_.push_back(&found->second);
    }
    return;
  }

  multi_properties::multi_properties(const multi_properties & source_)
  {
    _copy_impl_(source_);
    return;
  }

  multi_properties & multi_properties::operator=(const multi_properties & source_)
  {
    if (this != &source_) { // protect against invalid self-assignment
      _copy_impl_(source_);
    }
    return *this;
  }

  bool multi_properties::has_key(const std::string& a_key) const {
    entries_col_type::const_iterator found = entries_.find(a_key);
    return found != entries_.end();
  }

  bool multi_properties::has_key_with_meta(const std::string& a_key, const std::string& a_meta) const
  {
    entries_col_type::const_iterator found = entries_.find(a_key);
    if (found == entries_.end()) return false;
    return found->second.get_meta() == a_meta;
  }

  const std::string & multi_properties::key (int key_index_) const
  {
    int key_count = 0;
    entries_col_type::const_iterator iter = entries_.begin();
    for (;
         iter != entries_.end();
         ++iter, ++key_count) {
      if (key_count == key_index_) {
        break;
      };
    }
    DT_THROW_IF (iter == entries_.end(),
                 std::logic_error,
                 "Invalid key index '"
                 << key_index_ << "' !");
    return iter->first;
  }

  const std::string & multi_properties::ordered_key (int key_index_) const
  {
    int key_count = 0;
    entries_ordered_col_type::const_iterator iter = ordered_entries_.begin();
    for (;
         iter != ordered_entries_.end();
         ++iter, ++key_count) {
      if (key_count == key_index_) {
        break;
      }
    }
    DT_THROW_IF (iter == ordered_entries_.end(),
                 std::logic_error,
                 "Invalid ordered key index '"
                 << key_index_ << "' !");
    return (*iter)->get_key ();
  }

  void multi_properties::keys(std::vector<std::string>&k) const
  {
    for (entries_col_type::const_iterator iter = entries_.begin();
         iter != entries_.end();
         ++iter) {
      k.push_back(iter->first);
    }
    return;
  }

  std::vector<std::string> multi_properties::keys () const
  {
    std::vector<std::string> k;
    keys(k);
    return k;
  }


  std::vector<std::string> multi_properties::ordered_keys () const
  {
    std::vector<std::string> k;
    ordered_keys(k);
    return k;
  }

  void multi_properties::ordered_keys(std::vector<std::string>&k) const
  {
    for (entries_ordered_col_type::const_iterator iter = ordered_entries_.begin();
         iter != ordered_entries_.end();
         ++iter) {
      entry * e = *iter;
      k.push_back(e->get_key());
    }
    return;
  }

  bool multi_properties::has_section(const std::string& a_key) const {
    return this->has_key(a_key);
  }

  const multi_properties::entry& multi_properties::get(
                                                       const std::string& a_key) const {
    entries_col_type::const_iterator found = entries_.find(a_key);
    DT_THROW_IF (found == entries_.end(),
                 std::logic_error,
                 "Key '" << a_key << "' is not used !");
    return found->second;
  }

  multi_properties::entry& multi_properties::grab(const std::string& a_key) {
    entries_col_type::iterator found = entries_.find(a_key);
    DT_THROW_IF (found == entries_.end(),
                 std::logic_error,
                 "Key '" << a_key << "' is not used !");
    return found->second;
  }

  const properties& multi_properties::get_section_const(const std::string& a_key) const {
    return get_section(a_key);
  }

  const properties& multi_properties::get_section(const std::string& a_key) const {
    return this->get(a_key).get_properties();
  }

  properties& multi_properties::grab_section(const std::string& a_key) {
    return this->grab(a_key).grab_properties();
  }

  void multi_properties::remove_impl(const std::string& a_key) {
    entries_ordered_col_type::iterator found = ordered_entries_.end();
    for (entries_ordered_col_type::iterator i = ordered_entries_.begin();
         i != ordered_entries_.end();
         ++i) {
      entry *e = *i;
      if (e->get_key() == a_key) {
        found = i;
        break;
      }
    }
    if (found != ordered_entries_.end()) {
      ordered_entries_.erase(found);
    }
    entries_.erase(a_key);
  }


  void multi_properties::remove(const std::string& a_key) {
    entries_col_type::iterator found = entries_.find(a_key);
    DT_THROW_IF (found == entries_.end(),
                 std::logic_error,
                 "Key '" << a_key << "' is not used !");
    multi_properties::remove_impl(a_key);
  }


  void multi_properties::add_impl(const std::string& a_key,
                                  const std::string& a_meta) {
    add_impl2 (a_key, a_meta);
  }

  properties& multi_properties::add_impl2(const std::string& a_key,
                                          const std::string& a_meta) {
    DT_THROW_IF (entries_.find(a_key) != entries_.end(),
                 std::logic_error,
                 "Key '" << a_key << "' is already used !");
    /*
    // Fix ticket #83:
    if (! meta_label_.empty() && a_meta.empty()) {
    DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
    "Key '" << a_key << "' has a an empty '" << meta_label_ << "' !");
    }
    */
    if (meta_label_.empty() && !a_meta.empty()) {
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Key '" << a_key << "' will ignore meta '" << a_meta << "' !");
    }
    entries_[a_key] = entry(a_key, a_meta);
    ordered_entries_.push_back(&entries_[a_key]);
    return entries_[a_key].grab_properties ();
  }


  void multi_properties::add(const std::string& a_key,
                             const std::string& a_meta) {
    this->add_impl(a_key, a_meta);
  }


  void multi_properties::add(const std::string& a_key,
                             const properties& a_props) {
    this->add_impl(a_key);
    entries_[a_key].grab_properties() = a_props;
  }


  void multi_properties::add(const std::string& a_key,
                             const std::string& a_meta,
                             const properties& a_props) {
    this->add_impl(a_key, a_meta);
    entries_[a_key].grab_properties() = a_props;
  }


  properties& multi_properties::add_section(const std::string& a_key,
                                            const std::string& a_meta)
  {
    return add_impl2 (a_key, a_meta);
  }

  void multi_properties::write(const std::string& a_filename,
                               uint32_t options) const
  {
    config w(options);
    w.write(a_filename, *this);
    return;
  }

  void multi_properties::read(const std::string& a_filename,
                              uint32_t options) {
    config r(options);
    r.read(a_filename, *this);
    return;
  }

  multi_properties::config::config(uint32_t options_, const std::string & topic_)
  {
    _init_defaults();
    if (options_ & SKIP_PRIVATE_SECTIONS) {
      _skip_private_sections_ = true;
    }
    if (options_ & SKIP_PRIVATE_PROPS) {
      _skip_private_properties_ = true;
    }
    if (options_ & FORBID_VARIANTS) {
      _forbid_variants_ = true;
    }
    if (options_ & HEADER_FOOTER) {
      _header_footer_ = true;
    }
    if (options_ & LOG_MUTE) {
      set_logging(datatools::logger::PRIO_FATAL);
    }
    if (options_ & LOG_DEBUG) {
      set_logging(datatools::logger::PRIO_DEBUG);
    }
    if (options_ & LOG_TRACE) {
      set_logging(datatools::logger::PRIO_TRACE);
    }
    if (options_ & RESOLVE_PATH) {
      _resolve_path_ = true;
    }
    if (!topic_.empty()) {
      set_topic(topic_);
    }
    return;
  }

  datatools::logger::priority multi_properties::config::get_logging() const
  {
    return _logging_;
  }

  void multi_properties::config::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  void multi_properties::config::_init_defaults()
  {
    _logging_ = datatools::logger::PRIO_WARNING;
    _skip_private_sections_ = false;
    _skip_private_properties_ = false;
    _forbid_variants_ = false;
    _header_footer_ = false;
    _requested_topic_ = false;
    _resolve_path_ = false;
    _current_line_number_ = -1;
    return;
  }

  void multi_properties::config::reset()
  {
    _topic_.clear();
    _current_filename_.clear();
    _init_defaults();
    return;
  }

  void multi_properties::config::_set_current_filename(const std::string & filename_)
  {
    _current_filename_ = filename_;
    return;
  }

  void multi_properties::config::read(const std::string & a_filename, multi_properties & target_)
  {
    std::string filename = a_filename;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::runtime_error,
                  "Cannot resolve filename '" + a_filename + "'!");
    }
    std::ifstream fin(filename.c_str());
    DT_THROW_IF(!fin,
                std::runtime_error,
                "Cannot open filename '" + a_filename + "' (resolved as '" + filename + "'!");
    _set_current_filename(filename);
    read(fin, target_);
    fin.close();
    _current_filename_.clear();
    _current_line_number_ = -1;
    return;
  }

  void multi_properties::config::read(std::istream& in, multi_properties& target)
  {
    _read(in, target);
    return;
  }

  void multi_properties::config::write(const std::string& a_filename,
                                       const multi_properties& source_)
  {
    std::string filename = a_filename;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::runtime_error,
                  "Cannot resolve filename '" + a_filename + "'!");
    }
    std::ofstream fout(filename.c_str());
    DT_THROW_IF (!fout,
                 std::runtime_error,
                 "Cannot open filename '" + a_filename + "' (resolved as '" + filename + "'!");
    _set_current_filename(filename);
    _write(fout, source_);
    fout.close();
    _current_filename_.clear();
    _current_line_number_ = -1;
    return;
  }

  void multi_properties::config::write(std::ostream& out,
                                       const multi_properties& source)
  {
    _write(out, source);
    return;
  }

  bool multi_properties::config::has_topic() const
  {
    return !_topic_.empty();
  }

  void multi_properties::config::set_topic(const std::string & topic)
  {
    if (!topic.empty()) {
      uint32_t validation_flags = 0;
      // validation_flags != NV_NO_HYPHEN
      DT_THROW_IF(!name_validation(topic, validation_flags),
                  std::logic_error,
                  "Keyword/topic '" << topic << "' contains forbidden characters!");
      _requested_topic_ = true;
    } else {
      _requested_topic_ = false;
    }
    _topic_ = topic;
    return;
  }

  const std::string & multi_properties::config::get_topic() const
  {
    return _topic_;
  }

  void multi_properties::config::_write(std::ostream& a_out, const multi_properties& target_)
  {
    uint32_t pcfg_options = 0;
    pcfg_options |= properties::config::SMART_MODULO;
    pcfg_options |= properties::config::RESOLVE_PATH;
    if (_skip_private_properties_) {
      pcfg_options |= properties::config::SKIP_PRIVATE;
    }
    properties::config pcfg(pcfg_options);
    if (_header_footer_) {
      a_out << _format::COMMENT_CHAR << _format::SPACE_CHAR
            << "List of sections of configuration properties (datatools::multi_properties)"
            << std::endl;
      a_out << std::endl;
    }

    if (has_topic() && _requested_topic_) {
      a_out << "#@topic" << _format::SPACE_CHAR << get_topic() << std::endl;
    }
    if (target_.has_description()) {
      a_out << "#@description" << _format::SPACE_CHAR << target_.get_description() << std::endl;
    }
    a_out << "#@key_label" << _format::SPACE_CHAR << _format::QUOTES_CHAR << target_.get_key_label() << _format::QUOTES_CHAR
          << std::endl;
    a_out << "#@meta_label" << _format::SPACE_CHAR << _format::QUOTES_CHAR << target_.get_meta_label() << _format::QUOTES_CHAR
          << std::endl;
    a_out << std::endl;

    for (entries_ordered_col_type::const_iterator i = target_.ordered_entries_.begin();
         i != target_.ordered_entries_.end();
         ++i) {
      const entry *pentry = *i;
      const std::string& name = pentry->get_key();
      const entry& an_entry = *pentry;
      bool skip_this_section = false;

      // Apply criterion to skip the section:
      if (_skip_private_sections_) {
        if (properties::key_is_private(name)) {
          skip_this_section = true;
        }
      }

      if (skip_this_section) {
        continue;
      }

      a_out << _format::OPEN_CHAR
            << target_.get_key_label() << _format::ASSIGN_CHAR
            << _format::QUOTES_CHAR << name << _format::QUOTES_CHAR;
      if (an_entry.has_meta()) {
        a_out << _format::SPACE_CHAR
              << target_.get_meta_label() << _format::ASSIGN_CHAR
              << _format::QUOTES_CHAR << an_entry.get_meta() << _format::QUOTES_CHAR;
      }
      a_out << _format::CLOSE_CHAR << std::endl << std::endl;

      pcfg.write(a_out, an_entry.get_properties());
      a_out << std::endl;
    }

    if (_header_footer_) {
      a_out << _format::COMMENT_CHAR << _format::SPACE_CHAR << "End of list of sections of configuration properties (datatools::multi_properties)"
            << std::endl;
    }
    return;
  }

  void multi_properties::config::_read(std::istream& in_, multi_properties& target_)
  {
    std::string line_in;
    std::string mprop_description;
    std::string mprop_key_label;
    std::string mprop_meta_label;
    std::string mprop_format;
    int closed_section_first_line_number = -1;
    int new_section_first_line_number = -1;

    bool line_goon = false;
    int line_count = 0;
    std::ostringstream current_block_oss;
    std::string current_key = "";
    std::string current_meta = "";

    _current_line_number_ = 0;
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

    while (in_) {
      bool append_block_line = true;
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
        // a new line
        line_in = line_get;
        line_count++;
      }
      line_goon = false;

      // Check if line should go on:
      if (line_continue) line_goon = true;

      bool blocks_started = false;
      bool process_block = false;
      std::string new_key = "";
      std::string new_meta = "";

      if (!line_goon) {
        bool skip_line = false;
        std::string line = line_in;

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

            if (token == "@variant_section_only") {
              DT_THROW_IF(!enable_variants, std::logic_error,
                          (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                          "Line " << _current_line_number_ << ": "
                          << "Variant directives are forbidden!");
              DT_THROW_IF(!can_variant_section_only, std::logic_error,
                          (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                          "Line " << _current_line_number_ << ": "
                          << "'@variant_section_only' directive can only be set at section start!");
              std::string variant_path_rule;
              iss >> std::ws >> variant_path_rule;
              variant_section_only = variant_path_rule;
              DT_LOG_DEBUG(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "Next section is active only with variant '"
                           << variant_section_only << "'");
              append_block_line = false;
              can_variant_section_only = false;
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "'can_variant_section_only' is inhibited just after parsing a '@variant_section_only' directive.");
            }

            if (token == "@format" && mprop_format.empty()) {
              iss >> std::ws;
              std::string format;
              std::getline(iss, format);
              if (!format.empty()) {
                mprop_format = format;
                DT_THROW_IF(mprop_format != "datatools::multi_properties",
                            std::logic_error,
                            (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                            "Line " << _current_line_number_ << ": "
                            << "The '@format' directive does not match the expected 'datatools::multi_properties'!");
              }
              append_block_line = false;
            } else if (token == "@description" && mprop_description.empty()) {
              iss >> std::ws;
              std::string desc;
              std::getline(iss, desc);
              if (!desc.empty()) {
                mprop_description = desc;
                target_.set_description(mprop_description);
              }
              append_block_line = false;
            } else if (token == "@key_label" && mprop_key_label.empty()) {
              iss >> std::ws;
              std::string key_label;
              uint32_t reader_flags = 0;
              DT_THROW_IF (!io::read_quoted_string(iss, key_label, reader_flags),
                           std::logic_error,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "Unquoted value for '@key_label'");
              std::string tmp;
              std::getline(iss, tmp);
              if (!key_label.empty()) {
                mprop_key_label = key_label;
                if (target_.get_key_label().empty()) {
                  target_.set_key_label(mprop_key_label);
                } else {
                  DT_THROW_IF (target_.key_label_ != mprop_key_label,
                               std::logic_error,
                               (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                               "Line " << _current_line_number_ << ": "
                               << "Incompatible key label '"
                               << mprop_key_label
                               << "' with required '"
                               << target_.key_label_ << "' !");
                }
              }
              append_block_line = false;
            } else if (token == "@meta_label" && mprop_meta_label.empty()) {
              iss >> std::ws;
              std::string meta_label;
              uint32_t reader_flags = 0;
              DT_THROW_IF (!io::read_quoted_string(iss, meta_label, reader_flags),
                           std::logic_error,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "Unquoted value for '@meta_label'");
              std::string tmp;
              std::getline(iss, tmp);
              if (meta_label.empty()) {
                DT_THROW_IF (!target_.meta_label_.empty(),
                             std::logic_error,
                             (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                             "Line " << _current_line_number_ << ": "
                             << "Missing meta label with setup '"
                             << target_.meta_label_ << "' !");
              } else {
                mprop_meta_label = meta_label;
                if (target_.meta_label_.empty()) {
                  target_.set_meta_label(mprop_meta_label);
                } else {
                  DT_THROW_IF (target_.meta_label_ != mprop_meta_label,
                               std::logic_error,
                               (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                               "Line " << _current_line_number_ << ": "
                               << "Incompatible meta label '"
                               << mprop_meta_label
                               << "' with required '"
                               << target_.meta_label_ << "' !");
                }
              }
              append_block_line = false;
            }
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
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "Skip line  '" << line << "'");
            } else {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "Do not skip line  '" << line << "'");
            }
          }
        } // if ( ! skip_line )

        DT_LOG_TRACE(_logging_,
                     (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                     "Line '" << line << "'");
        DT_LOG_TRACE(_logging_,
                     (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                     "append_block_line=" << append_block_line);

        // Parse line:
        if (!skip_line)  {
          // std::cerr << "DEVEL: " << "PROCESSING LINE = '" << line << "'" << std::endl;
          std::istringstream iss_line(line);
          char c = 0;
          iss_line >> c >> std::ws;
          // Search for 'key/meta' line:
          // XXX
          if (c == _format::OPEN_CHAR) {
            if (! blocks_started) {
              blocks_started = true;
              // std::cerr << "DEVEL: " << "blocks_started: new_section_first_line_number = [" << new_section_first_line_number << "]..." << std::endl;
              if (new_section_first_line_number > 0) {
                closed_section_first_line_number = new_section_first_line_number;
                // std::cerr << "DEVEL: " << "Previous SECTION started at line [" << closed_section_first_line_number << "]..." << std::endl;
              }
              new_section_first_line_number = _current_line_number_;
              // std::cerr << "DEVEL: " << "New SECTION starts at line [" << _current_line_number_ << "]..." << std::endl;
            }
            // Parse 'key/meta' line:
            iss_line >> std::ws;
            std::string key_label;
            std::getline(iss_line, key_label, _format::ASSIGN_CHAR);
            DT_THROW_IF (key_label != target_.key_label_,
                         std::logic_error,
                         (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                         "Line " << _current_line_number_ << ": "
                         << "Incompatible key label '"
                         << key_label
                         << "' with required '"
                         << target_.key_label_ << "' !");
            new_key.clear();
            uint32_t key_reader_flags = datatools::io::reader_allow_trailing_characters;
            DT_THROW_IF (! io::read_quoted_string(iss_line, new_key, key_reader_flags),
                         std::logic_error,
                         (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                         "Line " << _current_line_number_ << ": "
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
                DT_THROW_IF (meta_label != target_.meta_label_,
                             std::logic_error,
                             (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                             "Line " << _current_line_number_ << ": "
                             << "Incompatible meta label '"
                             << meta_label
                             << "' with required '"
                             << target_.meta_label_ << "' !");
                uint32_t meta_reader_flags = datatools::io::reader_allow_trailing_characters;
                DT_THROW_IF (!io::read_quoted_string(iss_line, new_meta, meta_reader_flags),
                             std::logic_error,
                             (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                             "Line " << _current_line_number_ << ": "
                             << "Cannot read quoted string meta value from line '"
                             << line << "' !");
              }
            } else {
              // Case with missing meta directive:
              if (!target_.get_meta_label().empty()) {
                DT_LOG_WARNING(_logging_,
                               (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                               "Line " << _current_line_number_ << ": "
                               << "Missing meta directive with meta label '"
                               << target_.get_meta_label() << "' for container described by '"
                               << mprop_description << "'!");
              }
              /*
              // 2014-02-26 FM: we accept if the meta directive (meta_label="blah") is missing...
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
                         (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                         "Line " << _current_line_number_ << ": "
                         << "Cannot read 'key/meta' closing symbol !");
            DT_LOG_TRACE(_logging_,
                         (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                         "Line " << _current_line_number_ << ": "
                         << "Found a section header  = [" << key_label << "=" << '"' << new_key << "\"]");
            if (!meta_label.empty()) {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "  with meta label = [" << meta_label << "=" << '"' << new_meta << "\"]");
            }
            can_variant_section_only = true;
            DT_LOG_TRACE(_logging_,
                         (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                         "Line " << _current_line_number_ << ": "
                         << "'can_variant_section_only' is enabled = " << can_variant_section_only);
            process_block = true;

            {
              // Detection of trailing bits after section definition :
              //  * Warn:  '[key_label="new_key" meta_label="new_meta"] Blah-blah '
              //  * Pass:  '[key_label="new_key" meta_label="new_meta"] # Blah-blah '
              std::string trailing_bits;
              iss_line >> trailing_bits;
              boost::trim(trailing_bits);
              if (trailing_bits.length() > 0) {
                if (trailing_bits[0] != _format::COMMENT_CHAR) {
                  DT_LOG_WARNING(_logging_,
                                 (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                                 "Line " << _current_line_number_ << ": " <<
                                 "There are unprocessed trailing characters '" << trailing_bits
                                 << "' after definition of new section '" << new_key << "'!");
                }
              }
            }

          } else {
            // Append line to the current block stream:
            if (append_block_line) {
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "appending block line = '" << line << "'");
              can_variant_section_only = false;
              DT_LOG_TRACE(_logging_,
                           (_current_filename_.empty() ? "" : "File '" +_current_filename_ + "': ") <<
                           "Line " << _current_line_number_ << ": "
                           << "'can_variant_section_only' is inhibited because we have found a line to be appended to a section block.");
              current_block_oss << line << std::endl;
            }
          }
        } // !skip_line

      } // if ( ! line_goon )

      // in_ >> std::ws;

      // At the end of the stream, we should process the current block, if any:
      if (in_.eof()) process_block = true;

      if (process_block) {

        if (datatools::logger::is_trace(_logging_)) {
          DT_LOG_TRACE(_logging_, "**************************************");
          DT_LOG_TRACE(_logging_, " BLOCK                                ");
          DT_LOG_TRACE(_logging_, "**************************************");
          std::cerr << current_block_oss.str() << std::endl;
          DT_LOG_TRACE(_logging_, "**************************************");
        }
        if (!current_key.empty()) {
          bool load_it = true;
          bool variant_section_devel = logger::is_trace(vpp.get_logging());
          bool variant_section_only_found = false;
          bool variant_section_only_reverse = false;
          if (!variant_section_only.empty()) {
            command::returned_info cri = vpp.resolve_variant(variant_section_only,
                                                             variant_section_only_found,
                                                             variant_section_only_reverse);
            DT_THROW_IF(cri.is_failure(), std::logic_error,
                        "Cannot preprocess variant section only directive from '"
                        << variant_section_only << "' : " << cri.get_error_message());
            if (variant_section_devel) {
              DT_LOG_TRACE(_logging_,
                           "VPP ==> variant_section_only_found='"
                           << variant_section_only_found << "'");
              DT_LOG_TRACE(_logging_,
                           "VPP ==> variant_section_only_reverse='"
                           << variant_section_only_reverse << "'");
            }
            if (! variant_section_only.empty()) {
              load_it = variant_section_only_found;
              if (variant_section_only_reverse) {
                load_it = !variant_section_only_found;
              }
            }
            variant_section_only.clear();
          }
          if (_skip_private_sections_) {
            if (properties::key_is_private(current_key)) {
              load_it = false;
            }
          }

          if (load_it) {
            DT_LOG_TRACE(_logging_, "load the current section '" << current_key << "' with meta='" << current_meta << "'");
            target_.add(current_key, current_meta);
            multi_properties::entry & e = target_.grab(current_key);
            uint32_t pcr_options = 0;
            pcr_options |= properties::config::RESOLVE_PATH;
            if (_skip_private_properties_) {
              pcr_options |= properties::config::SKIP_PRIVATE;
            }
            properties::config pcr(pcr_options);
            pcr.set_reader_input(_current_filename_); //, closed_section_first_line_number);
            pcr.set_section_info(current_key, closed_section_first_line_number);
            std::istringstream block_iss(current_block_oss.str());
            pcr.read(block_iss, e.grab_properties());
            // Reset the current block stream:
            current_block_oss.str("");
          }
        }
        // update new key/meta values:
        current_key = new_key;
        current_meta = new_meta;
      }

    } // while ( *_in )
    if (mprop_key_label.empty()
        && mprop_meta_label.empty()
        && mprop_format.empty()
        && target_.size() == 0) {
      DT_LOG_WARNING(_logging_,
                     "Could not load any configuration section from the input stream !"
                     << "The input stream seems not to have to proper \"datatools::multi_properties\" format! "
                     << "Please check the input file/stream!");
    }

  } /* end of multi_properties::config::read */

  void multi_properties::dump(std::ostream& a_out) const {
    this->tree_dump(a_out, "datatools::multi_properties:");
  }

  void multi_properties::tree_dump(std::ostream& a_out,
                                   const std::string& a_title,
                                   const std::string& a_indent,
                                   bool a_inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;
    if (!a_title.empty()) a_out << indent << a_title << std::endl;

    if (!description_.empty()) {
      a_out << indent << i_tree_dumpable::tag
            << "Description  : " <<  this->get_description() << std::endl;
    }

    if (!key_label_.empty()) {
      a_out << indent << i_tree_dumpable::tag
            << "Key label    : \""
            << this->get_key_label()
            << "\"" << std::endl;
    }

    if (!meta_label_.empty()) {
      a_out << indent << i_tree_dumpable::tag
            << "Meta label   : \"" << this->get_meta_label()
            << "\"" << std::endl;
    }

    {
      a_out << indent << i_tree_dumpable::tag
            << "Entries      : ";
      if (entries_.size() == 0) {
        a_out << "<empty>";
      } else {
        a_out << "[" << entries_.size() << "]";
      }

      a_out << std::endl;
      for (entries_col_type::const_iterator i = entries_.begin();
           i != entries_.end();
           ++i) {
        const std::string& local_key = i->first;
        const entry& a_entry = i->second;
        a_out << indent;
        std::ostringstream indent_oss;
        indent_oss << indent;
        entries_col_type::const_iterator j = i;
        j++;
        a_out << i_tree_dumpable::skip_tag;
        indent_oss << i_tree_dumpable::skip_tag;

        if (j == entries_.end()) {
          a_out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::inherit_skip_tag(a_inherit);
        } else {
          a_out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        a_out << "Entry : " << '"' << local_key << '"';

        if (properties::key_is_private(local_key)) a_out << " [private]";

        a_out << std::endl;
        a_entry.tree_dump(a_out, "", indent_oss.str());
      }
    }

    {
      int rank = 0;
      a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
            << "Ordered entries      : ";
      if (ordered_entries_.size() == 0) {
        a_out << "<empty>";
      } else {
        a_out << "[" << ordered_entries_.size() << "]";
      }
      a_out << std::endl;

      for (entries_ordered_col_type::const_iterator i = ordered_entries_.begin();
           i != ordered_entries_.end();
           ++i) {
        const entry *p_entry = *i;
        a_out << indent;
        std::ostringstream indent_oss;
        indent_oss << indent;
        entries_ordered_col_type::const_iterator j = i;
        j++;
        a_out << i_tree_dumpable::inherit_skip_tag(a_inherit);
        indent_oss << i_tree_dumpable::inherit_skip_tag(a_inherit);

        if (j == ordered_entries_.end()) {
          a_out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::inherit_skip_tag(a_inherit);
        } else {
          a_out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }

        std::string local_key = p_entry->get_key();
        a_out << "Entry [rank=" << rank << "] : " << '"' << local_key << '"';

        if (properties::key_is_private(local_key)) a_out << " [private]";

        a_out << std::endl;
        rank++;
      }
    }
  }

} // end of namespace datatools
