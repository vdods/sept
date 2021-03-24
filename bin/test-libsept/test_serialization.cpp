// 2020.08.03 - Victor Dods

#include <iomanip>
#include <iostream>
#include <limits>
#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/abstract_type_of.hpp"
#include "sept/ArrayTerm.hpp"
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/EndOfFile.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"
#include "sept/deserialize.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/serialize.hpp"
#include <sstream>

class AsHex {
public:

    AsHex () = delete;
    AsHex (std::string const &str) : m_str(str) { }
    AsHex (std::string &&str) : m_str(std::move(str)) { }
    template <typename... Args_>
    explicit AsHex (Args_&&... args) : m_str(std::forward<Args_>(args)...) { }

    std::string const &str () const { return m_str; }

private:

    std::string m_str;
};

std::ostream &operator << (std::ostream &out, AsHex const &a) {
    std::ostringstream s_out;

    s_out << std::hex;

    size_t i = 0;
    for (uint8_t c : a.str()) {
        if (i > 0) {
            if (i % 0x10 == 0)
                s_out << '\n';
            else
                s_out << ' ';
        }
        s_out << std::setw(2) << std::setfill('0') << int(c);
        ++i;
    }
    s_out << '\n';

    return out << s_out.str();
}

template <typename T_>
void test_serialization_roundtrip (lvd::Log &test_log, lvd::req::Context &req_context, T_ const &t) {
    // There are two kinds of serialization to test:
    // -    Direct type serialization (e.g. sept::serialize(sept::ArrayTerm_c const &, std::ostream &out))
    // -    sept::Data serialization (i.e. sept::serialize_data(sept::Data const &, std::ostream &out))

    using DecayedT_ = std::decay_t<T_>;
//     test_log << lvd::Log::dbg() << "test_serialization_roundtrip;\n    value is " << LVD_REFLECT(t) << "\n    " << LVD_REFLECT(typeid(T_)) << "\n    " << LVD_REFLECT(typeid(DecayedT_)) << '\n';

    sept::Data expected_data{t};

    // Direct type serialization
    {
        std::ostringstream out;
        sept::serialize(t, out);

        test_log << lvd::Log::dbg() << "Raw-typed value " << LVD_REFLECT(t) << " serializes as:\n" << lvd::IndentGuard() << AsHex(out.str());

        std::istringstream in(out.str());
        auto actual_data = sept::deserialize_data(in);
//         test_log << lvd::Log::dbg() << "Actual, deserialized data:\n    value is " << actual_data << "\n    " << LVD_REFLECT(actual_data.type()) << '\n';
        in.peek(); // To ensure we arrive at EOF if it's there.
        LVD_TEST_REQ_IS_TRUE(in.eof());

//         LVD_TEST_REQ_IS_TRUE(actual_data.can_cast<DecayedT_>());
        if (actual_data.can_cast<DecayedT_>())
            LVD_TEST_REQ_EQ(actual_data.cast<DecayedT_ const &>(), t);
        LVD_TEST_REQ_EQ(actual_data, expected_data);
    }

    // sept::Data serialization
    {
        // Make sure the round trip preserves the data.
        std::ostringstream out;
        sept::serialize_data(expected_data, out);

        test_log << lvd::Log::dbg() << "Data " << LVD_REFLECT(expected_data) << " serializes as:\n" << lvd::IndentGuard() << AsHex(out.str());

        std::istringstream in(out.str());
        auto actual_data = sept::deserialize_data(in);
//         test_log << lvd::Log::dbg() << "Actual, deserialized data:\n    value is " << actual_data << "\n    " << LVD_REFLECT(actual_data.type()) << '\n';
        in.peek(); // To ensure we arrive at EOF if it's there.
        LVD_TEST_REQ_IS_TRUE(in.eof());

//         LVD_TEST_REQ_IS_TRUE(actual_data.can_cast<DecayedT_>());
        if (actual_data.can_cast<DecayedT_>())
            LVD_TEST_REQ_EQ(actual_data.cast<DecayedT_ const &>(), t);
        LVD_TEST_REQ_EQ(actual_data, expected_data);
    }
}

#define TEST_SERIALIZATION_ROUNDTRIP(t) test_serialization_roundtrip(test_log, req_context, t)

LVD_TEST_BEGIN(595__serialization__000__NonParametricTerm)
    TEST_SERIALIZATION_ROUNDTRIP(sept::Term);
    TEST_SERIALIZATION_ROUNDTRIP(sept::NonParametricTerm);
//     TEST_SERIALIZATION_ROUNDTRIP(sept::ParametricTerm);

    auto term = sept::Term;
    TEST_SERIALIZATION_ROUNDTRIP(term);
    auto non_parametric_term = sept::NonParametricTerm;
    TEST_SERIALIZATION_ROUNDTRIP(non_parametric_term);
