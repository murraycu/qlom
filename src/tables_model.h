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

#ifndef QLOM_TABLES_MODEL_H_
#define QLOM_TABLES_MODEL_H_

#include "table.h"

#include <QAbstractListModel>
#include <QList>
#include <QStringList>

namespace Qlom
{

/** Data display roles for GlomTablesModel. */
enum TablesModelRoles {
    TableNameRole = 32 /**< retrieves the table name from the model */
};

} // namespace Qlom

/** A model of the tables in a Glom document.
 *  The model contains a list of Table objects, which contain both the name of
 *  the tables for display to the user and for accessing the database.
 *  Currently, the QlomTable list is not stored in the model, and is only used
 *  for populating the model with data at construction time. */
class QlomTablesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** Construct a new model from a Glom document.
     *  @param[in] tableList a list of QlomTable objects to read information
     *  from
     *  @param[in] parent a parent QObject, that will take ownership of the
     *  model */
    explicit QlomTablesModel(const QList<QlomTable> tableList,
        QObject *parent = 0);

    /** Calculate the number of rows in the model.
     *  @param[in] parent the parent index in the heirarchy
     *  @returns the number of rows in the model */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /** Retrieve data from the model.
     *  @param[in] index the index to retrieve data from
     *  @param[in] role the Qt::ItemDataRole to retrieve data for
     *  @returns the data from the model */
    QVariant data(const QModelIndex &index, int role) const;

    /** Retrieve the column titles from the model.
     *  @param[in] section the index of the header item
     *  @param[in] orientation the orientation of the header
     *  @param[in] role the Qt::ItemDataRole to retrieve header data for
     *  @returns the header data from the model */
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;

private:
    QList<QlomTable> theTableList; /**< the list of tables in the Glom
                                    *   document */
};

#endif /* QLOM_TABLES_MODEL_H_ */
