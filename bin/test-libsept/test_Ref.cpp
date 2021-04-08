// 2021.03.31 - Victor Dods

#include <array>
#include <lvd/test.hpp>
#include "req.hpp"
#include "sept/GlobalSymRef.hpp"
#include "sept/LocalSymRef.hpp"
#include "sept/MemRef.hpp"
#include "sept/NPType.hpp"

LVD_TEST_BEGIN(572__Ref__0)
    // Test MemRef's instantiation.
    sept::Data x{10.5};
    test_log << lvd::Log::trc()
             << LVD_REFLECT(x) << '\n'
             << LVD_REFLECT(x.can_cast<double>()) << '\n'
             << LVD_REFLECT(x.can_cast<double const>()) << '\n'
             << LVD_REFLECT(x.is_ref()) << '\n'
             << LVD_REFLECT(x.cast<double>()) << '\n'
//              << LVD_REFLECT(x.is_ref_and_can_cast<double>()) << '\n'
//              << LVD_REFLECT(x.is_ref_and_can_cast<double const>()) << '\n'
             << '\n';
//     LVD_TEST_REQ_EQ(sept::MemRef(&x), sept::make_mem_ref(&x));
    LVD_TEST_REQ_EQ(sept::MemRef(&x), sept::RefTerm_c(lvd::make_nnup<sept::MemRefTermImpl>(&x)));

    // Test abstract_type_of
    auto r = sept::MemRef(&x);
    auto dr = sept::Data{r};
    test_log << lvd::Log::trc()
             << LVD_REFLECT(dr) << '\n'
             << LVD_REFLECT(dr.can_cast<double>()) << '\n'
             << LVD_REFLECT(dr.can_cast<double const>()) << '\n'
             << LVD_REFLECT(dr.is_ref()) << '\n'
             << LVD_REFLECT(dr.cast<double>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<double>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<double const>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<int>()) << '\n'
//              << LVD_REFLECT(dr.is_ref_and_can_cast<float>()) << '\n'
             << '\n';

    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(x), sept::Float64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(r), sept::Float64);
    LVD_TEST_REQ_EQ(sept::abstract_type_of_data(dr), sept::Float64);

    LVD_TEST_REQ_EQ(x, sept::Float64(10.5));
    // TODO
//     LVD_TEST_REQ_EQ(r, sept::Float64(10.5));
//     LVD_TEST_REQ_EQ(dr, sept::Float64(10.5));

LVD_TEST_END

