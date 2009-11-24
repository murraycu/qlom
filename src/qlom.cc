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

#include <iostream>

#include <QApplication>

#include <libglom/init.h>

#include "config.h"

void printUsage()
{
    std::cout << "Usage: qlom [absolute_file_path]" << std::endl;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("openismus");
    QCoreApplication::setOrganizationDomain("openismus.com");
    QCoreApplication::setApplicationName(PACKAGE_NAME);

    Glom::libglom_init();

    const QStringList options = app.arguments();
    MainWindow *mainWindow = 0;
    switch (options.size()) {
    case 1:
        mainWindow = new MainWindow();
        break;
    case 2:
        mainWindow = new MainWindow(*(++options.begin()));
        break;
    default:
        printUsage();
        return 1;
        break;
    }

    if (mainWindow) {
        mainWindow->show();
    }
    else {
        return 1;
    }
    const int result = app.exec();
    delete mainWindow;

    return result;
}
