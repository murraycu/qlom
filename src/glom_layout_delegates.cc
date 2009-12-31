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

// begin GlomFieldFormattingDelegate impl
GlomFieldFormattingDelegate::GlomFieldFormattingDelegate(const Glom::FieldFormatting& formatting, QObject *parent)
: QStyledItemDelegate(parent),
  theFormattingUsed(formatting)
{}

GlomFieldFormattingDelegate::~GlomFieldFormattingDelegate()
{}


// begin GlomLayoutItemFieldDelegate impl
GlomLayoutItemFieldDelegate::GlomLayoutItemFieldDelegate(const Glom::FieldFormatting& formatting, QObject *parent)
: GlomFieldFormattingDelegate(formatting, parent)
{}

GlomLayoutItemFieldDelegate::~GlomLayoutItemFieldDelegate()
{}

QString GlomLayoutItemFieldDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);

    // Check whether the display text was a double in its previous life (hence
    // the strict check over the whole string) and remove trailing zeroes if
    // true (this is a Glom convention used for its numeric type).
    static const QRegExp matchDouble("\\d+\\.\\d+");
    if(matchDouble.exactMatch(value.toString())) {
        return removeTrailingZeroes(value.toString());
    }

    return value.toString();
}

QString GlomLayoutItemFieldDelegate::removeTrailingZeroes(const QString& str) const
{
    QStringList doubleParts = str.split('.');
    QString reduceMe = doubleParts[1];

    // This loop probably doesn't scale very well, but then again we don't
    // expect a lot of iterations anyway.
    while(reduceMe.endsWith('0')) {
        reduceMe.chop(1);
    }

    if(!reduceMe.isEmpty()) {
        return QString("%1.%2").arg(doubleParts[0], reduceMe);
    } else {
        return doubleParts[0];
    }
}

// begin GlomLayoutItemTextDelegate impl
GlomLayoutItemTextDelegate::GlomLayoutItemTextDelegate(const Glom::FieldFormatting& formatting, QObject *parent)
: GlomFieldFormattingDelegate(formatting, parent)
{}

GlomLayoutItemTextDelegate::~GlomLayoutItemTextDelegate()
{}

void GlomLayoutItemTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem boldFont(option);
    boldFont.font.setBold(true);

    QStyledItemDelegate::paint(painter, boldFont, index);
}

QSize GlomLayoutItemTextDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}
