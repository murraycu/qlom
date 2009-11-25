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

#ifndef QLOM_GLOM_LAYOUT_MODEL_H_
#define QLOM_GLOM_LAYOUT_MODEL_H_

#include "glom_table.h"
#include "query_builder.h"
#include "error_reporter.h"

#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QString>
#include <libglom/document/document.h>

/** A model to show rows of data from a Glom database. */
class GlomLayoutModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    /* Note: QSqlDatabase should be passed by const reference, but the base
       class makes the same mistake.
       TODO: File a bug when there is a public Qt bug tracker. */

    /** Create a model of the layouts in a table from a Glom document.
     *  @param[in] document a Glom document
     *  @param[in] tableName the table name
     *  @param[in] error reporting facility to be used by GlomLayoutModel
     *  @param[in] parent a parent QObject
     *  @param[in] db a database connection, or the default connection */
    explicit GlomLayoutModel(const Glom::Document *document,
        const GlomTable &table, ErrorReporter &error, QObject *parent = 0,
        QSqlDatabase db = QSqlDatabase());

    /** Get the table name used in the model.
     *  @returns the table name */
    QString tableDisplayName() const;

private:
    /** Filters input data.
     *  @param[in] index the index to retrieve data from
     *  @param[in] role the Qt::ItemDataRole to retrieve fata for
     *  @returns the data from the model */
    QVariant data(const QModelIndex &index, int role) const;

    /** Read relationships from the Glom document, and apply them to the model.
     *  @param[in] relationships a list of relationships to apply */
    void applyRelationships(const QList<GlomRelationship> &relationships);

    /** Discard columns that are not mentioned in the document.
     *  @param[in] layoutItem the layout containing the columns that should not
     be discarded */
    void createProjectionFromLayoutGroup(
        const Glom::sharedptr<const Glom::LayoutItem> &layoutItem);

    QlomQueryBuilder queryBuilder; /**< an SQL query builder which is used to
                                        fill the model */
    QString theTableDisplayName; /**< the display name of the layout table */
    ErrorReporter &theErrorReporter; /**< the facility used to report
                                          errors, a dynamic dependency */

    QString escapeFieldAsString(QString field) const;

    QString escapeField(QString field) const;
};

#endif /* QLOM_GLOM_LAYOUT_MODEL_H_ */
