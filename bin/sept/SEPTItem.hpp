// 2020.03.28 - Victor Dods

#pragma once

#include <QVariant>
#include "sept/Data.hpp"
#include "sept/TreeNode_t.hpp"

class SEPTItem : public sept::TreeNode_t<sept::Data*,SEPTItem> {
public:

    using TreeNode = sept::TreeNode_t<sept::Data*,SEPTItem>;

    SEPTItem () = delete;
    SEPTItem (SEPTItem const &) = delete;
    SEPTItem (SEPTItem &&) = delete;
    explicit SEPTItem (sept::Data *sept_data);
    // This exists only to make the "invisible root" node needed in order to show the
    // "visible" root node (see SEPTModel).
    explicit SEPTItem (std::unique_ptr<SEPTItem> &&child);

    // Recursively populate the data() pointers, creating children as necessary.
    void populate (sept::Data *sept_data);
    // Non-recursively update the data() pointer of this SEPTItem to the given one.
    // This doesn't change any of its children.
    void update (sept::Data *sept_data);

    QVariant data_as_QVariant (QModelIndex const &index, int role) const;
    bool insert_children (size_t position, size_t count, sept::Data const &value);
    bool remove_children (size_t position, size_t count);
    bool set_data_as_QVariant (QModelIndex const &index, QVariant const &value, int role);

    bool is_editable (size_t column) const;
    bool is_checkable (size_t column) const;
    // This is well-defined only if is_checkable(column) returns true.
//     QVariant checked_state (size_t column) const;

private:

    // Traverse this SEPTItem (as a tree) and verify that all the sept::Data pointers are as expected.
    void verify (sept::Data const *expected_sept_data) const;
};
