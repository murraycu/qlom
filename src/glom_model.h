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

#include <QAbstractTableModel>
#include <QStringList>

class GlomModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    GlomModel(Glom::Document document, QObject *parent = 0);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
      int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

  private:
    QList<QStringList> table_model;
};

#endif /* QLOM_GLOM_MODEL_H_ */
