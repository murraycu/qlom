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

#ifndef QLOM_GLOM_MODEL_H_
#define QLOM_GLOM_MODEL_H_

#include <QAbstractListModel>
#include <QStringList>
#include <libglom/document/document.h>
#include <libglom/init.h>

class GlomModel : public QAbstractListModel
{
  Q_OBJECT

  public:
    explicit GlomModel(Glom::Document& document, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
      int role = Qt::DisplayRole) const;

  private:
    QStringList table_names;
};

#endif /* QLOM_GLOM_MODEL_H_ */
