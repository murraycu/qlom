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

#ifndef QLOM_TABLE_H_
#define QLOM_TABLE_H_

#include "relationship.h"

#include <QList>
#include <QString>

/** A table in a Glom document.
 *  Designed for use in a QlomDocument, QlomTable has four construct-time
 *  properties: the name, the display name, a list of relationships and the
 *  flags. The properties cannot be changed once a table has been constructed,
 *  but can be accessed with the displayName(), tableName() and relationships()
 *  methods, and the flags can be checked with the isHidden() and isDefault()
 *  methods. */
class QlomTable
{

public:
    /** Flags to indicate the status of a table. */
    enum QlomTableFlags {
        INVALID_TABLE = 0, /**< invalid status */
        HIDDEN_TABLE = 1, /**< hidden table */
        DEFAULT_TABLE = 2 /**< default table */
    };

    /** A table in a Glom document.
     *  @param[in] tableName the name of the table in the database
     *  @param[in] displayName the name of the table for display
     *  @param[in] relationships a list of relationships
     *  @param[in] flags flags for the table */
    QlomTable(const QString &tableName, const QString &displayName,
        const QList<QlomRelationship> &relationships,
        const QFlags<QlomTableFlags> &flags);

    /** Get the table name for display to the user.
     *  @returns the name of the table for display */
    QString displayName() const;

    /** Get the table name for use with a database.
     *  @returns the name of the table for use with a database */
    QString tableName() const;

    /** Get the list of relationships.
     *  @returns the list of relationships */
    QList<QlomRelationship> relationships() const;

    /** Predicate to check for hidden flag.
     *  @returns whether the table is a hidden table */
    bool isHidden() const;

    /** Predicate to check for default flag.
     *  @returns whether the table is the default table */
    bool isDefault() const;

private:
    QString theDisplayName; /**< the table name, for display to the user */
    QString theTableName; /**< the table name, as in the database */
    QList<QlomRelationship> theRelationships; /**< a list of relationships */
    QFlags<QlomTableFlags> theFlags; /**< flags for default or hidden tables */
};

#endif /* QLOM_DOCUMENT_H_ */
