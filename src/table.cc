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

#include "table.h"

QlomTable::QlomTable(const QString &tableName, const QString &displayName,
    const QList<QlomRelationship> &relationships, const QFlags<QlomTableFlags>& flags) :
    theDisplayName(displayName),
    theTableName(tableName),
    theRelationships(relationships),
    theFlags(flags)
{}

QString QlomTable::displayName() const
{
    return theDisplayName;
}

QString QlomTable::tableName() const
{
    return theTableName;
}

QList<QlomRelationship> QlomTable::relationships() const
{
    return theRelationships;
}

bool QlomTable::isHidden() const
{
    return (theFlags & HIDDEN_TABLE);
}

bool QlomTable::isDefault() const
{
    return (theFlags & DEFAULT_TABLE);
}
