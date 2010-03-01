/* Qlom is copyright Openismus GmbH, 2009, 2010.
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
#include "list_view.h"
#include "tables_model.h"
#include "utils.h"

#include <memory>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QtCore>
#include <QtGui>

#include "config.h"

QlomMainWindow::QlomMainWindow() :
    theGlomDocument(this),
    theMainWidget(0),
    theTablesTreeView(0),
    theListLayoutView(0),
    theTablesComboBox(0),
    theValidFlag(true)
{
  setup();
}

QlomMainWindow::QlomMainWindow(const QString &filepath) :
    theGlomDocument(this),
    theMainWidget(0),
    theTablesTreeView(0),
    theListLayoutView(0),
    theTablesComboBox(0),
    theValidFlag(true)
{
    setup();

    if(!theGlomDocument.loadDocument(filepath)) {
        theValidFlag = false;
        return;
    }

    QlomTablesModel *model = theGlomDocument.createTablesModel();
    theTablesTreeView->setModel(model);

    connect(theTablesTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(theTablesTreeviewDoubleclicked(QModelIndex)));
    show();

    // Open default table.
    showDefaultTable();
}

bool QlomMainWindow::isValid() const
{
    return theValidFlag;
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
    readSettings();
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

    connect(&theGlomDocument, SIGNAL(errorRaised(QlomError)),
        this, SLOT(receiveError(QlomError)));

    theMainWidget = new QStackedWidget(this);

    // Create page containing the treeview.
    theTablesTreeView = new QTreeView;
    theTablesTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    theTablesTreeView->setAlternatingRowColors(true);
    theMainWidget->addWidget(theTablesTreeView);

    // Create page containing the table and the navigation widget.
    QWidget *tableContainer = new QWidget;

    theListLayoutView = new QlomListView(tableContainer);
    theListLayoutView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    theListLayoutView->setAlternatingRowColors(true);
    theListLayoutView->setShowGrid(false);
    theListLayoutView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    theTablesComboBox = new QComboBox(tableContainer);
    connect(theTablesComboBox, SIGNAL(activated(int)),
        this, SLOT(showTableFromIndex(const int)));

    QPushButton *listLayoutBackButton =
      new QPushButton(tr("&Back to table list"), tableContainer);
    connect(listLayoutBackButton, SIGNAL(clicked(bool)),
        this, SLOT(showTablesList()));

    QWidget *navigationContainer = new QWidget;
    QHBoxLayout *navigationLayout = new QHBoxLayout(navigationContainer);
    navigationLayout->addWidget(theTablesComboBox, 1);
    navigationLayout->addWidget(listLayoutBackButton, 0, Qt::AlignRight);

    QVBoxLayout *tableLayout = new QVBoxLayout(tableContainer);
    tableLayout->addWidget(navigationContainer);
    tableLayout->addWidget(theListLayoutView);

    theMainWidget->addWidget(tableContainer);

    setCentralWidget(theMainWidget);
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
    const Qlom::QlomErrorDomain errorDomain) const
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
    // Close the document before opening a document.
    fileCloseTriggered();

    // TODO: check whether this can be replaced with
    // http://doc.trolltech.com/4.6/qfiledialog.html#getOpenFileName
    // Saves the ugly deletions.

    // Modal dialogs can be deleted by code elsewhere, hence this resource is
    // wrapped in a QPointer. However, it's only correct if *each* access to
    // this resource is 0-checked afterwards. See
    // http://www.kdedevelopers.org/node/3918#comment-8645
    QPointer<QFileDialog> dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setNameFilter("Glom document (*.glom)");

    if (dialog->exec()) {
        bool wasDocumentLoaded = false;

        if (dialog) {
            // TODO: If we already allow multi-selection, then what does the
            // user expect? That we open one, or all?
            QStringList files = dialog->selectedFiles();
            wasDocumentLoaded = theGlomDocument.loadDocument(files.first());
        }

        if (!wasDocumentLoaded) {
            delete dialog;
            return;
        }

        // Document was loaded:
        QlomTablesModel *model = theGlomDocument.createTablesModel();
        theTablesTreeView->setModel(model);
        theTablesComboBox->setModel(model);

        // Open default table.
        showDefaultTable();
    }

    delete dialog;
}

void QlomMainWindow::fileCloseTriggered()
{
    theTablesTreeView->deleteLater();
    theTablesTreeView = new QTreeView(this);
    theTablesTreeView->setAlternatingRowColors(true);
    theMainWidget->insertWidget(0, theTablesTreeView);
    theMainWidget->setCurrentIndex(0);
    setWindowTitle(qApp->applicationName());
    connect(theTablesTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(tablesTreeviewDoubleclicked(QModelIndex)));
}

void QlomMainWindow::fileQuitTriggered()
{
    qApp->quit();
}

void QlomMainWindow::helpAboutTriggered()
{
    showAboutDialog();
}

void QlomMainWindow::showTablesList()
{
    theMainWidget->setCurrentIndex(0);
}

void QlomMainWindow::tablesTreeviewDoubleclicked(const QModelIndex& index)
{
    const QString &tableName = index.data(Qlom::TableNameRole).toString();
    QlomListLayoutModel *model = theGlomDocument.createListLayoutModel(tableName);
    showTable(model);
}

void QlomMainWindow::showDefaultTable()
{
    // Show the default table, or the first non-hidden table, if there is one.
    QlomListLayoutModel *model =
      theGlomDocument.createDefaultTableListLayoutModel();

    if (model) {
        showTable(model);
    }
}

void QlomMainWindow::showTable(QlomListLayoutModel *model)
{
    Q_ASSERT(0 != model);

    const QString tableDisplayName(model->tableDisplayName());
    theTablesComboBox->setCurrentIndex(
        theTablesComboBox->findText(tableDisplayName));

    theListLayoutView->hide();
    model->setParent(theListLayoutView);
    theListLayoutView->setModel(model);
    //listLayoutView->resizeColumnsToContents();
    theListLayoutView->show();

    theMainWidget->setCurrentIndex(1);
    setWindowTitle(tableDisplayName);

    // Marks model as "read-only" here, because the view has no way to edit it.
    theListLayoutView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Setup delegates for all columns, if available.
    for(int index = 0; index < model->columnCount(); ++index) {
        theListLayoutView->setupDelegateForColumn(index);
    }

    // Setup details button for last column.
    const int columnIndex = model->columnCount() - 1;
    //model->insertColumnAt(colIdx);
    model->setHeaderData(columnIndex, Qt::Horizontal, QVariant(tr("Actions")));
    QlomButtonDelegate *buttonDelegate =
        new QlomButtonDelegate(tr("Details"), theListLayoutView);
    connect(buttonDelegate, SIGNAL(buttonPressed(QModelIndex)),
        this, SLOT(onDetailsPressed(QModelIndex)));
    theListLayoutView->setItemDelegateForColumn(columnIndex, buttonDelegate);
}

void QlomMainWindow::showTableFromIndex(const int index)
{
    QlomListLayoutModel *model =
        theGlomDocument.createListLayoutModel(
            theTablesTreeView->model()->index(index, 0)
                .data(Qlom::TableNameRole).toString());

    if (model) {
        showTable(model);
    }
}

void QlomMainWindow::onDetailsPressed(const QModelIndex &index)
{
    QMessageBox::critical(this, tr("Details button pressed"),
        tr("Cell index: (%1, %2)").arg(index.column()).arg(index.row()));
}
