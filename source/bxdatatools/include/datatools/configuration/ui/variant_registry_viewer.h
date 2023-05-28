/// \file datatools/configuration/ui/variant_registry_viewer.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-06
 * Last modified : 2014-10-06
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A viewer/editor widget for configuration variant registry.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_VIEWER_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_VIEWER_H

// Standard library:
#include <string>

// Third party:
// - Qt:
#include <QWidget>
#include <QObject>

// This project:
#include <datatools/logger.h>

// Forward declarations:
class QLabel;
class QTreeView;
class QCheckBox;
class QPushButton;
class QComboBox;
class QModelIndex;

namespace datatools {

  namespace qt {

    // Forward declaration:
    class led;

  }

  namespace configuration {

    class variant_registry;

    namespace ui {

      // Forward declarations:
      class variant_registry_tree_model;
      class parameter_item_delegate;

      /// \brief A widget to browse/edit a configuration variant registry
      class variant_registry_viewer : public QWidget
      {
        Q_OBJECT

      public:

        /// Default constructor
        variant_registry_viewer(QWidget * parent_ = nullptr);

        /// Destructor
        virtual ~variant_registry_viewer();

        /// Set the registry tree model
        void set_registry_tree_model(variant_registry_tree_model &, bool devel_mode_ = false);

        /// Check if a registry tree model is handled
        bool has_registry_tree_model() const;

        /// Return a reference to the mutable registry tree model
        variant_registry_tree_model & grab_registry_tree_model();

        /// Return a reference to the non mutable registry tree model
        const variant_registry_tree_model & get_registry_tree_model() const;

        /// Return a reference to the non mutable registry
        const datatools::configuration::variant_registry & get_registry() const;

        /// Set the logging priority
        void set_logging(datatools::logger::priority p_);

        /// Return the ISC flag
        bool is_inhibit_secondary_choices() const;

        /// Set the inhibit secondary choices flag
        void set_inhibit_secondary_choices(bool isc_);
 
      public slots:

        /// Slot: set the checkbox state from the model read-only flag
        void slot_update_read_only_cb(bool read_only_);

        /// Slot: set the checkbox state from the inhibit combo secondaries flag
        void slot_update_isc_cb(bool isc_);

        /// Slot: set the checkbox state from the inhibit combo secondaries flag
        void slot_update_isc_delegate(bool isc_);

        /// Slot: set the checkbox state from the inhibit combo secondaries flag
        void slot_update_isc(bool isc_);

        /// Slot: set the model read-only flag from the checkbox state
        void slot_update_model_read_only_from_check_state(int check_state_);

        /// Slot: set the inhibit combo secondaries flag from the checkbox state
        void slot_update_isc_from_check_state(int check_state_);

        /// Slot: dump the registry
        void slot_dump_registry();

        /// Slot: dump the registry model
        void slot_dump_model();

        /// Slot: set the logging priority
        void slot_update_logging(const QString &);

        /// Slot: update the logging priority
        void slot_update_logging_combo(datatools::logger::priority);

        /// Slot: adapt the view
        void slot_expand_all();

        /// Slot: adapt the view
        void slot_expand_only_active();

        /// Slot: adapt the view
        void slot_compute_expand(const QModelIndex & index_);

        /// Slot: adapt the view
        void slot_compute_expand2(const QModelIndex & top_left_index_,
                                  const QModelIndex & bottom_right_index_);

        /// Slot: adapt the view
        void slot_collapse_all();

        /// Slot: adapt the view
        void slot_update_leds();

        /// Slot: store the current status to a buffer
        void slot_snapshot();

        /// Slot: load the current status from a buffer
        void slot_restore();

        /// Slot: load the registry from a file
        void slot_load_from_file();

        /// Slot: store the registry in a file
        void slot_store_to_file() const;

      signals:

        /// Signal: emitted when the model has changed
        void sig_model_changed();

        /// Signal: emitted when the logging has changed
        void sig_logging_changed(datatools::logger::priority);

        // /// Signal: emitted when the read-only state has changed
        // void sig_read_only_changed(bool);

        /// Signal: emitted when the inhibit secondary choices state has changed
        void sig_inhibit_secondary_choices_changed(bool);

      protected:

        /// Construct the viewer
        void _construct();

        /// Construct the development part of the GUI
        void _construct_devel();

      private:

        bool _devel_mode_ = false; //!< Development mode flag
        bool _inhibit_secondary_choices_ = false;
        datatools::logger::priority   _logging_ = datatools::logger::PRIO_FATAL;     //!< Logging priority
        variant_registry_tree_model * _registry_tree_model_ = nullptr; //!< The variant registry tree model
        QLabel *      _registry_name_title_label_ = nullptr;   //!< Label for the title of the registry name
        QLabel *      _registry_name_display_label_ = nullptr; //!< Display the registry name
        QLabel *      _accomplished_label_ = nullptr;          //!< Label for accomplished status
        datatools::qt::led * _accomplished_led_ = nullptr;     //!< LED for accomplished status
        QCheckBox *   _read_only_cb_ = nullptr;                //!< Check box for the read-only lock
        QCheckBox *   _inhibit_secondary_choices_cb_ = nullptr;  //!< Check box for combo secondaries inhibition
        QTreeView *   _tree_view_ = nullptr;                   //!< Tree view
        parameter_item_delegate * _value_delegate_ = nullptr;  //!< Item delegate for parameters' values
        QPushButton * _expand_all_button_ = nullptr;           //!< Button for expand all items
        QPushButton * _expand_only_active_button_ = nullptr;   //!< Button for expand only all active items
        QPushButton * _collapse_all_button_ = nullptr;         //!< Button for expand all items
        QPushButton * _dump_button_ = nullptr;                 //!< Button for dumping the registry
        QLabel *      _logging_label_ = nullptr;  //!< Label for logging
        QComboBox *   _logging_combo_ = nullptr;  //!< Combo to select the logging prority

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_VIEWER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
