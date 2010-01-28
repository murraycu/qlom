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
#include "document.h"
#include "error.h"
#include "tables_model.h"
#include "list_layout_model.h"
#include "layout_delegates.h"

#include <memory>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QtCore>
#include <QtGui>

#include "config.h"

QlomMainWindow::QlomMainWindow() :
    glomDocument(this),
    valid(true)
{
  setup();
}

QlomMainWindow::QlomMainWindow(const QString &filepath) :
    glomDocument(this),
    valid(true)
{
    setup();

    if(!glomDocument.loadDocument(filepath)) {
        valid = false;
        return;
    }

    QlomTablesModel *model = glomDocument.createTablesModel();
    centralTreeView->setModel(model);

    connect(centralTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(treeviewDoubleclicked(QModelIndex)));
    show();

    // Open default table.
    showDefaultTable();
}

bool QlomMainWindow::isValid() const
{
    return valid;
}

void QlomMainWindow::receiveError(const QlomError &error)
{
    if(!error.what().isNull()) {
        QPointer<QMessageBox> dialog = new QMessageBox(this);
        dialog->setText(errorDomainLookup(error.domain()));
        dialog->setDetailedText(error.what());

        // Set icon style and dialog title according to error severity.
        switch (error.severity()) {
        case Qlom::CRITICAL_ERROR_SEVERITY:
            dialog->setWindowTitle(tr("Critical error"));
            dialog->setIcon(QMessageBox::Critical);
            break;
        case Qlom::WARNING_ERROR_SEVERITY:
            dialog->setWindowTitle(tr("Warning"));
            dialog->setIcon(QMessageBox::Warning);
            break;
        }

        dialog->exec();
        delete dialog;
    }

    // TODO: Whether to shut down the application should be for the caller to 
    // decide. murrayc.
    // QApplication::exit() does not work for us because libglom eats up the
    // important signals:
    // http://git.gnome.org/browse/glom/tree/glom/libglom/connectionpool.cc#n538
    /* If the error message was non-empty then the error message was shown to
     * the user too. All that remains is to shut down the application. */
    if(Qlom::CRITICAL_ERROR_SEVERITY == error.severity()) {
        exit(EXIT_FAILURE);
    }
}

void QlomMainWindow::setup()
{
    setWindowTitle(qApp->applicationName());

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
    centralTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setCentralWidget(centralTreeView);

    readSettings();
}

QlomMainWindow::~QlomMainWindow()
{
    writeSettings();
    QSqlDatabase::database().close();
}

void QlomMainWindow::showAboutDialog()
{
    // About dialogs are window-modal in Qt, except on Mac OS X.
    QMessageBox::about(this, tr("About Qlom"), tr(PACKAGE_NAME "\n"
          "A Qt Glom database viewer\n"
          "Copyright 2009 Openismus GmbH"));
    /* lupdate does not recognise the above string, although if the string is
     * manually concatenated then it works fine. TODO: File bug. */
}

void QlomMainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("MainWindow/Size", size());
    settings.setValue("MainWindow/InternalProperties", saveState());
}

void QlomMainWindow::readSettings()
{
    QSettings settings;
    resize(settings.value("MainWindow/Size", sizeHint()).toSize());
    restoreState(settings.value("MainWindow/InternalProperties").toByteArray());
}

QString QlomMainWindow::errorDomainLookup(
    const Qlom::QlomErrorDomain errorDomain)
{
    switch (errorDomain) {
    case Qlom::DOCUMENT_ERROR_DOMAIN:
        return tr("An error occurred while reading the Glom Document");
        break;
    case Qlom::DATABASE_ERROR_DOMAIN:
        return tr("An error occurred with the database");
        break;
    case Qlom::LOGIC_ERROR_DOMAIN:
        return tr("The programmmer had a logic error.");
        break;
    default:
        qCritical("Unhandled error domain: %i", errorDomain);
        return tr("Unhandled error domain");
        break;
    }
}

void QlomMainWindow::fileOpenTriggered()
{
    QPointer<QFileDialog> dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setNameFilter("Glom document (*.glom)");
    if (dialog->exec()) {
        // Close the document before opening a document.
        fileCloseTriggered();

        QStringList files = dialog->selectedFiles();
        if(!glomDocument.loadDocument(files.first()))
        {
            delete dialog;
            return;
        }

        QlomTablesModel *model = glomDocument.createTablesModel();
        centralTreeView->setModel(model);
        // Open default table.
        showDefaultTable();
    }
    delete dialog;
}

void QlomMainWindow::fileCloseTriggered()
{
    centralTreeView->deleteLater();
    centralTreeView = new QTreeView(this);
    setCentralWidget(centralTreeView);
    connect(centralTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(treeviewDoubleclicked(QModelIndex)));
}

void QlomMainWindow::fileQuitTriggered()
{
    qApp->quit();
}

void QlomMainWindow::helpAboutTriggered()
{
    showAboutDialog();
}

void QlomMainWindow::treeviewDoubleclicked(const QModelIndex& index)
{
    const QString &tableName = index.data(Qlom::TableNameRole).toString();
    QlomListLayoutModel *model = glomDocument.createListLayoutModel(tableName);
    showTable(model);

    const QString tableDisplayName = index.data().toString();
}

void QlomMainWindow::showDefaultTable()
{
    // Show the default table, or the first non-hidden table, if there is one.
    QlomListLayoutModel *model =
      glomDocument.createDefaultTableListLayoutModel();

    if (model) {
        showTable(model);
    }
}

void QlomMainWindow::showTable(QlomListLayoutModel *model)
{
    Q_ASSERT(0 != model);

    QMainWindow *tableModelWindow = new QMainWindow;
    QTableView *view = new QTableView(tableModelWindow);

    tableModelWindow->setAttribute(Qt::WA_DeleteOnClose);
    tableModelWindow->setCentralWidget(view);
    tableModelWindow->setWindowTitle(model->tableDisplayName());
    tableModelWindow->show();
    tableModelWindow->raise();
    tableModelWindow->activateWindow();

    model->setParent(view);
    view->setModel(model);

    // Marks model as "read-only" here, because the view has no way to edit it.
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Setup delegates for all columns, if available.
    for(int idx = 0; idx < model->columnCount(); ++idx) {
        view->setItemDelegateForColumn(idx, model->createDelegateFromColumn(idx));
    }

    // Setup edit button for last column.
    const int colIdx = model->columnCount();
    model->insertColumnAt(colIdx);
    model->setHeaderData(colIdx, Qt::Horizontal, QVariant(tr("Actions")));
    view->setItemDelegateForColumn(colIdx, new QlomButtonDelegate(tr("Details"), view));

    view->resizeColumnsToContents();
}
