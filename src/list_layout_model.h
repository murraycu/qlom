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

#ifndef QLOM_LIST_LAYOUT_MODEL_H_
#define QLOM_LIST_LAYOUT_MODEL_H_

class QlomErrorReporter;

#include "table.h"
#include "layout_delegates.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>

#include <libglom/document/document.h>
#include <libglom/data_structure/layout/layoutgroup.h>

/** A model to show a list layout from a Glom document.
 *  The list layout model obtains all the information that is required at
 *  construction time, and is then treated as read-only. */
class QlomListLayoutModel : public QSqlTableModel
{
    Q_OBJECT

public:
    /* Note: QSqlDatabase should be passed by const reference, but the base
       class makes the same mistake.
       TODO: File a bug when there is a public Qt bug tracker. */

    /** Create a model of a list layout from a Glom document and a table.
     *  The parameters are construct-time only, and thereafter the object is
     *  read-only.
     *  @param[in] document a Glom document
     *  @param[in] table the Glom table containing a list layout
     *  @param[in] error error-reporting facility
     *  @param[in] parent a parent QObject
     *  @param[in] db a database connection, or the default connection */
    explicit QlomListLayoutModel(const Glom::Document *document,
        const QlomTable &table, QlomErrorReporter &error, QObject *parent = 0,
        QSqlDatabase db = QSqlDatabase());

    /** Get the table name used in the model, for display to the user.
     *  @returns the table name */
    QString tableDisplayName() const;

    /** Applies Glom's FieldFormatting to a QStyledItemDelegate. Since the view
     *  only knows about columns (during setup) we need to map columns to
     *  LayoutItems, as it is already done in the query builder, even if not
     *  stated explicitly.
     *  @param[in] column the column to create a delegate for
     *  @returns the style delegate to be managed by a view, or 0 if the
     *  specified column cannot be formatted customly. */
    QStyledItemDelegate * createDelegateFromColumn(int column) const;

private:
    /** A wrapper for Glom::Utils::build_sql_select_with_where_clause()
      * @param[in] table the name of the table
      * @param[in] layoutGroup a shared pointer to a Glom LayoutGroup that is
      *            suitable for a list view (i.e., contains LayoutItem_Fields).
      * @returns the SQL query as string.
      */
    QString buildQuery(const Glib::ustring& table,
                       const Glom::sharedptr<const Glom::LayoutGroup> &layoutGroup);

    QString theTableDisplayName; /**< the display name of the layout table */
    Glom::sharedptr<const Glom::LayoutGroup> theLayoutGroup; /**< the layout group used for the list layout */
    QlomErrorReporter &theErrorReporter; /**< the facility used to report
                                           errors, a dynamic dependency */

};

#endif /* QLOM_LIST_LAYOUT_MODEL_H_ */
