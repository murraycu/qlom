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

#include "glom_tables_model.h"
#include "utils.h"

#include <libglom/document/document.h>

GlomTablesModel::GlomTablesModel(const Glom::Document *document,
    QObject *parent) :
    QAbstractListModel(parent),
    glomDoc(document)
{
    // Read out table names from document, and add them to the model.
    const std::vector<Glib::ustring> tables(glomDoc->get_table_names());
    for(std::vector<Glib::ustring>::const_iterator iter(tables.begin());
        iter != tables.end(); ++iter) {
        tableNames.push_back(ustringToQstring(*iter));
        tableDisplayNames.push_back(ustringToQstring(
              glomDoc->get_table_title(*iter)));
    }
}

int GlomTablesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return tableNames.length();
}

QVariant GlomTablesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(index.row() >= tableNames.length()) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return tableDisplayNames.at(index.row());
        break;
    case Qlom::TableNameRole:
        return tableNames.at(index.row());
        break;
    default:
        return QVariant();
        break;
    }
}

QVariant GlomTablesModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return tr("Table titles");
        }
    }

    return QAbstractListModel::headerData(section, orientation, role);
}
