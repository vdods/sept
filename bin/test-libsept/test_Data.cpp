// 2020.04.09 - Victor Dods

#include <cstdint>
#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ArrayTerm.hpp"
#include "sept/Data.hpp"

struct GoodDonkey {
    GoodDonkey () = delete;
    GoodDonkey (GoodDonkey const &other) : m_value(other.m_value) {
        ++ms_copy_constructor_count;
//         std::cout << "GoodDonkey copy constructor called; " << LVD_REFLECT(m_value) << '\n';
    }
    GoodDonkey (GoodDonkey &&other) noexcept : m_value(other.m_value) {
        ++ms_move_constructor_count;
//         std::cout << "GoodDonkey move constructor called; " << LVD_REFLECT(m_value) << '\n';
    }
    GoodDonkey (int value) : m_value(value) { }

    GoodDonkey &operator = (GoodDonkey const &other) {
        ++ms_copy_assignment_count;
//         std::cout << "GoodDonkey copy assignment called; " << LVD_REFLECT(m_value) << '\n';
        m_value = other.m_value;
        return *this;
    }
    GoodDonkey &operator = (GoodDonkey &&other) noexcept {
        ++ms_move_assignment_count;
//         std::cout << "GoodDonkey move assignment called; " << LVD_REFLECT(m_value) << '\n';
        m_value = other.m_value;
        return *this;
    }

    static void reset_counters () {
        ms_copy_constructor_count = 0;
        ms_move_constructor_count = 0;
        ms_copy_assignment_count = 0;
        ms_move_assignment_count = 0;
    }

    int m_value;
    static int ms_copy_constructor_count;
    static int ms_move_constructor_count;
    static int ms_copy_assignment_count;
    static int ms_move_assignment_count;
};

int GoodDonkey::ms_copy_constructor_count = 0;
int GoodDonkey::ms_move_constructor_count = 0;
int GoodDonkey::ms_copy_assignment_count = 0;
int GoodDonkey::ms_move_assignment_count = 0;

std::ostream &operator << (std::ostream &out, GoodDonkey const &d) {
    return out << "GoodDonkey(" << d.m_value << ')';
}

struct BadDonkey {
    BadDonkey () = delete;
    BadDonkey (BadDonkey const &other) : m_value(other.m_value) {
        ++ms_copy_constructor_count;
//         std::cout << "BadDonkey copy constructor called; " << LVD_REFLECT(m_value) << '\n';
    }
    BadDonkey (BadDonkey &&other) : m_value(other.m_value) {
        ++ms_move_constructor_count;
//         std::cout << "BadDonkey move constructor called; " << LVD_REFLECT(m_value) << '\n';
    }
    BadDonkey (int value) : m_value(value) { }

    BadDonkey &operator = (BadDonkey const &other) {
        ++ms_copy_assignment_count;
//         std::cout << "BadDonkey copy assignment called; " << LVD_REFLECT(m_value) << '\n';
        m_value = other.m_value;
        return *this;
    }
    BadDonkey &operator = (BadDonkey &&other) {
        ++ms_move_assignment_count;
//         std::cout << "BadDonkey move assignment called; " << LVD_REFLECT(m_value) << '\n';
        m_value = other.m_value;
        return *this;
    }

    static void reset_counters () {
        ms_copy_constructor_count = 0;
        ms_move_constructor_count = 0;
        ms_copy_assignment_count = 0;
        ms_move_assignment_count = 0;
    }

    int m_value;
    static int ms_copy_constructor_count;
    static int ms_move_constructor_count;
    static int ms_copy_assignment_count;
    static int ms_move_assignment_count;
};

int BadDonkey::ms_copy_constructor_count = 0;
int BadDonkey::ms_move_constructor_count = 0;
int BadDonkey::ms_copy_assignment_count = 0;
int BadDonkey::ms_move_assignment_count = 0;

std::ostream &operator << (std::ostream &out, BadDonkey const &d) {
    return out << "BadDonkey(" << d.m_value << ')';
}

template <typename T_, typename Allocator_>
std::ostream &operator << (std::ostream &out, std::vector<T_,Allocator_> const &v) {
    out << '(';
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i+1 < v.size())
            out << ", ";
    }
    return out << ')';
}

LVD_TEST_BEGIN(200__Data__0)
    sept::Data d(3);
    LVD_TEST_REQ_EQ(d, 3);

    int n = d.cast<int>();
    LVD_TEST_REQ_EQ(n, 3);

    int const &cref_n = d.cast<int const &>();
    LVD_TEST_REQ_EQ(cref_n, 3);

    // This basically asserts that cref_n refers to the actual int inside of d.
    LVD_TEST_REQ_LEQ(reinterpret_cast<uint8_t const *>(&d), reinterpret_cast<uint8_t const *>(&cref_n));
    LVD_TEST_REQ_LEQ(reinterpret_cast<uint8_t const *>(&cref_n), reinterpret_cast<uint8_t const *>(&d) + sizeof(d));

    // Now verify that it can be changed via reference.
    int &ref_n = d.cast<int &>();
    LVD_TEST_REQ_EQ(ref_n, 3);
    LVD_TEST_REQ_LEQ(reinterpret_cast<uint8_t *>(&d), reinterpret_cast<uint8_t *>(&ref_n));
    LVD_TEST_REQ_LEQ(reinterpret_cast<uint8_t *>(&ref_n), reinterpret_cast<uint8_t *>(&d) + sizeof(d));
    ref_n = 1000;
    LVD_TEST_REQ_EQ(d, 1000);
