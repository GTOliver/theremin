#include "LeapCpp.h"

#include "Exception.h"

namespace lpp
{

void success_or_throw(eLeapRS result)
{
    if (result != eLeapRS_Success)
        throw Exception(result);
}

const char* get_error_message_impl(eLeapRS result)
{
    switch (result) {
        case (eLeapRS_Success):
            return "Success";
        case (eLeapRS_Timeout):
            return "Timeout";
        default:
            return "Error";
    }
}

} // namespace lpp
