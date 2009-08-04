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

#include <glibmm/convert.h>
#include <iostream>

#include "config.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("openismus");
  QCoreApplication::setOrganizationDomain("openismus.com");
  QCoreApplication::setApplicationName(PACKAGE_TARNAME);

  // TODO: Find a Qt way to get a URI from a filename:
  std::string uri;
  try
  {
    uri = Glib::filename_to_uri("/opt/gnome2/share/glom/doc/examples/example_music_collection.glom");
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "Exception from Glib::filename_to_uri(): " << ex.what();
    return 1;
  }
  
  MainWindow main_window(uri);
  main_window.show();

  return app.exec();
}
