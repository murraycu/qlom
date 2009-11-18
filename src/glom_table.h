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

#ifndef QLOM_GLOM_TABLE_H_
#define QLOM_GLOM_TABLE_H_

#include <QString>

class GlomTable
{

public:
    /** A table in a Glom document.
     *  @param[in] tableName the name of the table in the database
     *  @param[in] displayName the name of the table for display */
    GlomTable(QString tableName, QString displayName);

    /** Get the table name for display.
     *  @returns the name of the table for display */
    QString displayName() const;

    /** Get the table name for use with a database.
     *  @returns the name of the table for use with a database */
    QString tableName() const;

private:
    QString theDisplayName;
    QString theTableName;
};

#endif /* QLOM_GLOM_DOCUMENT_H_ */
