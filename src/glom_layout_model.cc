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

GlomLayoutModel::GlomLayoutModel(const Glom::Document& document,
  QString& table_name, QObject *parent, QSqlDatabase db) :
  QSqlRelationalTableModel(parent, db)
{
  setTable(table_name);

  apply_relationships(table_name, document);

  const Glib::ustring table_name_u = qstring_to_ustring(table_name);
  const Glom::Document::type_list_layout_groups list_layout =
    document.get_data_layout_groups("list", table_name_u);
  for(Glom::Document::type_list_layout_groups::const_iterator iter =
    list_layout.begin(); iter != list_layout.end(); ++iter)
  {
    Glom::sharedptr<Glom::LayoutGroup> layout_group = *iter;
    if(!layout_group)
    {
      continue;
    }
    else
    {
      keep_layout_items(layout_group);
    }
    break;
  }

  // Fill the model from the database.
  select();
}

// Apply relationships from Glom document to relational model.
void GlomLayoutModel::apply_relationships(QString& table_name,
  const Glom::Document& document)
{
  const Glib::ustring table_name_u = qstring_to_ustring(table_name);
  Glom::Document::type_vec_relationships relationships =
    document.get_relationships(table_name_u);
  for(Glom::Document::type_vec_relationships::const_iterator iter =
    relationships.begin(); iter != relationships.end(); ++iter)
  {
    const Glom::sharedptr<const Glom::Relationship> relationship = *iter;
    if(!relationship)
    {
      continue;
    }

    const QSqlRecord record = this->record();
    const QString from = ustring_to_qstring(relationship->get_from_field());
    const int index = record.indexOf(from);

    // If the index is invalid, or the primary key, ignore it.
    if(index == -1 || index == 0)
    {
      continue;
    }

    const QString to_table = ustring_to_qstring(relationship->get_to_table());
    const QString to_primary_key =
      ustring_to_qstring(relationship->get_to_field());
    /* TODO: Find a way to automatically retrieve a default field, rather than
             hardcoding the location. */
    const QString to_field = QString("name");
    setRelation(index, QSqlRelation(to_table, to_primary_key, to_field));
  }
}

/* Keep the colums respreseted by the layout items in the model, and discard
   the remaining columns. */
void GlomLayoutModel::keep_layout_items(Glom::sharedptr<Glom::LayoutItem> layout_item)
{
  Glom::sharedptr<Glom::LayoutGroup> layout_group =
    Glom::sharedptr<Glom::LayoutGroup>::cast_dynamic(layout_item);
  if(layout_group)
  {
    QStringList keep_items;
    const Glom::LayoutGroup::type_list_items items = layout_group->get_items();
    for(Glom::LayoutGroup::type_list_items::const_iterator iter = items.begin();
      iter != items.end(); ++iter)
    {
      Glom::sharedptr<Glom::LayoutItem> layout_item = *iter;
      if(!layout_item)
      {
        continue;
      }

      // Only keep these layout items in the model.
      QString keep_item =
        ustring_to_qstring(layout_item->get_layout_display_name());
      if(keep_item.isEmpty())
      {
        continue;
      }
      else
      {
        keep_items.push_back(keep_item);
      }
    }

    for(int index = 0; index < record().count(); ++index)
    {
      // Remove columns from model.
      if(!keep_items.contains(record().fieldName(index)))
      {
        removeColumn(index);
      }
    }
  }
}
