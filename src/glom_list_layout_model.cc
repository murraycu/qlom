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

#include "glom_list_layout_model.h"
#include "error_reporter.h"
#include "utils.h"
#include "qlom_error.h"

#include <QSqlQuery>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QStringList>
#include <QRegExp>

/**  This class creates a model from Glom layout groups and layout items,
  *  suitable for list and detail views.
  */
// We don't check for nullptr in document and error?
GlomListLayoutModel::GlomListLayoutModel(const Glom::Document *document,
    const GlomTable &table, ErrorReporter &error,
    QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    theTableDisplayName(table.displayName()), // TODO: Add GlomTable member.
    theErrorReporter(error)
{
    setTable(table.tableName());
    theQueryBuilder.addRelation(table.tableName());

    /* mikhas (2009/11/27) - Fixes faulty joins. Glom does not join tables,
     * neither in list view nor in details view but appearantly only in portal
     * views.
     */
    applyRelationships(table.relationships());

    // The first item in a list layout group is always a main layout group.
    const Glib::ustring tableNameU(qstringToUstring(table.tableName()));
    const Glom::Document::type_list_layout_groups listLayout(
        document->get_data_layout_groups("list", tableNameU));

    /* TODO: wrap in a get_list_model, so that the checks are kept together in
     * one place. */
    if (1 == listLayout.size()) {
        theLayoutGroup = listLayout[0];
        createProjectionFromLayoutGroup(listLayout[0]);
        QSqlQuery query = theQueryBuilder.getDistinctSqlQuery();
        setQuery(query);
    } else {
        /* Display a warning message if the Glom document could not provide
         * us with a main layout group. */
        error.raiseError(QlomError(Qlom::DATABASE_ERROR_DOMAIN,
            tr("%1: no list model found").arg("GlomLayoutModel"),
            Qlom::WARNING_ERROR_SEVERITY));
    }
}

QString GlomListLayoutModel::tableDisplayName() const
{
    return theTableDisplayName;
}

QStyledItemDelegate * GlomListLayoutModel::createDelegateFromColumn(int column) const
{
    // Need to respect the following constraint: The layout item in
    // theLayoutGroup that can be found at the position column points to has to
    // be a LayoutItem_Text or a LayoutItem_Field.
    // However, this method is not used efficiently, considering how most items
    // in a list view are field items. If LayoutItem_Text and LayoutItem_Field
    // had a common base clase featuring the get_formatting_used() API we could
    // get rid of the most annoying part at least: the dynamic casts.
    const Glom::LayoutGroup::type_list_const_items items = theLayoutGroup->get_items();
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter = items.begin();
        iter != items.end();
        ++iter) {
        if (column == std::distance(items.begin(), iter)) {
            Glom::sharedptr<const Glom::LayoutItem_Text> textItem = Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
            if(textItem)
                return new GlomLayoutItemTextDelegate(textItem->get_formatting_used(), GlomLayoutItemTextDelegate::GlomSharedField());

            Glom::sharedptr<const Glom::LayoutItem_Field> fieldItem = Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
            if(fieldItem)
                return new GlomLayoutItemFieldDelegate(fieldItem->get_formatting_used(),
                                                       fieldItem->get_full_field_details());
        }
    }

    return 0;
}

void GlomListLayoutModel::applyRelationships(
    const QList<GlomRelationship> &relationships)
{
    for (QList<GlomRelationship>::const_iterator relationship(
        relationships.begin()); relationship != relationships.end();
        ++relationship) {
        const QString from(relationship->fromColumn());
        const QString toTable(relationship->toTable());
        const QString toPrimaryKey(relationship->toPrimaryKey());

        theQueryBuilder.equiJoinWith(toTable,
            QString("%1.%2").arg(toTable).arg(toPrimaryKey),
            QString("%1.%2").arg(tableName()).arg(from));
    }
}

void GlomListLayoutModel::createProjectionFromLayoutGroup(
    const Glom::sharedptr<const Glom::LayoutItem> &layoutItem)
{
    Glom::sharedptr<const Glom::LayoutGroup> layoutGroup =
        Glom::sharedptr<const Glom::LayoutGroup>::cast_dynamic(layoutItem);

    if (layoutGroup) {
        const Glom::LayoutGroup::type_list_const_items items =
          layoutGroup->get_items();
        for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
            items.begin();
            iter != items.end();
            ++iter) {
            /* The default setting for a projection is
             * "table.col AS display_name", whereas display_name is used for
             *  the column heading. */
            const QString currColName = ustringToQstring((*iter)->get_name());
            const QString currDisplayName =
                ustringToQstring((*iter)->get_title_or_name());

            // Check whether there is a static text item to display.
            Glom::sharedptr<const Glom::LayoutItem_Text> staticTextItem =
                Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(
                    *iter);

            if (staticTextItem) {
                theQueryBuilder.addProjection(QString("%1 AS %3")
                    .arg(escapeFieldAsString(ustringToQstring(
                        staticTextItem->get_text())))
                    .arg(escapeField(currDisplayName)));
            } else {
                theQueryBuilder.addProjection(QString("%1.%2 AS %3")
                    .arg(escapeField(tableName()))
                    .arg(escapeField(currColName))
                    .arg(escapeField(currDisplayName)));
            }
        }
    }
}


QString GlomListLayoutModel::escapeFieldAsString(const QString &field) const
{
    return ('\'' + QString(field).replace('\'', "''") + '\'');
}

QString GlomListLayoutModel::escapeField(const QString &field) const
{
    return database().driver()->escapeIdentifier(field, QSqlDriver::FieldName);
}
