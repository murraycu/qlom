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

#include "utils.h"
#include <QLocale>

Glib::ustring qstringToUstring(const QString& qstring)
{
    return Glib::ustring(qstring.toUtf8().constData());
}

QString ustringToQstring(const Glib::ustring& ustring)
{
    return QString::fromUtf8(ustring.c_str());
}

/** Get the current locale ID, for use with libglom method calls
 * such as TableInfo::get_table_title(locale_id);
 * For instance, en_US.
 */
std::string getCurrentLocaleId()
{
    return QLocale().name().toStdString();
}
