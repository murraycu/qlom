/* Qlom is copyright Openismus GmbH, 2009, 2010.
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

#include "document.h"
#include "error.h"
#include "tables_model.h"
#include "list_layout_model.h"
#include "connection_dialog.h"
#include "utils.h"

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QPointer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringListModel>
#include <QFileInfo>

#include <iostream>

#include <libglom/document/document.h>
#include <glibmm/convert.h>

#include "config.h"

QlomDocument::QlomDocument(QObject *parent) :
    QObject(parent),
    document(0)
{
    /* No document case. */
 }

bool QlomDocument::loadDocument(const QString &filepath)
{
    QFileInfo info(filepath);
    if(!info.exists()) {
        qWarning("The file does not exist with filepath %s",
            qPrintable(filepath));

        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("The file does not exist"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        return false;
    }

    // filepathToUri provides an error if it fails.
    const std::string uri(filepathToUri(filepath));
    if(uri.empty()) {
        return false;
    }

    // Load a Glom document with a given file URI.
    document = new Glom::Document();
    document->set_file_uri(uri);
    int failure_code = 0;
    const bool test = document->load(failure_code);
    if(!test) {
        qWarning("Document loading failed with uri %s (failure_code=%d)", 
            uri.c_str(), failure_code);

        QString message;
        if(failure_code == Glom::Document::LOAD_FAILURE_CODE_FILE_VERSION_TOO_NEW) {
           message = tr("The document's format is too new.");
        }
        else {
           message = tr("libglom failed to load the Glom document");
        }
       
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN, message,
            Qlom::CRITICAL_ERROR_SEVERITY);
        return false;
    }

    /* Check that the document is not an example document, which must be
     * resaved — that would be an extra feature. */
    if(document->get_is_example_file()) {
        qWarning("Document is an example file, cannot process");
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Cannot open the document because it is an example file"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        return false;
    }

    /* Check that the document is not self-hosting, because that would require
     * starting/stopping PostgreSQL. */
    switch (document->get_hosting_mode()) {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
    {
        /* Modal dialogs can be deleted by code elsewhere, hence this resource
         * is wrapped in a QPointer. However, it's only correct if *each*
         * access to this resource is 0-checked afterwards. See
         * http://www.kdedevelopers.org/node/3918#comment-8645 */
        QPointer<QlomConnectionDialog> connectionDialog =
                new QlomConnectionDialog(*document);
        bool keepOffering = true;
        bool connected = false;

        while (keepOffering) {
            // TODO: request a connection dialog from MainWindow.

            const int result = connectionDialog->exec();
            if (result == QDialog::Accepted) {
                if (connectionDialog) {
                    if (connectionDialog->connectionWasSuccessful()) {
                        // The connection worked with these settings.
                        connected = true;
                        keepOffering = false;
                    } else {
                        // Let the user try again, until he clicks Cancel.
                        keepOffering = true;
                    }
                }
            } else if (result == QDialog::Rejected) {
                // The user cancelled the dialog.
                keepOffering = false;
            } else {
                qWarning("GlomDocument::loadDocument() Unexpected exec() result.");
            }
        }
            
        delete connectionDialog;
        connectionDialog = 0;

        if (!connected) {
            qWarning("Failed to connect to the PostgreSQL server");
            theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
                tr("Failed to connect to the PostgreSQL server"),
                Qlom::CRITICAL_ERROR_SEVERITY);
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_SQLITE:
    {
        if(!openSqlite()) {
            qWarning("Failed to open the SQLite database");
            theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
                tr("Failed to open the SQLite database"),
                Qlom::CRITICAL_ERROR_SEVERITY);
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_POSTGRES_SELF:
    // Fall through.
    default:
        qWarning("Database type not supported, cannot process");
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Database type not supported, failed to open the Glom document"),
            Qlom::CRITICAL_ERROR_SEVERITY);
        return false;
        break;
    }

    fillTableList();
    return true;
}

QlomTablesModel * QlomDocument::createTablesModel()
{
    return new QlomTablesModel(tableList, qobject_cast<QObject*>(this));
}

QlomListLayoutModel * QlomDocument::createListLayoutModel(
    const QString &tableName)
{
    for (typeTableList::const_iterator iter = tableList.begin();
         iter != tableList.end();
         ++iter) {
        if ((*iter).tableName() == tableName) {
            bool error = false;
            QlomListLayoutModel *model = new QlomListLayoutModel(document, *iter, error, this);
            if (error) {
                qWarning("GlomLayoutModel: no list model found");
                theLastError = QlomError(Qlom::DATABASE_ERROR_DOMAIN,
                    tr("%1: no list model found").arg("GlomLayoutModel"),
                    Qlom::WARNING_ERROR_SEVERITY);
                delete model;
                model = 0;
                return 0;
            }
            return model;
        }
    }

    /* TODO: change from critical to warning once Qlom can handle failed model
     * initialisations. */
    qWarning("Cannot find requested table.");
    theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
        tr("Cannot find requested table."), Qlom::CRITICAL_ERROR_SEVERITY);

    return 0;
}

