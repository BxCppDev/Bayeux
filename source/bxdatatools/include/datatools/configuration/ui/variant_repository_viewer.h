/// \file datatools/configuration/ui/variant_repository_viewer.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-19
 * Last modified : 2014-10-19
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

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_VIEWER_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_VIEWER_H

// Standard library:
#include <string>
#include <map>
#include <memory>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Qt:
#include <QWidget>
#include <QObject>

// Forward declarations:
class QLabel;
class QCheckBox;
class QPushButton;
class QComboBox;
class QTabWidget;
class QIcon;

// This project:
#include <datatools/logger.h>

namespace datatools {

  namespace qt {
    class led;
  }

  namespace configuration {

    // Forward declaration:
    class variant_registry;
    class variant_repository;

    namespace ui {

      // Forward declaration:
      class variant_registry_tree_model;
      class variant_registry_viewer;

      /// \brief A wrapper that handles a registry tree model and its associated viewer object
      class registry_model_wrapper
      {
      public:

        /// Default constructor
        registry_model_wrapper(QWidget * parent_ = 0);

        /// Destructor
        ~registry_model_wrapper();

        /// Reset
        void reset();

        /// Check model
        bool has_model() const;

        /// Return a non mutable reference to the model
        const variant_registry_tree_model & get_model() const;

        /// Return a mutable reference to the model
        variant_registry_tree_model & grab_model();

        /// Check viewer
        bool has_viewer() const;

        /// Return a non mutable reference to the viewer
        const variant_registry_viewer & get_viewer() const;

        /// Return a mutable reference to the viewer
        variant_registry_viewer & grab_viewer();

        /// Construct
        variant_registry_viewer * construct_registry_viewer(variant_registry & registry_);

      private:

        uint32_t                      _flags_;  //!< Flags
        QWidget *                     _parent_; //!< Parent widget
        variant_registry_tree_model * _model_;  //!< Handle to the embedded registry model
        variant_registry_viewer     * _viewer_; //!< Handle to the embedded registry viewer

      };

      /// \brief A widget to browse/edit a configuration variant registry
      class variant_repository_viewer : public QWidget
      {
        Q_OBJECT

      public:

        /// Dictionary of registry UI models
        typedef std::map<std::string, registry_model_wrapper> model_dict_type;

        /// Default constructor
        variant_repository_viewer(QWidget * parent_ = 0);

        /// Destructor
        virtual ~variant_repository_viewer();

        /// Set the logging priority
        void set_logging(datatools::logger::priority p_);

        /// Check if the viewer has a repository
        bool has_repository() const;

        /// Set the variant repository
        void set_repository(variant_repository & var_rep_);

        /// Return the mutable reference to the repository
        variant_repository & grab_repository();

        /// Return the non mutable reference to the repository
        const variant_repository & get_repository() const;

        /// Check if the read-only flag
        bool is_read_only() const;

        /// Set the read-only flag
        void set_read_only(bool);

        /// Check if a restore buffer is available
        bool has_restore_buffer() const;

        /// Reset the restore buffer is available
        void reset_restore_buffer();

      public slots:

        /// Slot: update the Read-only flag from the check box state
        void slot_update_read_only_from_check_state(int check_state_);

        /// Slot: update the Read-only check box state
        void slot_update_read_only_cb(bool read_only_);

        /// Slot: action at registry changed
        void slot_at_registry_changed(std::string changed_registry_);

        /// Slot: action at registry data changed
        void slot_at_registry_data_changed(std::string changed_registry_, std::string changed_data_path_);

        /// Slot: update registry tabs
        void slot_update_tabs();

        /// Slot: set read-only flag
        void slot_at_set_read_only(bool);

        /// Slot: update LEDs
        void slot_update_leds();

        /// Slot: general update
        void slot_update();

        /// Slot: dump
        void slot_dump() const;

        /// Slot: set the logging priority
        void slot_update_logging(const QString &);

        /// Slot: store the current status to a buffer
        void slot_restore(std::string * buffer_ = 0);

        /// Slot: load the current status from a buffer
        void slot_snapshot(std::string * buffer_ = 0);

        /// Slot: load the repository from a file
        void slot_load_from_file();

        /// Slot: store the repository in a file
        void slot_store_to_file();

      signals:

        void sig_read_only_changed(bool);

        void sig_repository_changed();

      protected:

        /// Construct the viewer
        void _construct();

        /// Construct the development part of the GUI
        void _construct_devel();

        /// Compute the registries activation
        void _compute_active_registries();

      private:

        bool _devel_mode_;                             //!< Development mode flag
        datatools::logger::priority _logging_;         //!< Logging priority
        variant_repository * _repository_;             //!< Handle to a variant repository
        bool                 _read_only_ = true;       //!< Read-only flag (default to true)
        model_dict_type _models_;                      //!< Dictionary of registry tree models
        QLabel *      _repository_name_title_label_;   //!< Label for the title of the repository name
        QLabel *      _repository_name_display_label_; //!< Display the repository name
        QLabel *      _repository_org_title_label_;    //!< Label for the title of the organization name
        QLabel *      _repository_org_display_label_;  //!< Display the organization name
        QLabel *      _repository_app_title_label_;    //!< Label for the title of the application name
        QLabel *      _repository_app_display_label_;  //!< Display the application name
        QLabel *      _accomplished_label_;       //!< Label for accomplished status
        datatools::qt::led * _accomplished_led_;  //!< LED for accomplished status
        QCheckBox *   _read_only_cb_;             //!< Check box for the read-only lock
        QTabWidget *  _registry_tabs_;            //!< Tabs for registry
        std::map<std::string, int> _tab_indexes_; //!< Tab indexes
        QPushButton * _dump_button_;              //!< Button for dump
        QLabel *      _logging_label_;            //!< Label for logging
        QComboBox *   _logging_combo_;            //!< Combo to select the logging prority
        QIcon *       _unaccomplished_icon_;      //!< Unaccomplished icon
        QIcon *       _null_icon_;                //!< Null icon
        std::unique_ptr<std::string> _restore_buffer_; //!< Handle to the restore buffer

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_VIEWER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
