#include "LeapCpp.h"

#include "LeapC.h"

namespace lpp {

    long long get_now() {
        return LeapGetNow();
    }

} // namespace lpp
