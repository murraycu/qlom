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

GlomModel::GlomModel(Glom::Document document, QObject *parent) :
  QAbstractTableModel(parent)
{
  const std::vector<Glib::ustring> table_names = document.get_table_names();
  for(std::vector<Glib::ustring>::const_iterator iter = table_names.begin();
    iter != table_names.end(); ++iter)
  {
    table_model.append(*iter);
  }
}

int GlomModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return table_model.count();
}

int GlomModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return table_model.at(0).count();
}

QVariant GlomModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if(orientation == Qt::Horizontal)
  {
    if(orientation == Qt::DisplayRole)
    {
      return table_model.at(0).at(section);
    }
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant GlomModel::data(const QModelIndex &index, int role) const
{
  if(index.isValid())
  {
    return QVariant();
  }
  
  QString table_record = table_model.at(index.row());

  if(role == Qt::DisplayRole || role == Qt::EditRole)
  {
    return table_model.at(index.column());
  }
  if(role == Qt::ToolTipRole)
  {
    QString tip("<table>");
    QString key;
    QString value;
    int max_lines = record.count();
    for(int i = 0; i < max_lines; ++i)
    {
      key = headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
      value = record.at(i);
      if(!value.isEmpty())
      {
        tip += QString("<tr><td><b>%1</b>: %2</td></tr>").arg(key, value);
      }
    }
    tip += "</table>";
    return tip;
  }
  return QVariant();
}
