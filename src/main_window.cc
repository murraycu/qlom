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
#include "qlom_error.h"
#include "glom_tables_model.h"
#include "glom_layout_model.h"
#include "glom_layout_delegates.h"

#include <memory>
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

MainWindow::MainWindow() :
    glomDocument(this)
{
  setup();
}

MainWindow::MainWindow(const QString &filepath) :
    glomDocument(this)
{
    setup();

    glomDocument.loadDocument(filepath);
    GlomTablesModel *model = glomDocument.createTablesModel();
    centralTreeView->setModel(model);

    connect(centralTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(treeviewDoubleclicked(QModelIndex)));
    show();
    // Open default table.
    showDefaultTable();
}

void MainWindow::receiveError(const QlomError &error)
{
    if(!error.what().isNull()) {
        QMessageBox dialog(this);
        dialog.setText(errorDomainLookup(error.domain()));
        dialog.setDetailedText(error.what());

        // Set icon style and dialog title according to error severity.
        switch (error.severity()) {
        case Qlom::CRITICAL_ERROR_SEVERITY:
            dialog.setWindowTitle(tr("Critical error"));
            dialog.setIcon(QMessageBox::Critical);
            break;
        case Qlom::WARNING_ERROR_SEVERITY:
            dialog.setWindowTitle(tr("Warning"));
            dialog.setIcon(QMessageBox::Warning);
            break;
        }

        dialog.exec();
    }

    // If the error message was non-empty then the error message was shown to the user, too.
    // What's left to do is to shutdown the application.
    if(Qlom::CRITICAL_ERROR_SEVERITY == error.severity()) {
        exit(EXIT_FAILURE);
    }
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

    connect(fileOpen, SIGNAL(triggered(bool)),
        this, SLOT(fileOpenTriggered()));
    connect(fileClose, SIGNAL(triggered(bool)),
        this, SLOT(fileCloseTriggered()));
    connect(fileQuit, SIGNAL(triggered(bool)),
        this, SLOT(fileQuitTriggered()));
    connect(helpAbout, SIGNAL(triggered(bool)),
        this, SLOT(helpAboutTriggered()));

    connect(&glomDocument.errorReporter(), SIGNAL(errorRaised(QlomError)),
        this, SLOT(receiveError(QlomError)));

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

QString MainWindow::errorDomainLookup(
    const Qlom::QlomErrorDomain errorDomain)
{
    switch (errorDomain) {
    case Qlom::DOCUMENT_ERROR_DOMAIN:
        return tr("An error occurred while reading the Glom Document");
        break;
    case Qlom::DATABASE_ERROR_DOMAIN:
        return tr("An error occurred with the database");
        break;
    default:
        qCritical("Unhandled error domain: %i", errorDomain);
        return tr("Unhandled error domain");
        break;
    }
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
        if(!glomDocument.loadDocument(files.first()))
        {
            return;
        }

        GlomTablesModel *model = glomDocument.createTablesModel();
        centralTreeView->setModel(model);
        // Open default table.
        showDefaultTable();
    }
}

void MainWindow::fileCloseTriggered()
{
    centralTreeView->deleteLater();
    centralTreeView = new QTreeView(this);
    setCentralWidget(centralTreeView);
    connect(centralTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(treeviewDoubleclicked(QModelIndex)));
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
    GlomLayoutModel *model =
        glomDocument.createListLayoutModel(tableName);
    showTable(model);

    const QString tableDisplayName = index.data().toString();
    statusBar()->showMessage(tr("%1 table opened").arg(tableDisplayName));
}

void MainWindow::showDefaultTable()
{
    GlomLayoutModel *model =
      glomDocument.createDefaultTableListLayoutModel();
    showTable(model);
    statusBar()->showMessage(tr("Default table opened"));
}

void MainWindow::showTable(GlomLayoutModel *model)
{
    QMainWindow *tableModelWindow = new QMainWindow(this);
    tableModelWindow->setAttribute(Qt::WA_DeleteOnClose);

    QTableView *view = new QTableView(tableModelWindow);
    model->setParent(tableModelWindow);

    // experimental code to see whether we can use delegates much the same way
    // as we would use cell renderers.
    // TODO: have a setupDelegates method that looks up the columns in a model
    // and installs the correct delegates for a table view.

    // tests trailing zeroes with the id column (usually 1st).
    view->setItemDelegateForColumn(0, new GlomNumericDelegate(tableModelWindow));

    // tests font formating with the 2nd & 3rd column.
    GlomTextDelegate *textDelegate = new GlomTextDelegate(tableModelWindow);
    view->setItemDelegateForColumn(1, textDelegate);
    view->setItemDelegateForColumn(2, textDelegate);

    view->setModel(model);
    view->resizeColumnsToContents();
    tableModelWindow->setCentralWidget(view);
    tableModelWindow->setWindowTitle(model->tableDisplayName());

    tableModelWindow->show();
    tableModelWindow->raise();
    tableModelWindow->activateWindow();
}
