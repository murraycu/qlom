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

#include "query_builder.h"

QlomQueryBuilder::QlomQueryBuilder()
{}

QlomQueryBuilder::~QlomQueryBuilder()
{}

void QlomQueryBuilder::addProjection(const QString& projection)
{
    Q_ASSERT(projection != 0 && !projection.isEmpty());

    projections.push_back(projection);
}

void QlomQueryBuilder::addRelation(const QString& relation)
{
    Q_ASSERT(relation != 0 && !relation.isEmpty());

    relations.push_back(relation);
}

void QlomQueryBuilder::equiJoinWith(const QString& relation,
    const QString& lhs, const QString& rhs)
{
    Q_ASSERT(relation != 0 && !relation.isEmpty() && lhs != 0 && !lhs.isEmpty()
        && rhs != 0 && !rhs.isEmpty());

    QString builder =
        QString("JOIN %1 ON (%2 = %3) ").arg(relation).arg(lhs).arg(rhs);

    equiJoins.push_back(builder);
}

void QlomQueryBuilder::setSelection(const QString& theSelection)
{
    Q_ASSERT(theSelection != 0 && !theSelection.isEmpty());

    selection = QString("WHERE (%1) ").arg(theSelection);
}

void QlomQueryBuilder::setOrderBy(const QString& theOrderBy)
{
    Q_ASSERT(theOrderBy != 0 && !theOrderBy.isEmpty());

    orderBy = QString("ORDER BY (%1)").arg(theOrderBy);
}

QString QlomQueryBuilder::getQuery() const
{
    Q_ASSERT(!projections.isEmpty());

    QString builder;

    builder.push_back(QString("SELECT %1").arg(projections.join(", ")));
    builder.push_back(buildQueryWithoutSelect());

    return builder;
}

QString QlomQueryBuilder::getDistinctQuery() const
{
    Q_ASSERT(!projections.isEmpty());

    QString builder;

    builder.push_back(
        QString("SELECT DISTINCT %1").arg(projections.join(", ")));
    builder.push_back(buildQueryWithoutSelect());

    return builder;
}

QString QlomQueryBuilder::buildQueryWithoutSelect() const
{
    Q_ASSERT(!relations.isEmpty());

    QString query;

    query.push_back(QString(" FROM %1 ").arg(relations.join(", ")));
    query.push_back(equiJoins.join(" "));
    query.push_back(selection);
    query.push_back(orderBy);

    return query;
}
