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
    const QlomTable &table, bool &error,
    QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    theTableDisplayName(table.displayName()) // TODO: Add GlomTable member.
{
    error = false;
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
            addStaticTextColumns(group);
            adjustColumnHeaders(group);
        }
    } else {
        error = true;
    }
}

void QlomListLayoutModel::addStaticTextColumns(const Glom::sharedptr<const Glom::LayoutGroup> &layoutGroup)
{
    const Glom::LayoutGroup::type_list_const_items items = layoutGroup->get_items();

    int colsIdx = 0;
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
         items.begin();
         iter != items.end();
         ++iter) {
         bool flag = false;

         Glom::sharedptr<const Glom::LayoutItem_Text> text = Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
         if (text) {
             insertColumn(colsIdx); // inserts before, and it is allowed to fail!
             flag = true;
         }

         theStaticTextColumnIndices.push_back(flag);
         ++colsIdx;
    }
}

void QlomListLayoutModel::adjustColumnHeaders(const Glom::sharedptr<const Glom::LayoutGroup> &layoutGroup)
{
    const Glom::LayoutGroup::type_list_const_items items = layoutGroup->get_items();

    int colsIdx = 0;
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
         items.begin();
         iter != items.end();
         ++iter) {
         setHeaderData(colsIdx, Qt::Horizontal, QVariant(ustringToQstring((*iter)->get_title_or_name())));
         ++colsIdx;
    }
}

QString QlomListLayoutModel::tableDisplayName() const
{
    return theTableDisplayName;
}

const QlomListLayoutModel::GlomSharedLayoutItems QlomListLayoutModel::getLayoutItems() const
{
    return theLayoutGroup->get_items();
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


    // We need to pad the column count of the model created by the SQL query so
    // that it matches the column count after inserting the buttons for the
    // actions columns in the view. Else, resorting would again change the
    // column count by 1, thus losing the extra column in the process.
    // spaceHolder is my current solution to that problem.
    //
    // Glom generated "table"."" in the SQL query projection if the item is
    // empty. However, we can assume that each Glom relation consists of at
    // least one field (even if that means it's only the primary key).
    //
    // TODO: separate view issues from this model, probably with a proxy model.
    Glom::sharedptr<const Glom::LayoutItem_Field> placeHolder;

    int idx = 0;
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
         items.begin();
         iter != items.end();
         ++iter) {
         Glom::sharedptr<const Glom::LayoutItem_Field> field = Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
         if (field) {

             // Copy the first field we find into the place holder
             if (!placeHolder) {
                 placeHolder = field;
             }

             Glom::sharedptr<const Glom::Field> details = field->get_full_field_details();
             if (details && details->get_primary_key()) {
                sort_clause.push_back(Glom::type_pair_sort_field(field, true));
             }

             fields.push_back(field);
             setHeaderData(idx, Qt::Horizontal, QVariant(ustringToQstring(field->get_title_or_name())));
             ++idx;
         }

         Glom::sharedptr<const Glom::LayoutItem_Text> text = Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
         if (text) {
             ++idx;
             insertColumn(idx); // inserts before
         }
    }

    // Assume that at least one column was queried, basically.
    Q_ASSERT(placeHolder);

    // Make a const copy for the list.
    fields.push_back(placeHolder);
    // Pad the static text column lookup list accordingly.
    theStaticTextColumnIndices.push_back(false);

    Glib::ustring query = Glom::Utils::build_sql_select_with_where_clause(table, fields, where_clause, extra_join, sort_clause, group_by);
    return ustringToQstring(query);
}

bool QlomListLayoutModel::insertColumnAt(int colIdx)
{
    theStaticTextColumnIndices.push_back(false);
    return QSqlTableModel::insertColumn(colIdx);
}



bool QlomListLayoutModel::canFetchMore()
{
    const bool canFetchMore = QSqlTableModel::canFetchMore();

    // Drop the shared lock by *finishing* the transaction.
    // Otherwise, other db connections cannot write to the opened table.
    // Once we fetched all data, there is also no need to keep a lock, since we
    // don't intend to write to the table (even if so, that'd be another
    // transaction).
    if(!canFetchMore)
        database().commit();

    return canFetchMore;
}

QVariant QlomListLayoutModel::data(const QModelIndex &index, int role) const
{
    int colsIdx = index.column();
    if (colsIdx >= theStaticTextColumnIndices.size()) {
        qWarning("Invalid model column requested.");
        colsIdx = 0;
    }

    // Return the empty string which creates a "non-null" QString for the
    // QVariants. For valid QVariants containing null values, the style
    // delegate's displayText() is not called.
    if (theStaticTextColumnIndices[colsIdx] && Qt::DisplayRole == role)
        return QVariant(QString(""));

   return QSqlTableModel::data(index, role);
}

