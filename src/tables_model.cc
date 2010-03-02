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

#include "tables_model.h"

QlomTablesModel::QlomTablesModel(QList<QlomTable> tableList, QObject *parent) :
    QAbstractListModel(parent),
    theTableList(tableList)
{
}

int QlomTablesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return theTableList.length();
}

QVariant QlomTablesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(index.row() >= theTableList.length()) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return theTableList.at(index.row()).displayName();
        break;
    case Qlom::TableNameRole:
        return theTableList.at(index.row()).tableName();
        break;
    default:
        return QVariant();
        break;
    }
}

QVariant QlomTablesModel::headerData(int section, Qt::Orientation orientation,
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
