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

#ifndef QLOM_CONNECTION_DIALOG_H_
#define QLOM_CONNECTION_DIALOG_H_

#include <QDialog>
#include <libglom/document/document.h>

class QLineEdit;
namespace Glom
{
class Document;
};

/** A dialog to ask the user for connection details.
 *  Some database backends require authentication details, which can be
 *  requested with a ConnectionDialog.
 *
 *  The default connection for QSqlDatabase is created and opened by the dialog.
 *
 *  @see openSqlite()
 */
class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(const Glom::Document &document,
        QWidget *parent = 0);

    /** Whether the entered settings allowed a connection to 
     * be made when the user accepted the dialog (clicked Connect).
     * This is only meaningful if exec() returned Accepted.
     */
    bool connectionWasSuccessful() const;

private:
    const Glom::Document &glomDoc;
    QLineEdit *host;
    QLineEdit *database;
    QLineEdit *user;
    QLineEdit *password;

    bool theConnectionWasSuccessful;

    void openPostgresql();

private Q_SLOTS:
    void databaseConnect();
};

#endif /* QLOM_CONNECTION_DIALOG_H_ */
