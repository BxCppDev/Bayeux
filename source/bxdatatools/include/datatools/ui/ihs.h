//! \file  datatools/ui/ihs.h
//! \brief The interface hierachical system
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
    class ihs : public enriched_base
    {
    public:

      //! \brief Node type
      enum node_type {
        NODE_INVALID   = -1, //!< Invalid node type
        NODE_INTERFACE =  0, //!< Interface node
        NODE_COMMAND   =  1  //!< Command node
      };

      //! Convert node type to label
      static std::string node_type_to_label(node_type);

      //! Convert label to node type
      static node_type label_to_node_type(const std::string & label_);

      //! \brief A node in the IHS
      class node : public i_tree_dumpable
      {
      public:

        //! Default constructor
        node();

        //! Constructor
        node(node_type type_, const std::string & full_path_);

        //! Destructor
        virtual ~node();

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

        //! Check if node has an interface set
        bool has_interface() const;

        //! Check if node is a command node (file)
        bool is_command() const;

        //! Check if node has a command set
        bool has_command() const;

        //! Return the mutable metadata
        datatools::properties & grab_metadata();

        //! Return the metadata
        const datatools::properties & get_metadata() const;

        //! Return the full path
        const std::string & get_full_path() const;

        //! Set the full path
        void set_full_path(const std::string &);

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
        virtual void tree_dump(std::ostream & out_ = std::clog,
                               const std::string & title_  = "",
                               const std::string & indent_ = "",
                               bool inherit = false) const;

      private:

        ihs                    * _ihs_;  //!< Host IHS
        node_type                _type_; //!< Node type
        datatools::properties    _metadata_; //!< Metadata associated to the node
        std::string              _full_path_; //!< Full path
        bool                     _owned_interface_ = false; //!< Interface ownership flag
        base_command_interface * _interface_; //!< Handle to an external command interface
        bool                     _owned_command_ = false; //!< Command ownership flag
        base_command           * _command_;   //!< Handle to an external command
        node *                   _parent_node_; //!< Handle to the parent node
        std::set<node *>         _children_; //! Collection of child nodes

        friend class ihs;

      };

      typedef std::map<std::string, node> node_dict_type;

      //! Default constructor
      ihs();

      //! Destructor
      virtual ~ihs();

      //! Add a command interface given its parent's full path and its basename
      void add_interface(const std::string & parent_path_,
                         const std::string & interface_name_);

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

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit = false) const;

    protected:

      //! Return the mutable node at given full path
      node & _grab_node(const std::string & path_);

      //! Return the node at given full path
      const node & _get_node(const std::string & path_) const;

    private:

      node_dict_type _nodes_;

    };

  } // namespace ui

} // namespace datatools

#endif //  DATATOOLS_UI_IHS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
