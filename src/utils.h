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

#ifndef QLOM_UTILS_H_
#define QLOM_UTILS_H_

#include <QString>
#include <glibmm/ustring.h>

/** Convert a QString to a Glib::ustring.
 *  @param[in] qstring the string to convert
 *  @returns the converted string */
Glib::ustring qstringToUstring(const QString& qstring);

/** Convert a Glib::ustring to a QString.
 *  @param[in] ustring the string to convert
 *  @returns the converted string */
QString ustringToQstring(const Glib::ustring& ustring);

/** Get the current locale ID, for use with libglom method calls
 * such as TableInfo::get_table_title(locale_id);
 * For instance, en_US.
 */
std::string getCurrentLocaleId();

#endif /* QLOM_UTILS_H_ */
