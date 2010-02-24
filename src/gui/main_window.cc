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
#include "utils.h"

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
    tablesTreeView->setModel(model);

    connect(tablesTreeView, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(tablesTreeviewDoubleclicked(QModelIndex)));
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

    connect(&glomDocument.errorReporter(), SIGNAL(errorRaised(QlomError)),
        this, SLOT(receiveError(QlomError)));

    QWidget* notTheMainWidget = new QWidget(this);
    mainWidget = new QStackedWidget(notTheMainWidget);

    QWidget *tablesTreeWidget = new QWidget;
    tablesTreeView = new QTreeView(tablesTreeWidget);
    tablesTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainWidget->addWidget(tablesTreeWidget);

    QWidget *listLayoutWidget = new QWidget;
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listLayoutWidget->setSizePolicy(sizePolicy);
    QVBoxLayout *listLayoutLayout = new QVBoxLayout(listLayoutWidget);
    listLayoutBackButton = new QPushButton("Back to table list",
        listLayoutWidget);
    listLayoutLayout->addWidget(listLayoutBackButton);
    connect(listLayoutBackButton, SIGNAL(clicked(bool)),
        this, SLOT(showTablesList()));

    listLayoutView = new QlomListView(listLayoutWidget);
    listLayoutView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listLayoutLayout->addWidget(listLayoutView);

    /* Adding a widget does not change the current widget, iff the
     * QStackedWidget is not empty. */
    mainWidget->addWidget(listLayoutWidget);

    setCentralWidget(notTheMainWidget);
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
        tablesTreeView->setModel(model);
        // Open default table.
        showDefaultTable();
    }
    delete dialog;
}

void QlomMainWindow::fileCloseTriggered()
{
    tablesTreeView->deleteLater();
    tablesTreeView = new QTreeView(this);
    mainWidget->insertWidget(0, tablesTreeView);
    mainWidget->setCurrentIndex(0);
    setWindowTitle(qApp->applicationName());
    connect(tablesTreeView, SIGNAL(doubleClicked(QModelIndex)),
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
    mainWidget->setCurrentIndex(0);
}

void QlomMainWindow::tablesTreeviewDoubleclicked(const QModelIndex& index)
{
    const QString &tableName = index.data(Qlom::TableNameRole).toString();
    QlomListLayoutModel *model = glomDocument.createListLayoutModel(tableName);
    showTable(model);
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

    setWindowTitle(model->tableDisplayName());
    model->setParent(listLayoutView);
    listLayoutView->setModel(model);
    listLayoutView->resizeColumnsToContents();
    mainWidget->setCurrentIndex(1);

    // Marks model as "read-only" here, because the view has no way to edit it.
    listLayoutView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Setup delegates for all columns, if available.
    for(int index = 0; index < model->columnCount(); ++index) {
        listLayoutView->setupDelegateForColumn(index);
    }

    // Setup details button for last column.
    const int columnIndex = model->columnCount() - 1;
    //model->insertColumnAt(colIdx);
    model->setHeaderData(columnIndex, Qt::Horizontal, QVariant(tr("Actions")));
    QlomButtonDelegate *buttonDelegate =
        new QlomButtonDelegate(tr("Details"), listLayoutView);
    connect(buttonDelegate, SIGNAL(buttonPressed(QModelIndex)),
        this, SLOT(onDetailsPressed(QModelIndex)));
    listLayoutView->setItemDelegateForColumn(columnIndex, buttonDelegate);
}

void QlomMainWindow::onDetailsPressed(const QModelIndex &index)
{
    QMessageBox::critical(this, tr("Details button pressed"),
        tr("Cell index: (%1, %2)").arg(index.column()).arg(index.row()));
}

QlomListView::QlomListView(QWidget *parent)
: QTableView(parent), theLastColumnIndex(-1), theToggledFlag(false)
{
    connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
        this, SLOT(onHeaderSectionPressed(int)));
}

QlomListView::~QlomListView()
{}

void QlomListView::setupDelegateForColumn(int column)
{
    QlomListLayoutModel *model =
        qobject_cast<QlomListLayoutModel *>(this->model());

    if (model) {
        QStyledItemDelegate *delegate =
          QlomListView::createDelegateFromColumn(model, column);
        setItemDelegateForColumn(column, delegate);
    }
}

QStyledItemDelegate * QlomListView::createDelegateFromColumn(
    QlomListLayoutModel *model, int column)
{
    /* Need to respect the following constraint: The layout item in
     * theLayoutGroup that can be found at the position column points to has to
     * be a LayoutItem_Text or a LayoutItem_Field.
     * However, this method is not used efficiently, considering how most items
     * in a list view are field items. If LayoutItem_Text and LayoutItem_Field
     * had a common base clase featuring the get_formatting_used() API we could
     * get rid of the most annoying part at least: the dynamic casts. */
    const QlomListLayoutModel::GlomSharedLayoutItems items = model->getLayoutItems();
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
        items.begin(); iter != items.end(); ++iter) {
        if (column == std::distance(items.begin(), iter)) {
            Glom::sharedptr<const Glom::LayoutItem_Text> textItem =
                Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
            if(textItem)
                return new QlomLayoutItemTextDelegate(
                    textItem->get_formatting_used(),
                    QlomLayoutItemTextDelegate::GlomSharedField(),
                    ustringToQstring(textItem->get_text()));

            Glom::sharedptr<const Glom::LayoutItem_Field> fieldItem =
                Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
            if(fieldItem)
                return new QlomLayoutItemFieldDelegate(
                    fieldItem->get_formatting_used(),
                    fieldItem->get_full_field_details());
        }
    }

    return 0;
}

void QlomListView::onHeaderSectionPressed(int colIdx)
{
    Qt::SortOrder order = Qt::DescendingOrder;

    if (colIdx == theLastColumnIndex) {
        order = (theToggledFlag ? Qt::DescendingOrder : Qt::AscendingOrder);
        theToggledFlag = !theToggledFlag;
    } else if (-1 != theLastColumnIndex) {
        // Switching from another column => some arbitrary sorting is applied,
        // so we start with Qt::AscendingOrder again.
        order = Qt::AscendingOrder;
    }
    theLastColumnIndex = colIdx;

    // TODO: If this modifies the sorting in the model, we'd need a proxy model
    // here.
    sortByColumn(colIdx, order);
}
