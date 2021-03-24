// 2020.07.15 - Victor Dods

#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapType.hpp"
#include "sept/OrderedMapTerm.hpp"

LVD_TEST_BEGIN(550__OrderedMap__OrderedMap)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::OrderedMap), &sept::OrderedMapType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::OrderedMap), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::OrderedMap), sept::OrderedMapType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::OrderedMap), sept::NonParametricTerm);

//     // Test OrderedMap's instantiation.
//     test_log << lvd::Log::dbg() << sept::OrderedMap(std::pair(10,true),std::pair(20,false),std::pair(30,true)) << '\n';
//     LVD_TEST_REQ_EQ(sept::OrderedMap(std::pair(10,true),std::pair(20,false),std::pair(30,true)), sept::OrderedMapTerm_c(sept::DataOrderedMap{std::pair(10,true),std::pair(20,false),std::pair(30,true)}));
//     LVD_TEST_REQ_EQ(sept::OrderedMap(std::pair(10,123),std::pair(20,246),std::pair(30,369)), sept::OrderedMapTerm_c(sept::DataOrderedMap{std::pair(10,123),std::pair(20,246),std::pair(30,369)}));

    // Test OrderedMapTerm_c's size

    sept::OrderedMapTerm_c empty{sept::DataOrderedMap{}};
    sept::OrderedMapTerm_c x{{std::pair(10,123)}};
//     sept::OrderedMapTerm_c xx{sept::DataOrderedMap{std::pair(10,123),std::pair(20,246)}};
    sept::OrderedMapTerm_c xx{{std::pair(10,123),std::pair(20,246)}};
    sept::OrderedMapTerm_c xxx{{std::pair(10,123),std::pair(20,246),std::pair(30,369)}};
    sept::OrderedMapTerm_c xy{{std::pair(10,123),std::pair(20,888)}};
    sept::OrderedMapTerm_c yx{{std::pair(10,222),std::pair(20,246)}};
    sept::OrderedMapTerm_c yy{{std::pair(10,222),std::pair(20,888)}};

    test_log << lvd::Log::dbg() << LVD_REFLECT(empty) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(x) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(xx) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(xxx) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(xy) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(yx) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(yy) << '\n';

    LVD_TEST_REQ_EQ(empty.size(), size_t(0));
    LVD_TEST_REQ_EQ(x.size(), size_t(1));
    LVD_TEST_REQ_EQ(xx.size(), size_t(2));
    LVD_TEST_REQ_EQ(xxx.size(), size_t(3));

    // Test instantiation via OrderedMap

    LVD_TEST_REQ_EQ(sept::OrderedMap(), empty);
    LVD_TEST_REQ_EQ(sept::OrderedMap(std::pair(10,123)), x);
    LVD_TEST_REQ_EQ(sept::OrderedMap(std::pair(10,123),std::pair(20,246)), xx);
    LVD_TEST_REQ_EQ(sept::OrderedMap(std::pair(10,123),std::pair(20,246),std::pair(30,369)), xxx);

    // Test OrderedMap's compare

    SEPT_TEST_REQ_COMPARE(x, x, 0);
    SEPT_TEST_REQ_COMPARE(x, xx, -1);
    SEPT_TEST_REQ_COMPARE(xx, x, 1);
    SEPT_TEST_REQ_COMPARE(xx, xx, 0);
    SEPT_TEST_REQ_COMPARE(xx, xxx, -1);
    SEPT_TEST_REQ_COMPARE(xxx, xx, 1);
    SEPT_TEST_REQ_COMPARE(xxx, xxx, 0);
    SEPT_TEST_REQ_COMPARE(xx, xy, -1);
    SEPT_TEST_REQ_COMPARE(xy, xx, 1);
    SEPT_TEST_REQ_COMPARE(xx, yx, -1);
    SEPT_TEST_REQ_COMPARE(yx, yy, -1);
    SEPT_TEST_REQ_COMPARE(yy, yx, 1);
LVD_TEST_END

LVD_TEST_BEGIN(550__OrderedMap__OrderedMapDC)
    // First, compare exact identity of the singletons involved.
    LVD_TEST_REQ_EQ(&sept::abstract_type_of(sept::OrderedMapDC), &sept::OrderedMapType);
    LVD_TEST_REQ_EQ(&sept::concrete_type_of(sept::OrderedMapDC), &sept::NonParametricTerm);

    // Now, compare values
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::OrderedMapDC), sept::OrderedMapType);
    LVD_TEST_REQ_EQ(sept::concrete_type_of(sept::OrderedMapDC), sept::NonParametricTerm);

//     // Test OrderedMap's instantiation.
//     LVD_TEST_REQ_EQ(
//         sept::OrderedMapDC(sept::Bool,3)(true,false,true),
//         sept::OrderedMapTerm_c(std::vector{true,false,true}).with_constraint(sept::OrderedMapDC(sept::Bool,3))
//     );
LVD_TEST_END
