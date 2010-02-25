/* Qlom is copyright Openismus GmbH, 2010
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

#include "list_view.h"
#include "layout_delegates.h"
#include "list_layout_model.h"
#include "utils.h"

#include <QHeaderView>

QlomListView::QlomListView(QWidget *parent)
: QTableView(parent), theLastColumnIndex(-1), theToggledFlag(false)
{
    connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
        this, SLOT(onHeaderSectionPressed(int)));
}

QlomListView::~QlomListView()
{}

void QlomListView::setupDelegateForColumn(int column)
{
    QlomListLayoutModel *model =
        qobject_cast<QlomListLayoutModel *>(this->model());

    if (model) {
        QStyledItemDelegate *delegate =
          QlomListView::createDelegateFromColumn(model, column);
        setItemDelegateForColumn(column, delegate);
    }
}

QStyledItemDelegate * QlomListView::createDelegateFromColumn(
    QlomListLayoutModel *model, int column)
{
    /* Need to respect the following constraint: The layout item in
     * theLayoutGroup that can be found at the position column points to has to
     * be a LayoutItem_Text or a LayoutItem_Field.
     * However, this method is not used efficiently, considering how most items
     * in a list view are field items. If LayoutItem_Text and LayoutItem_Field
     * had a common base clase featuring the get_formatting_used() API we could
     * get rid of the most annoying part at least: the dynamic casts. */
    const QlomListLayoutModel::GlomSharedLayoutItems items =
        model->getLayoutItems();
    for (Glom::LayoutGroup::type_list_const_items::const_iterator iter =
        items.begin(); iter != items.end(); ++iter) {
        if (column == std::distance(items.begin(), iter)) {
            Glom::sharedptr<const Glom::LayoutItem_Text> textItem =
                Glom::sharedptr<const Glom::LayoutItem_Text>::cast_dynamic(*iter);
            if(textItem)
                return new QlomLayoutItemTextDelegate(
                    textItem->get_formatting_used(),
                    QlomLayoutItemTextDelegate::GlomSharedField(),
                    ustringToQstring(textItem->get_text()));

            Glom::sharedptr<const Glom::LayoutItem_Field> fieldItem =
                Glom::sharedptr<const Glom::LayoutItem_Field>::cast_dynamic(*iter);
            if(fieldItem)
                return new QlomLayoutItemFieldDelegate(
                    fieldItem->get_formatting_used(),
                    fieldItem->get_full_field_details());
        }
    }

    return 0;
}

void QlomListView::onHeaderSectionPressed(int colIdx)
{
    Qt::SortOrder order = Qt::DescendingOrder;

    if (colIdx == theLastColumnIndex) {
        order = (theToggledFlag ? Qt::DescendingOrder : Qt::AscendingOrder);
        theToggledFlag = !theToggledFlag;
    } else if (-1 != theLastColumnIndex) {
        // Switching from another column => some arbitrary sorting is applied,
        // so we start with Qt::AscendingOrder again.
        order = Qt::AscendingOrder;
    }
    theLastColumnIndex = colIdx;

    // TODO: If this modifies the sorting in the model, we'd need a proxy model
    // here.
    sortByColumn(colIdx, order);
}
