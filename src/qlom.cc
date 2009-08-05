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

  /* Qt does not have an equivalent to GOption, except the arguments() static
     method used below. */
  std::list<QString> options = app.arguments().toStdList();
  if(options.size() < 2)
  {
    std::cout << "Usage: glom filepath" << std::endl;
    return 1;
  }

  QString filepath_qt = *++options.begin();

  /* TODO: toStdString() converts UTF-8 to ASCII, suggesting that it will
     corrupt some filepaths. Avoid that - we just want a raw char*. */
  std::string filepath = filepath_qt.toStdString();

  /* Qt does not have the concept of file URIs. However, only '/' is supported
     for directory separators, so prepending "file://" to the path and escaping
     the necessary characters should work, i.e. filename_to_uri(). */
  std::string uri;
  try
  {
    uri = Glib::filename_to_uri(filepath);
  }
  catch(const Glib::ConvertError& ex)
  {
    std::cerr << "Exception from Glib::filename_to_uri(): " << ex.what() << std::endl;
    return 1;
  }

  MainWindow main_window(uri);
  main_window.show();

  return app.exec();
}
