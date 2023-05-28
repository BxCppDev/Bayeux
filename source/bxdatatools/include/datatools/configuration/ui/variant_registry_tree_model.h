/// \file datatools/configuration/ui/variant_registry_tree_model.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-06
 * Last modified : 2022-11-30
 *
 * Copyright (C) 2014-2022 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Qt-based tree model for a variant registry.
 */

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_TREE_MODEL_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_TREE_MODEL_H

// Standard library:
#include <memory>

// Third party:
// - Qt:
#include <QObject>
#include <QString>
#include <QList>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

// This project
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

namespace datatools {

  namespace configuration {

    // Forward declarations:
    class variant_record;
    class variant_registry;

    namespace ui {

      /// \brief Node/item of the tree model wrapping a variant registry record
      class tree_item : public QObject,
                        public datatools::i_tree_dumpable
      {
        Q_OBJECT

      public:

        /// \brief Index of the column for record rendering
        enum column_index {
          CI_KEY   = 0,  //!< The index of the 'Key' column
          CI_TYPE  = 1,  //!< The index of the 'Type' column
          CI_VALUE = 2,  //!< The index of the 'Value' column
          CI_DESCRIPTION = 3 //!< The index of the 'Description' column
        };

        /// Default constructor
        tree_item(variant_record & vr_, tree_item * parent_ = 0);

        /// Destructor
        virtual ~tree_item();

        /// Append a child item
        void append_child(tree_item * child_);

        /// Return a mutable child per row
        tree_item * grab_child(int row_);

        /// Return a non mutable child per row
        const tree_item * get_child(int row_) const;

        /// Return the number of children
        int get_child_count() const;

        /// Return the number of column
        int get_column_count() const;

        /// Return the mutable handled variant record
        variant_record & grab_record();

        /// Return the non mutable handled variant record
        const variant_record & get_record() const;

        /// Return the current row for this item
        int get_row() const;

        /// Check if the node has a parent
        bool has_parent() const;

        /// Return the mutable parent item
        tree_item * grab_parent();

        /// Return the non mutable parent item
        const tree_item * get_parent() const;

        /// Return the data stored at a given column
        /**
         *  @par column_ the column index (0: key, 1: type, 2: value)
         *  @return a QVariant hosting the value
         *
         */
        QVariant data(int column_) const;

        /// Set the data stored at a given column
        bool setData(int column_, const QVariant & value_);

        /// Unset the data stored at a given column
        bool unsetData(int column_);

        /// Set the parent tree item
        void set_parent(tree_item * parent_);

        /// Set the logging priority
        void set_logging(datatools::logger::priority p_);

        /// Return the logging priority
        datatools::logger::priority get_logging() const;

        /// Return the mutable list of children
        QList<tree_item *> & grab_children();

        /// Return the non mutable list of children
        const QList<tree_item *> & get_children() const;

        /// Smart print
        virtual void tree_dump(std::ostream & out = std::clog,
                               const std::string & title_ = "",
                               const std::string & indent_ = "",
                               bool inherit_ = false) const;

      private:

        logger::priority _logging_ = logger::priority::PRIO_ERROR;  //!< Logging priority
        variant_record *            _record_ = nullptr;   //!< Handled variant record
        tree_item *                 _parent_ = nullptr;   //!< Parent node/item
        QList<tree_item *>          _children_; //!< Collection of children nodes/items

        friend class variant_registry_tree_model;

      };

