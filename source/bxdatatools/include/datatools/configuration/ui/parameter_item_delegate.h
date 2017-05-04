/// \file datatools/configuration/ui/parameter_item_delegate.h
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
 *   Qt-based parameter item delegate for a tree view.
 *
 * History:
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

      private:

        const QModelIndex * _index_; //!< The model index of the real parameter tree node
        QInputDialog  * _real_dialog_; //!< Input dialog used to choose the real value
        const variant_record * _real_record_;  //!< Handle to the real parameter record
        const parameter_model * _param_model_; //!< Handle to the parameter model
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

      private:

        QFileDialog::AcceptMode _accept_mode_; //!< The accept mode
        const QModelIndex *     _index_;       //!< The model index of the path parameter tree node
        QFileDialog * _path_dialog_; //!< File dialog used to choose the file path
        const variant_record * _path_record_; //!< Handle to the path parameter record
        const parameter_model * _param_model_; //!< Handle to the parameter model
        QString _path_; //!< File path

      };

      /// \brief Item delegate for parameter value
      class parameter_item_delegate : public QStyledItemDelegate
      {

        Q_OBJECT

      public:

        /// Default constructor
        parameter_item_delegate(QObject *parent = 0);

        /// Desstructor
        virtual ~parameter_item_delegate();

        /// Create editor widget
        virtual QWidget *createEditor(QWidget * parent_,
                                      const QStyleOptionViewItem & option_,
                                      const QModelIndex & index_) const;

        /// Set the initial value in the editor at given index
        virtual void setEditorData(QWidget * editor_,
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

      private slots:

        // void _slot_path_edit_(const QModelIndex & index_);

      private:

       datatools::logger::priority  _logging_; //!< Logging priority

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_PARAMETER_ITEM_DELEGATE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
