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

#include "main_window.h"
#include "connection_dialog.h"
#include "open_sqlite.h"
#include "utils.h"

#include <iostream>

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

#include <libglom/document/document.h>
#include <libglom/init.h>
#include <glibmm/convert.h>

#include "config.h"

Glom::Document* document = 0;
MainWindow *main_window = 0;

void print_usage()
{
  std::cout << "Usage: glom absolute_file_path" << std::endl;
}

std::string qlom_filepath_to_uri(const QString& filepath_qt)
{
  std::string filepath(filepath_qt.toUtf8().constData());

  /* Qt does not have the concept of file URIs. However, only '/' is supported
     for directory separators, so prepending "file://" to the path and escaping
     the necessary characters should work, i.e. filename_to_uri(). */
  std::string uri;
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try
  {
    uri = Glib::filename_to_uri(filepath);
  }
  catch(const Glib::ConvertError& convert_exception)
  {
    std::cerr << "Exception from Glib::filename_to_uri(): " <<
      convert_exception.what() << std::endl;
  }
#else /* !GLIBMM_EXCEPTIONS_ENABLED */
  std::auto_ptr<Glib::Error> convert_error;
  uri = Glib::filename_to_uri(filepath, convert_error);
  if(convert_error.get())
  {
    std::cerr << "Error from Glib::filename_to_uri(): "
      << convert_error->what() << std::endl;
  }
#endif /* GLIBMM_EXCEPTIONS_ENABLED */

  return uri;
}

/** Load the document and show it in the main window.
 @result true for success, false otherwise
 */
bool load_document(const QString& filepath)
{
  const std::string uri(qlom_filepath_to_uri(filepath));
  if(uri.empty())
  {
    print_usage();
    return false;
  }

  /* The Glom::Document is instantiated dynamically because a static instance
     could be instantiated before libglom has been initialised. */
  document = new Glom::Document();
  document->set_file_uri(uri);
  int failure_code = 0;
  const bool test = document->load(failure_code);
  if(!test)
  {
    std::cerr << "Document loading failed with uri=" << uri << std::endl;
    return false;
  }

  /* Check that the document is not an example document, which must be resaved
     — that would be an extra feature. */
  if(document->get_is_example_file())
  {
    std::cerr << "Document is an example file, cannot process" << std::endl;
    return false;
  }

  /* Check that the document is not self-hosting, because that would require
     starting/stopping PostgreSQL. */
  switch (document->get_hosting_mode())
  {
    case Glom::Document::HOSTING_MODE_POSTGRES_CENTRAL:
      {
        ConnectionDialog *connection_dialog = new ConnectionDialog(*document);
        if(connection_dialog->exec() != QDialog::Accepted)
        {
          QMessageBox::critical(0, qApp->applicationName(),
              "Could not connect to database server", QMessageBox::Ok,
              QMessageBox::NoButton, QMessageBox::NoButton);
          return false;
        }
      }
      break;
    case Glom::Document::HOSTING_MODE_SQLITE:
      {
        if(!openSqlite(*document))
        {
          return false;
        }
      }
      break;
    case Glom::Document::HOSTING_MODE_POSTGRES_SELF:
    default:
      std::cerr << "Database type not supported, cannot process" << std::endl;
      return false;
      break;
  }

  main_window = new MainWindow(*document);
  main_window->show();

  return true;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("openismus");
    QCoreApplication::setOrganizationDomain("openismus.com");
    QCoreApplication::setApplicationName(PACKAGE_NAME);

    Glom::libglom_init();

    QString filepath;
    
    /* Qt does not have an equivalent to GOption, except the arguments() static
       method used below. */
    std::list<QString> options = app.arguments().toStdList();
    if (options.size() >= 2) {
        filepath = *(++options.begin()); 
    }

    // If no filepath was specified on the command line, ask for one in the UI.
    if (filepath.isEmpty()) {
        filepath = QFileDialog::getOpenFileName(0 /* parent window */, 
            "Open Glom file", 
            "", "Glom files (*.glom);;All files (*)");
        if(filepath.isEmpty()) {
          // The user cancelled the dialog.
          return 0;
        }
    }

    if (!load_document(filepath)) {
        // Load the Glom document.
        return 1;
    }

    const int result = app.exec();

    delete main_window;
    delete document;

    return result;
}
