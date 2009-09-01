/* Qlom is copyright Openismus GmbH, 2009
 *
 * This file is part of Qlom
 *
 * Qlom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Qlom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qlom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "main_window.h"
#include "glom_model.h"
#include "utils.h"

#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QStatusBar>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>

#include <config.h>

MainWindow::MainWindow(const Glom::Document &document) :
  glom_doc(document),
  table_model_opened(false)
{
  setWindowTitle(qApp->applicationName());

  // The statusBar shows tooltips for menuitems.
  statusBar();
  statusBar()->showMessage(tr("Qlom successfully started"));

  // Create the menu:
  // TODO: Doesn't Qt have some stock UI items? murrayc
  // Currently, no, although see:
  // http://labs.trolltech.com/blogs/2009/02/13/freedesktop-icons-in-qt/
  QAction *file_quit = new QAction(tr("&Quit"), this);
  file_quit->setShortcut(tr("Ctrl+Q"));
  file_quit->setStatusTip(tr("Quit the application"));
  QAction *help_about = new QAction(tr("About"), this);
  help_about->setShortcut(tr("Ctrl+A"));
  help_about->setStatusTip(
    tr("Display credits and license information for Qlom"));

  QMenu *file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(file_quit);
  QMenu *about_menu = menuBar()->addMenu(tr("&Help"));
  about_menu->addAction(help_about);

  QObject::connect(
    help_about, SIGNAL(triggered(bool)), this, SLOT(on_help_about_triggered()));
  QObject::connect(
    file_quit, SIGNAL(triggered(bool)), this, SLOT(on_file_quit_triggered()));

  GlomModel *model = new GlomModel(glom_doc, this);

  QTreeView *central_treeview = new QTreeView(this);
  central_treeview->setModel(model);
  setCentralWidget(central_treeview);

  table_model_dialog = new QDialog(this);

  connect(central_treeview, SIGNAL(doubleClicked(const QModelIndex&)),
    this, SLOT(on_treeview_doubleclicked(const QModelIndex&)));

  read_settings();
}

MainWindow::~MainWindow()
{
  write_settings();
  QSqlDatabase::database().close();
}

void MainWindow::show_about_dialog()
{
  // About dialogs are window-modal in Qt, except on Mac OS X.
  QMessageBox::about(this, tr("About Qlom"), tr(PACKAGE_NAME "\n"
    "A Qt Glom database viewer\n"
    "Copyright 2009 Openismus GmbH"));
}

void MainWindow::write_settings()
{
  QSettings settings;
  settings.setValue("MainWindow/Size", size());
  settings.setValue("MainWindow/InternalProperties", saveState());
}

void MainWindow::read_settings()
{
  QSettings settings;
  resize(settings.value("MainWindow/Size", sizeHint()).toSize());
  restoreState(settings.value("MainWindow/InternalProperties").toByteArray());
}

void MainWindow::on_file_quit_triggered()
{
  qApp->quit();
}

void MainWindow::on_help_about_triggered()
{
  show_about_dialog();
}

void MainWindow::on_treeview_doubleclicked(const QModelIndex& index)
{
  setup_table_view(index.data().toString());

  table_model_dialog->show();
  table_model_dialog->raise();
  table_model_dialog->activateWindow();
}

/* Create a new view and layout if the dialog has not been opened, otherwise
   re-use the existing dialog. */
void MainWindow::setup_table_view(QString table_name)
{
  QTableView *view = 0;
  QSqlRelationalTableModel *model = 0;

  if(!table_model_opened)
  {
    view = new QTableView(table_model_dialog);
    model = new QSqlRelationalTableModel();
    setup_table_model(model, table_name);

    view->setModel(model);
    view->setItemDelegate(new QSqlRelationalDelegate(view));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);
    table_model_dialog->setLayout(layout);
    table_model_opened = true;
  }
  else
  {
    view = table_model_dialog->findChild<QTableView*>();
    /* According to the QAbstractItemView documentation, if a parent widget is
       passed in, container ownership is used, so it is safe to set a new model
       and to not delete the old model.
       http://doc.trolltech.com/4.5/qabstractitemview.html#setModel */
    model = qobject_cast<QSqlRelationalTableModel*>(view->model());
    setup_table_model(model, table_name);
  }
  table_model_dialog->setWindowTitle(table_name);
}

void MainWindow::setup_table_model(QSqlRelationalTableModel *model, QString table_name)
{
  model->setTable(table_name);
  model->select();

  Glom::Document::type_vec_relationships relationships =
    glom_doc.get_relationships(qstring_to_ustring(table_name));
  for(Glom::Document::type_vec_relationships::const_iterator iter =
    relationships.begin(); iter != relationships.end(); ++iter)
  {
    const Glom::sharedptr<const Glom::Relationship> relationship = *iter;

    if(!relationship)
    {
      continue;
    }

    const QSqlRecord record = model->record();
    const QString from = ustring_to_qstring(relationship->get_from_field());
    const int index = record.indexOf(from);

    // If the index is invalid, or the primary key, ignore it.
    if(index == -1 || index == 0)
    {
      continue;
    }

    const QString to_table = ustring_to_qstring(relationship->get_to_table());
    const QString to_primary_key =
      ustring_to_qstring(relationship->get_to_field());
    /* TODO: Find a way to automatically retrieve a default field, rather than
             hardcoding the location. */
    const QString to_field = QString("name");
    model->setRelation(index,
      QSqlRelation(to_table, to_primary_key, to_field));
  }

  model->select();
  // Remove the primary key from the model.
  model->removeColumn(0);
}
