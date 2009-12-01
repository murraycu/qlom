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

#ifndef QLOM_GLOM_LIST_LAYOUT_MODEL_H_
#define QLOM_GLOM_LIST_LAYOUT_MODEL_H_

class ErrorReporter;

#include "glom_table.h"
#include "query_builder.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>

#include <libglom/document/document.h>

/** A model to show a list layout from a Glom document.
 *  The list layout model obtains all the information that is required at
 *  construction time, and is then treated as read-only. */
class GlomListLayoutModel : public QSqlTableModel
{
    Q_OBJECT

public:
    /* Note: QSqlDatabase should be passed by const reference, but the base
       class makes the same mistake.
       TODO: File a bug when there is a public Qt bug tracker. */

    /** Create a model of a list layout from a Glom document and a GlomTable.
     *  The parameters are construct-time only, and thereafter the object is
     *  read-only.
     *  @param[in] document a Glom document
     *  @param[in] table the Glom table containing a list layout
     *  @param[in] error error-reporting facility
     *  @param[in] parent a parent QObject
     *  @param[in] db a database connection, or the default connection */
    explicit GlomListLayoutModel(const Glom::Document *document,
        const GlomTable &table, ErrorReporter &error, QObject *parent = 0,
        QSqlDatabase db = QSqlDatabase());

    /** Get the table name used in the model, for display to the user.
     *  @returns the table name */
    QString tableDisplayName() const;

private:
    /** Read relationships from the document, and apply them to the model.
     *  Takes a list of relationships from the GlomTable as a parameter, and
     *  applies the relationships to the SQL query for the list layout model.
     *  @param[in] relationships a list of relationships to apply */
    void applyRelationships(const QList<GlomRelationship> &relationships);

    /** Discard columns that are not mentioned in the Glom document.
     *  Builds an SQL query from the layout group, hiding columns that should
     *  not be shown and displaying static text items that are described in the
     *  document.
     *  @param[in] layoutItem the layout containing the columns that should not
     be discarded */
    void createProjectionFromLayoutGroup(
        const Glom::sharedptr<const Glom::LayoutItem> &layoutItem);

    QlomQueryBuilder queryBuilder; /**< an SQL query builder which is used to
                                        fill the model */
    QString theTableDisplayName; /**< the display name of the layout table */
    ErrorReporter &theErrorReporter; /**< the facility used to report errors, a
                                          dynamic dependency */

    /** Escape the SQL primitive.
     *  QtSql only has two roles for escaping in a projection:
     *  1. QSqlDriver::FieldName,
     *  2. QSqlDriver::TableName.
     *  In fact, there is a third role, namely type primitives: "SELECT 1",
     *  "SELECT 'aString'", etc. where the escapeIdentifier call, for an
     *  imaginary role "QSqlDriver::Primitive", would escape using single
     *  quotes.
     *  @param[in] field the SQL primitive to escape
     *  @returns the escaped string */
    QString escapeFieldAsString(const QString &field) const;

    /** Escape the field name according to the current database escaping rules.
     *  @param[in] field the field name to escape
     *  @returns the escaped string */
    QString escapeField(const QString &field) const;
};

#endif /* QLOM_GLOM_LIST_LAYOUT_MODEL_H_ */
