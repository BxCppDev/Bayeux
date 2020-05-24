// datatools/ui/ihs.cc - Interface Hierarchical System
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/ui/ihs.h>

// Third Party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>
#include <datatools/exception.h>

namespace datatools {

  namespace ui {

    // static
    void ihs::path_remove_scheme(const std::string & path_, std::string & out_)
    {
      std::size_t found = path_.find(':');
      if (found != std::string::npos) {
        out_ = path_.substr(found + 1);
      } else {
        out_ = path_;
      }
      return;
    }

    // static
    bool ihs::path_has_scheme(const std::string & path_, const std::string & scheme_)
    {
      std::size_t found = path_.find(':');
      if (found == std::string::npos) {
        return false;
      }
      std::string scheme = path_.substr(0, found);
      if (!scheme_.empty()) {
        if (scheme != scheme_) return false;
      }
      return true;
    }

    // static
    std::string ihs::node_type_to_label(node_type type_)
    {
      if (type_ == NODE_INTERFACE) return std::string("interface");
      if (type_ == NODE_COMMAND) return std::string("command");
      return std::string("");
    }

    ihs::node_type ihs::label_to_node_type(const std::string & label_)
    {
      if (label_ == node_type_to_label(NODE_INTERFACE)) {
        return NODE_INTERFACE;
      } else if (label_ == node_type_to_label(NODE_COMMAND)) {
        return NODE_COMMAND;
      }
      return NODE_INVALID;
    }

    ihs::node::node()
    {
      _ihs_ = nullptr;
      _type_ = NODE_INVALID;
      _owned_interface_ = false;
      _interface_ = nullptr;
      _owned_command_ = false;
      _command_ = nullptr;
      _parent_node_ = nullptr;
      return;
    }

    ihs::node::node(node_type type_, const std::string & full_path_)
    {
      _ihs_ = nullptr;
      _type_ = NODE_INVALID;
      _owned_interface_ = false;
      _interface_ = nullptr;
      _owned_command_ = false;
      _command_ = nullptr;
      _parent_node_ = nullptr;
      set_type(type_);
      set_full_path(full_path_);
      return;
    }

    ihs::node::~node()
    {
      reset();
      return;
    }

    void ihs::node::reset()
    {
      _ihs_ = nullptr;
      reset_interface();
      reset_command();
      _description_.clear();
      return;
    }

    void ihs::node::reset_command()
    {
      if (_owned_command_ && _command_ != nullptr) {
        delete _command_;
        _command_ = nullptr;
      }
      _owned_command_ = false;
      return;
    }

    void ihs::node::reset_interface()
    {
      if (_owned_interface_ && _interface_ != nullptr) {
        delete _interface_;
        _interface_ = nullptr;
      }
      _owned_interface_ = false;
      return;
    }

    void ihs::node::set_type(node_type type_)
    {
      _type_ = type_;
      return;
    }

    ihs::node_type ihs::node::get_type() const
    {
      return _type_;
    }

    bool ihs::node::is_root() const
    {
      if (_type_ != NODE_INTERFACE) return false;
      if (_parent_node_ != nullptr) return false;
      if (_full_path_ != datatools::ui::path::root_path()) return false;
      return true;
    }

    bool ihs::node::is_valid() const
    {
      if (_type_ == NODE_INVALID) return false;
      if (_full_path_.empty()) return false;
      if (!is_root()) {
        if (is_interface() && ! has_interface()) return false;
        if (is_command() && ! has_command()) return false;
      }
      return true;
    }

    bool ihs::node::is_interface() const
    {
      return _type_ == NODE_INTERFACE;
    }

    bool ihs::node::is_command() const
    {
      return _type_ == NODE_COMMAND;
    }

    bool ihs::node::has_description() const
    {
      return !_description_.empty();
    }

    void ihs::node::set_description(const std::string & desc_)
    {
      _description_ = desc_;
      return;
    }

