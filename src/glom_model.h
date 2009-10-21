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

namespace Glom
{
class Document;
};

namespace Qlom
{

/** Roles for GlomModel. */
enum GlomModelRoles {
    TableNameRole = 32 /**< retrieves the table name from the model */
};

} // namespace Qlom

class GlomModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** Construct a new model from a Glom document.
     *  @param[in] document a Glom document to read metadata from
     *  @param[in] parent a parent QObject, that will take ownership of the
     model */
    explicit GlomModel(const Glom::Document& document, QObject *parent = 0);

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
    const Glom::Document &glomDoc; /**< the Glom document */
    QStringList tableNames; /**< the list of tables in the Glom document */
};

#endif /* QLOM_GLOM_MODEL_H_ */
