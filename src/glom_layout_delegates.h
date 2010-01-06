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

#include <libglom/data_structure/layout/fieldformatting.h>

/** This is the base class for Glom::FieldFormatting-specific delegates used to
  * format the style of Glom::LayoutItems in the view. In principle, each
  * Glom::LayoutItem implementing get_formatting_used() (which is not part of
  * the interface, though) gets its own delegate class derived from
  * GlomFieldFormattingDelegate.
  */
class GlomFieldFormattingDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // Explicit ctor, although it might have been useful for implicit type conversions.
    explicit GlomFieldFormattingDelegate(const Glom::FieldFormatting& formatting, QObject *parent = 0);
    virtual ~GlomFieldFormattingDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const;

protected:
    const Glom::FieldFormatting theFormattingUsed;
};


class GlomLayoutItemFieldDelegate : public GlomFieldFormattingDelegate
{
    Q_OBJECT

public:
    explicit GlomLayoutItemFieldDelegate(const Glom::FieldFormatting& formatting, QObject *parent = 0);
    virtual ~GlomLayoutItemFieldDelegate();

    virtual QString displayText(const QVariant &value, const QLocale &locale) const;

private:
    QString applyNumericFormatting(double numeric) const;
};

class GlomLayoutItemTextDelegate : public GlomFieldFormattingDelegate
{
    Q_OBJECT

public:
    GlomLayoutItemTextDelegate(const Glom::FieldFormatting& formatting, QObject *parent = 0);
    ~GlomLayoutItemTextDelegate();
};
#endif // QLOM_GLOM_LAYOUT_DELEGATE_H_
