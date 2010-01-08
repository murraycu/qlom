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

#ifndef QLOM_QLOM_QUERY_BUILDER_H_
#define QLOM_QLOM_QUERY_BUILDER_H_

#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include <QMap>
#include <QVariant>

/** A simplistic SQL query builder.
 *  This class represents a very simplistic SQL query builder, to create
 *  queries that can fill a QSqlTableModel without losing flexibility. It tries
 *  to stick to SQL-99, but for now the only "officially" supported backend
 *  will be SQLite, since Qlom targets the Maemo platform.
 */
class QlomQueryBuilder
{
public:

    /** Create a bare SQL query builder. */
    QlomQueryBuilder();
    ~QlomQueryBuilder();

    /** Add a projection to the query.
     *  For example, "SELECT [DISTINCT] projection".
     *  @param[in] projection a projection string to add */
    void addProjection(const QString& projection);

    /** Add a relation to the query.
     *  For example, "FROM rel1 [,rel2]".
     *  @param[in] relation the relation string to add */
    void addRelation(const QString& relation);

    /** Add an equi-join to the query.
     *  For example, "JOIN rel ON lhs = rhs".
     *  @param[in] relation the relation string to add
     *  @param[in] lhs the left-hand side of the join
     *  @param[in] rhs the right-hand side of the join
     *  @param[in] enableLeftJoin defaults to enable left joins */
    void equiJoinWith(const QString& relation, const QString& lhs,
        const QString& rhs, bool enableLeftJoin = true);

    /** Set the selection of the query.
     *  For example, "WHERE selection".
     *  @param[in] selection the selection string to add */
    void setSelection(const QString& selection);

    /** Set the order clause of the query.
     *  For example, "ORDER BY order_by".
     *  @param[in] orderBy the order clause to add */
    void setOrderBy(const QString& orderBy);

    /** Returns the built query, but does not check for validity. */
    QString getQuery() const;

    /** Returns the built distinct query, but does not check for validity. */
    QString getDistinctQuery() const;

    /** Returns a prepared statement. */
    QSqlQuery getDistinctSqlQuery();
    QSqlQuery getSqlQuery(bool distinct = false);

    /** Forwarding method for internal QSqlQuery. */
    void bindValue(const QString &placeholder, const QVariant &val);

private:

    /** Returns the built query, without the SELECT statement. */
    QString buildQueryWithoutSelect() const;

    QStringList projections; /**< the list of projections */
    QStringList relations; /**< the list of relations */
    QStringList equiJoins; /**< the list of equi-joins */
    QString selection; /**< the selection */
    QString orderBy; /**< the order clause */

    typedef QMap<QString, QVariant> BoundParametersMap;
    BoundParametersMap boundParameters; /**< map of the bound parameters */
};

#endif /* QLOM_QLOM_QUERY_BUILDER_H_ */
