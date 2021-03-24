// 2020.04.29 - Victor Dods

#pragma once

#include "SEPTItem.hpp"
#include <QModelIndex>
#include <set>

// Indices may have arbitrary columns, but we're really interested in which rows are selected
// (noting that QItemSelectionModel's selectedRows method requires all columns in a row to be
// selected in order for it to count.  This fanciness with RowIndex and std::set is to achieve:
// -    Removing leaf-most items first, so that if a parent and a child are both selected
//      to be deleted, the child gets deleted first.
// -    Removing rows in descending order so nothing is invalidated improperly.
// -    Using only one QModelIndex per row (e.g. if multiple cells in the same row are selected)
// RowIndex exists really just to cache the tree_depth value of m_index's SEPTItem (which is
// stored in m_index.internalPointer().
struct RowIndex {
    size_t m_tree_depth;
    QModelIndex &m_index;

    RowIndex () = delete;
    RowIndex (QModelIndex &index)
    :   m_tree_depth(static_cast<SEPTItem*>(index.internalPointer())->tree_depth())
    ,   m_index(index)
    { }
};

struct RowIndexOrderAscending {
    bool operator() (RowIndex const &lhs, RowIndex const &rhs) const {
        return lhs.m_tree_depth < rhs.m_tree_depth
                ||
                (lhs.m_tree_depth == rhs.m_tree_depth &&
                lhs.m_index.row() < rhs.m_index.row())
                ||
                (lhs.m_index.row() == rhs.m_index.row() &&
                lhs.m_index.internalPointer() < rhs.m_index.internalPointer());
    }
};

struct RowIndexOrderDescending {
    bool operator() (RowIndex const &lhs, RowIndex const &rhs) const {
        return lhs.m_tree_depth > rhs.m_tree_depth
                ||
                (lhs.m_tree_depth == rhs.m_tree_depth &&
                lhs.m_index.row() > rhs.m_index.row())
                ||
                (lhs.m_index.row() == rhs.m_index.row() &&
                lhs.m_index.internalPointer() > rhs.m_index.internalPointer());
    }
};

typedef std::set<RowIndex,RowIndexOrderAscending> AscendingRowIndexSet;
typedef std::set<RowIndex,RowIndexOrderDescending> DescendingRowIndexSet;
