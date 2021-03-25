// 2020.03.28 - Victor Dods

#include "SEPTItem.hpp"

#include <cassert>
#include <QModelIndex>
#include <QTextStream>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"
#include <sstream>
#include <stdexcept>
#include <string>

SEPTItem::SEPTItem (sept::Data *sept_data) {
    populate(sept_data);
    verify(sept_data); // TODO: Make into debug-only
}

SEPTItem::SEPTItem (std::unique_ptr<SEPTItem> &&child) {
    assert(child != nullptr);
    push_back(std::move(child));
}

void SEPTItem::populate (sept::Data *sept_data) {
    assert(sept_data != nullptr);
    assert(sept_data->has_value());
    assign_data(sept_data);

    if (data()->type() == typeid(sept::ArrayTerm_c)) {
        auto &a = data()->cast<sept::ArrayTerm_c&>();
        // Recursively construct the contents.
        for (sept::Data &element : a.elements())
            emplace_back(&element);
    }
}

void SEPTItem::update (sept::Data *sept_data) {
    assert(sept_data != nullptr);
    assert(sept_data->has_value());
    assign_data(sept_data);
    verify(sept_data); // TODO: Make into debug-only
}

QVariant SEPTItem::data_as_QVariant (QModelIndex const &index, int role) const {
    assert(index.isValid());
    assert(data() != nullptr);
    assert(data()->has_value());

//     std::cout << "SEPTItem::data_as_QVariant(column = " << column << ", role = " << role << ", *data() = " << *data() << ")\n";

    // For now, all columns and roles will produce return values that are strictly a function
    // of data() and no other state variables.
    switch (index.column()) {
        case 0: { // Column 0 is the "Key" column.
            switch (role) {
                case Qt::DisplayRole:
                    if (tree_depth() == 1) // This is the visible root
                        return QVariant(); // Empty
                    else
                        return QString::fromStdString(LVD_FMT(index.row()));

                case Qt::EditRole:
                case Qt::CheckStateRole: // QCheckState
//                     assert(false && "this is not yet supported");
                    return QVariant();

                case Qt::TextAlignmentRole: // Qt::Alignment
                    return Qt::AlignLeft;

                case Qt::DecorationRole: // QColor, QIcon, or QPixmap
                case Qt::ToolTipRole: // QString
                case Qt::StatusTipRole: // QString
                case Qt::WhatsThisRole: // QString
                case Qt::FontRole: // QFont
                case Qt::BackgroundRole: // QBrush
                case Qt::ForegroundRole: // QBrush
                case Qt::InitialSortOrderRole: // Qt::SortOrder
                default:
                    return QVariant();
            }
        }

        case 1: { // Column 1 is the "Term" column.
            switch (role) {
                case Qt::DisplayRole:
                    // There seems to be 2 bugs in QStyledItemDelegate regarding DisplayRole for booleans:
                    // - It displays as "true" or "false", but the dropdown menu shows "True" and "False".
                    // - When it switches to editing mode and the dropdown appears, it doesn't clear the
                    //   previously rendered content, so there's a mess of overlapping text.
                    // Therefore, for now, just disable the DisplayRole for boolean.
                    if (data()->type() == typeid(bool)) return QVariant();
//                     if (data()->type() == typeid(bool)) return QString(data()->cast<bool>() ? "True" : "False");
                case Qt::EditRole:
                    if (data()->type() == typeid(int)) return data()->cast<int>();
                    else if (data()->type() == typeid(int64_t)) return QVariant::fromValue(data()->cast<int64_t>());
                    else if (data()->type() == typeid(double)) return data()->cast<double>();
                    else if (data()->type() == typeid(char)) return data()->cast<char>();
                    else if (data()->type() == typeid(bool)) return data()->cast<bool>();
                    else if (data()->type() == typeid(sept::ArrayTerm_c)) return QString::fromStdString(LVD_FMT("ArrayTerm_c of size " << data()->cast<sept::ArrayTerm_c const &>().size()));
                    else {
                        std::cout << "HIPPO: " << *data() << '\n';
                        assert(false && "not supported");
                        return QVariant();
                    }

                case Qt::CheckStateRole: // QCheckState
                    if (!is_checkable(index.column()))
                        return QVariant();

                    assert(data()->type() == typeid(bool) && "this is based on is_checkable");
                    return data()->cast<bool>() ? Qt::Checked : Qt::Unchecked;

                case Qt::TextAlignmentRole: // Qt::Alignment
                    if (data()->type() == typeid(int)) return Qt::AlignRight;
                    else if (data()->type() == typeid(int64_t)) return Qt::AlignRight;
                    else if (data()->type() == typeid(double)) return Qt::AlignRight;
                    else if (data()->type() == typeid(char)) return Qt::AlignRight;
                    else if (data()->type() == typeid(bool)) return Qt::AlignRight;
                    else if (data()->type() == typeid(sept::ArrayTerm_c)) return Qt::AlignLeft;
                    else {
                        std::cout << "HIPPO: " << *data() << '\n';
                        assert(false && "not supported");
                        return QVariant();
                    }

                case Qt::DecorationRole: // QColor, QIcon, or QPixmap
                case Qt::ToolTipRole: // QString
                case Qt::StatusTipRole: // QString
                case Qt::WhatsThisRole: // QString
                case Qt::FontRole: // QFont
                case Qt::BackgroundRole: // QBrush
                case Qt::ForegroundRole: // QBrush
                case Qt::InitialSortOrderRole: // Qt::SortOrder
                default:
                    return QVariant();
            }
            assert(false && "shouldn't fall through to here");
            return QVariant();
        }

        case 2: { // Column 2 is the "Type" column.
            switch (role) {
                case Qt::DisplayRole: {
                    // Don't bother with a switch statement, just return the string representation
                    // of the data's type.
                    return QString::fromStdString(LVD_FMT(sept::abstract_type_of_data(*data())));
                }

                case Qt::EditRole:
                case Qt::CheckStateRole: // QCheckState
//                     assert(false && "this is not yet supported");
                    return QVariant();

                case Qt::TextAlignmentRole: // Qt::Alignment
                    return Qt::AlignLeft;

                case Qt::DecorationRole: // QColor, QIcon, or QPixmap
                case Qt::ToolTipRole: // QString
                case Qt::StatusTipRole: // QString
                case Qt::WhatsThisRole: // QString
                case Qt::FontRole: // QFont
                case Qt::BackgroundRole: // QBrush
                case Qt::ForegroundRole: // QBrush
                case Qt::InitialSortOrderRole: // Qt::SortOrder
                default:
                    return QVariant();
            }
            assert(false && "shouldn't fall through to here");
            return QVariant();
        }

        default:
            assert(false && "invalid column");
            return QVariant();
    }
}

