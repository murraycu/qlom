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

#include "list_layout_model.h"
#include "error_reporter.h"
#include "utils.h"
#include "error.h"

#include <libglom/utils.h>
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
QlomListLayoutModel::QlomListLayoutModel(const Glom::Document *document,
    const QlomTable &table, QlomErrorReporter &error,
    QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    theTableDisplayName(table.displayName()), // TODO: Add GlomTable member.
    theErrorReporter(error)
{
    setTable(table.tableName());

    // The first item in a list layout group is always a main layout group.
    const Glib::ustring tableNameU(qstringToUstring(table.tableName()));
    const Glom::Document::type_list_layout_groups listLayout(
        document->get_data_layout_groups("list", tableNameU));

    /* TODO: wrap in a get_list_model, so that the checks are kept together in
     * one place. */
    if (1 == listLayout.size()) {
        theLayoutGroup = listLayout[0];
        Glom::sharedptr<const Glom::LayoutGroup> group =
            Glom::sharedptr<const Glom::LayoutGroup>::cast_dynamic(theLayoutGroup);
        if (group) {
            QSqlQuery query = buildQuery(tableNameU, group);
            setQuery(query);
        }
    } else {
        /* Display a warning message if the Glom document could not provide
         * us with a main layout group. */
        error.raiseError(QlomError(Qlom::DATABASE_ERROR_DOMAIN,
            tr("%1: no list model found").arg("GlomLayoutModel"),
            Qlom::WARNING_ERROR_SEVERITY));
    }
}

QString QlomListLayoutModel::tableDisplayName() const
{
    return theTableDisplayName;
}

QString QlomListLayoutModel::buildQuery(const Glib::ustring& table,
                                        const Glom::sharedptr<const Glom::LayoutGroup> &layoutGroup)
{
    Glib::ustring where_clause;
    Glib::ustring extra_join;
    Glom::type_sort_clause sort_clause;
    Glib::ustring group_by;
    Glom::Utils::type_vecConstLayoutFields fields;
    const Glom::LayoutGroup::type_list_const_items items = layoutGroup->get_items();

    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
         items.begin();
         iter != items.end();
         ++iter) {
         Glom::sharedptr<const Glom::LayoutItem_Field> field = Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
         if (field) {
             fields.push_back(field);
         }
    }

    Glib::ustring query = Glom::Utils::build_sql_select_with_where_clause(table, fields, where_clause, extra_join, sort_clause, group_by);
    return ustringToQstring(query);
}

QStyledItemDelegate * QlomListLayoutModel::createDelegateFromColumn(int column) const
{
    /* Need to respect the following constraint: The layout item in
     * theLayoutGroup that can be found at the position column points to has to
     * be a LayoutItem_Text or a LayoutItem_Field.
     * However, this method is not used efficiently, considering how most items
     * in a list view are field items. If LayoutItem_Text and LayoutItem_Field
     * had a common base clase featuring the get_formatting_used() API we could
     * get rid of the most annoying part at least: the dynamic casts. */
    const Glom::LayoutGroup::type_list_const_items items =
      theLayoutGroup->get_items();
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
        items.begin(); iter != items.end(); ++iter) {
        if (column == std::distance(items.begin(), iter)) {
            Glom::sharedptr<const Glom::LayoutItem_Text> textItem =
                Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
            if(textItem)
                return new QlomLayoutItemTextDelegate(
                    textItem->get_formatting_used(),
                    QlomLayoutItemTextDelegate::GlomSharedField());

            Glom::sharedptr<const Glom::LayoutItem_Field> fieldItem =
                Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
            if(fieldItem)
                return new QlomLayoutItemFieldDelegate(
                    fieldItem->get_formatting_used(),
                    fieldItem->get_full_field_details());
        }
    }

    return 0;
}
