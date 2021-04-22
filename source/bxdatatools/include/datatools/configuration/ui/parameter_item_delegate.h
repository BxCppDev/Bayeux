/// \file datatools/configuration/ui/parameter_item_delegate.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-06
 * Last modified : 2016-11-15
 *
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Qt-based parameter item delegate for a tree view of a variant registry.
 *
 */

// http://qt-project.org/doc/qt-4.8/model-view-programming.html#delegate-classes
// http://qt-project.org/doc/qt-4.8/qstyleoptionviewitem.html

#ifndef DATATOOLS_CONFIGURATION_UI_PARAMETER_ITEM_DELEGATE_H
#define DATATOOLS_CONFIGURATION_UI_PARAMETER_ITEM_DELEGATE_H

// Third party:
// - Qt:
#include <QObject>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>

// Forward declaration
class QWidget;
class QModelIndex;
class QAbstractItemModel;
class QComboBox;

// This project:
#include <datatools/logger.h>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class parameter_model;
    class variant_record;
    class variant_repository;

    namespace ui {

      /// \brief Button to launch a real dialog widget
      class RealDialogLauncherButton : public QPushButton
      {
        Q_OBJECT

      public:

        /// Constructor
        RealDialogLauncherButton(const QModelIndex & index_,
                                 const QString & button_text_ = "Set value",
                                 QWidget * parent_ = 0 );
         /// Destructor
        virtual ~RealDialogLauncherButton();

        /// Return the unit value
        double get_unit_value() const;

        /// Return the selected value
        double get_value() const;

        /// Return the unit symbol
        const std::string & get_unit_symbol() const;

      public slots:

        void slot_launch_real_dialog();

        void slot_set_value();

      signals:

        void signal_launch_real_dialog(const QModelIndex & index_);

      public:

        datatools::logger::priority logging = datatools::logger::PRIO_FATAL; //!< Logging priority

      private:

        const QModelIndex *     _index_       = nullptr; //!< The model index of the real parameter tree node
        QInputDialog  *         _real_dialog_ = nullptr; //!< Input dialog used to choose the real value
        const variant_record *  _real_record_ = nullptr; //!< Handle to the real parameter record
        const parameter_model * _param_model_ = nullptr; //!< Handle to the associated parameter model
        std::string _unit_label_;   //!< The unit label
        std::string _unit_symbol_;  //!< The unit symbol
        double      _unit_value_;   //!< The unit value
        double      _value_;        //!< The selected real value

      };

      /// \brief Button to launch a file dialog widget
      class FileDialogLauncherButton : public QPushButton
      {
        Q_OBJECT

      public:

        /// Constructor
        FileDialogLauncherButton(const QModelIndex & index_,
                                 const QString & button_text_ = "Choose file path",
                                 QWidget * parent_ = 0 );
        /// Destructor
        virtual ~FileDialogLauncherButton();

        bool is_open() const;

        bool is_save() const;

        const QString & get_path() const;

      public slots:

        void slot_launch_file_dialog();

        void slot_set_path();

      signals:

        void signal_launch_file_dialog(const QModelIndex & index_);

      public:

        datatools::logger::priority logging = datatools::logger::PRIO_FATAL; //!< Logging priority

      private:

        QFileDialog::AcceptMode _accept_mode_;           //!< The accept mode
        const QModelIndex *     _index_ = nullptr;       //!< The model index of the path parameter tree node
        QFileDialog *           _path_dialog_ = nullptr; //!< File dialog used to choose the file path
        const variant_record *  _path_record_ = nullptr; //!< Handle to the path parameter record
        const parameter_model * _param_model_ = nullptr; //!< Handle to the parameter model
        QString                 _path_;                  //!< File path

      };

      /// \brief Item delegate for parameter value
      class parameter_item_delegate : public QStyledItemDelegate
      {

        Q_OBJECT

      public:

        /// Default constructor
        parameter_item_delegate(QObject * parent_ = 0);

        /// Desstructor
        virtual ~parameter_item_delegate();

        /// Check if the parent repository is set
        bool has_parent_repository() const;

        /// Set the parent repository
        void set_parent_repository(const datatools::configuration::variant_repository &);

        /// Return the parent repository
        const datatools::configuration::variant_repository & get_parent_repository() const;

        /// Check if dynamically enabled values is allowed
        bool allowed_dynamically_enabled_values() const;

        /// Create editor widget
        virtual QWidget *createEditor(QWidget * parent_,
                                      const QStyleOptionViewItem & option_,
                                      const QModelIndex & index_) const;

        /// Set the initial value in the editor at given index
        virtual void setEditorData(QWidget * editor_,
                                   const QModelIndex & index_) const;

        /// Set the initial value in the editor at given index
        void setEditorData2(QWidget * editor_,
                            const QModelIndex & index_) const;

        // Set the model data
        virtual void setModelData(QWidget * editor_,
                                  QAbstractItemModel * model_,
                                  const QModelIndex & index_) const;


        virtual void updateEditorGeometry(QWidget * editor_,
                                          const QStyleOptionViewItem & option_,
                                          const QModelIndex & index_) const;

        // QSize sizeHint(const QStyleOptionViewItem & option_,
        //                const QModelIndex & index_) const;

        // void paint(QPainter * painter_,
        //            const QStyleOptionViewItem & option_,
        //            const QModelIndex & index_) const;

        /// Set the logging priority
        void set_logging(datatools::logger::priority p_);

        /// Return the logging priority
        datatools::logger::priority get_logging() const;

        bool are_disabled_groups_hidden() const;

        void set_hide_disabled_groups(bool);
 
        bool are_disabled_values_hidden() const;

        void set_hide_disabled_values(bool);

        bool has_max_combo_rank() const;
        
        void set_max_combo_rank(int);
                                            

      private slots:

        QWidget * _create_boolean_editor(QWidget * parent_,
                                         const QStyleOptionViewItem & option_,
                                         const QModelIndex & index_,
                                         const variant_record & var_rec_) const;

        QWidget * _create_integer_editor(QWidget * parent_,
                                         const QStyleOptionViewItem & option_,
                                         const QModelIndex & index_,
                                         const variant_record & var_rec_) const;

        QWidget * _create_real_editor(QWidget * parent_,
                                      const QStyleOptionViewItem & option_,
                                      const QModelIndex & index_,
                                      const variant_record & var_rec_) const;

        QWidget * _create_string_editor(QWidget * parent_,
                                        const QStyleOptionViewItem & option_,
                                        const QModelIndex & index_,
                                        const variant_record & var_rec_) const;

        bool _set_boolean_editor_data(QWidget * editor_,
                                      const QModelIndex & index_,
                                      const variant_record & var_rec_) const;

        bool _set_integer_editor_data(QWidget * editor_,
                                      const QModelIndex & index_,
                                      const variant_record & var_rec_) const;

        bool _set_real_editor_data(QWidget * editor_,
                                   const QModelIndex & index_,
                                   const variant_record & var_rec_) const;

        bool _set_string_editor_data(QWidget * editor_,
                                     const QModelIndex & index_,
                                     const variant_record & var_rec_) const;

        // void _slot_path_edit_(const QModelIndex & index_);

      private:

        datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; //!< Logging priority
        const variant_repository *  _parent_repository_ = nullptr; //!< Handle to the parent repository
        // bool _hide_disabled_groups_ = false;
        // bool _hide_disabled_values_ = false;
        bool _hide_disabled_groups_ = true;
        bool _hide_disabled_values_ = true;
        int  _max_combo_rank_ = -1;

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_PARAMETER_ITEM_DELEGATE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