bool SEPTItem::insert_children (size_t position, size_t count, sept::Data const &value) {
    // Disallow inserting children into the invisible "real" root node.
    if (is_root())
        return false;

    if (position > child_count())
        return false;

    assert(data() != nullptr);
    verify(data()); // TODO: Make into debug-only

    // TODO: Here is where support for other hierarchical types (e.g. Map) would be added
    if (data()->type() == typeid(sept::ArrayTerm_c)) {
        auto &this_array = data()->cast<sept::ArrayTerm_c &>();
        assert(this_array.elements().size() == this->child_count());

        // Determine which of the array's element iterators (i.e. pointers) will be invalidated in this
        // insert operation.  From cppreference.com, the docs for std::vector<...>::insert:
        //     Causes reallocation if the new size() is greater than the old capacity(). If the
        //     new size() is greater than capacity(), all iterators and references are invalidated.
        //     Otherwise, only the iterators and references before the insertion point remain valid.
        //     The past-the-end iterator is also invalidated.
        size_t new_size = this_array.elements().size() + count;
        size_t first_invalidated_index = new_size > this_array.elements().capacity() ? 0 : position;

        // Add elements to the SEPT itself (this makes copies of value).
        this_array.elements().insert(this_array.elements().begin()+position, count, value);
        // Now add the corresponding SEPTItem children, populating them with pointers to the corresponding
        // elements of this_array.
        {
            std::vector<std::unique_ptr<SEPTItem>> new_children;
            new_children.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                // Note that this will construct an SEPTItem and recursively populate it with the given sept::Data.
                new_children.emplace_back(std::make_unique<SEPTItem>(&this_array.elements()[position+i]));
            }
            this->insert(position, std::make_move_iterator(new_children.begin()), std::make_move_iterator(new_children.end()));
        }
        // Now because the insert operation invalidated a bunch of pointers, make sure to update
        // the necessary children.
        for (size_t i = first_invalidated_index; i < position; ++i) {
            SEPTItem *child = this->child(i);
            assert(child != nullptr);
            assert(child->data() != nullptr);
            // This just updates the pre-existing child to point to the SEPT data that it used to.
            child->update(&this_array.elements()[i]);
        }
        for (size_t i = position+count; i < this_array.elements().size(); ++i) {
            SEPTItem *child = this->child(i);
            assert(child != nullptr);
            assert(child->data() != nullptr);
            // This just updates the pre-existing child to point to the SEPT data that it used to.
            child->update(&this_array.elements()[i]);
        }

        // TODO: Consider using some sort of relative index, like a pointer to the parent SEPT (e.g. ArrayTerm_c)
        // and the index within that array.  This would be equivalent to caching the value of
        // index_within_parent.

        verify(data()); // TODO: Make into debug-only

        return true;
    } else {
        throw std::runtime_error(LVD_FMT("can't insert children into type " << data()->type().name()));
    }
}

