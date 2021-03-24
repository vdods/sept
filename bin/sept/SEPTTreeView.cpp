// 2020.04.26 - Victor Dods

#include "SEPTTreeView.hpp"

#include "SEPTModel.hpp"
#include <QErrorMessage>
#include "RowIndexSet.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/proj/ElementExtraction.hpp"

SEPTModel *SEPTTreeView::model () const {
    assert(dynamic_cast<SEPTModel*>(QTreeView::model()) != nullptr);
    return static_cast<SEPTModel*>(QTreeView::model());
}

void SEPTTreeView::resize_columns_to_contents () {
    for (size_t i = 0; i < size_t(model()->columnCount()); ++i)
        resizeColumnToContents(i);
}

void SEPTTreeView::set_source_view (SEPTTreeView *source_view) {
    assert(m_source_view == nullptr && "source_view already set");
    m_source_view = source_view;
}

sept::Data SEPTTreeView::extract_elements () const {
    QModelIndexList indices = selectionModel()->selectedIndexes();

    // Collect selected indices
    AscendingRowIndexSet row_index_set;
    for (int i = 0; i < indices.size(); ++i) {
        QModelIndex &index = indices[i];
        row_index_set.emplace(index);
    }
    // Iterate through them in order
    auto index_array = sept::Array();
    for (auto const &row_index : row_index_set) {
        auto const &index = row_index.m_index;
        index_array.elements().emplace_back(int64_t(index.row()));
    }
    // Execute the projection and use that data.
    sept::proj::ElementExtraction ee;
    return ee.project_with_complement(model()->root_data(), index_array);
}

void SEPTTreeView::integrate_data (sept::Data const &projection_with_complement) {
    // Steal the model so that we own it and can mutate it.

    sept::proj::ElementExtraction ee;
    if (!ee.can_embed(projection_with_complement, model()->root_data())) {
        QErrorMessage(this).showMessage("Can't integrate data");
        return;
    }
    model()->transform_root_data([&](sept::Data &root_data){
        ee.embed(projection_with_complement, root_data);
    });
    // TODO: Figure out if any updating needs to happen.
}

void SEPTTreeView::initialize () {
    // Set properties
    setAllColumnsShowFocus(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setAnimated(false); // This makes opening/closing hierarchical items instant and snappy.
//     setSortingEnabled(true); // Doesn't work out of the box -- probably needs some definition of how to sort
//     sortByColumn(0, Qt::AscendingOrder);
    setEditTriggers(
//         QAbstractItemView::CurrentChanged|   // Editing starts whenever current item changes.
        QAbstractItemView::DoubleClicked|       // Editing starts when an item is double clicked.
        QAbstractItemView::SelectedClicked|     // Editing starts when clicking on an already selected item.
        QAbstractItemView::EditKeyPressed|      // Editing starts when the platform edit key has been pressed over an item.
        QAbstractItemView::AnyKeyPressed        // Editing starts when any key is pressed over an item.
    );
}
