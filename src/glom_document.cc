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

GlomDocument::GlomDocument() :
    document(0)
{
    /* No document case. */
}

bool GlomDocument::loadDocument(const QString &filepath, std::auto_ptr<QlomError> &error)
{
    /* Load a Glom document with a given filename. */
    const std::string uri(filepathToUri(filepath, error));
    if(uri.empty()) {
        return false;
    }

    document = new Glom::Document();
    document->set_file_uri(uri);
    int failure_code = 0;
    const bool test = document->load(failure_code);
    if(!test) {
        qCritical("Document loading failed with uri %s", uri.c_str());
        error = std::auto_ptr<QlomError>(
            new QlomError(tr("libglom failed to load the Glom document"),
                Qlom::CRITICAL_ERROR_SEVERITY));
        return false;
    }

    /* Check that the document is not an example document, which must be
     * resaved — that would be an extra feature. */
    if(document->get_is_example_file()) {
        qCritical("Document is an example file, cannot process");
        error = std::auto_ptr<QlomError>(new QlomError(
            tr("Cannot open the document because it is an example file"),
            Qlom::CRITICAL_ERROR_SEVERITY));
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
            error = std::auto_ptr<QlomError>(new QlomError(
                tr("Failed to connect to the PostgreSQL server"),
                Qlom::CRITICAL_ERROR_SEVERITY));
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_SQLITE:
    {
        if(!openSqlite()) {
            error = std::auto_ptr<QlomError>(new QlomError(
                tr("Failed to open the SQLite database"),
                Qlom::CRITICAL_ERROR_SEVERITY));
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_POSTGRES_SELF:
    // Fall through.
    default:
        qCritical("Database type not supported, cannot process");
        error = std::auto_ptr<QlomError>(new QlomError(
            tr("Database type not supported, failed to open the Glom document"),
            Qlom::CRITICAL_ERROR_SEVERITY));
        return false;
        break;
    }

    return true;
}

GlomTablesModel* GlomDocument::createTablesModel()
{
    return new GlomTablesModel(document, qobject_cast<QObject*>(this));
}

GlomLayoutModel* GlomDocument::createListLayoutModel(const QString &tableName)
{
    return new GlomLayoutModel(document, tableName,
        qobject_cast<QObject*>(this));
}

GlomLayoutModel* GlomDocument::createDefaultTableListLayoutModel()
{
    return new GlomLayoutModel(document,
        ustringToQstring(document->get_default_table()),
            qobject_cast<QObject*>(this));
}

std::string GlomDocument::filepathToUri(const QString &theFilepath,
   std::auto_ptr<QlomError> &error)
{
    std::string filepath(theFilepath.toUtf8().constData());
    std::string uri;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        uri = Glib::filename_to_uri(filepath);
    }
    catch(const Glib::ConvertError& convertException) {
        qCritical("Exception from Glib::filename_to_uri(): %s",
            convertException.what().c_str());
        error = std::auto_ptr<QlomError>(new QlomError(
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY));
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    uri = Glib::filename_to_uri(filepath, convertError);
    if(convertError.get()) {
        qCritical("Error from Glib::filename_to_uri(): %s",
            convertError->what());
        error = std::auto_ptr<QlomError>(new QlomError(
            tr("Failed to convert the document file name to a URI"),
            Qlom::CRITICAL_ERROR_SEVERITY));
    }
#endif /* GLIBMM_EXCEPTIONS_ENABLED */

    return uri;
}

bool GlomDocument::openSqlite()
{
    const QString backend("QSQLITE");
    QSqlDatabase db(QSqlDatabase::addDatabase(backend));

    const QSqlError error(db.lastError());
    if (error.isValid()) {
        // TODO: Give feedback in the UI.
        qCritical("Database backend \"%s\" does not exist\nError details: %s",
            backend.toUtf8().constData(), error.text().toUtf8().constData());
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
        return false;
    }

    if (!db.open()) {
        qCritical("Database connection could not be opened");
        return false;
    } else {
        return true;
    }
}
