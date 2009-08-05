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

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeView>

#include <config.h>

MainWindow::MainWindow(const Glom::Document &document)
{
  setWindowTitle(PACKAGE_NAME);

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

  GlomModel *model = new GlomModel(document, this);

  QTreeView *central_treeview = new QTreeView(this);
  central_treeview->setModel(model);
  setCentralWidget(central_treeview);

  QObject::connect(central_treeview, SIGNAL(doubleClicked(const QModelIndex&)),
    this, SLOT(on_treeview_doubleclicked(const QModelIndex&)));

  read_settings();
}

MainWindow::~MainWindow()
{
  write_settings();
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
  std::cout << index.data().toString().toLocal8Bit().constData() <<
    " was double-clicked" << std::endl;
}
