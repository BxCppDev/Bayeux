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
#include <boost/algorithm/string/predicate.hpp>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/exception.h>

namespace datatools {

  namespace ui {

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
      _ihs_ = 0;
      _type_ = NODE_INVALID;
      _external_interface_ = 0;
      _external_command_ = 0;
      _parent_node_ = 0;
      return;
    }

    ihs::node::node(node_type type_, const std::string & full_path_)
    {
      _ihs_ = 0;
      _type_ = NODE_INVALID;
      _external_interface_ = 0;
      _external_command_ = 0;
      _parent_node_ = 0;
      set_type(type_);
      set_full_path(full_path_);
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
      if (_full_path_ != ::datatools::ui::path::root_path()) return false;
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

    bool ihs::node::has_interface() const
    {
      return _external_interface_ != 0;
    }

    bool ihs::node::is_command() const
    {
      return _type_ == NODE_COMMAND;
    }

    bool ihs::node::has_command() const
    {
      return _external_command_ != 0;
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
      DT_THROW_IF(! boost::starts_with(fp_, ::datatools::ui::path::root_path()),
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
      _external_interface_ = &ci_;
      return;
    }

    void ihs::node::set_external_command(base_command & cmd_)
    {
      if (_type_ == NODE_INVALID) {
        _type_ = NODE_COMMAND;
      }
      DT_THROW_IF(! is_command(), std::logic_error,
                  "Invalid node type for command '" << cmd_.get_name() << "'!");
      _external_command_ = &cmd_;
      return;
    }

    base_command_interface & ihs::node::grab_interface()
    {
      DT_THROW_IF(!has_interface(), std::logic_error,
                  "Node '" << _full_path_ << "' is not an interface!");
      return *_external_interface_;
    }

    const base_command_interface & ihs::node::get_interface() const
    {
      DT_THROW_IF(!has_interface(), std::logic_error,
                  "Node '" << _full_path_ << "' is not an interface!");
      return *_external_interface_;
    }

    base_command & ihs::node::grab_command()
    {
      DT_THROW_IF(!has_command(), std::logic_error,
                  "Node '" << _full_path_ << "' is not a command!");
      return *_external_command_;
    }

    const base_command & ihs::node::get_command() const
    {
      DT_THROW_IF(!has_command(), std::logic_error,
                  "Node '" << _full_path_ << "' is not a command!");
      return *_external_command_;
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
             << "Interface : " << "[@" << _external_interface_ << ']' << std::endl;
      }

      if (is_command() && has_command()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Command : " << "[@" << _external_interface_ << ']' << std::endl;
      }

      if (has_parent_node()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Parent node : " << "[@" << _parent_node_ << ']' << std::endl;
      }

      if (has_children()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Child nodes : " << "[" << _children_.size() << ']' << std::endl;
        int counter = 0;
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

    ihs::ihs()
    {
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

    bool ihs::has_command(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      if (found == _nodes_.end()) return false;
      return found->second.has_command();
    }

    const base_command & ihs::get_command(const std::string & path_) const
    {
      node_dict_type::const_iterator found = _nodes_.find(path_);
      DT_THROW_IF(found == _nodes_.end(), std::logic_error,
                  "Node with path '" << path_ << "' does not exists!");
      DT_THROW_IF(! found->second.has_command(), std::logic_error,
                  "Command node with path '" << path_ << "' has no command!");
      return found->second.get_command();
    }

    base_command & ihs::grab_command(const std::string & path_)
    {
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
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_THROW_IF(interface_path_ == ::datatools::ui::path::root_path(),
                  std::logic_error,
                  "Cannot remove the root node!");
      DT_THROW_IF(! exists(interface_path_), std::logic_error,
                  "Node with path '" << interface_path_ << "' does not exists!");
      node & the_node = _grab_node(interface_path_);
      DT_THROW_IF(! the_node.is_interface(),
                  std::logic_error,
                  "Node '" << interface_path_ << "' is not a command interface node!");
      DT_LOG_TRACE(get_logging_priority(),
                   "About to remove interface node '" << interface_path_ << "'...");
      if (the_node.has_children()) {
        DT_THROW_IF(!recursive_, std::logic_error,
                    "Cannot remove command interface '" << interface_path_ << "' because it contains child nodes!");
        // Recursive removal of child nodes...
        for (std::set<node *>::iterator inode = the_node._children_.begin();
             inode != the_node._children_.end();
             inode++) {
          node * the_node = *inode;
          if (the_node->is_interface()) {
            DT_LOG_TRACE(get_logging_priority(),
                         "Removing interface node '" << the_node->get_full_path() << "'...");
            remove_interface(the_node->get_full_path(), recursive_);
          } else if (the_node->is_command()) {
            DT_LOG_TRACE(get_logging_priority(),
                         "Removing command node '" << the_node->get_full_path() << "'...");
            remove_command(the_node->get_full_path());
          }
        }
      }
      DT_LOG_TRACE(get_logging_priority(),
                   "Removing interface node '" << interface_path_ << "'...");
      if (the_node.has_parent_node()) {
        the_node.unset_parent_node();
      }
      _nodes_.erase(interface_path_);
      DT_LOG_TRACE(get_logging_priority(), "Done.");
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    void ihs::add_interface(const std::string & parent_path_,
                            const std::string & interface_name_)
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
      interface_node.set_parent_node(const_cast<node&>(parent_node));
      interface_node.set_ihs(*this);
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
        node * the_node = *inode;
        children_paths_.push_back(the_node->get_full_path());
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

    void ihs::tree_dump(std::ostream & out_,
                        const std::string & title_,
                        const std::string & indent_,
                        bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Nodes : [" << _nodes_.size() << ']' << std::endl;
      {
        int counter = 0;
        for (node_dict_type::const_iterator inode = _nodes_.begin();
             inode != _nodes_.end();
             inode++) {
          out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
          if (++counter == _nodes_.size()) {
            out_ << indent_ << i_tree_dumpable::last_tag;
          } else {
            out_ << indent_ << i_tree_dumpable::tag;
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