//     auto parametric_term = sept::ParametricTerm;
//     TEST_SERIALIZATION_ROUNDTRIP(parametric_term);

    TEST_SERIALIZATION_ROUNDTRIP(sept::Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::NonType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::NonParametricType);
//     TEST_SERIALIZATION_ROUNDTRIP(sept::ParametricType);

    TEST_SERIALIZATION_ROUNDTRIP(sept::Void);
    TEST_SERIALIZATION_ROUNDTRIP(sept::True);
    TEST_SERIALIZATION_ROUNDTRIP(sept::False);

    TEST_SERIALIZATION_ROUNDTRIP(sept::VoidType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::TrueType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::FalseType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::EmptyType);

    TEST_SERIALIZATION_ROUNDTRIP(sept::Bool);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint8);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint16);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint32);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint64);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint8);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint16);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint32);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint64);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Float32);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Float64);

    TEST_SERIALIZATION_ROUNDTRIP(sept::BoolType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint8Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint16Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint32Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Sint64Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint8Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint16Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint32Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Uint64Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Float32Type);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Float64Type);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array);

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC);
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD);
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC);
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMap);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::OutputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::Output);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::ClearOutputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::ClearOutput);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::EndOfFileType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::EndOfFile);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput);
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__100__ParametricTerm__POD)
    TEST_SERIALIZATION_ROUNDTRIP(true);
    TEST_SERIALIZATION_ROUNDTRIP(false);

    // BoolTerm_c and bool are in conflict.  Probably don't bother with BoolTerm_c.
