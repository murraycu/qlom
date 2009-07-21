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

#include <QApplication>
#include "main_window.h"
#include "config.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("openismus");
  QCoreApplication::setOrganizationDomain("openismus.com");
  QCoreApplication::setApplicationName(PACKAGE_TARNAME);

  MainWindow main_window;

  main_window.show();

  return app.exec();
}
