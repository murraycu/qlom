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
#include "utils.h"

#include <libglom/document/document.h>
#include <libglom/init.h>

GlomModel::GlomModel(const Glom::Document& document, QObject *parent) :
  QAbstractListModel(parent),
  glom_doc(document)
{
  // Read out table names from document, and add them to the model.
  const std::vector<Glib::ustring> tables(document.get_table_names());
  for(std::vector<Glib::ustring>::const_iterator iter(tables.begin());
    iter != tables.end(); ++iter)
  {
    table_names.push_back(ustring_to_qstring(*iter));
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
  switch(role)
  {
    case Qt::DisplayRole:
      return ustring_to_qstring(glom_doc.get_table_title(qstring_to_ustring(table_names.at(index.row()))));
      break;
    case Qlom::TableNameRole:
      return table_names.at(index.row());
    default:
      return QVariant();
      break;
  }
}

QVariant GlomModel::headerData(int section, Qt::Orientation orientation,
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