LVD_TEST_BEGIN(572__Ref__1__MemRef)
    auto d = sept::Data{sept::Uint32(123456)};
    auto mr = sept::MemRef(&d);
    LVD_TEST_REQ_EQ(mr, d);
    auto dmr = sept::Data{mr};
    LVD_TEST_REQ_EQ(d, d);
    LVD_TEST_REQ_EQ(d, mr);
    LVD_TEST_REQ_EQ(d, dmr);
    LVD_TEST_REQ_EQ(mr, d);
    LVD_TEST_REQ_EQ(mr, mr);
    LVD_TEST_REQ_EQ(mr, dmr);
    LVD_TEST_REQ_EQ(dmr, d);
    LVD_TEST_REQ_EQ(dmr, mr);
    LVD_TEST_REQ_EQ(dmr, dmr);

    // Double reference
    auto mr_dmr = sept::MemRef(&dmr);
    LVD_TEST_REQ_EQ(mr_dmr, d);
    LVD_TEST_REQ_EQ(mr_dmr, mr);
    LVD_TEST_REQ_EQ(mr_dmr, dmr);
    LVD_TEST_REQ_EQ(mr_dmr, mr_dmr);
    LVD_TEST_REQ_EQ(d, mr_dmr);
    LVD_TEST_REQ_EQ(mr, mr_dmr);
    LVD_TEST_REQ_EQ(dmr, mr_dmr);
    LVD_TEST_REQ_EQ(mr_dmr, mr_dmr);

    // Due to referential transparency, all of these should inhabit sept::Uint32
    LVD_TEST_REQ_IS_TRUE(inhabits_data(d, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(mr, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(dmr, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(mr_dmr, sept::Uint32));
LVD_TEST_END

LVD_TEST_BEGIN(572__Ref__2__GlobalSymRef)
    // Clear the global SymbolTable before proceeding, in case it's been used in this process already.
    sept::global_symbol_table()->clear();

    auto d = sept::Data{sept::Uint32(123456)};
    sept::global_symbol_table()->define_symbol("hippo", d);
    auto sr = sept::GlobalSymRef("hippo");
    LVD_TEST_REQ_EQ(sr, d);
    auto dsr = sept::Data{sr};
    LVD_TEST_REQ_EQ(d, d);
    LVD_TEST_REQ_EQ(d, sr);
    LVD_TEST_REQ_EQ(d, dsr);
    LVD_TEST_REQ_EQ(sr, d);
    LVD_TEST_REQ_EQ(sr, sr);
    LVD_TEST_REQ_EQ(sr, dsr);
    LVD_TEST_REQ_EQ(dsr, d);
    LVD_TEST_REQ_EQ(dsr, sr);
    LVD_TEST_REQ_EQ(dsr, dsr);

    // Double reference
    sept::global_symbol_table()->define_symbol("ostrich", sr);
    auto sr_dsr = sept::GlobalSymRef("ostrich");
    LVD_TEST_REQ_EQ(sr_dsr, d);
    LVD_TEST_REQ_EQ(sr_dsr, sr);
    LVD_TEST_REQ_EQ(sr_dsr, dsr);
    LVD_TEST_REQ_EQ(sr_dsr, sr_dsr);
    LVD_TEST_REQ_EQ(d, sr_dsr);
    LVD_TEST_REQ_EQ(sr, sr_dsr);
    LVD_TEST_REQ_EQ(dsr, sr_dsr);
    LVD_TEST_REQ_EQ(sr_dsr, sr_dsr);

    // Due to referential transparency, all of these should inhabit sept::Uint32
    LVD_TEST_REQ_IS_TRUE(inhabits_data(d, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(sr, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(dsr, sept::Uint32));
    LVD_TEST_REQ_IS_TRUE(inhabits_data(sr_dsr, sept::Uint32));
LVD_TEST_END

LVD_TEST_BEGIN(572__Ref__3__LocalSymRef)
    // Clear the global SymbolTable before proceeding, in case it's been used in this process already.
    sept::global_symbol_table()->clear();

    // Test this with a local SymbolTable and the global SymbolTable
    auto base_symbol_tables = std::array<lvd::nnsp<sept::SymbolTable>,2>{
        lvd::make_nnsp<sept::SymbolTable>(), // Local SymbolTable
        sept::global_symbol_table(), // Global SymbolTable
    };

    for (auto const &base_symbol_table : base_symbol_tables) {
        auto d = sept::Data{sept::Uint32(123456)};
        base_symbol_table->define_symbol("hippo", d);
        auto sr = sept::LocalSymRef("hippo", base_symbol_table);
        LVD_TEST_REQ_EQ(sr, d);
        auto dsr = sept::Data{sr};
        LVD_TEST_REQ_EQ(d, d);
        LVD_TEST_REQ_EQ(d, sr);
        LVD_TEST_REQ_EQ(d, dsr);
        LVD_TEST_REQ_EQ(sr, d);
        LVD_TEST_REQ_EQ(sr, sr);
        LVD_TEST_REQ_EQ(sr, dsr);
        LVD_TEST_REQ_EQ(dsr, d);
        LVD_TEST_REQ_EQ(dsr, sr);
        LVD_TEST_REQ_EQ(dsr, dsr);

        // Double reference
        base_symbol_table->define_symbol("ostrich", sr);
        auto sr_dsr = sept::LocalSymRef("ostrich", base_symbol_table);
        LVD_TEST_REQ_EQ(sr_dsr, d);
        LVD_TEST_REQ_EQ(sr_dsr, sr);
        LVD_TEST_REQ_EQ(sr_dsr, dsr);
        LVD_TEST_REQ_EQ(sr_dsr, sr_dsr);
        LVD_TEST_REQ_EQ(d, sr_dsr);
        LVD_TEST_REQ_EQ(sr, sr_dsr);
        LVD_TEST_REQ_EQ(dsr, sr_dsr);
        LVD_TEST_REQ_EQ(sr_dsr, sr_dsr);

        // Due to referential transparency, all of these should inhabit sept::Uint32
        LVD_TEST_REQ_IS_TRUE(inhabits_data(d, sept::Uint32));
        LVD_TEST_REQ_IS_TRUE(inhabits_data(sr, sept::Uint32));
        LVD_TEST_REQ_IS_TRUE(inhabits_data(dsr, sept::Uint32));
        LVD_TEST_REQ_IS_TRUE(inhabits_data(sr_dsr, sept::Uint32));

        // Now push a SymbolTable
        auto child_symbol_table = base_symbol_table->push_symbol_table();
        // Define one that doesn't shadow anything
        auto x = sept::Float64(500.5);
        child_symbol_table->define_symbol("donkey", x);
        LVD_TEST_REQ_EQ(sept::LocalSymRef("donkey", child_symbol_table), x);
        // Now test a fallthrough
        LVD_TEST_REQ_EQ(sept::LocalSymRef("hippo", child_symbol_table), sept::LocalSymRef("hippo", base_symbol_table));
        // Now define a symbol that shadows a base symbol.
        auto y = sept::Float64(2500.25);
        child_symbol_table->define_symbol("hippo", y);
        LVD_TEST_REQ_EQ(sept::LocalSymRef("hippo", child_symbol_table), y);
        LVD_TEST_REQ_EQ(sept::LocalSymRef("hippo", base_symbol_table), d);

        lvd::test::call_function_and_expect_exception<std::runtime_error>([&](){
            test_log << lvd::Log::trc() << sept::LocalSymRef("donkey", base_symbol_table);
        });
        lvd::test::call_function_and_expect_exception<std::runtime_error>([&](){
            test_log << lvd::Log::trc() << sept::LocalSymRef("nonexistent", base_symbol_table);
        });
    }
LVD_TEST_END
