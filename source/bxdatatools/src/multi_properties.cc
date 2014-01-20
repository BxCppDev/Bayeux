// multi_properties.cc
// Ourselves
#include <datatools/multi_properties.h>

// Standard Library
#include <stdexcept>
#include <sstream>
#include <fstream>

// This Project
#include <datatools/exception.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                      "datatools::multi_properties");

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::multi_properties,
                                                               "datatools::utils::multi_properties");

namespace datatools {

DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(multi_properties,"datatools::multi_properties");

const char multi_properties::OPEN = '[';
const char multi_properties::CLOSE = ']';
const char multi_properties::COMMENT = '#';
const bool multi_properties::with_header_footer = true;
const bool multi_properties::without_header_footer = false;
const bool multi_properties::write_public_only = false;
const bool multi_properties::write_private_also = true;
const bool multi_properties::read_public_only = true;
const bool multi_properties::read_private_also = false;

const std::string & multi_properties::defaults::key_label()
{
  static std::string value;
  if (value.empty()) value = "name";
  return value;
}

const std::string & multi_properties::defaults::meta_label()
{
  static std::string value;
  if (value.empty()) value = "type";
  return value;
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


properties& multi_properties::entry::get_properties() {
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
               "Changing key label is not allowed !");
  key_label_ = a_key_label;
}


const std::string & multi_properties::get_key_label() const {
  return key_label_;
}


void multi_properties::set_meta_label(const std::string& a_meta_label) {
  DT_THROW_IF (this->size(),
               std::logic_error,
               "Changing meta label is not allowed !");
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

// ctor:
multi_properties::multi_properties() {
  debug_ = false;
  _init_("", "", "", false);
}

// ctor:
multi_properties::multi_properties(const std::string& a_key_label,
                                   const std::string& a_meta_label) {
  _init_(a_key_label,a_meta_label,"",false);
}

// ctor:
multi_properties::multi_properties(const std::string& a_key_label,
                                   const std::string& a_meta_label,
                                   const std::string& a_description,
                                   bool a_debug) {
  _init_(a_key_label,a_meta_label,a_description,a_debug);
}


// dtor:
multi_properties::~multi_properties() {
  ordered_entries_.clear();
  entries_.clear();
}


bool multi_properties::has_key(const std::string& a_key) const {
  entries_col_type::const_iterator found = entries_.find(a_key);
  return found != entries_.end();
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
      };
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


multi_properties::entry& multi_properties::get(const std::string& a_key) {
  entries_col_type::iterator found = entries_.find(a_key);
  DT_THROW_IF (found == entries_.end(),
               std::logic_error,
               "Key '" << a_key << "' is not used !");
  return found->second;
}

const properties& multi_properties::get_section_const(const std::string& a_key) const {
  return get_section(a_key);
}

const properties& multi_properties::get_section(
    const std::string& a_key) const {
  return this->get(a_key).get_properties();
}


properties& multi_properties::get_section(const std::string& a_key) {
  return this->get(a_key).get_properties();
}


properties& multi_properties::grab_section(const std::string& a_key) {
  return this->get(a_key).get_properties();
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

  if (meta_label_.empty() && a_meta.empty()) {
    DT_THROW_IF (true,
                 std::logic_error,
                 "Key '" << a_key << "' should not have an empty '" << meta_label_ << "' !");
  }
  entries_[a_key] = entry(a_key, a_meta);
  ordered_entries_.push_back(&entries_[a_key]);
  return entries_[a_key].get_properties ();
}


void multi_properties::add(const std::string& a_key,
                           const std::string& a_meta) {
  this->add_impl(a_key, a_meta);
}


void multi_properties::add(const std::string& a_key,
                           const properties& a_props) {
  this->add_impl(a_key);
  entries_[a_key].get_properties() = a_props;
}


void multi_properties::add(const std::string& a_key,
                           const std::string& a_meta,
                           const properties& a_props) {
  this->add_impl(a_key, a_meta);
  entries_[a_key].get_properties() = a_props;
}


properties& multi_properties::add_section(const std::string& a_key,
                                          const std::string& a_meta)
{
  return add_impl2 (a_key, a_meta);
}

void multi_properties::write(const std::string& a_filename,
                             bool a_header_footer,
                             bool a_write_private) const {
  std::ofstream fout(a_filename.c_str());
  DT_THROW_IF (!fout,
               std::runtime_error,
               "Cannot open file '" <<  a_filename << "' !");
  properties::config pcfg(true, properties::config::MODE_BARE, true);
  if (a_header_footer) {
    fout << "# List of multi-properties (datatools::multi_properties):" << std::endl;
    fout << std::endl;
  }
  fout << "#@description " << this->get_description() << std::endl;
  fout << "#@key_label   " << '"' << this->get_key_label() << '"'
       << std::endl;
  fout << "#@meta_label  " << '"' << this->get_meta_label()<< '"'
       << std::endl;
  fout << std::endl;

  for (entries_ordered_col_type::const_iterator i = ordered_entries_.begin();
       i != ordered_entries_.end();
       ++i) {
    const entry *pentry = *i;
    const std::string& name = pentry->get_key();
    const entry& an_entry = *pentry;
    if (!a_write_private) {
     if (properties::key_is_private(name)) {
        continue;
      }
    }
    fout << multi_properties::OPEN
         << this->get_key_label() << '='
         << '"' << name << '"';
    if (an_entry.has_meta()) {
      fout << ' '
          << this->get_meta_label() << '='
          << '"' << an_entry.get_meta() << '"';
    }
    fout << multi_properties::CLOSE << std::endl << std::endl;

    pcfg.write(fout, an_entry.get_properties());
    fout << std::endl;
  }

  if (a_header_footer) {
    fout << "# End of list of multi-properties." << std::endl;
  }
}


void multi_properties::read(const std::string& a_filename,
                            bool a_skip_private) {
  std::ifstream fin(a_filename.c_str());
  DT_THROW_IF (!fin,
               std::runtime_error,
               "Cannot open file '" <<  a_filename << "' !");
  this->read_impl(fin, a_skip_private);
}


void multi_properties::read_impl(std::istream& in_, bool a_skip_private) {
  std::string line_in;
  std::string mprop_description;
  std::string mprop_key_label;
  std::string mprop_meta_label;
  bool line_goon = false;
  int line_count = 0;
  std::ostringstream current_block_oss;
  std::string current_key = "";
  std::string current_meta = "";

  while (in_) {
    std::string line_get;
    std::getline(in_,line_get);
    bool line_continue = false;
    int sz = line_get.size ();
    if (sz > 0 && line_get[sz-1] == '\\') {
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

    // check if line should go on:
    if (line_continue) line_goon = true;

    bool process_block = false;
    std::string new_key = "";
    std::string new_meta = "";

    if (!line_goon) {
      bool skip_line = false;
      std::string line = line_in;

      // check if line is blank:
      std::istringstream check_iss(line_in);
      std::string check_word;
      check_iss >> std::ws >> check_word;

      if (check_word.empty()) skip_line = true;

      // check if line is a comment:
      if (!skip_line) {
        std::istringstream iss(line);
        char c = 0;
        iss >> c;

        if (c == '#') {
          iss >> std::ws;
          std::string token;
          iss >> token;

          if (token == "@description" && mprop_description.empty()) {
            iss >> std::ws;
            std::string desc;
            std::getline(iss, desc);
            if (!desc.empty()) {
              mprop_description = desc;
              this->set_description(mprop_description);
            }
          }

          if (token == "@key_label" && mprop_key_label.empty()) {
            iss >> std::ws;
            std::string key_label;
            DT_THROW_IF (!properties::config::read_quoted_string(iss, key_label),
                         std::logic_error,
                         "Unquoted value for 'key_label'");
            std::string tmp;
            std::getline(iss, tmp);

            if (!key_label.empty()) {
              mprop_key_label = key_label;
              if (key_label_.empty()) {
                this->set_key_label(mprop_key_label);
              } else {
                DT_THROW_IF (key_label_ != mprop_key_label,
                             std::logic_error,
                             "Incompatible key label '"
                             << mprop_key_label
                             << "' with setup '"
                             << key_label_ << "' !");
              }
            }
          }

          if (token == "@meta_label" && mprop_meta_label.empty()) {
            iss >> std::ws;
            std::string meta_label;
            DT_THROW_IF (!properties::config::read_quoted_string(iss, meta_label),
                         std::logic_error,
                         "Unquoted value for 'meta_label'");
            std::string tmp;
            std::getline(iss, tmp);
            if (meta_label.empty()) {
              DT_THROW_IF (!meta_label_.empty(),
                           std::logic_error,
                           "Missing meta label with setup '"
                           << meta_label_ << "' !");
            } else {
              mprop_meta_label = meta_label;
              if (meta_label_.empty()) {
                this->set_meta_label(mprop_meta_label);
              } else {
                DT_THROW_IF (meta_label_ != mprop_meta_label,
                             std::logic_error,
                             "Incompatible meta label '"
                             << mprop_meta_label
                             << "' with setup '"
                             << meta_label_ << "' !");
              }
            }
          }
          skip_line = true;
          {
            std::istringstream iss(line);
            std::string check;
            iss >> check;
            if (check.length() > 2 && check.substr(0,2) == "#@") skip_line = false;
          }
        }
      } // if ( ! skip_line )

      // parse line:
      if (!skip_line)  {
        std::istringstream iss(line);
        char c = 0;
        iss >> c >> std::ws;
        // search for 'key/meta' line:
        if (c == '[') {
          // parse 'key/meta' line:
          iss >> std::ws;
          std::string key_label;
          std::getline(iss, key_label, '=');
          DT_THROW_IF (key_label != this->get_key_label(),
                       std::logic_error,
                       "Incompatible key label '"
                       << key_label
                       << "' with setup '"
                       << key_label_ << "' !");
          DT_THROW_IF (! properties::config::read_quoted_string(iss, new_key),
                       std::logic_error,
                       "Cannot read quoted std::string key value from line '"
                       << line << "' !");
          iss >> std::ws;
          char dummy = iss.peek();
          if (dummy != ']') {
            std::string meta_label;
            std::getline(iss, meta_label, '=');
            if (!meta_label.empty()) {
              DT_THROW_IF (meta_label != this->get_meta_label(),
                           std::logic_error,
                           "Incompatible meta label '"
                           << meta_label
                           << "' with setup '"
                           << meta_label_ << "' !");
              DT_THROW_IF (!properties::config::read_quoted_string(iss, new_meta),
                           std::logic_error,
                           "Cannot read quoted std::string meta value from line '"
                           << line << "' !");
            }
          } else {
            DT_THROW_IF (!this->get_meta_label().empty(),
                         std::logic_error,
                         "Expected meta record '"
                         << this->get_meta_label()
                         << '='
                         << "\"???\"" << "' is missing !");
          }
          iss >> std::ws;
          c = 0;
          iss >> c;
          DT_THROW_IF (c != ']', std::logic_error, "Cannot read 'key/meta' closing symbol !");
          process_block = true;
        } else {
          // append line to the current block stream:
          current_block_oss << line << std::endl;
        }
      } // !skip_line

    } // if ( ! line_goon )

    in_ >> std::ws;

    if (in_.eof()) process_block = true;

    if (process_block) {
      if (!current_key.empty()) {
        bool load_it = true;
        if (a_skip_private) {
          if (properties::key_is_private(current_key)) {
            load_it = false;
          }
        }

        if (load_it) {
          this->add(current_key, current_meta);
          multi_properties::entry& e = this->get(current_key);
          properties::config pcr;
          std::istringstream block_iss(current_block_oss.str());
          pcr.read(block_iss, e.get_properties());
          current_block_oss.str("");
        }
      }
      // update new key/meta values:
      current_key = new_key;
      current_meta = new_meta;
    }

  } // while ( *_in )
} /* end of multi_properties::read_impl */


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
      const std::string& key = i->first;
      const entry& a_entry = i->second;
      a_out << indent;
      std::ostringstream indent_oss;
      indent_oss << indent;
      entries_col_type::const_iterator j = i;
      j++;
      //a_out << i_tree_dumpable::inherit_skip_tag (a_inherit);
      //indent_oss << i_tree_dumpable::inherit_skip_tag (a_inherit);
      a_out << i_tree_dumpable::skip_tag;
      indent_oss << i_tree_dumpable::skip_tag;

      if (j == entries_.end()) {
        a_out << i_tree_dumpable::last_tag;
        indent_oss << i_tree_dumpable::inherit_skip_tag(a_inherit);
      } else {
        a_out << i_tree_dumpable::tag;
        indent_oss << i_tree_dumpable::skip_tag;
      }
      a_out << "Entry : " << '"' << key << '"';

      if (properties::key_is_private(key)) a_out << " [private]";

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

      std::string key = p_entry->get_key();
      a_out << "Entry [rank=" << rank << "] : " << '"' << key << '"';

      if (properties::key_is_private(key)) a_out << " [private]";

      a_out << std::endl;
      rank++;
    }
  }
}

} // end of namespace datatools
