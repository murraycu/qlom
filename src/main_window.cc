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
#include "glom_document.h"
#include "glom_tables_model.h"
#include "glom_layout_model.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QStatusBar>
#include <QTableView>
#include <QTreeView>

#include "config.h"

MainWindow::MainWindow()
{
  setup();
}

MainWindow::MainWindow(const QString &filepath)
{
    setup();

    glomDocument.loadDocument(filepath);
    GlomTablesModel *model = glomDocument.createTablesModel();
    centralTreeView->setModel(model);
    connect(centralTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(treeviewDoubleclicked(const QModelIndex&)));
}

void MainWindow::setup()
{
    setWindowTitle(qApp->applicationName());

    // The statusBar shows tooltips for menuitems.
    statusBar()->showMessage(tr("Qlom started successfully"));

    // Create the menu.
    QAction *fileOpen = new QAction(tr("&Open"), this);
    fileOpen->setShortcut(tr("Ctrl+O", "Open file"));
    fileOpen->setStatusTip(tr("Open a Glom document"));
    QAction *fileClose = new QAction(tr("&Close"), this);
    fileClose->setShortcut(tr("Ctrl+W", "Close file"));
    fileClose->setStatusTip(tr("Close the current Glom document"));
    QAction *fileQuit = new QAction(tr("&Quit"), this);
    fileQuit->setShortcut(tr("Ctrl+Q", "Quit application"));
    fileQuit->setStatusTip(tr("Quit the application"));
    QAction *helpAbout = new QAction(tr("About"), this);
    helpAbout->setShortcut(tr("Ctrl+A", "About application"));
    helpAbout->setStatusTip(
        tr("Display credits and license information for Qlom"));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileOpen);
    fileMenu->addAction(fileClose);
    fileMenu->addAction(fileQuit);
    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(helpAbout);

    QObject::connect(
        fileOpen, SIGNAL(triggered(bool)), this, SLOT(fileOpenTriggered()));
    QObject::connect(
        fileClose, SIGNAL(triggered(bool)), this, SLOT(fileCloseTriggered()));
    QObject::connect(
        fileQuit, SIGNAL(triggered(bool)), this, SLOT(fileQuitTriggered()));
    QObject::connect(
        helpAbout, SIGNAL(triggered(bool)), this, SLOT(helpAboutTriggered()));

    centralTreeView = new QTreeView(this);
    setCentralWidget(centralTreeView);

    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    QSqlDatabase::database().close();
}

void MainWindow::showAboutDialog()
{
    // About dialogs are window-modal in Qt, except on Mac OS X.
    QMessageBox::about(this, tr("About Qlom"), tr(PACKAGE_NAME "\n"
          "A Qt Glom database viewer\n"
          "Copyright 2009 Openismus GmbH"));
    /* lupdate does not recognise the above string, although if the string is
     * manually concatenated then it works fine. TODO: File bug. */
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("MainWindow/Size", size());
    settings.setValue("MainWindow/InternalProperties", saveState());
}

void MainWindow::readSettings()
{
    QSettings settings;
    resize(settings.value("MainWindow/Size", sizeHint()).toSize());
    restoreState(settings.value("MainWindow/InternalProperties").toByteArray());
}

void MainWindow::fileOpenTriggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("Glom document (*.glom)");
    if (dialog.exec()) {
        // Close the document before opening a document.
        fileCloseTriggered();

        QStringList files = dialog.selectedFiles();
        glomDocument.loadDocument(files.first());
        GlomTablesModel *model = glomDocument.createTablesModel();
        centralTreeView->setModel(model);
    }
}

void MainWindow::fileCloseTriggered()
{
    delete centralTreeView;
    centralTreeView = new QTreeView(this);
    setCentralWidget(centralTreeView);
    connect(centralTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(treeviewDoubleclicked(const QModelIndex&)));
}

void MainWindow::fileQuitTriggered()
{
    qApp->quit();
}

void MainWindow::helpAboutTriggered()
{
    showAboutDialog();
}

void MainWindow::treeviewDoubleclicked(const QModelIndex& index)
{
    const QString &tableName = index.data(Qlom::TableNameRole).toString();
    GlomLayoutModel *model = glomDocument.createListLayoutModel(tableName);
    QMainWindow *tableModelWindow = new QMainWindow(this);
    QTableView *view = new QTableView(tableModelWindow);

    view->setModel(model);
    view->resizeColumnsToContents();
    tableModelWindow->setCentralWidget(view);
    // TODO: Get correct display table name from GlomDocument.
    tableModelWindow->setWindowTitle(tableName);
    statusBar()->showMessage(tr("%1 table opened").arg(tableName));

    tableModelWindow->show();
    tableModelWindow->raise();
    tableModelWindow->activateWindow();
}