QlomListLayoutModel * QlomDocument::createDefaultTableListLayoutModel()
{
    Q_ASSERT(document);

    QString defaultTable = ustringToQstring(document->get_default_table());

    // Don't try to show a non-hidden table even if it is specified as the 
    // default table.
    if (document->get_table_is_hidden(qstringToUstring(defaultTable)))
        defaultTable.clear();

    //Use the first non-hidden table if a default table is not specified.
    if (defaultTable.isEmpty()) {
        for (typeTableList::const_iterator iter = tableList.begin();
            iter != tableList.end(); ++iter) {
            const QlomTable& table = *iter;
            const QString tableName = table.tableName();
            if(!(document->get_table_is_hidden(qstringToUstring(tableName)))) {
                defaultTable = tableName;
                break;
            }
       }
    }

    if(defaultTable.isEmpty()) {
        qWarning("The document contains no non-hidden tables.");
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("The document contains no non-hidden tables."),
            Qlom::CRITICAL_ERROR_SEVERITY);
        return 0; //There were no non-hidden tables.
    }

    // Create the model for the table:
    // TODO: this code path needs testing, when it finds default tables and
    // when not.
    return createListLayoutModel(defaultTable);
}

QlomError QlomDocument::lastError() const
{
    return theLastError;
}

std::string QlomDocument::filepathToUri(const QString &theFilepath)
{
    const std::string filepath(theFilepath.toUtf8().constData());
    std::string uri;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        uri = Glib::filename_to_uri(filepath);
    }
    catch(const Glib::ConvertError& convertException) {
        qWarning("Exception from Glib::filename_to_uri(): %s",
            convertException.what().c_str());
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY);
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    uri = Glib::filename_to_uri(filepath, convertError);
    if(convertError.get()) {
        qWarning("Error from Glib::filename_to_uri(): %s",
            convertError->what());
        theLastError = QlomError(Qlom::DOCUMENT_ERROR_DOMAIN,
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY);
    }
#endif /* GLIBMM_EXCEPTIONS_ENABLED */

    return uri;
}

bool QlomDocument::openSqlite()
{
    const QString backend("QSQLITE");
    QSqlDatabase db(QSqlDatabase::addDatabase(backend));

    const QSqlError sqlError(db.lastError());
    if (sqlError.isValid()) {
        qWarning("Database backend \"%s\" does not exist\nError details: %s",
            qPrintable(backend),
            qPrintable(sqlError.text()));
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
        qWarning("Exception from Glib::filename_to_uri(): %s",
            convertException.what().c_str());
        return false;
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    std::string stdFilename(Glib::filename_from_uri(
        document->get_connection_self_hosted_directory_uri(),
        convertError));
    if (convertError.get()) {
        qWarning("Error from Glib::filename_from_uri(): %s",
            convertError->what());
        return false;
    }

    filename = ustringToQstring(Glib::filename_to_utf8(stdFilename,
        convertError));
    if (convertError.get()) {
        qWarning("Error from Glib::filename_to_utf8(): %s",
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
        qWarning("The SQLite database listed in the Glom document does not exist");
        return false;
    }

    if (!db.open()) {
        qWarning("Database connection could not be opened");
        return false;
    } else {
        return true;
    }
}

void QlomDocument::fillTableList()
{
    Q_ASSERT(tableList.empty());

    GlomTables tables = document->get_tables();
    for(GlomTables::const_iterator iter(tables.begin());
        iter != tables.end(); ++iter) {
        std::shared_ptr<Glom::TableInfo> table = *iter;
        Glom::Document::type_vec_relationships documentRelationships(
            document->get_relationships(table->get_name()));
        // Get a list of GlomRelationship items from the document.
        QList<QlomRelationship> relationships(
            fillRelationships(documentRelationships));

        // Check for flags.
        QFlags<QlomTable::QlomTableFlags> flags;
        if (table->get_hidden())
            flags |= QlomTable::HIDDEN_TABLE;
        if (table->get_default())
            flags |= QlomTable::DEFAULT_TABLE;

        // Fill the GlomDocument with a list of GlomTables.
        tableList.push_back(QlomTable(ustringToQstring(table->get_name()),
            ustringToQstring(document->get_table_title(table->get_name())),
            relationships, flags));
    }
}

QList<QlomRelationship> QlomDocument::fillRelationships(
    const Glom::Document::type_vec_relationships &documentRelationships)
{
    QList<QlomRelationship> relationships;
    for (Glom::Document::type_vec_relationships::const_iterator iter(
        documentRelationships.begin()); iter != documentRelationships.end();
        ++iter) {
        const std::shared_ptr<const Glom::Relationship>
            documentRelationship(*iter);
        relationships.push_back(QlomRelationship(
            ustringToQstring(documentRelationship->get_from_field()),
            ustringToQstring(documentRelationship->get_to_table()),
            ustringToQstring(documentRelationship->get_to_field())));
    }

    return relationships;
}
