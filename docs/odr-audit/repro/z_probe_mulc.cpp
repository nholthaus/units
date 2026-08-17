#include <units/mass.h>
#include <units/acceleration.h>
#ifdef WITH_FORCE
#include <units/force.h>
#endif
#include <typeinfo>
#include <cstdio>
int main(){ using R=decltype(units::kilograms<double>(1)*units::meters_per_second_squared<double>(1));
  std::printf("mulc R=%s\n", typeid(R).name());
  std::printf("mulc sizeof=%zu alignof=%zu trivialcopy=%d\n",
      sizeof(R), alignof(R), (int)__is_trivially_copyable(R)); }
