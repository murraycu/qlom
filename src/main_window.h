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

#include "glom_document.h"
#include "error_reporter.h"
#include <QMainWindow>

class QModelIndex;
class QTreeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /** Application main window. */
    MainWindow();

    /** Application main window.
     *  @param[in] filepath a path to a Glom document */
    explicit MainWindow(const QString &filepath);

    virtual ~MainWindow();

public Q_SLOTS:
    /** Receive errors.
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

    /** Lookup the text that corresponds to an error domain.
     *  @param[in] errorDomain the error domain to provide a string for */
    QString errorDomainLookup(const Qlom::QlomErrorDomain errorDomain);

    GlomDocument glomDocument; /**< a Glom document to view */
    QTreeView *centralTreeView; /**< a tree view for the table names model */

    ErrorReporter theErrorReporter; /** < an error reporting facility that can
                                          be used in lieu of C++ exceptions. */

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

};

#endif /* QLOM_MAIN_WINDOW_H_ */
