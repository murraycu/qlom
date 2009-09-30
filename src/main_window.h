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

#ifndef QLOM_MAIN_WINDOW_H_
#define QLOM_MAIN_WINDOW_H_

#include <QMainWindow>

class QDialog;
class QModelIndex;
class QSqlRelationalTableModel;
namespace Glom
{
class Document;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:

    /** 
     * @param document a glom document
     */
    MainWindow(const Glom::Document &document);

    virtual ~MainWindow();

  private:
    void show_about_dialog();
    void write_settings();
    void read_settings();
    void setup_table_view(QString table_name);
    void setup_table_model(QSqlRelationalTableModel *model, QString table_name);

    const Glom::Document &glom_doc;
    QMainWindow *table_model_window;
    bool table_model_opened;

  private Q_SLOTS:
    void on_file_quit_triggered();
    void on_help_about_triggered();
    void on_treeview_doubleclicked(const QModelIndex &index);
};

#endif /* QLOM_MAIN_WINDOW_H_ */
