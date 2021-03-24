// 2020.03.29 - Victor Dods

#pragma once

#include <lvd/Range_t.hpp>
#include <memory>
#include <ostream>
#include "sept/core.hpp"
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace sept {

// A generic tree node implementation.  Each node can optionally have a finite number of child nodes,
// indexed by the natural numbers.  Each node knows its own parent node.  Each node stores data
// of type given by the template parameter Data_.
//
// This class uses CRTP (https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
// to specify the derived class via the Derived_ template parameter, with a default of void (which
// would indicate there is no derived class).
//
// Note that you must use std::unique_ptr<Derived<Data_>> to handle any non-root tree node,
// and you must then explicitly add that node to the appropriate parent node (this is because
// there's no std::unique_from_this, with which the child could add itself to a parent specified
// in its constructor).
template <typename Data_, typename Derived_ = void>
class TreeNode_t {
public:

    using Data = Data_;
    // If Derived_ is void, then use this class as Derived.  Otherwise use Derived_ directly.
    using Derived = std::conditional_t<std::is_same_v<Derived_,void>,TreeNode_t,Derived_>;
//     static_assert(std::is_base_of_v<TreeNode_t,Derived>, "Derived_ must specify a subclass of TreeNode_t, or void to use TreeNode_t itself (this is the default behavior)");

    // Default constructor, initializes m_parent to nullptr.
    TreeNode_t () : m_parent(nullptr), m_data() { }
    // No copy-construction, because the child/parent pointer relationship wouldn't be preserved.
    TreeNode_t (TreeNode_t const &) = delete;
    // No move-construction, because the child/parent pointer relationship wouldn't be preserved.
    // Technically there exists a valid implementation (where the child/parent pointer relationship
    // is updated), but there really is no need for this.
    TreeNode_t (TreeNode_t &&) = delete;
    // Perfect-forward to construct m_data, initializes m_parent to nullptr.
    template <typename... Args_>
    TreeNode_t (Args_&&... args) : m_parent(nullptr), m_data(std::forward<Args_>(args)...) { }

    // No explicit destructor is needed, because all types handle their own deletion properly.

    // No copy-assignment, for same reason as no copying.
    Derived &operator = (TreeNode_t const &) = delete;
    // No move-assignment, because the child/parent pointer relationship wouldn't be preserved.
    // Technically there exists a valid implementation (where the child/parent pointer relationship
    // is updated), but there really is no need for this.
    Derived &operator = (TreeNode_t &&) = delete;

    // Returns data by const reference.
    Data const &data () const & { return m_data; }
    // Returns data by non-const reference.
    // NOTE: If children were indexed by value, then this would need to update the parent's indexing.
    Data &data () & { return m_data; }
    // Returns data by rvalue reference.
    Data &&data () && { return m_data; }

    // Returns true iff there is no parent.
    bool is_root () const { return m_parent == nullptr; }
    // Returns the parent, which could be nullptr.
    Derived const *parent () const { return m_parent; }
    // Returns the parent, which could be nullptr.
    Derived *parent () { return m_parent; }
    // Returns the tree depth of this node, i.e. the number of ancestors to get to the root.
    // In particular, the root node has a depth of 0.
    size_t tree_depth () const { return is_root() ? 0 : 1+m_parent->tree_depth(); }
    // Returns the number of children.
    size_t child_count () const { return m_children.size(); }
    // Returns the child at the given index, or throws if the index is invalid.
    Derived const *child (size_t index) const {
        return m_children.at(index).get();
    }
    // Returns the child at the given index, or throws if the index is invalid.
    Derived *child (size_t index) {
        return m_children.at(index).get();
    }
    // Returns the last child. Throws an exception if there are no children.
    Derived const *back () const {
        if (m_children.empty())
            throw std::runtime_error("can't call Derived::back with no children");
        return m_children.back().get();
    }
    // If there's no parent, then this returns 0.  Otherwise returns the index of this
    // node as a child of its parent, such that
    //
    //     this->parent()->child(this->index_within_parent()) == this
    //
    // This has O(child_count) complexity, since it has to iterate through the parent's children.
    size_t index_within_parent () const {
        if (m_parent == nullptr)
            return 0; // Arbitrary

        size_t i = 0;
        for (std::unique_ptr<Derived> const &child : m_parent->m_children) {
            if (child.get() == this)
                return i;
            ++i;
        }
        throw std::runtime_error("TreeNode_t<index_within_parent> did not find child in parent; if this happens, then there's an implementation bug in the parent/child indexing");
    }

    // Perfect-forward to set m_data.  This assigns to m_data, and will take advantage of
    // the move-assignment operator of Data if present, otherwise will use copy-assignment.
    // NOTE: If children were indexed by value, then this would need to update the parent's indexing.
    template <typename Arg_>
    void assign_data (Arg_ &&arg) {
        m_data = std::forward<Arg_>(arg);
    }
    // Perfect-forward to set m_data by in-place construction.  This assigns to m_data, and will
    // take advantage of the move-assignment operator of Data if present, otherwise will use copy-assignment.
    // NOTE: If children were indexed by value, then this would need to update the parent's indexing.
    template <typename... Args_>
    void emplace_data (Args_&&... args) {
        m_data = Data(std::forward<Args_>(args)...);
    }

    //
    // The following methods more or less correspond with std::vector methods, with only slight differences.
    //

    // Reserves space in the m_children vector.
    void reserve (size_t capacity) { m_children.reserve(capacity); }
    // Removes all children and deletes them.
    void clear () { m_children.clear(); }
    // Inserts a child at the given index.  It's an error to call insert if child
    // already has a parent, and an exception will be thrown in that case.  Returns
    // a (non-owning) pointer to the newly inserted child.
    Derived *insert (size_t index, std::unique_ptr<Derived> &&child) {
        if (child->m_parent != nullptr)
            throw std::runtime_error("can't call TreeNode_t::push_back if child already has parent");
        child->m_parent = static_cast<Derived*>(this);
        Derived *retval = child.get();
        m_children.insert(m_children.begin()+index, std::move(child));
        return retval;
    }
    // Inserts a range of children at the given index.  It's an error to call insert if any
    // of the children already has a parent, and an exception will be thrown in that case.
    // Note that because m_children uses std::unique_ptr, it's necessary to provide an iterator
    // range that uses move semantics, hence the std::move_iterator.  Also note that the constructor
    // of std::move_iterator is explicit, so this method won't automatically apply move semantics
    // to a non-move_iterator argument.
    template <typename InputIterator_>
    void insert (size_t index, std::move_iterator<InputIterator_> first, std::move_iterator<InputIterator_> last) {
        // First, check that each input child has no parent, and then set the parent.
        for (auto const &new_child : lvd::range(first, last)) {
            if (new_child->m_parent != nullptr)
                throw std::runtime_error("can't call TreeNode_t::insert if any child-to-be already has parent");
            new_child->m_parent = static_cast<Derived*>(this);
        }
        m_children.insert(m_children.begin()+index, first, last);
    }
    // Inserts a child at the given index, constructing the child in-place (the use of std::unique_ptr
    // will be automatic).  The added child will be returned as a non-owning pointer.
    template <typename... Args_>
    Derived *emplace (size_t index, Args_&&... args) {
        auto child = std::make_unique<Derived>(std::forward<Args_>(args)...);
        child->m_parent = static_cast<Derived*>(this);
        Derived *retval = child.get();
        m_children.insert(m_children.begin()+index, std::move(child));
        return retval;
    }
    // Erases the child at the given index.  If the child index is out of range,
    // an exception will be thrown.
    void erase (size_t index) {
        m_children.erase(m_children.begin()+index);
    }
    // Erases count children starting at the given index.  If index exceeds the number
    // of children, then no child will be erased.  If the range of children exceeds the
    // valid range (i.e. if index+count > this->child_count()), then count will be
    // truncated to this->child_could()-index, meaning that all children starting at
    // index will be erased.
    void erase (size_t index, size_t count) {
        // Check for no-op condition.
        if (index >= m_children.size())
            return; // Nothing to do.
        // Truncate count if necessary.
        if (index + count > m_children.size())
            count = m_children.size() - index;
        m_children.erase(m_children.begin()+index, m_children.begin()+index+count);
    }
    // Appends a child to the list of children.  Note that the child must be handled using
    // std::unique_ptr.  It's an error to call push_back if child already has a parent, and
    // an exception will be thrown in that case.  The added child will be returned as a
    // non-owning pointer.
    Derived *push_back (std::unique_ptr<Derived> &&child) {
        if (child->m_parent != nullptr)
            throw std::runtime_error("can't call TreeNode_t::push_back if child already has parent");
        child->m_parent = static_cast<Derived*>(this);
        Derived *retval = child.get();
        m_children.emplace_back(std::move(child));
        return retval;
    }
    // Appends a child to the list of children, constructing that child in-place (the use of
    // std::unique_ptr will be automatic).  The added child will be returned as a non-owning
    // pointer.
    template <typename... Args_>
    Derived *emplace_back (Args_&&... args) {
        m_children.emplace_back(std::make_unique<Derived>(std::forward<Args_>(args)...));
        auto &child = m_children.back();
        child->m_parent = static_cast<Derived*>(this);
        return child.get();
    }
    // Removes the last child from the children and returns it.  In the process of doing so,
    // it sets that child's parent to nullptr, thereby making it into a root node.  If you
    // don't move-assign the return value of this method, the child will be automatically deleted.
    // If there are no children, an exception will be thrown.
    std::unique_ptr<Derived> pop_back () {
        if (m_children.empty())
            throw std::runtime_error("can't call TreeNode_t::pop_back with no children");
        std::unique_ptr<Derived> retval(std::move(m_children.back()));
        retval->m_parent = nullptr;
        m_children.pop_back();
        return retval;
    }

    void print (std::ostream &out, size_t indent_level = 0) const {
        out << std::string(indent_level*4, ' ') << "TreeNode_t(data = " << m_data;
        if (is_root())
            out << ", is_root = true";
        if (m_children.empty()) {
            out << ")\n";
        } else {
            out << ", child_count = " << child_count() << ") {\n";
            for (auto const &child : m_children)
                child->print(out, indent_level+1);
            out << std::string(indent_level*4, ' ') << "}\n";
        }
    }

private:

    using ChildPtr = std::unique_ptr<Derived>;
    using ChildVector = std::vector<ChildPtr>;

    // Pointer to parent, or nullptr if this is a/the root node.
    Derived *m_parent;
    // Vector of children.  This assumes the children are indexed by natural numbers.
    // Other types of indexing could include keys (e.g. in a map) or the values themselves
    // (e.g. in a set).
    ChildVector m_children;
    // This holds the actual data for this TreeNode_t.
    Data m_data;
};

template <typename Data_, typename Derived_>
std::ostream &operator << (std::ostream &out, TreeNode_t<Data_,Derived_> const &tn) {
    tn.print(out, 0);
    return out;
}

} // end namespace sept
