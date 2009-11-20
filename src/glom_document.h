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
#include "glom_table.h"
#include "error_reporter.h"

#include <memory>
#include <string>
#include <QList>
#include <QString>

class GlomDocument : public ErrorReporter
{
    Q_OBJECT

public:
    GlomDocument(QObject *parent = 0);

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
    /** Convert a filepath to a URI.
     *  @param[in] theFilepath the absolute filepath
     *  @returns the URI of the file */
    std::string filepathToUri(const QString &theFilepath);

    /** Open an SQLite database connection.
     *  Creates and opens a default QSqlDatabase connection.
     *
     *  @see ConnectionDialog */
    bool openSqlite();

    /** Fill tableList with tables read from the document. */
    void fillTableList();

    /** Fill a GlomTable with relationships from the document.
     *  @param[in] documentRelationships a list of relationships from the
     document
     *  @returns a list of relationships */
    QList<GlomRelationship> fillRelationships(
        Glom::Document::type_vec_relationships documentRelationships);

    Glom::Document *document; /**< a Glom document */
    QList<GlomTable> tableList; /**< a list of table in the document */
};

#endif /* QLOM_GLOM_DOCUMENT_H_ */
