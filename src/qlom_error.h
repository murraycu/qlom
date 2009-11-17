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

#ifndef QLOM_QLOM_ERROR_H_
#define QLOM_QLOM_ERROR_H_

#include <QString>

namespace Qlom
{

enum QlomErrorSeverity {
    CRITICAL_ERROR_SEVERITY /**< critical errors which indicate that the
                                 requested action failed */
};

} // namespace Qlom

class QlomError
{

public:
    /** Creates a blank error. */
    QlomError();

    /** Creates an error with a description and a severity level.
     *  @param[in] what a description of the error
     *  @param[in] severity the severity of the error */
    explicit QlomError(QString what, Qlom::QlomErrorSeverity severity);

    /** Requests the description of the error.
     *  @returns a description of the error */
    QString what() const;

    /** Requests the severity of the error/
     *  @returns the severity level of the error */
    Qlom::QlomErrorSeverity severity() const;

private:
    const QString description; /**< the description of the error */
    const quint8 severityLevel; /**< the severity of the error */
};

#endif /* QLOM_QLOM_ERROR_H_ */
