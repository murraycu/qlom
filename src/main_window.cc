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

#include <libglom/document/document.h>
#include <libglom/init.h>

#include <config.h>

MainWindow::MainWindow()
{
  setWindowTitle(PACKAGE_NAME);

  statusBar();
  statusBar()->showMessage(tr("Qlom successfully started"));

  QToolBar *main_toolbar = addToolBar(tr("Main Toolbar"));
  main_toolbar->setObjectName("MainToolbar");

  QAction *file_quit = new QAction(tr("&Quit"), this);
  file_quit->setShortcut(tr("Ctrl+Q"));
  file_quit->setStatusTip(tr("Quit the application"));
  QAction *help_about = new QAction(tr("About"), this);
  help_about->setShortcut(tr("Ctrl+A"));
  help_about->setStatusTip(
    tr("Display credits and license information for Qlom"));

  QMenu *file_menu = menuBar()->addMenu(tr("&File"));
  file_menu->addAction(file_quit);
  QMenu *view_menu = menuBar()->addMenu(tr("&View"));
  view_menu->addAction(main_toolbar->toggleViewAction());
  QMenu *about_menu = menuBar()->addMenu(tr("&Help"));
  about_menu->addAction(help_about);

  main_toolbar->addAction(file_quit);

  QObject::connect(
    help_about, SIGNAL(triggered(bool)), this, SLOT(on_help_about_triggered()));
  QObject::connect(
    file_quit, SIGNAL(triggered(bool)), this, SLOT(on_file_quit_triggered()));

  Glom::libglom_init();

  Glib::ustring uri;
  try
  {
    uri = Glib::filename_to_uri("/opt/gnome2/share/glom/doc/examples/example_music_collection.glom");
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "Exception from Glib::filename_to_uri(): " << ex.what();
    return;
  }

  Glom::Document document;
  document.set_file_uri(uri);
  int failure_code = 0;
  const bool test = document.load(failure_code);
  if(!test)
  {
    return;
  }
  GlomModel *model = new GlomModel(document, this);

  QTreeView *central_treeview = new QTreeView(this);
  central_treeview->setModel(model);
  setCentralWidget(central_treeview);

  read_settings();
}

MainWindow::~MainWindow()
{
  write_settings();
}

void MainWindow::show_about_dialog()
{
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
