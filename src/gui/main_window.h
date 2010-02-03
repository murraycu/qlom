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

#ifndef QLOM_MAIN_WINDOW_H_
#define QLOM_MAIN_WINDOW_H_

#include "document.h"
#include "error_reporter.h"
#include "layout_delegates.h"
#include "list_layout_model.h"

#include <QMainWindow>
#include <QTableView>

class QlomListLayoutModel;
class QModelIndex;
class QTreeView;
class QlomModelIndexObject;

/** The main window is the central controller and view for Qlom.
 *  The main window both shows the main window and manages the Glom document,
 *  through the glomDocument member. Errors from QlomDocument are reported as
 *  a QlomError to the receiveError slot. From there, the error domain is
 *  looked up in the errorDomainLookup() method and a dialog is shown to the
 *  user. glomDocument has a QlomErrorReporter member, which is the only source
 *  of QlomError signals in Qlom. */
class QlomMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /** Application main window.
     *  Creates a main window with no Glom document loaded. */
    QlomMainWindow();

    /** Application main window.
     *  Creates a main window with a Glom document found at the filepath
     *  parameter.
     *  @param[in] filepath an absolute file path to a Glom document */
    explicit QlomMainWindow(const QString &filepath);

    virtual ~QlomMainWindow();

    // TODO: Should/Can the ErrorReporter system be used instead to tell 
    // the caller that MainWindow() failed? 
    /** Discover whether the constructor was successful.
     * This method exists because we do not want to use exceptions.
     * @returns true if the constructor was successful.
     */
    bool isValid() const;

public Q_SLOTS:
    /** Receive errors from ErrorReporter.
     *  QlomDocument, via the member glomDocument, is the only reporter of
     *  errors to the main window. The error is checked for content, with the
     *  error domain being looked up by the errorDomainLookup() method. A
     *  generic error message for the error domain is shown in a dialog, and
     *  the detailed error message is shown in the initially-hidden details
     *  section.
     *  @param error the error */
    void receiveError(const QlomError &error);

private:
    /** General setup method. */
    void setup();

    /** Show the application about dialog. */
    void showAboutDialog();

    /** Write the application settings to disk. */
    void writeSettings();

    /** Read the application settings from disk. */
    void readSettings();

    /** Show a list layout of the default table. */
    void showDefaultTable();

    /** Show a table from a list layout model in a new window.
     *  @param[in] model the model to show */
    void showTable(QlomListLayoutModel *model);

    /** Lookup the text that corresponds to an error domain.
     *  @param[in] errorDomain the error domain to provide a string for
     *  @returns a human-readable description of the error domain */
    QString errorDomainLookup(const Qlom::QlomErrorDomain errorDomain);

    QlomDocument glomDocument; /**< a Glom document to view */
    QTreeView *centralTreeView; /**< a tree view for the table names model */
    QlomErrorReporter theErrorReporter; /**< an error reporting facility that
                                          can be used in lieu of C++
                                          exceptions. */
    bool valid; /**< See isValid(). */

private Q_SLOTS:
    /** Slot for the signal from the Open menu item. */
    void fileOpenTriggered();

    /** Slot for the signal from the Close menu item. */
    void fileCloseTriggered();

    /** Slot for the signal from the Quit menu item. */
    void fileQuitTriggered();

    /** Slot for the signal from the Help menu item. */
    void helpAboutTriggered();

    /** Slot for the signal from a double-click on the table names treeview.
     *  @param[in] index the row that was double-clicked */
    void treeviewDoubleclicked(const QModelIndex &index);

    /** Slot to check whether the details buttons work.
     *  @param[in] object contains the index of the row that was clicked */
    void onDetailsPressed(QObject *obj);
};


/** This class extends the QTableView by a delegate factory specialised to the
 *  QlomListLayoutModel. */
class QlomListView : public QTableView
{
    Q_OBJECT

public:
    explicit QlomListView(QWidget *parent = 0);
    virtual ~QlomListView();

    /** Creates the necessary Qlom layout delegates for the current model and
      * installs them in the view. */
    void setupDelegateForColumn(int column);

    /** Applies Glom's FieldFormatting to a QStyledItemDelegate. Since the view
     *  only knows about columns (during setup) we need to map columns to
     *  LayoutItems, as it is already done in the query builder, even if not
     *  stated explicitly.
     *  @param[in] model the layout model that needs to be queried for layout items
     *  @param[in] column the column to create a delegate for
     *  @returns the style delegate to be managed by a view, or 0 if the
     *  specified column cannot be formatted customly. */
    static QStyledItemDelegate * createDelegateFromColumn(QlomListLayoutModel *model, int column);

public Q_SLOTS:
    /** Slot to sort columns. */
    void onHeaderSectionPressed(int colIdx);

private:
    int theLastColumnIndex; /**< the last column that was used for sorting, default is -1 (i.e., none). */
    bool theToggledFlag;
};


#endif /* QLOM_MAIN_WINDOW_H_ */
