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

#ifndef QLOM_GLOM_LAYOUT_MODEL_H_
#define QLOM_GLOM_LAYOUT_MODEL_H_

#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <libglom/document/document.h>
#include <libglom/init.h>

/** A model to show rows of data from a Glom database. */
class GlomLayoutModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    /** Create a model of the layouts in a table from a Glom document.
     *  @param[in] document a Glom document
     *  @param[in] table_name the table name TODO: Should this be const?
     *  @param[in] parent a parent QObject
     *  @param[in] db a database connection, or the default connection */
    explicit GlomLayoutModel(const Glom::Document &document,
        QString &tableName, QObject *parent = 0,
        QSqlDatabase db = QSqlDatabase());

private:
    /** Read relationships from the Glom document, and apply them to the model.
     *  @param[in] document a Glom document */
    void applyRelationships(const Glom::Document &document);

    /** Discard columns that are not mentioned in the document.
     *  @param[in] layout_item the layout containing the columns that should
     not be discarded */
    void keepLayoutItems(const Glom::sharedptr<Glom::LayoutItem> &layout_item);
};

#endif /* QLOM_GLOM_LAYOUT_MODEL_H_ */
