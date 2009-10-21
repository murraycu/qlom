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

#include "glom_layout_model.h"
#include "utils.h"

#include <QSqlIndex>
#include <QSqlRecord>
#include <QStringList>

GlomLayoutModel::GlomLayoutModel(const Glom::Document &document,
    const QString& table_name, QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db)
{
    setTable(table_name);

    applyRelationships(document);

    const Glib::ustring tableNameU(qstringToUstring(table_name));
    const Glom::Document::type_list_layout_groups listLayout(
        document.get_data_layout_groups("list", tableNameU));
    for (Glom::Document::type_list_layout_groups::const_iterator iter(
        listLayout.begin()); iter != listLayout.end(); ++iter) {
        Glom::sharedptr<Glom::LayoutGroup> layoutGroup(*iter);
        if (!layoutGroup) {
            continue;
        } else {
            keepLayoutItems(layoutGroup);
        }

        break;
    }

    // Fill the model from the database.
    select();
}

void GlomLayoutModel::applyRelationships(const Glom::Document &document)
{
    const Glib::ustring tableNameU(qstringToUstring(tableName()));
    Glom::Document::type_vec_relationships relationships(
        document.get_relationships(tableNameU));
    for (Glom::Document::type_vec_relationships::const_iterator iter(
        relationships.begin()); iter != relationships.end(); ++iter) {
        const Glom::sharedptr<const Glom::Relationship> relationship(*iter);
        if (!relationship) {
            // Skip if the relationship is invalid. Can this happen?
            continue;
        }

        const QSqlRecord record(this->record());
        const QString from(ustringToQstring(relationship->get_from_field()));
        const int index = record.indexOf(from);

        if (index == -1 || index == 0) {
            // If the index is invalid, or the primary key, ignore it.
            continue;
        }

        const QString toTable(ustringToQstring(relationship->get_to_table()));
        const QString toPrimaryKey(
            ustringToQstring(relationship->get_to_field()));
        /* TODO: Find a way to automatically retrieve a default field, rather
           than hardcoding the location. */
        const QString toField("name");
        setRelation(index, QSqlRelation(toTable, toPrimaryKey, toField));
    }
}

void GlomLayoutModel::keepLayoutItems(
    const Glom::sharedptr<const Glom::LayoutItem> &layoutItem)
{
    Glom::sharedptr<const Glom::LayoutGroup> layoutGroup(
        Glom::sharedptr<const Glom::LayoutGroup>::cast_dynamic(layoutItem));
    if (layoutGroup) {
        QStringList keep_items;
        const Glom::LayoutGroup::type_list_const_items
            items(layoutGroup->get_items());
        for (Glom::LayoutGroup::type_list_const_items::const_iterator
            iter(items.begin()); iter != items.end(); ++iter) {
            Glom::sharedptr<const Glom::LayoutItem> layoutItem(*iter);
            if (!layoutItem) {
                // Skip if the layoutItem is invalid. Can this happen?
                continue;
            }

            // Only keep these layout items in the model.
            QString keep_item(
                ustringToQstring(layoutItem->get_layout_display_name()));
            if(keep_item.isEmpty()) {
                continue;
            } else {
                keep_items.push_back(keep_item);
                /* Use the column names from the Document, not the database
                   table. */
                for (int index = 0; index < record().count(); ++index) {
                    if (keep_item == record().fieldName(index)) {
                        setHeaderData(index, Qt::Horizontal, ustringToQstring(
                            layoutItem->get_title_or_name()));
                    }
                }
            }
        }

        for (int index = 0; index < record().count(); ++index) {
            // Remove columns from model.
            if (!keep_items.contains(record().fieldName(index)))
                removeColumn(index);
        }
    }
}