    const std::string & ihs::node::get_description() const
    {
      return _description_;
    }

    datatools::properties & ihs::node::grab_metadata()
    {
      return _metadata_;
    }

    const datatools::properties & ihs::node::get_metadata() const
    {
      return _metadata_;
    }

    const std::string & ihs::node::get_full_path() const
    {
      return _full_path_;
    }

    void ihs::node::set_full_path(const std::string & fp_)
    {
      DT_THROW_IF(! ::datatools::ui::path::is_valid_path(fp_), std::logic_error,
                  "Invalid path name '" << fp_ << "'!");
      DT_THROW_IF(! boost::starts_with(fp_, datatools::ui::path::root_path()),
                  std::logic_error,
                  "Not a full path name '" << fp_ << "'!");
      _full_path_ = fp_;
      return;
    }

    void ihs::node::set_external_interface(base_command_interface & ci_)
    {
      if (_type_ == NODE_INVALID) {
        _type_ = NODE_INTERFACE;
      }
      DT_THROW_IF(! is_interface(), std::logic_error,
                  "Invalid node type for command interface '" << ci_.get_name() << "'!");
      _owned_interface_ = false;
      _interface_ = &ci_;
      return;
    }

    bool ihs::node::has_command() const
    {
      return _command_ != nullptr;
    }

    bool ihs::node::has_interface() const
    {
      return _interface_ != nullptr;
    }

    void ihs::node::set_interface(base_command_interface * ci_)
    {
      if (_type_ == NODE_INVALID) {
        _type_ = NODE_INTERFACE;
      }
      DT_THROW_IF(! is_interface(), std::logic_error,
                  "Invalid node type for command interface '" << ci_->get_name() << "'!");
      _owned_interface_ = true;
      _interface_ = ci_;
      return;
    }

    void ihs::node::set_command(base_command * cmd_)
    {
      if (_type_ == NODE_INVALID) {
        _type_ = NODE_COMMAND;
      }
      DT_THROW_IF(! is_command(), std::logic_error,
                  "Invalid node type for command '" << cmd_->get_name() << "'!");
      _owned_command_ = true;
      _command_ = cmd_;
      return;
    }

    void ihs::node::set_external_command(base_command & cmd_)
    {
      if (_type_ == NODE_INVALID) {
        _type_ = NODE_COMMAND;
      }
      DT_THROW_IF(! is_command(), std::logic_error,
                  "Invalid node type for command '" << cmd_.get_name() << "'!");
      _owned_command_ = false;
      _command_ = &cmd_;
      return;
    }

    base_command_interface & ihs::node::grab_interface()
    {
      DT_THROW_IF(!has_interface(), std::logic_error,
                  "Node '" << _full_path_ << "' is not an interface!");
      return *_interface_;
    }

    const base_command_interface & ihs::node::get_interface() const
    {
      DT_THROW_IF(!has_interface(), std::logic_error,
                  "Node '" << _full_path_ << "' is not an interface!");
      return *_interface_;
    }

    base_command & ihs::node::grab_command()
    {
      DT_THROW_IF(!has_command(), std::logic_error,
                  "Node '" << _full_path_ << "' is not a command!");
      return *_command_;
    }

    const base_command & ihs::node::get_command() const
    {
      DT_THROW_IF(!has_command(), std::logic_error,
                  "Node '" << _full_path_ << "' is not a command!");
      return *_command_;
    }

    bool ihs::node::is_child_of(node & node_) const
    {
      return node_._children_.count(const_cast<node*>(this)) > 0;
    }

    bool ihs::node::has_parent_node() const
    {
      return _parent_node_ != 0;
    }

    void ihs::node::unset_parent_node()
    {
      DT_THROW_IF(! has_parent_node(),
                  std::logic_error,
                  "Node '" << _full_path_ << "' has no parent node !");
      DT_LOG_TRACE(_ihs_->get_logging_priority(),
                   "Parent node is '" << _parent_node_->get_full_path() << "'...");
      _parent_node_->remove_child_node(*this);
      _parent_node_ = 0;
      return;
    }

