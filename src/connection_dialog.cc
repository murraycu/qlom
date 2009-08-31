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
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSqlDatabase>
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
  QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
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