      /// \brief Qt tree model for configuration variant registry
      class variant_registry_tree_model : public QAbstractItemModel,
                                          public datatools::i_tree_dumpable
      {
        Q_OBJECT

      public:

        /// Return the default label for top variant
        static const std::string & default_top_label();

        /// Constructor
        variant_registry_tree_model(QObject * parent_ = nullptr);

        /// Destructor
        virtual ~variant_registry_tree_model();

        /// Check if the model has a root
        bool has_root() const;

        /// Set the root node
        void set_root(tree_item * node_);

        /// Return the root node/item
        tree_item * grab_root();

        /// Return the root node/item
        const tree_item * get_root() const;

        /// Return the model index associated to the root
        QModelIndex rootIndex() const;

        /// Return the model index associated to a given row, column and parent
        QModelIndex index(int row_,
                          int column_,
                          const QModelIndex &parent_) const;

        /// Return the parent model index associated to a child model index
        QModelIndex parent(const QModelIndex &child_) const;

        /// Return the flags associated to a model index
        Qt::ItemFlags flags(const QModelIndex &index_) const;

        /// Return the number of rows of a model index
        int rowCount(const QModelIndex &parent_) const;

        /// Return the number of columns of a model index
        int columnCount(const QModelIndex &parent_) const;

        /// Return the value of the data associated to a given model index and role
        QVariant data(const QModelIndex &index_, int role_) const;

        /// Set the value of the data associated to a given model index and role
        bool setData(const QModelIndex & index_, const QVariant & value_, int role_ = Qt::EditRole);

        /// Unset the value of the data associated to a given model index and role
        bool unsetData(const QModelIndex & index_, int role_ = Qt::EditRole);


        /// Return the value of the header data associated to a given role
        QVariant headerData(int section_,
                            Qt::Orientation orientation_,
                            int role_) const;

        /// Construct the model from a registry and a registry name
        void construct(variant_registry & registry_,
                       const std::string & registry_name_ = "");

        /// Set the name of the registry
        void set_registry_name(const std::string & registry_name_);

        // Return the name of the registry
        const std::string & get_registry_name() const;

        /// Set the logging priority
        void set_logging(datatools::logger::priority p_);

        /// Return the logging priority
        datatools::logger::priority get_logging() const;

        /// set the seal flag
        void set_sealed(bool);
        
        /// Check the sealed flag
        bool is_sealed() const;

        /// Check the read-only flag
        bool is_read_only() const;

        /// Set the read-only flag
        void set_read_only(bool);

        /// Check if a registry is handled
        bool has_registry() const;

        /// Return a reference to the mutable registry
        variant_registry & grab_registry();

        /// Return a reference to the non mutable registry
        const variant_registry & get_registry() const;

        /// Check if a restore buffer is available
        bool has_restore_buffer() const;

        /// Reset the restore buffer is available
        void reset_restore_buffer();

        /// Smart print
        virtual void tree_dump(std::ostream & out = std::clog,
                               const std::string & title_ = "",
                               const std::string & indent_ = "",
                               bool inherit_ = false) const;

        /// Return the node/item associated to a given model index
        tree_item * node_from_index(const QModelIndex & index_) const;

      public slots:

        /// Slot: broadcast data change
        void slot_broadcast_data_change(const QModelIndex &, const QModelIndex &);

        /// Slot: store the current status to a buffer
        void slot_store_to_restore_buffer(std::string * buffer_ = 0);

        /// Slot: load the current status from a buffer
        void slot_load_from_restore_buffer(const std::string * buffer_ = 0);

        /// Slot: store the current status to a file
        void slot_store_to_reg_file(const QString & out_reg_filename_) const;

        /// Slot: load the current status from a file
        void slot_load_from_reg_file(const QString & in_reg_filename_);

      signals:

        void sig_read_only_changed(bool);

        void sig_registry_name_changed();

        void sig_logging_changed();

        void sig_registry_changed(std::string);

        void sig_registry_data_changed(std::string, std::string);

      private:

        /// Construct a node from a record in a registry
        tree_item * _construct_node_(variant_registry & registry_, variant_record & record_);

        /// Return the node/item associated to a given model index
        tree_item * _node_from_index_(const QModelIndex & index_) const;

      private:

        logger::priority   _logging_ = logger::priority::PRIO_ERROR; //!< Logging priority
        bool               _sealed_ = false;     //!< Sealed flag
        bool               _read_only_ = true;     //!< Read-only flag
        std::string        _registry_name_; //!< Registry name
        variant_registry * _registry_ = nullptr;      //!< Handle to the registry of configuration variant
        tree_item *        _root_ = nullptr;          //!< Root node/item of the tree model
        std::unique_ptr<std::string> _restore_buffer_; //!< Handle to the restore buffer

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_TREE_MODEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
