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

bool GlomDocument::loadDocument(const QString &filepath)
{
    /* Load a Glom document with a given filename. */
    const std::string uri(filepathToUri(filepath));
    if(uri.empty()) {
        return false;
    }

    document = new Glom::Document();
    document->set_file_uri(uri);
    int failure_code = 0;
    const bool test = document->load(failure_code);
    if(!test) {
        std::cerr << "Document loading failed with uri=" << uri << std::endl;
        return false;
    }

    /* Check that the document is not an example document, which must be
     * resaved — that would be an extra feature. */
    if(document->get_is_example_file()) {
        std::cerr << "Document is an example file, cannot process" << std::endl;
        return false;
    }

    /* Check that the document is not self-hosting, because that would require
     * starting/stopping PostgreSQL. */
    switch (document->get_hosting_mode()) {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
    {
        ConnectionDialog *connectionDialog = new ConnectionDialog(*document);
        if(connectionDialog->exec() != QDialog::Accepted) {
            QMessageBox::critical(0, PACKAGE_NAME,
                tr("Could not connect to database server"), QMessageBox::Ok,
                QMessageBox::NoButton, QMessageBox::NoButton);
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_SQLITE:
    {
        if(!openSqlite()) {
            return false;
        }
    }
    break;
    case Glom::Document::HOSTING_MODE_POSTGRES_SELF:
    // Fall through.
    default:
        std::cerr << "Database type not supported, cannot process" << std::endl;
        return false;
        break;
    }

    return true;
}

GlomTablesModel* GlomDocument::tablesModel()
{
    return new GlomTablesModel(document, qobject_cast<QObject*>(this));
}

GlomLayoutModel* GlomDocument::listLayoutModel(const QString &tableName)
{
    return new GlomLayoutModel(document, tableName,
        qobject_cast<QObject*>(this));
}

std::string GlomDocument::filepathToUri(const QString &theFilepath)
{
    std::string filepath(theFilepath.toUtf8().constData());
    std::string uri;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        uri = Glib::filename_to_uri(filepath);
    }
    catch(const Glib::ConvertError& convertException) {
        std::cerr << "Exception from Glib::filename_to_uri(): "
            << convertException.what() << std::endl;
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    uri = Glib::filename_to_uri(filepath, convertError);
    if(convertError.get()) {
        std::cerr << "Error from Glib::filename_to_uri(): "
            << convertError->what() << std::endl;
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
        std::cerr << "Database backend \"" << backend.toUtf8().constData()
          << "\" does not exist\n" << "Error details: "
          << error.text().toUtf8().constData() << std::endl;
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
        std::cerr << "Exception from Glib::filename_to_uri(): "
            << convertException.what() << std::endl;
        return false;
    }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
    std::auto_ptr<Glib::Error> convertError;
    std::string stdFilename(Glib::filename_from_uri(
        document->get_connection_self_hosted_directory_uri(),
        convertError));
    if (convertError.get()) {
        std::cerr << "Error from Glib::filename_from_uri(): "
            << convertError->what() << std::endl;
        return false;
    }

    filename = ustringToQstring(Glib::filename_to_utf8(stdFilename,
        convertError));
    if (convertError.get()) {
        std::cerr << "Error from Glib::filename_to_utf8(): "
            << convertError->what() << std::endl;
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
        std::cerr << "Sqlite database does not exist" << std::endl;
        return false;
    }

    if (!db.open()) {
        std::cerr << "Database connection could not be opened" << std::endl;
        return false;
    } else {
        return true;
    }
}
