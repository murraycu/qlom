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

#include "glom_table_model.h"

GlomTableModel::GlomTableModel(const Glom::Document &document,
  QString table_name, QObject *parent) :
  QAbstractTableModel(parent)
{
  // Read out field and header names from document, and add them to the model.
  const Glom::Document::type_vec_fields table_fields =
    document.get_table_fields(table_name.toUtf8().data());
  for(Glom::Document::type_vec_fields::const_iterator iter =
    table_fields.begin(); iter != table_fields.end(); ++iter)
  {
    Glom::sharedptr<Glom::Field> field = *iter;
    if(!field)
    {
      continue;
    }

    header_model.append(QString::fromUtf8(field->get_name().c_str()));
    table_model.append(QStringList(tr("blank")));
  }
}

int GlomTableModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return table_model.length();
}

int GlomTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  // Return the number of header items, i.e. the total number of columns.
  return header_model.length();
}

QVariant GlomTableModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if(orientation == Qt::Horizontal)
  {
    if(role == Qt::DisplayRole)
    {
      return header_model.at(section);
    }
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant GlomTableModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  if(index.row() >= table_model.length())
  {
    return QVariant();
  }

  QStringList table_record = table_model.at(index.row());

  if(role == Qt::DisplayRole || role == Qt::EditRole)
  {
    /* Currently, there is no data in the model.
    return table_record.at(index.column());
    */
    return QVariant();
  }
  if(role == Qt::ToolTipRole)
  {
    QString tip("<table>");
    QString key;
    QString value;
    const int max_lines = table_record.count();
    for(int i = 0; i < max_lines; ++i)
    {
      key = headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
      value = table_record.at(i);
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
