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

#ifndef QLOM_GLOM_RELATIONSHIP_H_
#define QLOM_GLOM_RELATIONSHIP_H_

#include <QString>

class GlomRelationship
{
public:
    /** Create a relationship to a column in another table.
     *  @param[in] fromColumn source column for the relationship
     *  @param[in] toTable destination table for the reationship
     *  @param[in] toPrimaryKey destination primary key of the relationship */
    GlomRelationship(const QString &fromColumn, const QString &toTable,
        const QString &toPrimaryKey);

    /** Get the source column of the relationship.
     *  @returns the source column. */
    QString fromColumn() const;

    /** Get the destination table of the relationship.
     *  @returns the destination table. */
    QString toTable() const;

    /** Get the primary key in the destination table of the relationship.
     *  @returns the destination primary key. */
    QString toPrimaryKey() const;

private:
    QString theFromColumn; /**< source column */
    QString theToTable; /**< destination table */
    QString theToPrimaryKey; /**< primary key in destination table */
};

#endif /* QLOM_GLOM_RELATIONSHIP_H_ */
