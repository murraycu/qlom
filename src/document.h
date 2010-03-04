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

#ifndef QLOM_DOCUMENT_H_
#define QLOM_DOCUMENT_H_


class QlomListLayoutModel;
class QlomTablesModel;

#include "table.h"
#include "error.h"

#include <memory>
#include <string>

#include <QList>
#include <QObject>
#include <QString>

#include <libglom/document/document.h>

/** A Glom document.
 *  A Glom document contains the information that is in a .glom file. It is
 *  initially blank, but a document can be loaded with the loadDocument()
 *  method. Models are obtained via the create*() calls. createTablesModel()
 *  creates a model for the list of tables in the document.
 *  createListLayoutModel() creates a model for the list layout of a
 *  specified table. createDefaultTableListLayoutModel() creates a model of the
 *  list layout for the default table of the document. As the create prefix
 *  suggests, the responsibility of destroying the models once they are no
 *  longer needed is placed on the caller. */
class QlomDocument : public QObject
{
    Q_OBJECT

public:
    typedef Glom::Document::type_listTableInfo GlomTables; // just an alias

    /** A Glom document.
     *  Constructor for a blank QlomDocument. Before the object is any use,
     *  loadDocument() should be called.
     *  @param[in] parent a parent object, which errors will be sent to */
    QlomDocument(QObject *parent = 0);

    /** Load a Glom document from a file.
     *  Loads a Glom document from a file. This method can be called on a
     *  QlomDocument safely, even if a document has already been loaded.
     *  @param[in] filepath the location of the Glom document as an absolute
     *  filepath.
     *  @returns true on success, false on failure */
    bool loadDocument(const QString &filepath);

    /** Get a list of tables in the document.
     *  Creates a new model of the list of tables in the Glom document. The
     *  model must be destroyed by the caller when it is no longer needed.
     *  @returns a model of the tables in the document */
    QlomTablesModel* createTablesModel();

    /** Get a layout from the document.
     *  Creates a new model of the list layout of the table given in the
     *  tableName, which must match the name of the table in the database. The
     *  model must be destroyed by the caller when it is no longer needed.
     *  @param[in] tableName the name of the table to provide a layout for
     *  @returns a model of the layout */
    QlomListLayoutModel* createListLayoutModel(const QString &tableName);

    /** Get a layout of the default table from the document.
     *  Creates a new model of the list layout of the default table specified
     *  in the Glom document. The model must be destroyed by the caller when it
     *  is no longer needed.
     *  @returns a model of the layout, or 0 if no default table was found */
    QlomListLayoutModel* createDefaultTableListLayoutModel();

    /** Returns the error of the last operation that has failed. */
    QlomError lastError() const;

private:
    /** Convert a filepath to a URI.
     *  Converts an absolute filepath into a file URI. Any errors that occur
     *  are signalled via the errorRaised signal.
     *  @param[in] theFilepath the absolute filepath
     *  @returns the URI of the file */
    std::string filepathToUri(const QString &theFilepath);

    /** Open an SQLite database connection.
     *  Creates and opens a default QSqlDatabase connection. If authentication
     *  details are required, opens a connection dialog via QlomConnectionDialog
     *  that requests them from the user. Any errors that occur are signalled
     *  via the errorRaised signal.
     *  @see QlomConnectionDialog
     *  @returns true on success, false on failure */
    bool openSqlite();

    /** Fill tableList with tables read from the document.
     *  Fills the tableList member with a list of QlomTables read from the Glom
     *  document. In turn, calls fillRelationships() to fill each table with a
     *  list of relationships.
     *  @see fillRelationships() */
    void fillTableList();

    /** Fill a list with relationships from the document.
     *  Fills a QList with GlomRelationships, that can then be used in a
     *  GlomTable.
     *  @param[in] documentRelationships a list of relationships from the
     document, for a single table
     *  @returns a list of relationships */
    QList<QlomRelationship> fillRelationships(
        const Glom::Document::type_vec_relationships &documentRelationships);

    Glom::Document *document; /**< libglom's representaton of a Glom document */
    QlomError theLastError; /**< contains the error of the last failed operation */

    typedef QList<QlomTable> typeTableList;
    typeTableList tableList; /**< a list of tables in the document */
};

#endif /* QLOM_DOCUMENT_H_ */
