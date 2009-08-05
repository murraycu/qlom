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

#include "glom_model.h"

GlomModel::GlomModel(const Glom::Document& document, QObject *parent) :
  QAbstractListModel(parent)
{
  // Read out table names from document, and add them to the model.
  const std::vector<Glib::ustring> tables = document.get_table_names();
  for(std::vector<Glib::ustring>::const_iterator iter = tables.begin();
    iter != tables.end(); ++iter)
  {
    table_names.push_back(QString::fromUtf8(iter->c_str()));
  }
}

int GlomModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return table_names.length();
}

QVariant GlomModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }
  if(index.row() >= table_names.length())
  {
    return QVariant();
  }
  if(role == Qt::DisplayRole)
  {
    return table_names.at(index.row());
  }
  else
  {
    return QVariant();
  }
}

QVariant GlomModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if(orientation == Qt::Horizontal)
  {
    if(role == Qt::DisplayRole)
    {
      return tr("Table names");
    }
  }

  return QAbstractListModel::headerData(section, orientation, role);
}
