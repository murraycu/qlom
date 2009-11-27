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

#include "glom_layout_delegate.h"

GlomLayoutDelegate::GlomLayoutDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{}

GlomLayoutDelegate::~GlomLayoutDelegate()
{}

void GlomLayoutDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize GlomLayoutDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QString GlomLayoutDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return QStyledItemDelegate::displayText(value, locale);
}
