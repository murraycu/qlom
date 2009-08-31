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

class ConnectionDialog : public QDialog
{
  Q_OBJECT

  public:
    ConnectionDialog(const Glom::Document& document, QWidget *parent = 0);

  private:
    const Glom::Document &glom_doc;
    QLineEdit *host;
    QLineEdit *database;
    QLineEdit *user;
    QLineEdit *password;

  private Q_SLOTS:
    void database_connect();
};

#endif /* QLOM_CONNECTION_DIALOG_H_ */
