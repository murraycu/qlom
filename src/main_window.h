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
#include <QMainWindow>

class QDialog;
class QModelIndex;
class QSqlRelationalTableModel;
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

private:
    /** General setup method. */
    void setup();

    /** Show the application about dialog. */
    void showAboutDialog();

    /** Write the application settings to disk. */
    void writeSettings();

    /** Read the application settings from disk. */
    void readSettings();

    GlomDocument glomDocument; /**< a Glom document to view */
    QTreeView *centralTreeView; /**< a tree view for the table names model */

private Q_SLOTS:
    /** Slot for the signal from the Quit menu item. */
    void onFileQuitTriggered();

    /** Slot for the signal from the Help menu item. */
    void onHelpAboutTriggered();

    /** Slot for the signal from a double-click on the table names treeview.
     *  @param[in] index the row that was double-clicked */
    void onTreeviewDoubleclicked(const QModelIndex &index);
};

#endif /* QLOM_MAIN_WINDOW_H_ */
