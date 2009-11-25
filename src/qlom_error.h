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

enum QlomErrorDomain {
    DOCUMENT_ERROR_DOMAIN, /** error in Glom document-handling */
    DATABASE_ERROR_DOMAIN
};

enum QlomErrorSeverity {
    WARNING_ERROR_SEVERITY, /**< warnings indicate that a non-critical request
                                 failed */
    CRITICAL_ERROR_SEVERITY /**< critical errors which indicate that the
                                 requested action failed */
};

} // namespace Qlom

class QlomError
{

public:
    /** Creates a blank error. */
    QlomError();

    /** Copy constructor.
     *  @param other the other error to copy */
    QlomError(const QlomError &other);

    /** Creates an error with a description and a severity level.
     *  @param[in] domain the domain of the error
     *  @param[in] what a description of the error
     *  @param[in] severity the severity of the error */
    QlomError(const Qlom::QlomErrorDomain domain, const QString &what,
        const Qlom::QlomErrorSeverity severity);

    /** Requests the domain of the error.
     *  @returns the domain of the error */
    Qlom::QlomErrorDomain domain() const;

    /** Requests the description of the error.
     *  @returns a description of the error */
    QString what() const;

    /** Requests the severity of the error/
     *  @returns the severity level of the error */
    Qlom::QlomErrorSeverity severity() const;

    bool operator==(const QlomError &other) const;

    bool operator!=(const QlomError &other) const;

    QlomError& operator=(const QlomError& other);

    /** Simple swap implementation
     *  @param[in,out] other the error to swap */
    void swap(QlomError& other);

private:
    quint8 errorDomain; /**< the debug domain of the error */
    QString description; /**< the description of the error */
    quint8 severityLevel; /**< the severity of the error */
};

#endif /* QLOM_QLOM_ERROR_H_ */
