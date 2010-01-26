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

QlomConnectionDialog::QlomConnectionDialog(const Glom::Document& document,
    QWidget *parent) :
    QDialog(parent),
    glomDoc(document),
    theConnectionWasSuccessful(false)
{
    setWindowTitle(tr("Connection details"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *description = new QLabel(
        tr("Please enter the connection details for the database server"));
    mainLayout->addWidget(description);

    QGridLayout *gridLayout = new QGridLayout();
    mainLayout->addLayout(gridLayout);

    // Display the connection details, allowing some to be changed:
    QLabel *hostLabel = new QLabel(tr("Hostname:"));
    gridLayout->addWidget(hostLabel, 0, 0);
    host = new QLineEdit(ustringToQstring(glomDoc.get_connection_server()));
    gridLayout->addWidget(host, 0, 2);

    // The database name is shown as read-only, just to give people a clue 
    // about what username and password they should use, in case it is not 
    // the same for the whole server.
    QLabel *database_label = new QLabel(tr("Database:"));
    gridLayout->addWidget(database_label, 1, 0);
    database = new QLineEdit(
        ustringToQstring(glomDoc.get_connection_database()));
    gridLayout->addWidget(database, 1, 2);
    database->setReadOnly(true);

    QLabel *userLabel = new QLabel(tr("Username:"));
    gridLayout->addWidget(userLabel, 2, 0);
    // Default to the UNIX user name, which is often the same as the Postgres user name:
    const char* systemUser = getenv("USER"); 
    user = new QLineEdit(systemUser);
    gridLayout->addWidget(user, 2, 2);

    QLabel *passwordLabel = new QLabel(tr("Password:"));
    gridLayout->addWidget(passwordLabel, 3, 0);
    password = new QLineEdit();
    gridLayout->addWidget(password, 3, 2);
    password->setEchoMode(QLineEdit::Password);
    password->setFocus();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(databaseConnect()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void QlomConnectionDialog::databaseConnect()
{
    theConnectionWasSuccessful = false;

    // Try to open a database connection, with the details from the dialog.
    switch (glomDoc.get_hosting_mode()) {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
        openPostgresql();
        break;
    default:
        qCritical("Unexpected database hosting mode");

        // Note that we don't use done(QDialog::Rejected) to tell the caller 
        // that the connection failed, because then there would be no way to 
        // distinguish between a failed connection and the user clicking Cancel.
        theConnectionWasSuccessful = false;
        done(QDialog::Accepted);
        break;
    }
}

// Open a central-server PostgreSQL connection.
void QlomConnectionDialog::openPostgresql()
{
    /* Create and open the default connection for QSqlDatabase,
       so other code can use QSqlDatabase::database() to get that open
       connection. */

    const QString backend(QString("QPSQL"));
    QSqlDatabase db(QSqlDatabase::addDatabase(backend));

    const QSqlError error(db.lastError());
    if (error.isValid()) {
        // TODO: Give feedback in the UI.
        qCritical("Database backend \"%s\" does not exist\nError details: %s",
            qPrintable(backend), qPrintable(error.text()));

        // Note that we don't use done(QDialog::Rejected) to tell the caller 
        // that the connection failed, because then there would be no way to 
        // distinguish between a failed connection and the user clicking Cancel.
        done(QDialog::Accepted);
        return;
    }

    db.setHostName(host->text());
    db.setDatabaseName( ustringToQstring(glomDoc.get_connection_database()) );
    db.setUserName(user->text());
    db.setPassword(password->text());

    // Try to connect on each port, starting with the one specified in the 
    // document.
    // TODO: Maybe libglom should tell us this list of ports to try, 
    // to avoid duplicating it here.
    typedef std::list<int> typeListPorts;
    typeListPorts listPorts;
    listPorts.push_back( glomDoc.get_connection_port() );
    listPorts.push_back(5432);
    listPorts.push_back(5433);
    listPorts.push_back(5434);
    listPorts.push_back(5435);
    listPorts.push_back(5436);


    typeListPorts listPortsTried; 
    for (typeListPorts::const_iterator iter = listPorts.begin(); 
        iter != listPorts.end(); ++iter) {
        const int port = *iter;
        typeListPorts::const_iterator iterTried = 
            std::find(listPortsTried.begin(), listPortsTried.end(), port);
        if (iterTried == listPortsTried.end()) {
            //Try this port. We haven't tried it before:
            db.setPort(port);
            if (db.open()) {
              theConnectionWasSuccessful = true;
              done(QDialog::Accepted);
              return;
            } else {
               //TODO: Show this detailed error in the UI.
               const QSqlError error = db.lastError();
               qCritical("Database connection (port %d) could not be opened.\n  Error: %s", port, qPrintable(error.text()));

               listPortsTried.push_back(port);
            }
        }
    }

    // The connection failed even after trying all network ports.
    // Note that we don't use done(QDialog::Rejected) to tell the caller 
    // that the connection failed, because then there would be no way to 
    // distinguish between a failed connection and the user clicking Cancel.
    theConnectionWasSuccessful = false;
    done(QDialog::Accepted);
}

bool QlomConnectionDialog::connectionWasSuccessful() const
{
  return theConnectionWasSuccessful;
}