//     TEST_SERIALIZATION_ROUNDTRIP(sept::Bool(true));
//     TEST_SERIALIZATION_ROUNDTRIP(sept::Bool(false));

    TEST_SERIALIZATION_ROUNDTRIP(int8_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(int8_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(int8_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(int8_t(-1));
    TEST_SERIALIZATION_ROUNDTRIP(int8_t(-2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int8_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int8_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(int16_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(int16_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(int16_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(int16_t(-1));
    TEST_SERIALIZATION_ROUNDTRIP(int16_t(-2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int16_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int16_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(int32_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(int32_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(int32_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(int32_t(-1));
    TEST_SERIALIZATION_ROUNDTRIP(int32_t(-2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int32_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int32_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(int64_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(int64_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(int64_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(int64_t(-1));
    TEST_SERIALIZATION_ROUNDTRIP(int64_t(-2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int64_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<int64_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(uint8_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(uint8_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(uint8_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint8_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint8_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(uint16_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(uint16_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(uint16_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint16_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint16_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(uint32_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(uint32_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(uint32_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint32_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint32_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(uint64_t(0));
    TEST_SERIALIZATION_ROUNDTRIP(uint64_t(1));
    TEST_SERIALIZATION_ROUNDTRIP(uint64_t(2));
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint64_t>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<uint64_t>::min());

    TEST_SERIALIZATION_ROUNDTRIP(0.0f);
    TEST_SERIALIZATION_ROUNDTRIP(1.0f);
    TEST_SERIALIZATION_ROUNDTRIP(-1.0f);
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::min());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::lowest());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::epsilon());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::round_error());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::infinity());
    // TODO: Have to test NaN separately, since `NaN == NaN` is false.
//     TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::quiet_NaN());
//     TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::signaling_NaN());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<float>::denorm_min());

    TEST_SERIALIZATION_ROUNDTRIP(0.0);
    TEST_SERIALIZATION_ROUNDTRIP(1.0);
    TEST_SERIALIZATION_ROUNDTRIP(-1.0);
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::min());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::max());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::lowest());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::epsilon());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::round_error());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::infinity());
    // TODO: Have to test NaN separately, since `NaN == NaN` is false.
//     TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::quiet_NaN());
//     TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::signaling_NaN());
    TEST_SERIALIZATION_ROUNDTRIP(std::numeric_limits<double>::denorm_min());
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__200__Array)
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array);

    auto a = sept::Array();
    TEST_SERIALIZATION_ROUNDTRIP(a);
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array());
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array());
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array(1));
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array(1,2));
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array(1,2,3));
    TEST_SERIALIZATION_ROUNDTRIP(sept::Array(1,2.0,true));

    TEST_SERIALIZATION_ROUNDTRIP(sept::Array(sept::Array(10,20),sept::Array(55.55,66.66,77.77),sept::Array(sept::True,sept::False,sept::Void)));
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__300__ArrayES)
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Bool,3));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Sint8,2));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::VoidType,10));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::EmptyType,10));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Float32,0));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Array,4));

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Float32,2)(0.0f, 1.0f));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Bool,3)(sept::True, sept::False, true));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Sint16,2)(int16_t(888), int16_t(20202)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::VoidType,4)(sept::Void, sept::Void, sept::Void, sept::Void));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::EmptyType,0)());
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayES(sept::Array,3)(sept::Array(), sept::Array(true, 3, 4.4), sept::Array(sept::Term)));
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__400__ArrayE)
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Bool));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Sint8));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::VoidType));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::EmptyType));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Float32));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Array));

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Float32)(0.0f, 1.0f));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Bool)(sept::True, sept::False, true));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Sint16)(int16_t(888), int16_t(20202)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::VoidType)(sept::Void, sept::Void, sept::Void, sept::Void));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::EmptyType)());
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayE(sept::Array)(sept::Array(), sept::Array(true, 3, 4.4), sept::Array(sept::Term)));
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__500__ArrayS)
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(0));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(1));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(2));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(3));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(4));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(10));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(100));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(100000000));

    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(2)(0.0f, 1.0f));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(3)(sept::True, sept::False, true));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(2)(int16_t(888), int16_t(20202)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(4)(sept::Void, sept::Void, sept::Void, sept::Void));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(0)());
    TEST_SERIALIZATION_ROUNDTRIP(sept::ArrayS(3)(sept::Array(), sept::Array(true, 3, 4.4), sept::Array(sept::Term)));
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__600__OrderedMap)
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMap);

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMap());
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMap(std::pair(10,123)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMap(std::pair(10,123), std::pair(20,246)));
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__700__OrderedMapDC)
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC);

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::Sint32,sept::Bool));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::Bool,sept::Float32));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::VoidType,sept::Term));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::Array,sept::Uint64));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::OrderedMap,sept::ArrayE(sept::ArrayS(2))));

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::Sint32,sept::Bool)(std::pair(123,true), std::pair(400,false)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::Bool,sept::Float32)(std::pair(true,1.0f), std::pair(false,0.0f)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapDC(sept::VoidType,sept::Term)(std::pair(sept::Void,56.89)));
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapDC(sept::Array,sept::Uint64)(
            std::pair(sept::Array(false,true,false),uint64_t(3)),
            std::pair(sept::Array(true,true,true,false,false),uint64_t(5))
        )
    );
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapDC(sept::OrderedMap,sept::ArrayE(sept::ArrayS(2)))(
            std::pair(
                sept::OrderedMap(std::pair(123,4), std::pair(369,12), std::pair(40404,0)),
                sept::ArrayE(sept::ArrayS(2))(sept::ArrayS(2)(123,4), sept::ArrayS(2)(369,12), sept::ArrayS(2)(40404,0))
            )
        )
    );
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__800__OrderedMapD)
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD);

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::Sint32));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::Bool));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::VoidType));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::Array));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::OrderedMap));

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::Sint32)(std::pair(123,true), std::pair(400,false)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::Bool)(std::pair(true,1.0f), std::pair(false,0.0f)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapD(sept::VoidType)(std::pair(sept::Void,56.89)));
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapD(sept::Array)(
            std::pair(sept::Array(false,true,false),uint64_t(3)),
            std::pair(sept::Array(true,true,true,false,false),uint64_t(5))
        )
    );
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapD(sept::OrderedMap)(
            std::pair(
                sept::OrderedMap(std::pair(123,4), std::pair(369,12), std::pair(40404,0)),
                sept::ArrayE(sept::ArrayS(2))(sept::ArrayS(2)(123,4), sept::ArrayS(2)(369,12), sept::ArrayS(2)(40404,0))
            )
        )
    );
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__800__OrderedMapC)
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC);

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Bool));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Float32));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Term));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Uint64));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::ArrayE(sept::ArrayS(2))));

    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Bool)(std::pair(123,true), std::pair(400,false)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Float32)(std::pair(true,1.0f), std::pair(false,0.0f)));
    TEST_SERIALIZATION_ROUNDTRIP(sept::OrderedMapC(sept::Term)(std::pair(sept::Void,56.89)));
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapC(sept::Uint64)(
            std::pair(sept::Array(false,true,false),uint64_t(3)),
            std::pair(sept::Array(true,true,true,false,false),uint64_t(5))
        )
    );
    TEST_SERIALIZATION_ROUNDTRIP(
        sept::OrderedMapC(sept::ArrayE(sept::ArrayS(2)))(
            std::pair(
                sept::OrderedMap(std::pair(123,4), std::pair(369,12), std::pair(40404,0)),
                sept::ArrayE(sept::ArrayS(2))(sept::ArrayS(2)(123,4), sept::ArrayS(2)(369,12), sept::ArrayS(2)(40404,0))
            )
        )
    );
LVD_TEST_END

LVD_TEST_BEGIN(595__serialization__900__control)
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::OutputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::Output);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::Output(123));

    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::ClearOutputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::ClearOutput);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInputType);
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput);

    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput(sept::Void));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput(sept::Sint32));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput(sept::VoidType));
    TEST_SERIALIZATION_ROUNDTRIP(sept::ctl::RequestSyncInput(sept::Array));
LVD_TEST_END
