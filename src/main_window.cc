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
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QStatusBar>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>

#include <config.h>

MainWindow::MainWindow()
{
  setup();
}

MainWindow::MainWindow(const QString &filepath)
{
    glomDocument.loadDocument(filepath);
    GlomTablesModel *model = glomDocument.tablesModel();
    setup();
    centralTreeView->setModel(model);
}

void MainWindow::setup()
{
    setWindowTitle(qApp->applicationName());

    // The statusBar shows tooltips for menuitems.
    statusBar()->showMessage(tr("Qlom successfully started"));

    // Create the menu.
    QAction *fileQuit = new QAction(tr("&Quit"), this);
    fileQuit->setShortcut(tr("Ctrl+Q"));
    fileQuit->setStatusTip(tr("Quit the application"));
    QAction *helpAbout = new QAction(tr("About"), this);
    helpAbout->setShortcut(tr("Ctrl+A"));
    helpAbout->setStatusTip(
        tr("Display credits and license information for Qlom"));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(fileQuit);
    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(helpAbout);

    QObject::connect(
        helpAbout, SIGNAL(triggered(bool)), this, SLOT(onHelpAboutTriggered()));
    QObject::connect(
        fileQuit, SIGNAL(triggered(bool)), this, SLOT(onFileQuitTriggered()));

    centralTreeView = new QTreeView(this);
    setCentralWidget(centralTreeView);

    connect(centralTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(onTreeviewDoubleclicked(const QModelIndex&)));

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

void MainWindow::onFileQuitTriggered()
{
    qApp->quit();
}

void MainWindow::onHelpAboutTriggered()
{
    showAboutDialog();
}

void MainWindow::onTreeviewDoubleclicked(const QModelIndex& index)
{
#if 0
    QString tableName(index.data(Qlom::TableNameRole).toString());
    QMainWindow *tableModelWindow = new QMainWindow(this);
    QTableView *view = new QTableView(tableModelWindow);

    /* Create a model for the rows of data. */
    GlomLayoutModel *model = new GlomLayoutModel(glomDoc, tableName);

    view->setModel(model);
    view->resizeColumnsToContents();
    tableModelWindow->setCentralWidget(view);
    tableModelWindow->setWindowTitle(ustringToQstring(
        glomDoc.get_table_title(qstringToUstring(tableName))));

    tableModelWindow->show();
    tableModelWindow->raise();
    tableModelWindow->activateWindow();
    statusBar()->showMessage(tr("%1 table opened").arg(tableName));
#endif
}
