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

#include "qlom_error.h"

#include <algorithm>

QlomError::QlomError() :
    severityLevel(0)
{
}

QlomError::QlomError(const QlomError &other) :
    description(other.what()),
    severityLevel(other.severity())
{
}

QlomError::QlomError(QString what, Qlom::QlomErrorSeverity severity) :
    description(what),
    severityLevel(severity)
{
}

QString QlomError::what() const
{
    if (!description.isNull()) {
        return description;
    } else {
        return QString();
    }
}

Qlom::QlomErrorSeverity QlomError::severity() const
{
    return static_cast<Qlom::QlomErrorSeverity>(severityLevel);
}

bool QlomError::operator==(const QlomError &other) const
{
    if(what() == other.what() && severity() == other.severity()) {
        return true;
    } else {
        return false;
    }
}

bool QlomError::operator!=(const QlomError &other) const
{
    return !(*this == other);
}

QlomError& QlomError::operator=(const QlomError& other)
{
    QlomError temp(other);
    swap(temp);
    return *this;
}

void QlomError::swap(QlomError& other)
{
    std::swap(description, other.description);
    std::swap(severityLevel, other.severityLevel);
}
