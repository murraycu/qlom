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

#include "layout_delegates.h"
#include "utils.h"

#include <QRegExp>
#include <QStringList>
#include <QAbstractItemView>
#include <QPushButton>
#include <QSignalMapper>

QlomFieldFormattingDelegate::QlomFieldFormattingDelegate(
    const Glom::Formatting &formatting, const GlomSharedField details,
    QObject *parent) :
    QStyledItemDelegate(parent),
    theFormattingUsed(formatting),
    theFieldDetails(details)
{}

QlomFieldFormattingDelegate::~QlomFieldFormattingDelegate()
{}

void QlomFieldFormattingDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

#if defined(Q_WS_X11) //TODO: Why isn't this defined for us?
    QColor::setAllowX11ColorNames(true);
#endif

    QColor fgColor = painter->pen().color();
    QString fgColorName =
        ustringToQstring(theFormattingUsed.get_text_format_color_foreground());

    if (!fgColorName.isEmpty()) {
        fgColor.setNamedColor(fgColorName);
    }

    QColor bgColor = Qt::transparent;
    QString bgColorName =
        ustringToQstring(theFormattingUsed.get_text_format_color_background());

    if (!bgColorName.isEmpty()) {
        bgColor.setNamedColor(bgColorName);
    }

    /* Tried to set fore- and background via setColor/setBrush and all roles
     * listed http://qt.nokia.com/doc/4.6/qpalette.html#ColorRole-enum,
     * highlight and text seem to be honored but not backround/base/window. I
     * am not too surprised though - we probably need to extend our model to
     * return the correct color per index using ItemDataRoles:
     * http://doc.trolltech.com/4.6/qt.html#ItemDataRole-enum */
    opt.palette.setColor(QPalette::Text, fgColor);

    // Still draw the background manually.
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRect(opt.rect);
    painter->restore();

    // Forward the modified QStyleOptionViewItem to the parent.
    QStyledItemDelegate::paint(painter, opt, index);
}

QSize QlomFieldFormattingDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QlomLayoutItemFieldDelegate::QlomLayoutItemFieldDelegate(
    const Glom::Formatting &formatting, const GlomSharedField details,
    QObject *parent) :
    QlomFieldFormattingDelegate(formatting, details, parent)
{}

QlomLayoutItemFieldDelegate::~QlomLayoutItemFieldDelegate()
{}

QString QlomLayoutItemFieldDelegate::displayText(const QVariant &value,
    const QLocale &locale) const
{
    switch(theFieldDetails->get_glom_type()) {
    case Glom::Field::TYPE_NUMERIC: {
        /* Check whether the display text was a double in its previous
         * life. If true, remove trailing zeroes and add thousand
         * separators (if requested). The Glom numeric type is treated as
         * doubles by the Sqlite backend. */
        bool conversionSucceeded = false;
        double numeric = value.toString().toDouble(&conversionSucceeded);

        if (conversionSucceeded) {
            return applyNumericFormatting(numeric, locale);
        }
    } break;

    case Glom::Field::TYPE_TEXT:
        return value.toString();

    // TODO: handle other display roles correctly.
    case Glom::Field::TYPE_INVALID:
    case Glom::Field::TYPE_DATE:
    case Glom::Field::TYPE_TIME:
    case Glom::Field::TYPE_BOOLEAN:
    case Glom::Field::TYPE_IMAGE:
        return value.toString();

    default:
        break;
    }

    return QString("(not implemented)");
}

QString QlomLayoutItemFieldDelegate::applyNumericFormatting(double numeric,
    const QLocale &locale) const
{
    Glom::NumericFormat numFormat = theFormattingUsed.m_numeric_format;

    QString currencyPrefix = QString();
    if (!numFormat.m_currency_symbol.empty()) {
        // Add a whitespace for the currency prefix.
        currencyPrefix =
            QString("%1 ").arg(ustringToQstring(numFormat.m_currency_symbol));
    }

    QLocale myLocale = QLocale(locale);

    if(!numFormat.m_use_thousands_separator) {
        myLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    } else {
        myLocale.setNumberOptions(0); // Do not rely on defaults here.
    }

    // TODO: check max precision in Glom source.
    int precision = (numFormat.m_decimal_places_restricted
        ? numFormat.m_decimal_places : numFormat.get_default_precision());
    /* 'g' trims trailing zeroes, although not documented in [1], whereas 'f'
       prints the decimal places instead of using mantisse + exponent.
       [1] http://doc.trolltech.com/4.6/qstring.html#argument-formats */
    char format = (numFormat.m_decimal_places_restricted ? 'f' : 'g');

    // This already removes trailing zeroes and also adds thousand separators.
    return QString("%1%2").arg(currencyPrefix)
        .arg(myLocale.toString(numeric, format, precision));
}

QlomLayoutItemTextDelegate::QlomLayoutItemTextDelegate(
    const Glom::Formatting &formatting, const GlomSharedField details,
    const QString &displayText, QObject *parent) :
    QlomFieldFormattingDelegate(formatting, details, parent),
    theDisplayText(displayText)
{}

QlomLayoutItemTextDelegate::~QlomLayoutItemTextDelegate()
{}

QString QlomLayoutItemTextDelegate::displayText(const QVariant &, const QLocale &) const
{
    return theDisplayText;
}

QlomModelIndexObject::QlomModelIndexObject(const QModelIndex &index, QObject *parent)
: QObject(parent),
  theIndex(index)
{}

QlomModelIndexObject::~QlomModelIndexObject()
{}

QModelIndex QlomModelIndexObject::index() const
{
    return theIndex;
}



QlomButtonDelegate::QlomButtonDelegate(const QString &label, QObject *parent) :
    QStyledItemDelegate(parent),
    theLabel(label)
{}

QlomButtonDelegate::~QlomButtonDelegate()
{}

QString QlomButtonDelegate::displayTest(const QVariant &, const QLocale&) const
{
    // Silence the output for this delegate.
    return QString();
}

void QlomButtonDelegate::paint(QPainter * /*painter*/, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    QAbstractItemView *view = qobject_cast<QAbstractItemView *>(parent());
    if(view && !view->indexWidget(index)) {
        QPushButton *button = new QPushButton(theLabel, view);

        // Bind the index to the button's pressed signal.
        QSignalMapper *bindIndex = new QSignalMapper(button);
        bindIndex->setMapping(button, new QlomModelIndexObject(index, button));
        connect(button, SIGNAL(pressed()),
                bindIndex, SLOT(map()));
        connect(bindIndex, SIGNAL(mapped(QObject *)),
                this, SLOT(onButtonPressed(QObject *)));

        view->setIndexWidget(index, button);
    }
}

void QlomButtonDelegate::onButtonPressed(QObject *obj)
{
    QlomModelIndexObject *indexObj = qobject_cast<QlomModelIndexObject *>(obj);
    if(indexObj) {
        Q_EMIT buttonPressed(indexObj->index());
    }
}