    void ihs::node::set_parent_node(node & node_)
    {
      DT_THROW_IF(is_child_of(node_),
                  std::logic_error,
                  "Node '" << _full_path_ << "' is already child of node '"
                  << node_.get_full_path() << "'!");
      _parent_node_ = &node_;
      _parent_node_->add_child_node(*this);
      return;
    }

    ihs::node & ihs::node::grab_parent_node()
    {
      DT_THROW_IF(! has_parent_node(), std::logic_error,
                 "Node '" << _full_path_ << "' has no parent node!");
      return *_parent_node_;
    }

    const ihs::node & ihs::node::get_parent_node() const
    {
      DT_THROW_IF(! has_parent_node(), std::logic_error,
                  "Node '" << _full_path_ << "' has no parent node!");
      return *_parent_node_;
    }

    std::size_t ihs::node::get_number_of_children() const
    {
      return _children_.size();
    }

    bool ihs::node::has_children() const
    {
      return _children_.size() > 0;
    }

    void ihs::node::set_ihs(ihs & ihs_)
    {
      _ihs_ = &ihs_;
      return;
    }

    void ihs::node::add_child_node(node & node_)
    {
      DT_THROW_IF(!is_interface(),
                  std::logic_error,
                  "Node '" << get_full_path() << "' cannot contain child nodes!");
      DT_THROW_IF(_children_.count(&node_) > 0,
                  std::logic_error,
                  "Node '" << get_full_path() << "' already contains child node '"
                  << node_.get_full_path() << "'!");
      _children_.insert(&node_);
      return;
    }

    void ihs::node::remove_child_node(node & node_)
    {
      DT_LOG_TRACE(_ihs_->get_logging_priority(),
                   "Child node '" << get_full_path() << "' : removing child node '" << node_.get_full_path() << "'...");
      DT_THROW_IF(!is_interface(),
                  std::logic_error,
                  "Node '" << get_full_path() << "' cannot contains child nodes!");
      DT_THROW_IF(_children_.count(&node_) == 0,
                  std::logic_error,
                  "Node '" << get_full_path() << "' has no child node '"
                  << node_.get_full_path() << "'!");
      _children_.erase(&node_);
      DT_LOG_TRACE(_ihs_->get_logging_priority(),
                   "Child node '" << get_full_path() << "' has no child node '" << node_.get_full_path() << "' anymore.");
      return;
    }

