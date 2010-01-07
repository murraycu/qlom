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
#include "utils.h"

#include <QRegExp>
#include <QStringList>

// begin GlomFieldFormattingDelegate impl
GlomFieldFormattingDelegate::GlomFieldFormattingDelegate(const Glom::FieldFormatting& formatting, const GlomSharedField details, QObject *parent)
: QStyledItemDelegate(parent),
  theFormattingUsed(formatting),
  theFieldDetails(details)
{}

GlomFieldFormattingDelegate::~GlomFieldFormattingDelegate()
{}

void GlomFieldFormattingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    // TODO: how to query that we are in the displayRole here?
    // TODO: query locale
    QString data = displayText(index.data(), QLocale());

    painter->save();

    // TODO: set font family + size
    //painter->setFont(opt.font);

    QColor::setAllowX11ColorNames(true);
    QColor fgColor = painter->pen().color();
    QString fgColorName = ustringToQstring(theFormattingUsed.get_text_format_color_foreground());

    if (!fgColorName.isEmpty())
        fgColor.setNamedColor(fgColorName);

    QColor bgColor = Qt::transparent; //painter->brush().color();
    QString bgColorName = ustringToQstring(theFormattingUsed.get_text_format_color_background());

    if (!bgColorName.isEmpty())
        bgColor.setNamedColor(bgColorName);

    // funny how the bold font overrides the pen width, no?
    painter->setPen(QPen(fgColor));
    painter->setBrush(bgColor);

    painter->drawRect(opt.rect);

    // TODO: query alignment + painter's origin (margin? padding?)
    painter->drawText(opt.rect, Qt::AlignLeft, data);

    painter->restore();
}

QSize GlomFieldFormattingDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index ) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

// begin GlomLayoutItemFieldDelegate impl
GlomLayoutItemFieldDelegate::GlomLayoutItemFieldDelegate(const Glom::FieldFormatting& formatting, const GlomSharedField details, QObject *parent)
: GlomFieldFormattingDelegate(formatting, details, parent)
{}

GlomLayoutItemFieldDelegate::~GlomLayoutItemFieldDelegate()
{}

QString GlomLayoutItemFieldDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    switch(theFieldDetails->get_glom_type())
    {
        case Glom::Field::TYPE_NUMERIC:
        {
            // Check whether the display text was a double in its previous life. If
            // true, remove trailing zeroes and add thousand separators (if requested).
            // The Glom numeric type is treated as doubles by the Sqlite backend.
            bool conversionSucceeded = false;
            double numeric = value.toString().toDouble(&conversionSucceeded);

            if(conversionSucceeded)
                return applyNumericFormatting(numeric, locale);
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

        default: break;
    }

    return QString("(not implemented)");
}

QString GlomLayoutItemFieldDelegate::applyNumericFormatting(double numeric, const QLocale &locale) const
{
    Glom::NumericFormat numFormat = theFormattingUsed.m_numeric_format;

    QString currencyPrefix = QString();
    if (!numFormat.m_currency_symbol.empty())
    {
        // Add a whitespace for the currency prefix.
        currencyPrefix = QString("%1 ").arg(ustringToQstring(numFormat.m_currency_symbol));
    }

    QLocale myLocale = QLocale(locale);

    if(!numFormat.m_use_thousands_separator)
        myLocale.setNumberOptions(QLocale::OmitGroupSeparator);

    // TODO: check max precision in Glom source.
    int precision = (numFormat.m_decimal_places_restricted ? numFormat.m_decimal_places : 12);
    // 'g' trims trailing zeroes, although not documented in [1], whereas 'f'
    // prints the decimal places instead of using mantisse + exponent.
    // [1] http://doc.trolltech.com/4.6/qstring.html#argument-formats
    char format = (numFormat.m_decimal_places_restricted ? 'f' : 'g');

    // This already removes trailing zeroes and also adds thousand separators.
    return QString("%1%2").arg(currencyPrefix)
                          .arg(myLocale.toString(numeric, format, precision));
}

// begin GlomLayoutItemTextDelegate impl
GlomLayoutItemTextDelegate::GlomLayoutItemTextDelegate(const Glom::FieldFormatting& formatting, const GlomSharedField details, QObject *parent)
: GlomFieldFormattingDelegate(formatting, details, parent)
{}

GlomLayoutItemTextDelegate::~GlomLayoutItemTextDelegate()
{}
