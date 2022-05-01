#pragma once

#include "LeapCpp.h"

namespace lpp
{

void success_or_throw(eLeapRS result);

const char* get_error_message_impl(eLeapRS result);

} // namespace lpp
