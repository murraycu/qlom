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

#include "utils.h"

#include <iostream>

#include <QDialog>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>

#include <libglom/document/document.h>
#include <glibmm/convert.h>

// Open an SQLite database connection.
bool open_sqlite(const Glom::Document& document)
{
  const QString backend = QString("QSQLITE");
  QSqlDatabase db = QSqlDatabase::addDatabase(backend);

  const QSqlError error = db.lastError();
  if(error.isValid())
  {
    // TODO: Give feedback in the UI.
    std::cerr << "Database backend \"" << backend.toUtf8().constData() <<
      "\" does not exist\n" << "Error details: " <<
      error.text().toUtf8().constData() << std::endl;
    return false;
  }

  // Build SQLite database absolute path.
  QString filename;
  try
  {
    filename = ustring_to_qstring(Glib::filename_to_utf8(Glib::filename_from_uri(document.get_connection_self_hosted_directory_uri())));
  }
  catch(Glib::ConvertError &convert_exception)
  {
    std::cerr << "Exception from Glib::filename_to_uri(): " <<
      convert_exception.what() << std::endl;
    return false;
  }
  filename.push_back('/'); // Qt only supports '/' as a path separator.
  filename.push_back(ustring_to_qstring(document.get_connection_database()));
  filename.push_back(".db"); // Yes, really.

  /* Check if the database exists, as otherwise a new, blank database will be
     created and the open will succeed. */
  const QFile sqlite_db(filename);
  if(sqlite_db.exists())
  {
    db.setDatabaseName(filename);
  }
  else
  {
    std::cerr << "Sqlite database does not exist" << std::endl;
    return false;
  }

  if(!db.open())
  {
    std::cerr << "Database connection could not be opened" << std::endl;
    return false;
  }
  else
  {
    return true;
  }
}
