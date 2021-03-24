// 2020.08.03 - Victor Dods

#include <lvd/req.hpp>
#include <lvd/test.hpp>
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/EndOfFile.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"

LVD_TEST_BEGIN(320__ctl__0__Output)
    LVD_TEST_REQ_EQ(sept::ctl::Output(123), sept::ctl::OutputTerm_c(123));
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::Output(123)), sept::ctl::Output);
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::Output), sept::ctl::OutputType);
LVD_TEST_END

LVD_TEST_BEGIN(320__ctl__1__ClearOutput)
    LVD_TEST_REQ_EQ(sept::ctl::ClearOutput, sept::ctl::ClearOutput_c());
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::ClearOutput), sept::ctl::ClearOutputType);
LVD_TEST_END

LVD_TEST_BEGIN(320__ctl__2__EndOfFile)
    LVD_TEST_REQ_EQ(sept::ctl::EndOfFile, sept::ctl::EndOfFile_c());
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::EndOfFile), sept::ctl::EndOfFileType);
LVD_TEST_END

LVD_TEST_BEGIN(320__ctl__3__RequestSyncInput)
    LVD_TEST_REQ_EQ(sept::ctl::RequestSyncInput, sept::ctl::RequestSyncInput_c());
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::RequestSyncInput), sept::ctl::RequestSyncInputType);

    LVD_TEST_REQ_EQ(sept::ctl::RequestSyncInputType, sept::ctl::RequestSyncInputType_c());
    LVD_TEST_REQ_EQ(sept::abstract_type_of(sept::ctl::RequestSyncInputType), sept::NonParametricType);
LVD_TEST_END
