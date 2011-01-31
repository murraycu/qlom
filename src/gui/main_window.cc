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
        this, SLOT(onTablesTreeviewDoubleclicked(QModelIndex)));
    show();

    // Open default table.
    showDefaultTable();
}

bool QlomMainWindow::isValid() const
{
    return theValidFlag;
}

void QlomMainWindow::showError(const QlomError &error)
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
        this, SLOT(onFileOpenTriggered()));
    connect(fileClose, SIGNAL(triggered(bool)),
        this, SLOT(onFileCloseTriggered()));
    connect(fileQuit, SIGNAL(triggered(bool)),
        this, SLOT(onFileQuitTriggered()));
    connect(helpAbout, SIGNAL(triggered(bool)),
        this, SLOT(onHelpAboutTriggered()));

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
        this, SLOT(onTablesComboActivated(const int)));

    QPushButton *listLayoutBackButton =
      new QPushButton(tr("&Back to table list"), tableContainer);
    connect(listLayoutBackButton, SIGNAL(clicked(bool)),
        this, SLOT(onBackButton()));

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
    QMessageBox::about(this, tr("About Qlom"),
          tr(PACKAGE_NAME " " PACKAGE_VERSION "\n"
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
        qWarning("Unhandled error domain: %i", errorDomain);
        return tr("Unhandled error domain");
        break;
    }
}

void QlomMainWindow::onFileOpenTriggered()
{
    // Close the document before opening a document.
    onFileCloseTriggered();

    /* Modal dialogs can be deleted by code elsewhere, hence this resource is
     * wrapped in a QPointer. However, it's only correct if *each* access to
     * this resource is 0-checked afterwards. See
     * http://www.kdedevelopers.org/node/3918#comment-8645 */
    QPointer<QFileDialog> dialog = new QFileDialog(this);
    // Only allow opening a single, existing file.
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setNameFilter("Glom document (*.glom)");

    if (dialog->exec()) {
        bool wasDocumentLoaded = false;

        if (dialog) {
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

void QlomMainWindow::onFileCloseTriggered()
{
    theTablesTreeView->deleteLater();
    theTablesTreeView = new QTreeView(this);
    theTablesTreeView->setAlternatingRowColors(true);
    theMainWidget->insertWidget(0, theTablesTreeView);
    theMainWidget->setCurrentIndex(0);
    setWindowTitle(qApp->applicationName());
    connect(theTablesTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(onTablesTreeviewDoubleclicked(QModelIndex)));
}

void QlomMainWindow::onFileQuitTriggered()
{
    qApp->quit();
}

void QlomMainWindow::onHelpAboutTriggered()
{
    showAboutDialog();
}

void QlomMainWindow::onBackButton()
{
    theMainWidget->setCurrentIndex(0);
}

void QlomMainWindow::onTablesTreeviewDoubleclicked(const QModelIndex& index)
{
    const QString &tableName = index.data(Qlom::TableNameRole).toString();
    QlomListLayoutModel *model = theGlomDocument.createListLayoutModel(tableName);
    if (model) {
        showTable(model);
    } else {
        const QlomError error = theGlomDocument.lastError();
        showError(error);
        if (error.severity() == Qlom::CRITICAL_ERROR_SEVERITY) {
            exit(EXIT_FAILURE);
        }
    }
}

void QlomMainWindow::showDefaultTable()
{
    // Show the default table, or the first non-hidden table, if there is one.
    QlomListLayoutModel *model =
      theGlomDocument.createDefaultTableListLayoutModel();

    if (model) {
        showTable(model);
    } else {
        const QlomError error = theGlomDocument.lastError();
        showError(error);
        if (error.severity() == Qlom::CRITICAL_ERROR_SEVERITY) {
            exit(EXIT_FAILURE);
        }
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

void QlomMainWindow::onTablesComboActivated(const int index)
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
