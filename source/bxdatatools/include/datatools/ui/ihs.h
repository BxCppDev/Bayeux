//! \file  datatools/ui/ihs.h
//! \brief The interface hierachical system
//
// Copyright (c) 2015-2017 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_UI_IHS_H
#define DATATOOLS_UI_IHS_H

// Standard libraries:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <map>
#include <vector>
#include <set>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>

// This project:
#include <datatools/enriched_base.h>
#include <datatools/properties.h>
#include <datatools/ui/base_command_interface.h>
#include <datatools/ui/base_command.h>

namespace datatools {

  namespace ui {

    //! \brief Interface Hierarchy System
    class ihs
      : public datatools::enriched_base
    {
    public:

      //! \brief Node type
      enum node_type {
        NODE_INVALID   = -1, //!< Invalid node type
        NODE_INTERFACE =  0, //!< Interface node (directory populated)
        NODE_COMMAND   =  1  //!< Command node   (file/leaf)
      };

      //! Convert node type to label
      static std::string node_type_to_label(node_type);

      //! Convert label to node type
      static node_type label_to_node_type(const std::string & label_);

      //! Remove the leading scheme from the path
      //!
      //! Examples:
      //!   "/path/to/resource" ->  "/path/to/resource" (unchanged)
      //!   "foo:/path/to/resource" ->  "/path/to/resource"
      static void path_remove_scheme(const std::string & path_, std::string & out_);

      //! Check if a path has a leading scheme.
      //! If scheme is given, its specific value is checked.
      //!
      //! Examples:
      //!   "/path/to/resource"     -> has no scheme
      //!   "foo:/path/to/resource" -> has scheme "foo"
      //!   "bar:/path/to/resource" -> has scheme "bar"
      static bool path_has_scheme(const std::string & path_, const std::string & scheme_ = "");

      // Future: Replace with URL validation utility
      // //! Check if a path is valid
      // static bool path_validation(const std::string & path_);

      //! \brief A node in the IHS
      class node
      : public datatools::i_tree_dumpable
      {
      public:

        //! Default constructor
        node();

        //! Constructor
        node(node_type type_, const std::string & full_path_);

        //! Destructor
        ~node() override;

        //! Reset the node
        void reset();

        //! Set the node type
        void set_type(node_type type_);

        //! Return the node type
        node_type get_type() const;

        //! Check if node is the root
        bool is_root() const;

        //! Check if node is valid
        bool is_valid() const;

        //! Check if node is an interface node (directory)
        bool is_interface() const;

        //! Check if node is a command node (file)
        bool is_command() const;

        //! Return the mutable metadata
        datatools::properties & grab_metadata();

        //! Return the metadata
        const datatools::properties & get_metadata() const;

        //! Return the full path
        const std::string & get_full_path() const;

        //! Set the full path
        void set_full_path(const std::string &);

        //! Check if node has a description
        bool has_description() const;

        //! Set the description
        void set_description(const std::string &);

        //! Return the description
        const std::string & get_description() const;

        //! Check if node has a command interface set
        bool has_interface() const;

        //! Check if node has a command set
        bool has_command() const;

        //! Set the command interface
        void set_interface(base_command_interface *);

        //! Set the external command interface
        void set_external_interface(base_command_interface &);

        //! Reset interface
        void reset_interface();

        //! Set the command
        void set_command(base_command *);

        //! Reset command
        void reset_command();

        //! Set the external command
        void set_external_command(base_command &);

        //! Return the command interface
        base_command_interface & grab_interface();

        //! Return the command interface
        const base_command_interface & get_interface() const;

        //! Return the command
        base_command & grab_command();

        //! Return the command
        const base_command & get_command() const;

        //! Check if the parent node is set
        bool has_parent_node() const;

        //! Set parent node
        void set_parent_node(node & node_);

        //! Remove parent node
        void unset_parent_node();

        //! Return the parent node (if set)
        node & grab_parent_node();

        //! Check if the node is child of another node
        bool is_child_of(node & node_) const;

        //! Return the parent node (if set)
        const node & get_parent_node() const;

        //! Add a child node
        void add_child_node(node & node_);

        //! Remove a child node
        void remove_child_node(node & node_);

        //! Return the number of child nodes
        std::size_t get_number_of_children() const;

        //! Check if some children are set
        bool has_children() const;

        //! Set the host IHS
        void set_ihs(ihs &);

        //! Smart print
        void tree_dump(std::ostream & out_ = std::clog,
                               const std::string & title_  = "",
                               const std::string & indent_ = "",
                               bool inherit = false) const override;

        //! Set trait flag
        void set_trait(const std::string & trait_label_, const bool set_ = true);

        //! Reset trait flag
        void reset_trait(const std::string & trait_label_);

        //! Check if a trait id set
        bool is_trait(const std::string & trait_label_) const;

      private:

        ihs                    * _ihs_ = nullptr;           //!< Hosting IHS
        node_type                _type_;                    //!< Node type
        datatools::properties    _metadata_;                //!< Metadata associated to the node
        std::string              _full_path_;               //!< Full path
        std::string              _description_;             //!< Description
        bool                     _owned_interface_ = false; //!< Interface ownership flag
        base_command_interface * _interface_ = nullptr;     //!< Handle to an external command interface
        bool                     _owned_command_ = false;   //!< Command ownership flag
        base_command           * _command_ = nullptr;       //!< Handle to an external command
        node *                   _parent_node_;             //!< Handle to the parent node
        std::set<node *>         _children_;                //!< Collection of child nodes

        friend class ihs;

      };

      typedef std::map<std::string, node> node_dict_type;

      //! Default constructor
      ihs(const std::string & scheme_ = "");

      //! Destructor
      ~ihs() override;

      // //! Return the root path
      // const std::string & get_root_path() const;

      //! Check is scheme is set
      bool has_scheme() const;

      //! Set the scheme
      void set_scheme(const std::string &);

      //! Return the scheme
      const std::string & get_scheme() const;

      //! Add an interface given its parent's full path and its basename
      void add_interface(const std::string & parent_path_,
                         const std::string & interface_name_,
                         const std::string & description_ = "");

      //! Add a command interface given its parent's full path
      void add_interface(const std::string & parent_path_,
                         base_command_interface * interface_);

      //! Add a command interface given its parent's full path
      void add_interface(const std::string & parent_path_,
                         base_command_interface & interface_);

      //! Remove a command interface given its full path
      void remove_interface(const std::string & interface_path_, bool recursive_ = false);

      //! Add a command given its parent's full path
      void add_command(const std::string & parent_path_,
                       base_command & cmd_);

      //! Add a command given its parent's full path
      void add_command(const std::string & parent_path_,
                       base_command * cmd_);

      //! Remove a command given its full path
      void remove_command(const std::string & command_path_);

      //! Remove path
      void remove(const std::string & path_, bool recursive_ = false);

      //! Check if node exists given its full path
      bool exists(const std::string & path_) const;

      //! Check if a path is a command interface
      bool is_interface(const std::string & path_) const;

      //! Check if a path has an associated command interface
      bool has_interface(const std::string & path_) const;

      //! Check if a path is associated to a command interface
      bool is_command(const std::string & path_) const;

      //! Set trait flag
      void set_trait(const std::string & path_,
                     const std::string & trait_label_,
                     const bool set_ = true);

      //! Reset trait flag
      void reset_trait(const std::string & path_,
                       const std::string & trait_label_);

      //! Check if a trait id set
      bool is_trait(const std::string & path_,
                    const std::string & trait_label_) const;

      //! Check if a path has an associated command
      bool has_command(const std::string & path_) const;

      //! Return the mutable command at given full path
      base_command & grab_command(const std::string & path_);

      //! Return the command at given full path
      const base_command & get_command(const std::string & path_) const;

      //! Return the mutable command interface at given full path
      base_command_interface & grab_interface(const std::string & path_);

      //! Return the command interface at given full path
      const base_command_interface & get_interface(const std::string & path_) const;

      //! Check if path has children
      bool has_children(const std::string & path_) const;

      //! Build the list of children paths of a path
      void build_children_paths(const std::string & path_,
                                std::vector<std::string> & children_paths_) const;

      //! Return the parent path of a path
      std::string get_parent_path(const std::string & path_) const;

      //! Check if a given path has a child with given name
      bool has_child(const std::string & path_, const std::string & name_) const;

      //! Reset
      void reset();

      //! Build a canonical path :
      //! remove leading scheme if any and '//' path leading decorator if any
      std::string canonical_path(const std::string & path_) const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit = false) const override;

      //! Build a set of paths
      void build_path(std::set<std::string> & paths_, const uint32_t flags_ = 0) const;

    protected:

      //! Return the mutable node at given full path
      node & _grab_node(const std::string & path_);

      //! Return the node at given full path
      const node & _get_node(const std::string & path_) const;

    private:

      //! Set the scheme
      void _set_scheme_(const std::string &);

    private:

      std::string    _scheme_; //!< Optional scheme associated to the path format
      node_dict_type _nodes_;  //!< Dictionary of nodes in the IHS

    };

  } // namespace ui

} // namespace datatools

#endif //  DATATOOLS_UI_IHS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
