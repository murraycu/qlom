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

#ifndef QLOM_MAIN_WINDOW_H_
#define QLOM_MAIN_WINDOW_H_

#include "document.h"
#include "layout_delegates.h"
#include "list_layout_model.h"

#include <QMainWindow>
#include <QTableView>

class QlomListLayoutModel;
class QComboBox;
class QModelIndex;
class QPushButton;
class QStackedWidget;
class QlomListView;
class QTreeView;
class QlomModelIndexObject;

/** The main window is the central controller and view for Qlom.
 *  The main window both shows the main window and manages the Glom document,
 *  through the glomDocument member. */
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

private:
    /** A generic error message for the error domain is shown in a dialog, and
     *  the detailed error message is shown in the initially-hidden details
     *  section.
     *  @param error the error */
    void showError(const QlomError &error);

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
    QString errorDomainLookup(const Qlom::QlomErrorDomain errorDomain) const;

    QlomDocument theGlomDocument; /**< a Glom document to view */
    QStackedWidget *theMainWidget; /**< a container for multiple stacked display
    widgets */
    QTreeView *theTablesTreeView; /**< a tree view for the table names model */
    QlomListView *theListLayoutView; /**< a table view for a list layout */
    QComboBox *theTablesComboBox; /**< a combo box for the table names model */
    bool theValidFlag; /**< See isValid(). */

private Q_SLOTS:
    /** Slot for the signal from the Open menu item. */
    void fileOpenTriggered();

    /** Slot for the signal from the Close menu item. */
    void fileCloseTriggered();

    /** Slot for the signal from the Quit menu item. */
    void fileQuitTriggered();

    /** Slot for the signal from the Help menu item. */
    void helpAboutTriggered();

    /** Slot to show the list of tables. */
    void showTablesList();

    /** Slot for the signal from a double-click on the table names treeview.
     *  @param[in] index the row that was double-clicked */
    void tablesTreeviewDoubleclicked(const QModelIndex &index);

    /** Slot to check whether the details buttons work.
     *  @param[in] index the row that was double-clicked */
    void onDetailsPressed(const QModelIndex &index);

    /** Slot to show a table given a numeric index.
     *  @param[in] index the index of the table to show */
    void showTableFromIndex(const int index);
};

#endif /* QLOM_MAIN_WINDOW_H_ */
