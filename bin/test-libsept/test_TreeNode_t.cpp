// 2020.03.29 - Victor Dods

#include "sept/TreeNode_t.hpp"

#include <cassert>
#include <iostream>
#include <lvd/test.hpp>

// A little contrived, but does move-semantic testing.
class Hippo {
public:

    Hippo () = delete;
    Hippo (Hippo const &) = delete;
    Hippo (Hippo &&other) : m_x(std::move(other.m_x)), m_y(std::move(other.m_y)) { }
    Hippo (double x, int y) : m_x(std::make_unique<double>(x)), m_y(std::make_unique<int>(y)) { }

    Hippo &operator = (Hippo const &) = delete;
    Hippo &operator = (Hippo &&other) {
        m_x = std::move(other.m_x);
        m_y = std::move(other.m_y);
        return *this;
    }

    double x () const { return *m_x; }
    int y () const { return *m_y; }

private:

    std::unique_ptr<double> m_x;
    std::unique_ptr<int> m_y;
};

std::ostream &operator << (std::ostream &out, Hippo const &h) {
    return out << "Hippo(x = " << h.x() << ", y = " << h.y() << ')';
}

// TODO: Actually use req.hpp to check values

LVD_TEST_BEGIN(800__TreeNode_t__0)
    using T = sept::TreeNode_t<int>;
    test_log << lvd::Log::dbg() << "creating root node\n";
    T root(123);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling push_back on root node\n";
    root.push_back(std::make_unique<T>(456));
    root.push_back(std::make_unique<T>(789));
    root.push_back(std::make_unique<T>(666));
    root.push_back(std::make_unique<T>(777));
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling emplace_back on root node\n";
    for (int i = 1000; i <= 1010; ++i)
        root.emplace_back(i);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling erase (single at 2) on root node\n";
    root.erase(2);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling erase (range 2 to 2+3) on root node\n";
    root.erase(2, 3);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling pop_back on root node\n";
    root.pop_back();
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling insert (single at 4) on root node\n";
    root.insert(4, std::make_unique<T>(99999));
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling insert (range 3 to 3+3) on root node\n";
    // NOTE: Because std::initializer_list involves a copy of its elements, can't use it for move-only types such as std::unique_ptr<T>.
    std::vector<std::unique_ptr<T>> new_range;
    new_range.emplace_back(std::make_unique<T>(101010));
    new_range.emplace_back(std::make_unique<T>(202020));
    new_range.emplace_back(std::make_unique<T>(303030));
    root.insert(3, std::make_move_iterator(new_range.begin()), std::make_move_iterator(new_range.end()));
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling emplace (single at 5) on root node\n";
    root.emplace(5, 66666666);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling assign_data on root.child(3)\n";
    root.child(3)->assign_data(4004);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling emplace_data on root.child(4)\n";
    root.child(4)->assign_data(6005);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';
LVD_TEST_END

LVD_TEST_BEGIN(800__TreeNode_t__1)
    using T = sept::TreeNode_t<Hippo>;
    test_log << lvd::Log::dbg() << "creating root node\n";
    T root(4.08, 333);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "adding child nodes\n";
    root.emplace_back(0.0001, 1000);
    root.emplace_back(0.0145, 2000);
    root.emplace_back(100.001, 3000);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling assign_data on root.child(1)\n";
    root.child(1)->assign_data(Hippo(8.8, 3030000));
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling emplace_data on root.child(1)\n";
    root.child(1)->emplace_data(999000.0, 10000000);
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';

    test_log << lvd::Log::dbg() << "calling insert (range 1 to 1+3) on root node\n";
    // NOTE: Because std::initializer_list involves a copy of its elements, can't use it for move-only types.
    std::vector<std::unique_ptr<T>> new_range;
    new_range.emplace_back(std::make_unique<T>(10.10, 10101010));
    new_range.emplace_back(std::make_unique<T>(20.20, 20202020));
    new_range.emplace_back(std::make_unique<T>(30.30, 30303030));
    // TODO: Could potentially try to support this, though it makes TreeNode_t::insert a little more complex.
//     std::vector<T> new_range;
//     new_range.emplace_back(10.10, 10101010);
//     new_range.emplace_back(20.20, 20202020);
//     new_range.emplace_back(30.30, 30303030);
    // TODO: Could potentially try to support this, though it makes TreeNode_t::insert a little more complex.
//     std::vector<Hippo> new_range;
//     new_range.emplace_back(10.10, 10101010);
//     new_range.emplace_back(20.20, 20202020);
//     new_range.emplace_back(30.30, 30303030);
    root.insert(1, std::make_move_iterator(new_range.begin()), std::make_move_iterator(new_range.end()));
    test_log << lvd::Log::dbg() << LVD_REFLECT(root) << '\n';
LVD_TEST_END
