## LeapCpp

This is a collection of C++ helpers to make the LeapC API easier to work with.

This is only intended for use in this project.

namespace is `lpp` for "Leap Plus Plus"

The use of the pimpl pattern is just so that `LeapCpp.h` minimises its dependencies on `LeapC.h`. This is so that the
dependency on LeapC could one day be made private.
