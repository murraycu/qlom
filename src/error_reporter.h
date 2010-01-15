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

#ifndef QLOM_ERROR_REPORTER_H_
#define QLOM_ERROR_REPORTER_H_

#include "qlom_error.h"

#include <QObject>

class ErrorReporter : public QObject
{
    Q_OBJECT

public:
    /** Get the last error raised on the object.
     *  @returns the last error */
    QlomError lastError() const;

    /** Raise a new error on the object.
     *  @param[in] error the error to raise */
    void raiseError(const QlomError &error);

Q_SIGNALS:
    /** Signal the error.
     *  @param[in] error the raised error */
    void errorRaised(QlomError error);

private:
    QlomError theLastError; /**< the most recently raised error */
};

#endif /* QLOM_ERROR_REPORTER_H_ */