    void ihs::node::tree_dump(std::ostream & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Type : " << _type_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Metadata : " << '[' << _metadata_.size() << ']' << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Full path : " << _full_path_ << std::endl;

      if (is_interface() && has_interface()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Interface : " << "[@" << _interface_ << ']' << std::endl;
      }

      if (is_command() && has_command()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Command : " << "[@" << _command_ << ']' << std::endl;
      }

      if (has_parent_node()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Parent node : " << "[@" << _parent_node_ << ']' << std::endl;
      }

      if (has_children()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Child nodes : " << "[" << _children_.size() << ']' << std::endl;
        size_t counter = 0;
        for (std::set<node *>::const_iterator ichild = _children_.begin();
             ichild != _children_.end();
             ichild ++) {
          const node * child_node = *ichild;
          out_ << indent_ << i_tree_dumpable::skip_tag;
          if (++counter == _children_.size()) {
            out_ <<  i_tree_dumpable::last_tag;
          } else {
            out_ <<  i_tree_dumpable::tag;
          }
          out_ << "Child : '" <<::datatools::ui::path::basename(child_node->get_full_path())
               << "' ";
          if (child_node->is_interface()) {
            out_ << "(interface)";
          } else if (child_node->is_command()) {
            out_ << "(command)";
          }
          out_ << std::endl;
        }
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << is_valid() << std::endl;

      return;
    }

    void ihs::node::set_trait(const std::string & trait_label_, const bool set_)
    {
      const std::string key = datatools::ui::traits::key_prefix() + trait_label_;
      if (set_) {
        if (grab_metadata().has_key(key)) {
          grab_metadata().change_boolean(key, true);
        } else {
          grab_metadata().store_boolean(key, true);
        }
      } else if (grab_metadata().has_key(key)) {
        grab_metadata().erase(key);
      }
      return;
    }

    void ihs::node::reset_trait(const std::string & trait_label_)
    {
      const std::string key = datatools::ui::traits::key_prefix() + trait_label_;
      if (grab_metadata().has_key(key)) {
        grab_metadata().erase(key);
      }
      return;
    }

    bool ihs::node::is_trait(const std::string & trait_label_) const
    {
      const std::string key = datatools::ui::traits::key_prefix() + trait_label_;
      if (get_metadata().has_key(key)) {
        bool value = get_metadata().fetch_boolean(key);
        if (value) return true;
      }
      return false;
    }

    /* IHS */

    ihs::ihs(const std::string & scheme_)
    {
      _set_scheme_(scheme_);
      // Create the root node:
      {
        node root_node;
        root_node.set_full_path(datatools::ui::path::root_path());
        root_node.set_type(NODE_INTERFACE);
        root_node.set_ihs(*this);
        _nodes_[datatools::ui::path::root_path()] = root_node;
      }
      return;
    }

    ihs::~ihs()
    {
      reset();
      return;
    }

    bool ihs::has_scheme() const
    {
      return !_scheme_.empty();
    }

    const std::string & ihs::get_scheme() const
    {
      return _scheme_;
    }

    void ihs::set_scheme(const std::string & scheme_)
    {
      DT_THROW_IF(_nodes_.size() > 1, std::logic_error,
                  "IHS scheme cannot be set/changed after child nodes have been added!");
      _set_scheme_(scheme_);
      return;
    }

    void ihs::_set_scheme_(const std::string & scheme_)
    {
      if (!scheme_.empty()) {
        // Future: use URL scheme validation utility
        std::string tst = boost::to_upper_copy<std::string>(scheme_);
        std::size_t found = tst.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+.-");
        DT_THROW_IF(found != std::string::npos, std::logic_error, "Invalid IHS scheme '" << scheme_ << "'!");
        found = tst.find_first_of("0123456789+.-");
        DT_THROW_IF(found == 0, std::logic_error, "Invalid IHS scheme '" << scheme_ << "'!");
      }
      _scheme_ = scheme_;
      return;
    }

    void ihs::reset()
    {
      _nodes_.clear();
      return;
    }

    bool ihs::exists(const std::string & path_) const
    {
      return _nodes_.find(path_) != _nodes_.end();
    }

    bool ihs::is_interface(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      return found->second.is_interface();
    }

    bool ihs::has_interface(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      return found->second.has_interface();
    }

    bool ihs::is_command(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      return found->second.is_command();
    }

    void ihs::set_trait(const std::string & path_,
                        const std::string & trait_label_,
                        const bool set_)
    {
      node_dict_type::iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "No node with path '" << path_ << "'!");
      node & n = found->second;
      n.set_trait(trait_label_, set_);
      return;
    }

    void ihs::reset_trait(const std::string & path_,
                          const std::string & trait_label_)
    {
      node_dict_type::iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "No node with path '" << path_ << "'!");
      node & n = found->second;
      n.reset_trait(trait_label_);
      return;
    }

    bool ihs::is_trait(const std::string & path_,
                       const std::string & trait_label_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      return n.is_trait(trait_label_);
    }

    /*
    bool ihs::is_interface_active(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_interface(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command interface!");
      if (n.has_interface()) {
        if (!n.get_interface().is_active()) {
          return false;
        }
      }
      if (!n.is_active()) return false;
      return true;
    }

    bool ihs::is_interface_broken(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_interface(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command interface!");
      if (n.has_interface()) {
        if (n.get_interface().is_broken()) {
          return true;
        }
      }
      if (n.is_broken()) return true;
      return false;
    }

    bool ihs::is_interface_hidden(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_interface(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command interface!");
      if (n.has_interface()) {
        if (n.get_interface().is_hidden()) {
          return true;
        }
      }
      if (n.is_hidden()) return true;
      return false;
    }

    bool ihs::is_command_active(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_command(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command!");
      if (n.has_command()) {
        if (!n.get_command().is_active()) {
          return false;
        }
      }
      if (!n.is_active()) return false;
      return true;
    }

    bool ihs::is_command_broken(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_command(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command!");
      if (n.has_command()) {
        if (n.get_command().is_broken()) {
          return true;
        }
      }
      if (n.is_broken()) return true;
      return false;
    }

    bool ihs::is_command_hidden(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      DT_THROW_IF(found == _nodes_.end(),
                  std::logic_error,
                  "No node with path '" << path_ << "'!");
      const node & n = found->second;
      DT_THROW_IF(!n.is_command(),
                  std::logic_error,
                  "Node with path '" << path_ << "' is not a command!");
      if (n.has_command()) {
        if (n.get_command().is_hidden()) {
          return true;
        }
      }
      if (n.is_hidden()) return true;
      return false;
    }
    */

    bool ihs::has_command(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      return found->second.has_command();
    }

    const base_command & ihs::get_command(const std::string & path_) const
    {
      DT_THROW_IF(path_.empty(), std::logic_error, "Node with empty path!");
      node_dict_type::const_iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      DT_THROW_IF(! found->second.has_command(), std::logic_error,
                  "Command node with path '" << path_ << "' has no command!");
      return found->second.get_command();
    }

    base_command & ihs::grab_command(const std::string & path_)
    {
      DT_THROW_IF(path_.empty(), std::logic_error, "Node with empty path!");
      node_dict_type::iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      DT_THROW_IF(! found->second.has_command(), std::logic_error,
                  "Command node with path '" << path_ << "' has no command!");
      return found->second.grab_command();
    }

    const base_command_interface & ihs::get_interface(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      DT_THROW_IF(! found->second.has_interface(), std::logic_error,
                  "Command node with path '" << path_ << "' has no interface!");
      return found->second.get_interface();
    }

    base_command_interface & ihs::grab_interface(const std::string & path_)
    {
      node_dict_type::iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      DT_THROW_IF(! found->second.has_interface(), std::logic_error,
                  "Command node with path '" << path_ << "' has no interface!");
      return found->second.grab_interface();
    }

    ihs::node & ihs::_grab_node(const std::string & path_)
    {
      node_dict_type::iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      return found->second;
    }

    const ihs::node & ihs::_get_node(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      return found->second;
    }

    void ihs::add_command(const std::string & parent_path_,
                          base_command * cmd_)
    {
      DT_THROW_IF(! exists(parent_path_),
                  std::logic_error,
                  "Node with path '" << parent_path_ << "' already exist!");
      const node & parent_node = _get_node(parent_path_);
      DT_THROW_IF(! parent_node.is_interface(),
                  std::logic_error,
                  "Parent node '" << parent_path_ << "' is not an interface node!");
      std::string command_full_path;
      datatools::ui::path::build_leaf_full_path(parent_path_,
                                                cmd_->get_name(),
                                                command_full_path);
      // Create the command node and add it in the parent node
      {
        node dummy_node;
        _nodes_[command_full_path] = dummy_node;
      }
      node & cmd_node = _nodes_.find(command_full_path)->second;
      cmd_node.set_full_path(command_full_path);
      cmd_node.set_type(NODE_COMMAND);
      cmd_node.set_command(cmd_);
      cmd_node.set_parent_node(const_cast<node&>(parent_node));
      cmd_node.set_ihs(*this);
      return;
    }

    void ihs::add_command(const std::string & parent_path_,
                          base_command & cmd_)
    {
      DT_THROW_IF(! exists(parent_path_),
                  std::logic_error,
                  "Node with path '" << parent_path_ << "' already exist!");
      const node & parent_node = _get_node(parent_path_);
      DT_THROW_IF(! parent_node.is_interface(),
                  std::logic_error,
                  "Parent node '" << parent_path_ << "' is not an interface node!");
      std::string command_full_path;
      datatools::ui::path::build_leaf_full_path(parent_path_,
                                                cmd_.get_name(),
                                                command_full_path);
      // Create the command node and add it in the parent node
      {
        node dummy_node;
        _nodes_[command_full_path] = dummy_node;
      }
      node & cmd_node = _nodes_.find(command_full_path)->second;
      cmd_node.set_full_path(command_full_path);
      cmd_node.set_type(NODE_COMMAND);
      cmd_node.set_external_command(cmd_);
      cmd_node.set_parent_node(const_cast<node&>(parent_node));
      cmd_node.set_ihs(*this);
      return;
    }

    void ihs::remove(const std::string & path_, bool recursive_)
    {
      if (is_interface(path_)) {
        remove_interface(path_, recursive_);
      } else if (is_command(path_)) {
        remove_command(path_);
      }
      DT_THROW(std::logic_error, "Invalid path '" << path_ << "'!");
    }

    void ihs::remove_command(const std::string & command_path_)
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_THROW_IF(! exists(command_path_),
                  std::logic_error,
                  "Node with path '" << command_path_ << "' does not exists!");
      node & the_node = _grab_node(command_path_);
      DT_THROW_IF(! the_node.is_command(),
                  std::logic_error,
                  "Node '" << command_path_ << "' is not a command node!");
      DT_LOG_TRACE(get_logging_priority(),
                   "About to remove command node '" << command_path_ << "'...");
      if (the_node.has_parent_node()) {
        the_node.unset_parent_node();
      }
      _nodes_.erase(command_path_);
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    void ihs::remove_interface(const std::string & interface_path_, bool recursive_)
    {
      auto logging = get_logging_priority();
      logging = datatools::logger::PRIO_DEBUG;
      DT_LOG_TRACE_ENTERING(logging);
      DT_THROW_IF(interface_path_ == datatools::ui::path::root_path(),
                  std::logic_error,
                  "Cannot remove the root node!");
      DT_THROW_IF(! exists(interface_path_), std::logic_error,
                  "Node with path '" << interface_path_ << "' does not exists!");
      node & the_node = _grab_node(interface_path_);
      DT_THROW_IF(! the_node.is_interface(),
                  std::logic_error,
                  "Node '" << interface_path_ << "' is not a command interface node!");
      DT_LOG_DEBUG(logging,
                   "About to remove interface node '" << interface_path_ << "'...");
      if (the_node.has_children()) {
        DT_THROW_IF(!recursive_, std::logic_error,
                    "Cannot remove command interface '" << interface_path_ << "' because it contains child nodes!");
        // Recursive removal of child nodes...
        while (the_node._children_.size()) {
          node * the_local_node = *the_node._children_.begin();
          if (the_local_node->is_interface()) {
            DT_LOG_DEBUG(logging,
                         "Removing interface node '" << the_local_node->get_full_path() << "'...");
            remove_interface(the_local_node->get_full_path(), recursive_);
          } else if (the_local_node->is_command()) {
            DT_LOG_DEBUG(logging,
                         "Removing command node '" << the_local_node->get_full_path() << "'...");
            remove_command(the_local_node->get_full_path());
            DT_LOG_DEBUG(logging,
                         "Command node '" << the_local_node->get_full_path() << "' has been removed.");
           }
        }
      }
      DT_LOG_DEBUG(logging,
                   "Removing interface node '" << interface_path_ << "'...");
      if (the_node.has_parent_node()) {
        the_node.unset_parent_node();
      }
      _nodes_.erase(interface_path_);
      DT_LOG_DEBUG(logging, "Done.");
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    void ihs::add_interface(const std::string & parent_path_,
                            const std::string & interface_name_,
                            const std::string & description_)
    {
      DT_THROW_IF(! exists(parent_path_), std::logic_error,
                  "Node with path '" << parent_path_ << "' does not exists!");
      DT_THROW_IF(! ::datatools::ui::path::is_valid_name(interface_name_),
                  std::logic_error,
                  "Interface name '" << interface_name_ << "' is not valid!");
      const node & parent_node = _get_node(parent_path_);
      DT_THROW_IF(! parent_node.is_interface(),
                  std::logic_error,
                  "Parent node '" << parent_path_ << "' is not an interface node!");
      std::string interface_full_path;
      datatools::ui::path::build_leaf_full_path(parent_path_,
                                                interface_name_,
                                                interface_full_path);
      // Create and add the interface node:
      {
        node dummy_node;
        _nodes_[interface_full_path] = dummy_node;
      }
      node & interface_node = _nodes_.find(interface_full_path)->second;;
      interface_node.set_type(NODE_INTERFACE);
      interface_node.set_full_path(interface_full_path);
      if (!description_.empty()) {
        interface_node.set_description(description_);
      }
      interface_node.set_parent_node(const_cast<node&>(parent_node));
      interface_node.set_ihs(*this);
      return;
    }

    void ihs::add_interface(const std::string & parent_path_,
                            base_command_interface * interface_)
    {

      DT_THROW_IF(! exists(parent_path_), std::logic_error,
                  "Node with path '" << parent_path_ << "' does not exists!");
      const node & parent_node = _get_node(parent_path_);
      DT_THROW_IF(! parent_node.is_interface(),
                  std::logic_error,
                  "Parent node '" << parent_path_ << "' is not an interface node!");
      std::string interface_full_path;
      datatools::ui::path::build_leaf_full_path(parent_path_,
                                                interface_->get_name(),
                                                interface_full_path);
      // Create and add the interface node:
      {
        node dummy_node;
        _nodes_[interface_full_path] = dummy_node;
      }
      node & interface_node = _nodes_.find(interface_full_path)->second;;
      interface_node.set_type(NODE_INTERFACE);
      interface_node.set_full_path(interface_full_path);
      interface_node.set_interface(interface_);
      interface_node.set_parent_node(const_cast<node&>(parent_node));
      interface_node.set_ihs(*this);

      // Create and add the command nodes from the interface's published commands:
      std::vector<std::string> cmd_names;
      interface_->build_command_names(cmd_names);
      for (int icmd = 0; icmd < (int) cmd_names.size(); icmd++) {
        const std::string & cmd_name = cmd_names[icmd];
        base_command & cmd = interface_->grab_command(cmd_name);
        add_command(interface_full_path, cmd);
      }

      return;
    }

    void ihs::add_interface(const std::string & parent_path_,
                            base_command_interface & interface_)
    {

      DT_THROW_IF(! exists(parent_path_), std::logic_error,
                  "Node with path '" << parent_path_ << "' does not exists!");
      const node & parent_node = _get_node(parent_path_);
      DT_THROW_IF(! parent_node.is_interface(),
                  std::logic_error,
                  "Parent node '" << parent_path_ << "' is not an interface node!");
      std::string interface_full_path;
      datatools::ui::path::build_leaf_full_path(parent_path_,
                                                interface_.get_name(),
                                                interface_full_path);
      // Create and add the interface node:
      {
        node dummy_node;
        _nodes_[interface_full_path] = dummy_node;
      }
      node & interface_node = _nodes_.find(interface_full_path)->second;;
      interface_node.set_type(NODE_INTERFACE);
      interface_node.set_full_path(interface_full_path);
      interface_node.set_external_interface(interface_);
      interface_node.set_parent_node(const_cast<node&>(parent_node));
      interface_node.set_ihs(*this);

      // Create and add the command nodes from the interface's published commands:
      std::vector<std::string> cmd_names;
      interface_.build_command_names(cmd_names);
      for (int icmd = 0; icmd < (int) cmd_names.size(); icmd++) {
        const std::string & cmd_name = cmd_names[icmd];
        base_command & cmd = interface_.grab_command(cmd_name);
        add_command(interface_full_path, cmd);
      }

      return;
    }

    bool ihs::has_children(const std::string & path_) const
    {
       DT_THROW_IF(! exists(path_), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      const node & the_node = _get_node(path_);
      return the_node._children_.size() > 0;
    }

    void ihs::build_children_paths(const std::string & path_,
                                   std::vector<std::string> & children_paths_) const
    {
      children_paths_.clear();
      DT_THROW_IF(! exists(path_), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      const node & the_node = _get_node(path_);
      children_paths_.reserve(the_node._children_.size());
      for (std::set<node *>::iterator inode = the_node._children_.begin();
           inode != the_node._children_.end();
           inode++) {
        node * the_child_node = *inode;
        children_paths_.push_back(the_child_node->get_full_path());
      }
      return;
    }

    bool ihs::has_child(const std::string & path_, const std::string & name_) const
    {
      // DT_THROW_IF(! exists(path_), std::logic_error,
      //             "Node with path '" << path_ << "' does not exists!");
      if (! exists(path_)) {
        return false;
      }
      const node & the_node = _get_node(path_);
      if (! the_node.is_interface()) {
        return false;
      }
      std::string full_child_path;
      datatools::ui::path::build_leaf_full_path(path_,
                                                name_,
                                                full_child_path);
      // DT_THROW_IF(! exists(full_child_path),
      //             std::logic_error,
      //             "Node '" << full_child_path << "' does not exist!");
      if (! exists(full_child_path)) {
        return false;
      }
      const node & the_child_node = _get_node(full_child_path);
      return the_child_node.get_parent_node().get_full_path() == path_;
    }

    std::string ihs::get_parent_path(const std::string & path_) const
    {
      DT_THROW_IF(! exists(path_), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      const node & the_node = _get_node(path_);
      if (the_node.has_parent_node()) {
        return the_node.get_parent_node().get_full_path();
      }
      return "";
    }

    std::string ihs::canonical_path(const std::string & path_) const
    {
      std::string cp;
      path_remove_scheme(path_, cp);
      if (boost::starts_with(cp, "//")) {
        cp = cp.substr(2);
      }
      return cp;
    }

    void ihs::build_path(std::set<std::string> & paths_, const uint32_t /* flags_ */) const
    {
      paths_.clear();
      for (const auto & p : _nodes_) {
        bool select = true;
        if (select) {
          paths_.insert(p.first);
        }
      }
      return;
    }

    void ihs::tree_dump(std::ostream & out_,
                        const std::string & title_,
                        const std::string & indent_,
                        bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag
           << "Scheme : ";
      if (!_scheme_.empty()) {
        out_ << "'" << _scheme_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Nodes : [" << _nodes_.size() << ']' << std::endl;
      {
        size_t counter = 0;
        for (node_dict_type::const_iterator inode = _nodes_.begin();
             inode != _nodes_.end();
             inode++) {
          out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
          if (++counter == _nodes_.size()) {
            out_ << i_tree_dumpable::last_tag;
          } else {
            out_ << i_tree_dumpable::tag;
          }
          out_ << "Node '" << inode->first << "' : ";
          const node & the_node = inode->second;
          if (the_node.is_interface() || the_node.is_command()) {
            out_ << "<" << node_type_to_label(the_node.get_type()) << ">";
          }
          out_ << std::endl;
        }
      }
      return;
    }

  } // namespace ui

} // namespace datatools