bool SEPTItem::remove_children (size_t position, size_t count) {
    // Disallow removing children from the invisible "real" root node.
    if (is_root())
        return false;

    if (position + count > child_count())
        return false;

    verify(data()); // TODO: Make into debug-only

    // TODO: Here is where support for other hierarchical types (e.g. Map) would be added
    if (data()->type() == typeid(sept::ArrayTerm_c)) {
        auto &this_array = data()->cast<sept::ArrayTerm_c &>();
        assert(this_array.elements().size() == this->child_count());

        // Erase SEPT values, then erase corresponding children.
        this_array.elements().erase(this_array.elements().begin()+position, this_array.elements().begin()+position+count);
        this->erase(position, count);
        assert(this_array.elements().size() == this->child_count());
        // Go through and update the sept_data pointers in the shifted elements.
        for (size_t i = position; i < this_array.elements().size(); ++i) {
            SEPTItem *child = this->child(i);
            assert(child != nullptr);
            assert(child->data() != nullptr);
            // This just updates the pre-existing child to point to the SEPT data that it used to.
            child->update(&this_array.elements()[i]);
        }

        verify(data()); // TODO: Make into debug-only

        return true;
    } else {
        throw std::runtime_error(LVD_FMT("can't insert children into type " << data()->type().name()));
    }
}

bool SEPTItem::set_data_as_QVariant (QModelIndex const &index, QVariant const &value, int role) {
    if (index.column() != 1) {
        assert(false && "can only set data for column 1");
        return false;
    }

    assert(data() != nullptr);
    assert(data()->has_value());

    // For now, all columns and roles will strictly influence data() and no other state variables.

    switch (role) {
        case Qt::DisplayRole: // QString
            assert(false && "shouldn't be calling set_data_as_QVariant with role = Qt::DisplayRole");
            return false;

        case Qt::EditRole: // QString
            if (data()->type() == typeid(int)) *data() = value.toInt();
            else if (data()->type() == typeid(double)) *data() = value.toDouble();
            else if (data()->type() == typeid(char)) *data() = value.toChar();
            else if (data()->type() == typeid(bool)) *data() = value.toBool();
            else throw std::runtime_error(LVD_FMT("unsupported data()->type: " << data()->type()));
            return true;

        case Qt::CheckStateRole: // QCheckState
            assert(is_checkable(index.column()));
            assert(data()->type() == typeid(bool) && "this is based on is_checkable");
//             std::cout << "SEPTItem::set_data_as_QVariant(column = " << index.column() << ", value.type() = " << value.type() << ", role = " << role << ")\n";
            *data() = value.toInt() == Qt::Checked;
            return true;

        case Qt::DecorationRole: // QColor, QIcon, or QPixmap
        case Qt::ToolTipRole: // QString
        case Qt::StatusTipRole: // QString
        case Qt::WhatsThisRole: // QString
        case Qt::FontRole: // QFont
        case Qt::TextAlignmentRole: // Qt::Alignment
        case Qt::BackgroundRole: // QBrush
        case Qt::ForegroundRole: // QBrush
        case Qt::InitialSortOrderRole: // Qt::SortOrder
        default:
            assert(false && "not supported");
            return false;
    }
}

bool SEPTItem::is_editable (size_t column) const {
    assert(data() != nullptr);
    assert(data()->has_value());

    if (column == 1) {
        // Can't (yet?) directly set an ArrayTerm_c value.
        if (data()->type() == typeid(sept::ArrayTerm_c))
            return false;
        else
            return true;
    } else {
        // Other columns are not yet editable.
        return false;
    }
}

bool SEPTItem::is_checkable (size_t column) const {
    assert(data() != nullptr);
    assert(data()->has_value());

    if (column == 1)
        return data()->type() == typeid(bool);
    else
        return false;
}

// QVariant SEPTItem::checked_state (size_t column) const {
//     assert(data() != nullptr);
//     assert(data()->has_value());
//
//     if (column == 0 && data()->type() == typeid(bool))
//         return data()->cast<bool>() ? Qt::Checked : Qt::Unchecked;
//
//     assert(false && "this probably shouldn't happen");
//     return QVariant();
// }

void SEPTItem::verify (sept::Data const *expected_sept_data) const {
    assert(data() != nullptr);
    assert(data()->has_value());
    assert(data()->type() != typeid(std::vector<std::vector<bool>>)); // This is just designed to access *data() and if OOB, to trigger a crash.
    assert(data() == expected_sept_data);

    // Recursively verify.
    if (data()->type() == typeid(sept::ArrayTerm_c)) {
        auto &a = data()->cast<sept::ArrayTerm_c&>();
        assert(a.elements().size() == child_count());
        for (size_t i = 0; i < a.elements().size(); ++i) {
            child(i)->verify(&a.elements()[i]);
        }
    }
}
