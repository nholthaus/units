#include <units/area.h>
#ifdef WITH_LENGTH
#include <units/length.h>
#endif
#include <typeinfo>
#include <cstdio>
int main(){ using R=decltype(units::sqrt(units::square_meters<double>(1)));
  std::printf("sqrt R=%s\n", typeid(R).name());
  std::printf("sqrt sizeof=%zu alignof=%zu trivialcopy=%d\n",
      sizeof(R), alignof(R), (int)__is_trivially_copyable(R)); }
