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
#include <QInputDialog>
#include <QtSql>
#include "main_window.h"

#include <libglom/document/document.h>
#include <libglom/init.h>
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
    std::cout << "Usage: glom absolute_file_path" << std::endl;
    return 1;
  }

  QString filepath_qt = *++options.begin();

  std::string filepath = filepath_qt.toUtf8().constData();

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

  // Load the Glom document:
  Glom::libglom_init();

  Glom::Document document;
  document.set_file_uri(uri);
  int failure_code = 0;
  const bool test = document.load(failure_code);
  if(!test)
  {
    std::cerr << "Document loading failed with uri=" << uri << std::endl;
    return 1;
  }

  /* Check that the document is:
     a) Not an example document, which must be resaved - that would be an extra
     feature.
     b) Not self-hosting, because that would require starting/stopping
     PostgreSQL. */
  if(document.get_is_example_file())
  {
    std::cerr << "Document is an example file, cannot process" << std::endl;
    return 1;
  }

  if(document.get_hosting_mode() !=
    Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL)
  {
    std::cerr <<
      "Document is not hosted on an external PostgreSQL server, cannot process"
      << std::endl;
    return 1;
  }

  // Try to open a database connection, and request a password.
  QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
  db.setHostName(document.get_connection_server().c_str());
  db.setDatabaseName(document.get_connection_database().c_str());
  db.setUserName(document.get_connection_user().c_str());

  bool ok;
  QString pw = QInputDialog::getText(0, ("Enter password"),
    ("Please enter a password for the database"), QLineEdit::Password,
    QString(), &ok);
  if(ok && !pw.isEmpty())
  {
    db.setPassword(pw);
  }
  else
  {
    std::cerr << "No password entered, or dialog cancelled" << std::endl;
    return 1;
  }

  if(!db.open())
  {
    std::cerr << "Database connection could not be opened" << std::endl;
    return 1;
  }

  MainWindow main_window(document);
  main_window.show();

  return app.exec();
}