LVD_TEST_END

LVD_TEST_BEGIN(200__Data__1)
    sept::Data d(sept::make_array(1,2,3));
//     std::cout << LVD_REFLECT(d) << '\n';
    LVD_TEST_REQ_EQ(d, sept::make_array(1,2,3));

    sept::ArrayTerm_c a = d.cast<sept::ArrayTerm_c>();
//     std::cout << LVD_REFLECT(a) << '\n';
    LVD_TEST_REQ_EQ(a, sept::make_array(1,2,3));

    sept::ArrayTerm_c const &cref_a = d.cast<sept::ArrayTerm_c const &>();
    LVD_TEST_REQ_EQ(cref_a, sept::make_array(1,2,3));

    // It isn't true that &cref_a will be inside the memory footprint of d, since sept::ArrayTerm_c
    // is too big for the internal buffer of Data (i.e. std::any).

    // Now verify that it can be changed via reference.
    sept::ArrayTerm_c &ref_a = d.cast<sept::ArrayTerm_c &>();
    LVD_TEST_REQ_EQ(ref_a, sept::make_array(1,2,3));
    ref_a = sept::make_array(22,44,66);
    LVD_TEST_REQ_EQ(d, sept::make_array(22,44,66));
LVD_TEST_END

LVD_TEST_BEGIN(200__vector_insert__0)
    GoodDonkey::reset_counters();
    std::vector<GoodDonkey> v;
    v.reserve(4); // So that the insertion doesn't require a realloc.
    v.emplace_back(GoodDonkey(1));
    v.emplace_back(GoodDonkey(2));
    v.emplace_back(GoodDonkey(3));
//     test_log << lvd::Log::dbg() << "before insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_constructor_count, 3);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_assignment_count, 0);
    // Because GoodDonkey's move constructor is `noexcept`, std::vector will use the move
    // constructor to displace the existing elements in this insert call.
    v.insert(v.begin()+1, GoodDonkey(4));
//     test_log << lvd::Log::dbg() << "after insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_constructor_count, 4);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_assignment_count, 2);
LVD_TEST_END

LVD_TEST_BEGIN(200__vector_insert__1)
    BadDonkey::reset_counters();
    std::vector<BadDonkey> v;
    v.reserve(4); // So that the insertion doesn't require a realloc.
    v.emplace_back(BadDonkey(1));
    v.emplace_back(BadDonkey(2));
    v.emplace_back(BadDonkey(3));
//     test_log << lvd::Log::dbg() << "before insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_constructor_count, 3);
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_assignment_count, 0);
    // This is super stupid, even though BadDonkey has a move constructor, std::vector::insert
    // still uses the copy constructor.  Which is totally unacceptable when the vector's
    // element type has a nontrivial copy (e.g. if the element type were std::vector<T>).
    v.insert(v.begin()+1, BadDonkey(4));
//     test_log << lvd::Log::dbg() << "after insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_constructor_count, 4);
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_assignment_count, 2);
LVD_TEST_END

LVD_TEST_BEGIN(200__vector_resize__0)
    GoodDonkey::reset_counters();
    std::vector<GoodDonkey> v;
    v.reserve(3); // So that the insert requires a realloc.
    GoodDonkey *initial_ptr = &v[0];
    LVD_TEST_REQ_EQ(v.capacity(), size_t(3));
    v.emplace_back(GoodDonkey(1));
    v.emplace_back(GoodDonkey(2));
    v.emplace_back(GoodDonkey(3));
//     test_log << lvd::Log::dbg() << "before insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_constructor_count, 3);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_assignment_count, 0);
    // This emplace_back should cause a reallocation, and because GoodDonkey's move constructor
    // is `noexcept`, std::vector will use the move constructor when transitioning elements to
    // the new allocation.
    v.emplace_back(GoodDonkey(4));
//     test_log << lvd::Log::dbg() << "after insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_NEQ(&v[0], initial_ptr);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_constructor_count, 7);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(GoodDonkey::ms_move_assignment_count, 0);
LVD_TEST_END

LVD_TEST_BEGIN(200__vector_resize__1)
    BadDonkey::reset_counters();
    std::vector<BadDonkey> v;
    v.reserve(3); // So that the insert requires a realloc.
    BadDonkey *initial_ptr = &v[0];
    LVD_TEST_REQ_EQ(v.capacity(), size_t(3));
    v.emplace_back(BadDonkey(1));
    v.emplace_back(BadDonkey(2));
    v.emplace_back(BadDonkey(3));
//     test_log << lvd::Log::dbg() << "before insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_constructor_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_constructor_count, 3);
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_assignment_count, 0);
    // This emplace_back should cause a reallocation, and because BadDonkey's move constructor
    // is NOT `noexcept`, std::vector will use the copy constructor when transitioning elements
    // to the new allocation.
    v.emplace_back(BadDonkey(4));
//     test_log << lvd::Log::dbg() << "after insert: " << LVD_REFLECT(v) << '\n';
    LVD_TEST_REQ_NEQ(&v[0], initial_ptr);
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_constructor_count, 3);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_constructor_count, 4);
    LVD_TEST_REQ_EQ(BadDonkey::ms_copy_assignment_count, 0);
    LVD_TEST_REQ_EQ(BadDonkey::ms_move_assignment_count, 0);
LVD_TEST_END
