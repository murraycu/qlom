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

#include <QSqlQuery>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QStringList>
#include <QRegExp>

GlomLayoutModel::GlomLayoutModel(const Glom::Document *document,
    const GlomTable table, QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db),
    theTableDisplayName(table.displayName()) // TODO: Add GlomTable member.
{
    setTable(table.tableName());
    queryBuilder.addRelation(table.tableName());

    applyRelationships(table.relationships());

    const Glib::ustring tableNameU(qstringToUstring(table.tableName()));
    const Glom::Document::type_list_layout_groups listLayout(
        document->get_data_layout_groups("list", tableNameU));
    for (Glom::Document::type_list_layout_groups::const_iterator iter(
        listLayout.begin()); iter != listLayout.end(); ++iter) {
        Glom::sharedptr<Glom::LayoutGroup> layoutGroup(*iter);
        if (!layoutGroup) {
            continue;
        } else {
            createProjectionFromLayoutGroup(layoutGroup);
        }

        break;
    }

    setQuery(QSqlQuery(queryBuilder.getDistinctQuery()));
}

QString GlomLayoutModel::tableDisplayName() const
{
    return theTableDisplayName;
}

void GlomLayoutModel::applyRelationships(
    const QList<GlomRelationship> relationships)
{
    for (QList<GlomRelationship>::const_iterator relationship(
        relationships.begin()); relationship != relationships.end();
        ++relationship) {
        const QString from(relationship->fromColumn());
        const QString toTable(relationship->toTable());
        const QString toPrimaryKey(relationship->toPrimaryKey());

        queryBuilder.equiJoinWith(toTable,
            QString("%1.%2").arg(toTable).arg(toPrimaryKey),
            QString("%1.%2").arg(tableName()).arg(from));
    }
}

void GlomLayoutModel::createProjectionFromLayoutGroup(
    const Glom::sharedptr<const Glom::LayoutItem> &layoutItem)
{
    Glom::sharedptr<const Glom::LayoutGroup> layoutGroup(
        Glom::sharedptr<const Glom::LayoutGroup>::cast_dynamic(layoutItem));
    if (layoutGroup) {
        const Glom::LayoutGroup::type_list_const_items
            items(layoutGroup->get_items());
        for (Glom::LayoutGroup::type_list_const_items::const_iterator
            iter(items.begin()); iter != items.end(); ++iter) {
            Glom::sharedptr<const Glom::LayoutItem> layoutItem(*iter);
            if (!layoutItem) {
                // Skip if the layoutItem is invalid. Can this happen?
                continue;
            }

            /* The default setting for a projection is
               "table.col AS display_name". */
            QString currTableName(tableName());
            QString currColName(ustringToQstring(layoutItem->get_name()));
            QString currDisplayName(
                ustringToQstring(layoutItem->get_title_or_name()));

            queryBuilder.addProjection(QString("%1.%2 AS \"%3\"")
                .arg(currTableName).arg(currColName)
                .arg(currDisplayName));
        }
    }
}

QVariant GlomLayoutModel::data(const QModelIndex &index, int role) const
{
    static const QRegExp matchDouble("\\d+\\.\\d+");

    if(!index.isValid() && Qt::DisplayRole != role) {
        return QVariant();
    }

    const QVariant &data = QSqlTableModel::data(index, role);

    if(matchDouble.exactMatch(data.toString())) {
        QStringList doubleParts = data.toString().split(".");
        QString reduceMe = doubleParts[1];
        while(reduceMe.endsWith("0")) {
            reduceMe.chop(1);
        }

        if(!reduceMe.isEmpty()) {
            return QVariant(QString("%1.%2").arg(doubleParts[0], reduceMe));
        } else {
            return QVariant(doubleParts[0]);
        }
    }

    return QSqlTableModel::data(index, role);
}
