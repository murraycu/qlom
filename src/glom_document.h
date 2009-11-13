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

#ifndef QLOM_GLOM_DOCUMENT_H_
#define QLOM_GLOM_DOCUMENT_H_

namespace Glom
{
class Document;
};

#include "glom_tables_model.h"
#include "glom_layout_model.h"

#include <string>
#include <QString>

class GlomDocument : public QObject
{
    Q_OBJECT;

public:
    GlomDocument();

    /** Load a Glom document from a file.
     *  @param[in] filepath the location of the Glom document
     *  @returns true on success, false on failure */
    bool loadDocument(const QString &filepath);

    /** Get a list of tables in the document.
     *  @returns a model of the tables in the document */
    GlomTablesModel* createTablesModel();

    /** Get a layout from the document.
     *  @returns a model of the layout */
    GlomLayoutModel* createListLayoutModel(const QString &tableName);

    /** Get a layout of the default table from the document.
     *  @returns a model of the layout */
    GlomLayoutModel* createDefaultTableListLayoutModel();

private:
    std::string filepathToUri(const QString &theFilepath);
    /** Open an SQLite database connection.
     *  Creates and opens a default QSqlDatabase connection.
     *
     *  @see ConnectionDialog */
    bool openSqlite();
    Glom::Document *document; /**< a Glom document */
};

#endif /* QLOM_GLOM_DOCUMENT_H_ */
