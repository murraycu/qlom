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

#ifndef QLOM_LAYOUT_DELEGATE_H_
#define QLOM_LAYOUT_DELEGATE_H_

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QSize>

#include <libglom/data_structure/layout/fieldformatting.h>
#include <libglom/data_structure/field.h>

/** This is the base class for Glom::FieldFormatting-specific delegates used to
  * format the style of Glom::LayoutItems in the view. In principle, each
  * Glom::LayoutItem implementing get_formatting_used() (which is not part of
  * the interface, though) gets its own delegate class derived from
  * GlomFieldFormattingDelegate.
  */
class QlomFieldFormattingDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    typedef Glom::sharedptr<const Glom::Field> GlomSharedField;

    /* Explicit ctor, although it might have been useful for implicit type
       conversions. */
    explicit QlomFieldFormattingDelegate(
        const Glom::FieldFormatting &formatting, const GlomSharedField details,
        QObject *parent = 0);
    virtual ~QlomFieldFormattingDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

protected:
    const Glom::FieldFormatting theFormattingUsed;
    const GlomSharedField theFieldDetails;
};


class QlomLayoutItemFieldDelegate : public QlomFieldFormattingDelegate
{
    Q_OBJECT

public:
    explicit QlomLayoutItemFieldDelegate(
        const Glom::FieldFormatting &formatting, const GlomSharedField details,
        QObject *parent = 0);
    virtual ~QlomLayoutItemFieldDelegate();

    virtual QString displayText(const QVariant &value, const QLocale &locale)
        const;

private:
    QString applyNumericFormatting(double numeric, const QLocale &locale) const;
};

class QlomLayoutItemTextDelegate : public QlomFieldFormattingDelegate
{
    Q_OBJECT

public:
    QlomLayoutItemTextDelegate(const Glom::FieldFormatting &formatting,
        const GlomSharedField details, const QString &displayText, QObject *parent = 0);
    virtual ~QlomLayoutItemTextDelegate();

    /** Ignore the value from the model for static text item and return the
     * stored value - theDisplayText - instead.
     *  @param[in] unused
     *  @param[in] unused
     */
    virtual QString displayText(const QVariant &, const QLocale &) const;

private:
    QString theDisplayText;
};

/** QSignalMappers only accept a limited subset of types for their bindins, one
 *  of them being QObjects. This class acts as a closure for QModelIndices */
class QlomModelIndexObject : public QObject
{
   Q_OBJECT

public:
    explicit QlomModelIndexObject(const QModelIndex &index, QObject *parent = 0);
    virtual ~QlomModelIndexObject();

    QModelIndex index() const;

private:
    QModelIndex theIndex;
};

/** This class inserts a button into the view (at the queried model index)
  * when it is *first* asked to paint itself. */
class QlomButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit QlomButtonDelegate(const QString &label, QObject *parent = 0);
    virtual ~QlomButtonDelegate();

    /** Overriden to silence the output of this delegate, since it is meant to
      * show a widget. */
    virtual QString displayTest(const QVariant &value, const QLocale &locale)
        const;

    /** This method assumes:
      * 1) parent() returns a valid QAbstractItemView,
      * 2) There is no QWidget at index yet,
      * 3) The widget at index does not get replaced (this is a soft
      *    assumption, paint(.) would simply not recreate the button widget in
      *    that case). */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

Q_SIGNALS:
    void buttonPressed(const QModelIndex &index);

private:
    QString theLabel;

private Q_SLOTS:
    /** Helper slot to unwrap the QObject from the QSignalMapper. */
    void onButtonPressed(QObject *obj);
};
#endif // QLOM_LAYOUT_DELEGATE_H_
