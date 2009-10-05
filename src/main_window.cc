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
#include "glom_layout_model.h"
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
  glom_doc(document)
{
  setWindowTitle(qApp->applicationName());

  // The statusBar shows tooltips for menuitems.
  statusBar()->showMessage(tr("Qlom successfully started"));

  // Create the menu.
  // Qt does not have stock items, but it might get stock _icons_:
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
  QString table_name(index.data(Qlom::TableNameRole).toString());
  QMainWindow *table_model_window = new QMainWindow(this);
  QTableView *view = new QTableView(table_model_window);
  GlomLayoutModel *model = new GlomLayoutModel(glom_doc, table_name);

  view->setModel(model);
  view->resizeColumnsToContents();
  view->setItemDelegate(new QSqlRelationalDelegate(view));
  table_model_window->setCentralWidget(view);
  table_model_window->setWindowTitle(ustring_to_qstring(
    glom_doc.get_table_title(qstring_to_ustring(table_name))));

  table_model_window->show();
  table_model_window->raise();
  table_model_window->activateWindow();
  statusBar()->showMessage(tr("%1 table opened").arg(table_name));
}
