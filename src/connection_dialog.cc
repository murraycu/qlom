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
    glomDoc(document)
{
    setWindowTitle(tr("Connection details"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gridLayout = new QGridLayout();
    QLabel *description = new QLabel(
        tr("Please enter the connection details for the database server"));
    mainLayout->addWidget(description);
    mainLayout->addLayout(gridLayout);

    QLabel *hostLabel = new QLabel(tr("Hostname:"));
    gridLayout->addWidget(hostLabel, 0, 0);
    host = new QLineEdit(ustringToQstring(glomDoc.get_connection_server()));
    gridLayout->addWidget(host, 0, 2);
    QLabel *database_label = new QLabel(tr("Database:"));
    gridLayout->addWidget(database_label, 1, 0);
    database = new QLineEdit(
        ustringToQstring(glomDoc.get_connection_database()));
    gridLayout->addWidget(database, 1, 2);
    database->setReadOnly(true);
    QLabel *userLabel = new QLabel(tr("Username:"));
    gridLayout->addWidget(userLabel, 2, 0);
    user = new QLineEdit();
    gridLayout->addWidget(user, 2, 2);
    QLabel *passwordLabel = new QLabel(tr("Password:"));
    gridLayout->addWidget(passwordLabel, 3, 0);
    password = new QLineEdit();
    gridLayout->addWidget(password, 3, 2);
    password->setEchoMode(QLineEdit::Password);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(databaseConnect()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ConnectionDialog::databaseConnect()
{
    // Try to open a database connection, with the details from the dialog.
    switch (glomDoc.get_hosting_mode()) {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
        openPostgresql();
        break;
    default:
        std::cerr << "Unexpected database hosting mode" << std::endl;
        done(QDialog::Rejected);
        break;
    }

}

// Open a central-server PostgreSQL connection.
void ConnectionDialog::openPostgresql()
{
    const QString backend(QString("QPSQL"));
    QSqlDatabase db(QSqlDatabase::addDatabase(backend));

    const QSqlError error(db.lastError());
    if (error.isValid()) {
        // TODO: Give feedback in the UI.
        std::cerr << "Database backend \"" << backend.toUtf8().constData()
            << "\" does not exist\n" << "Error details: "
            << error.text().toUtf8().constData() << std::endl;
        done(QDialog::Rejected);
    }

    db.setHostName(host->text());
    db.setDatabaseName(database->text());
    db.setUserName(user->text());
    db.setPassword(password->text());

    if (!db.open()) {
        //TODO: Tell the user in the UI.
        std::cerr << "Database connection could not be opened" << std::endl;
        done(QDialog::Rejected);
    } else {
        done(QDialog::Accepted);
    }
}
