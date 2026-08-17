#include <units/length.h>
#ifdef WITH_AREA
#include <units/area.h>
#endif
#include <typeinfo>
#include <cstdio>
int main(){ using R=decltype(units::meters<double>(1)*units::meters<double>(1));
  std::printf("mul2 R=%s\n", typeid(R).name());
  std::printf("mul2 sizeof=%zu alignof=%zu trivialcopy=%d\n",
      sizeof(R), alignof(R), (int)__is_trivially_copyable(R)); }
