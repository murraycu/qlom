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

#include "connection_dialog.h"
#include "utils.h"

#include <QDialogButtonBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVBoxLayout>

ConnectionDialog::ConnectionDialog(const Glom::Document& document,
  QWidget *parent) :
  QDialog(parent),
  glom_doc(document)
{
  setWindowTitle(tr("Connection details"));

  QVBoxLayout *main_layout = new QVBoxLayout(this);
  QGridLayout *grid_layout = new QGridLayout();
  QLabel *description = new QLabel(
    tr("Please enter the connection details for the database server"));
  main_layout->addWidget(description);
  main_layout->addLayout(grid_layout);

  QLabel *host_label = new QLabel(tr("Hostname:"));
  grid_layout->addWidget(host_label, 0, 0);
  host = new QLineEdit(ustring_to_qstring(document.get_connection_server()));
  grid_layout->addWidget(host, 0, 2);
  QLabel *database_label = new QLabel(tr("Database:"));
  grid_layout->addWidget(database_label, 1, 0);
  database = new QLineEdit(
    ustring_to_qstring(document.get_connection_database()));
  grid_layout->addWidget(database, 1, 2);
  database->setReadOnly(true);
  QLabel *user_label = new QLabel(tr("Username:"));
  grid_layout->addWidget(user_label, 2, 0);
  user = new QLineEdit();
  grid_layout->addWidget(user, 2, 2);
  QLabel *password_label = new QLabel(tr("Password:"));
  grid_layout->addWidget(password_label, 3, 0);
  password = new QLineEdit();
  grid_layout->addWidget(password, 3, 2);
  password->setEchoMode(QLineEdit::Password);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
    QDialogButtonBox::Cancel);
  main_layout->addWidget(buttons);

  connect(buttons, SIGNAL(accepted()), this, SLOT(database_connect()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ConnectionDialog::database_connect()
{
  // Try to open a database connection, with the details from the dialog.
  switch(glom_doc.get_hosting_mode())
  {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
      {
        const QString backend = QString("QSPQL");
        open_postgresql();
      }
      break;
    case Glom::Document::HOSTING_MODE_SQLITE:
      {
        const QString backend = QString("QSQLITE");
        open_sqlite();
      }
      break;
    default:
      std::cerr << "Unexpected database hosting mode" << std::endl;
      done(QDialog::Rejected);
      break;
  }

}

// Open a PostgreSQL connection.
void ConnectionDialog::open_postgresql()
{
  const QString backend = QString("QPSQL");
  QSqlDatabase db = QSqlDatabase::addDatabase(backend);

  const QSqlError error = db.lastError();
  if(error.isValid())
  {
    // TODO: Give feedback in the UI.
    std::cerr << "Database backend \"" << backend.toUtf8().constData() <<
      "\" does not exist\n" << "Error details: " <<
      error.text().toUtf8().constData() << std::endl;
    done(QDialog::Rejected);
  }
  db.setHostName(host->text());
  db.setDatabaseName(database->text());
  db.setUserName(user->text());
  db.setPassword(password->text());

  if(!db.open())
  {
    std::cerr << "Database connection could not be opened" << std::endl;
    done(QDialog::Rejected);
  }
  else
  {
    done(QDialog::Accepted);
  }
}

// Open an SQLite database connection.
// TODO: Move the logic out of the dialog, as there are no connection settings.
void ConnectionDialog::open_sqlite()
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
    done(QDialog::Rejected);
  }
  QString filename = ustring_to_qstring(Glib::filename_to_utf8(Glib::filename_from_uri(glom_doc.get_connection_self_hosted_directory_uri())));
  filename.push_back('/'); // Qt only supports '/' as a path separator.
  filename.push_back(database->text());
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
    done(QDialog::Rejected);
  }

  if(!db.open())
  {
    std::cerr << "Database connection could not be opened" << std::endl;
    done(QDialog::Rejected);
  }
  else
  {
    done(QDialog::Accepted);
  }
}
