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

#include "glom_document.h"
#include "qlom_error.h"
#include "glom_tables_model.h"
#include "connection_dialog.h"
#include "utils.h"

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringListModel>

#include <iostream>

#include <libglom/document/document.h>
#include <glibmm/convert.h>

#include "config.h"

GlomDocument::GlomDocument(QObject *parent) :
    document(0)
{
    /* No document case. */
    connect(this, SIGNAL(errorRaised(QlomError)),
        parent, SLOT(receiveError(QlomError)));
}

bool GlomDocument::loadDocument(const QString &filepath)
{
    // filepathToUri raises an error if it fails.
    const std::string uri(filepathToUri(filepath));
    if(uri.empty()) {
        qCritical("Empty file URI");
        return false;
    }

    // Load a Glom document with a given file URI.
    document = new Glom::Document();
    document->set_file_uri(uri);
    int failure_code = 0;
    const bool test = document->load(failure_code);
    if(!test) {
        qCritical("Document loading failed with uri %s", uri.c_str());
        const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("libglom failed to load the Glom document"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
        return false;
    }

    /* Check that the document is not an example document, which must be
     * resaved — that would be an extra feature. */
    if(document->get_is_example_file()) {
        qCritical("Document is an example file, cannot process");
        const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Cannot open the document because it is an example file"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
        return false;
    }

    /* Check that the document is not self-hosting, because that would require
     * starting/stopping PostgreSQL. */
    switch (document->get_hosting_mode()) {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
    {
        // TODO: request a connection dialog from MainWindow.
        ConnectionDialog *connectionDialog = new ConnectionDialog(*document);
        if(connectionDialog->exec() != QDialog::Accepted) {
            const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
                tr("Failed to connect to the PostgreSQL server"),
                Qlom::CRITICAL_ERROR_SEVERITY);
            raiseError(error);
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_SQLITE:
    {
        if(!openSqlite()) {
            const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
                tr("Failed to open the SQLite database"),
                Qlom::CRITICAL_ERROR_SEVERITY);
            raiseError(error);
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_POSTGRES_SELF:
    // Fall through.
    default:
        qCritical("Database type not supported, cannot process");
        const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Database type not supported, failed to open the Glom document"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
        return false;
        break;
    }

    fillTableList();
    return true;
}

GlomTablesModel* GlomDocument::createTablesModel()
{
    return new GlomTablesModel(tableList, qobject_cast<QObject*>(this));
}

GlomLayoutModel* GlomDocument::createListLayoutModel(const QString &tableName)
{
    QList<GlomTable>::const_iterator table(tableList.begin());
    for (; table != tableList.end()
        && table->tableName() != tableName; ++table) {
    }
    Q_ASSERT(table != tableList.end());

    return new GlomLayoutModel(document, *table, qobject_cast<QObject*>(this));
}

GlomLayoutModel* GlomDocument::createDefaultTableListLayoutModel()
{
    QList<GlomTable>::const_iterator table(tableList.begin());
    for (; table != tableList.end() &&
        table->tableName() != ustringToQstring(document->get_default_table());
        ++table) {
    }
    Q_ASSERT(table != tableList.end());

    return new GlomLayoutModel(document, *table, qobject_cast<QObject*>(this));
}

std::string GlomDocument::filepathToUri(const QString &theFilepath)
{
    const std::string filepath(theFilepath.toUtf8().constData());
    std::string uri;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        uri = Glib::filename_to_uri(filepath);
    }
    catch(const Glib::ConvertError& convertException) {
        qCritical("Exception from Glib::filename_to_uri(): %s",
            convertException.what().c_str());
        const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    uri = Glib::filename_to_uri(filepath, convertError);
    if(convertError.get()) {
        qCritical("Error from Glib::filename_to_uri(): %s",
            convertError->what());
        const QlomError error(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
    }
#endif /* GLIBMM_EXCEPTIONS_ENABLED */

    return uri;
}

bool GlomDocument::openSqlite()
{
    const QString backend("QSQLITE");
    QSqlDatabase db(QSqlDatabase::addDatabase(backend));

    const QSqlError sqlError(db.lastError());
    if (sqlError.isValid()) {
        // TODO: Give feedback in the UI.
        qCritical("Database backend \"%s\" does not exist\nError details: %s",
            backend.toUtf8().constData(),
            sqlError.text().toUtf8().constData());
        const QlomError error(Qlom::DATABASE_ERROR_DOMAIN,
            tr("%1 database backend does not exist. Further details:\n%2")
                .arg(backend).arg(sqlError.text()),
                Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
        return false;
    }

    // Build SQLite database absolute path.
    QString filename;
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        filename = ustringToQstring(Glib::filename_to_utf8(
            Glib::filename_from_uri(
                document->get_connection_self_hosted_directory_uri())));
    }
    catch (const Glib::ConvertError &convertException) {
        qCritical("Exception from Glib::filename_to_uri(): %s",
            convertException.what().c_str());
        return false;
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    std::string stdFilename(Glib::filename_from_uri(
        document->get_connection_self_hosted_directory_uri(),
        convertError));
    if (convertError.get()) {
        qCritical("Error from Glib::filename_from_uri(): %s",
            convertError->what());
        return false;
    }

    filename = ustringToQstring(Glib::filename_to_utf8(stdFilename,
        convertError));
    if (convertError.get()) {
        qCritical("Error from Glib::filename_to_utf8(): %s",
            convertError->what());
        return false;
    }
#endif /* GLIBMM_EXCEPTIONS_ENABLED */
    filename.push_back('/'); // Qt only supports '/' as a path separator.
    filename.push_back(ustringToQstring(document->get_connection_database()));
    filename.push_back(".db"); // Yes, really.

    /* Check if the database exists, as otherwise a new, blank database will be
         created and the open will succeed. */
    const QFile sqliteDb(filename);
    if (sqliteDb.exists()) {
        db.setDatabaseName(filename);
    } else {
        qCritical("Sqlite database does not exist");
        const QlomError error(Qlom::DATABASE_ERROR_DOMAIN,
            tr("The SQLite database listed in the Glom document does not exist"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        raiseError(error);
        return false;
    }

    if (!db.open()) {
        qCritical("Database connection could not be opened");
        return false;
    } else {
        return true;
    }
}

void GlomDocument::fillTableList()
{
    if (tableList.empty()) {
        const std::vector<Glib::ustring> tables(document->get_table_names());
        for(std::vector<Glib::ustring>::const_iterator iter(tables.begin());
            iter != tables.end(); ++iter) {
            Glom::Document::type_vec_relationships documentRelationships(
                document->get_relationships(*iter));
            // Get a list of GlomRelationship items from the document.
            QList<GlomRelationship> relationships(
                fillRelationships(documentRelationships));
            // Fill the GlomDocument with a list of GlomTables.
            tableList.push_back(GlomTable(ustringToQstring(*iter),
                ustringToQstring(document->get_table_title(*iter)),
                relationships));
        }
    } else {
        qWarning("Filling tableList when it it not empty");
        tableList.clear();
        fillTableList(); // Recurse.
    }

    return;
}

QList<GlomRelationship> GlomDocument::fillRelationships(
    const Glom::Document::type_vec_relationships &documentRelationships)
{
    QList<GlomRelationship> relationships;
    for (Glom::Document::type_vec_relationships::const_iterator iter(
        documentRelationships.begin()); iter != documentRelationships.end();
        ++iter) {
        const Glom::sharedptr<const Glom::Relationship>
            documentRelationship(*iter);
        relationships.push_back(GlomRelationship(
            ustringToQstring(documentRelationship->get_from_field()),
            ustringToQstring(documentRelationship->get_to_table()),
            ustringToQstring(documentRelationship->get_to_field())));
    }

    return relationships;
}
