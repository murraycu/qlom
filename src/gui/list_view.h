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

#ifndef QLOM_LIST_VIEW_H_
#define QLOM_LIST_VIEW_H_

#include "document.h"

#include <QStyledItemDelegate>
#include <QTableView>

/** This class extends the QTableView by a delegate factory specialised to the
 *  QlomListLayoutModel. */
class QlomListView : public QTableView
{
    Q_OBJECT

public:
    explicit QlomListView(QWidget *parent = 0);
    virtual ~QlomListView();

    /** Creates the necessary Qlom layout delegates for the current model and
      * installs them in the view. */
    void setupDelegateForColumn(int column);

    /** Applies Glom's Formatting to a QStyledItemDelegate. Since the view
     *  only knows about columns (during setup) we need to map columns to
     *  LayoutItems, as it is already done in the query builder, even if not
     *  stated explicitly.
     *  @param[in] model the layout model that needs to be queried for layout items
     *  @param[in] column the column to create a delegate for
     *  @returns the style delegate to be managed by a view, or 0 if the
     *  specified column cannot be formatted customly. */
    static QStyledItemDelegate * createDelegateFromColumn(
        QlomListLayoutModel *model, int column);

public Q_SLOTS:
    /** Slot to sort columns. */
    void onHeaderSectionPressed(int columnIndex);

private:
    int theLastColumnIndex; /**< the last column that was used for sorting, default is -1 (i.e., none). */
    bool theToggledFlag;
};

#endif /* QLOM_LIST_VIEW_H_ */
