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

#include "glom_layout_delegates.h"

#include <QRegExp>
#include <QStringList>

// begin GlomNumericDelegate impl
GlomNumericDelegate::GlomNumericDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{}

GlomNumericDelegate::~GlomNumericDelegate()
{}

QString GlomNumericDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);

    static const QRegExp matchDouble("\\d+\\.\\d+");

    if(matchDouble.exactMatch(value.toString())) {
        QStringList doubleParts = value.toString().split('.');
        QString reduceMe = doubleParts[1];
        while(reduceMe.endsWith('0')) {
            reduceMe.chop(1);
        }

        if(!reduceMe.isEmpty()) {
            return QString("%1.%2").arg(doubleParts[0], reduceMe);
        } else {
            return doubleParts[0];
        }
    }

    return value.toString();
}

// begin GlomTextDelegate impl
GlomTextDelegate::GlomTextDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{}

GlomTextDelegate::~GlomTextDelegate()
{}

void GlomTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem boldFont(option);
    boldFont.font.setBold(true);

    QStyledItemDelegate::paint(painter, boldFont, index);
}

QSize GlomTextDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}
