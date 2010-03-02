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

#include "gui/main_window.h"

#include <iostream>

#include <QApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>

#include <libglom/init.h>

#include "config.h"

void printUsage()
{
    std::cout << "Usage: qlom [absolute_file_path]" << std::endl;
}

/** Check that the necessary database drivers are installed,
 * and warn the user if they are not.
 * @result Whether the installation is OK to continue.
 */
bool checkInstallation()
{
    /* Check that the PostgreSQL or the SQLite QSQL driver is installed,
     * because qlom will not be useful without it. */
    QStringList drivers = QSqlDatabase::drivers();
    QStringList::const_iterator end = drivers.end();
    for(QStringList::const_iterator iter = drivers.begin(); iter != end;
        ++iter) {
        const QString name = *iter;
        if(name == QLatin1String("QPSQL") || name == QLatin1String("QSQLITE"))
            return true;
    }

    QMessageBox::critical(0, QObject::tr("Incomplete Qlom Installation"),
        QObject::tr("Your installation of Glom is not complete, because no "
                    "QtSQL database driver (QPSQL or SQLite) is "
                    "available on your system.\n\nPlease report this bug to "
                    "your vendor or your system administrator so it can be "
                    "corrected."));

    return false;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("openismus");
    QCoreApplication::setOrganizationDomain("openismus.com");
    QCoreApplication::setApplicationName(PACKAGE_NAME);

    Glom::libglom_init();

    if(!checkInstallation())
      return EXIT_FAILURE;

    const QStringList options = app.arguments();
    QlomMainWindow *mainWindow = 0;
    switch (options.size()) {
    case 1:
        mainWindow = new QlomMainWindow();
        break;
    case 2:
        mainWindow = new QlomMainWindow(*(++options.begin()));
        break;
    default:
        printUsage();
        return 1;
        break;
    }

    if (mainWindow && mainWindow->isValid()) {
        mainWindow->show();
    }
    else {
        printUsage();
        return EXIT_FAILURE;
    }

    const int result = app.exec();
    delete mainWindow;

    if(result != EXIT_SUCCESS)
      printUsage();

    return result;
}
