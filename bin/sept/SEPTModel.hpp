// 2020.03.28 - Victor Dods

#pragma once

#include <functional>
#include <memory>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "sept/Data.hpp"

class SEPTItem;

class SEPTModel : public QAbstractItemModel {
    Q_OBJECT

public:

    SEPTModel (sept::Data const &root_data, QObject *parent = nullptr);
    SEPTModel (sept::Data &&root_data, QObject *parent = nullptr);
    ~SEPTModel ();

    sept::Data const &root_data () const { return m_root_data; }

    // Applies the given transformation to root_data -- this resets the whole model.
    void transform_root_data (std::function<void(sept::Data&)> const &transform);

    QVariant data (QModelIndex const &index, int role) const override;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index (int row, int column, QModelIndex const &parent_index = QModelIndex()) const override;
    QModelIndex parent (QModelIndex const &index) const override;

    int rowCount (QModelIndex const &parent_index = QModelIndex()) const override;
    int columnCount (QModelIndex const &parent_index = QModelIndex()) const override;

    Qt::ItemFlags flags (QModelIndex const &index) const override;
    bool setData (const QModelIndex &index, QVariant const &value, int role = Qt::EditRole) override;

    bool insertRows (int position, int row_count, QModelIndex const &parent_index = QModelIndex()) override;
    bool removeRows (int position, int row_count, QModelIndex const &parent_index = QModelIndex()) override;

private:

    SEPTItem *get_item (QModelIndex const &index) const;

    void initialize_root_items ();

    sept::Data m_root_data;
    // Non-owning pointer to the "visible" root item that appears as an expandable item in QTreeView.
    SEPTItem *m_visible_root_item;
    // Owning pointer to the invisible "real" root item for this QAbstractItemModel.
    // It seems like this hack wouldn't be necessary in QStandardItemModel or QTreeWidget, as
    // they have an invisibleRootItem that can be used with QTreeView->setRootIndex, which probably
    // corresponds to m_real_root_item.
    // See https://stackoverflow.com/questions/43862366/qtreeview-disable-showing-of-root-node
    std::unique_ptr<SEPTItem> m_real_root_item;
};
