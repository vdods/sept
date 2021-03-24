// 2020.03.28 - Victor Dods

#include "SEPTModel.hpp"

#include "SEPTItem.hpp"
#include <iostream>
#include <QtWidgets>

SEPTModel::SEPTModel (sept::Data const &root_data, QObject *parent)
    :   QAbstractItemModel(parent)
    ,   m_root_data(root_data)
{
    initialize_root_items();
}

SEPTModel::SEPTModel (sept::Data &&root_data, QObject *parent)
    :   QAbstractItemModel(parent)
    ,   m_root_data(std::move(root_data))
{
    initialize_root_items();
}

SEPTModel::~SEPTModel () { }

void SEPTModel::transform_root_data (std::function<void(sept::Data&)> const &transform) {
    // NOTE: Ideally, this could just make minimal changes to the model to do minimal violence
    // to the attached views, but that's too hard for now.

    beginResetModel();
    m_visible_root_item = nullptr;  // This was a non-owning pointer anyway.
    m_real_root_item.reset();       // This was the owning pointer.
    transform(m_root_data);          // Apply the transformation.
    initialize_root_items();        // Initialize anew.
    endResetModel();
}

QVariant SEPTModel::data (QModelIndex const &index, int role) const {
    if (!index.isValid())
        return QVariant();

    SEPTItem *item = get_item(index);
    return item->data_as_QVariant(index, role);
}

Qt::ItemFlags SEPTModel::flags (QModelIndex const &index) const {
    if (!index.isValid())
        return 0;

    auto f = QAbstractItemModel::flags(index);

    SEPTItem *item = get_item(index);
    if (item != nullptr) {
        if (item->is_editable(index.column()))
            f |= Qt::ItemIsEditable;
        if (item->is_checkable(index.column()))
            f |= Qt::ItemIsUserCheckable;
    }

    return f;
}

QVariant SEPTModel::headerData (int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return "Key"; // Key/Index
        else if (section == 1)
            return "Term";
        else if (section == 2)
            return "Type";
    }

    return QVariant();
}

QModelIndex SEPTModel::index (int row, int column, QModelIndex const &parent_index) const {
    if (parent_index.isValid() && parent_index.column() != 0)
//     if (parent_index.isValid())
        return QModelIndex();

    SEPTItem *parent_item = get_item(parent_index);

    SEPTItem *child_item = parent_item->child(row);
    if (child_item != nullptr)
        return createIndex(row, column, child_item);
    else
        return QModelIndex();
}

bool SEPTModel::insertRows (int position, int row_count, QModelIndex const &parent_index) {
    // Presumably this happens when attempting to insert at the root.
    if (!parent_index.isValid())
        return false;

    if (position < 0 || row_count <= 0)
        return false; // Signed-int indexing is dumb!

    SEPTItem *parent_item = get_item(parent_index);
    bool success;

    beginInsertRows(parent_index, position, position + row_count - 1);
    success = parent_item->insert_children(size_t(position), size_t(row_count), sept::Data(666)); // TEMP HACK dummy value
    endInsertRows();

    return success;
}

bool SEPTModel::removeRows (int position, int row_count, QModelIndex const &parent_index) {
    // Presumably this happens when attempting to insert at the root.
    if (!parent_index.isValid())
        return false;

    if (position < 0 || row_count <= 0)
        return false; // Signed-int indexing is dumb!

    SEPTItem *parent_item = get_item(parent_index);
    bool success;

    beginRemoveRows(parent_index, position, position + row_count - 1);
    success = parent_item->remove_children(size_t(position), size_t(row_count));
    endRemoveRows();

    return success;
}

QModelIndex SEPTModel::parent (QModelIndex const &index) const {
    if (!index.isValid())
        return QModelIndex();

    SEPTItem *child_item = get_item(index);
    SEPTItem *parent_item = child_item->parent();

    if (parent_item == m_real_root_item.get())
        return QModelIndex();

    return createIndex(parent_item->index_within_parent(), 0, parent_item);
}

int SEPTModel::rowCount (QModelIndex const &parent_index) const {
    return get_item(parent_index)->child_count();
}

int SEPTModel::columnCount (QModelIndex const &parent_index) const {
    return 3;
}

bool SEPTModel::setData (QModelIndex const &index, QVariant const &value, int role) {
    if (role != Qt::EditRole)
        return false;

    SEPTItem *item = get_item(index);
    bool result = item->set_data_as_QVariant(index, value, role);

    if (result) {
        emit dataChanged(index, index);
//         std::cout << "data changed.  m_root_data = " << m_root_data << '\n';
    }

    return result;
}

SEPTItem *SEPTModel::get_item (QModelIndex const &index) const {
    if (index.isValid()) {
        SEPTItem *item = static_cast<SEPTItem*>(index.internalPointer());
        if (item != nullptr)
            return item;
    }
    return m_real_root_item.get();
}

void SEPTModel::initialize_root_items () {
    // This odd construction is to compensate for the fact that QTreeView doesn't
    // explicitly show the root node of the model.  Thus we make an artificial "visible"
    // root node, which is a child of the invisible "real" root node.
    auto visible_root_item = std::make_unique<SEPTItem>(&m_root_data);
    m_visible_root_item = visible_root_item.get();
    m_real_root_item = std::make_unique<SEPTItem>(std::move(visible_root_item));
}
