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

#ifndef QLOM_GLOM_LAYOUT_DELEGATE_H_
#define QLOM_GLOM_LAYOUT_DELEGATE_H_

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QSize>

class GlomNumericDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GlomNumericDelegate(QObject *parent);
    ~GlomNumericDelegate();

    virtual QString displayText(const QVariant &value, const QLocale &locale) const;
};

class GlomTextDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GlomTextDelegate(QObject *parent);
    ~GlomTextDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const;
};
#endif // QLOM_GLOM_LAYOUT_DELEGATE_H_
