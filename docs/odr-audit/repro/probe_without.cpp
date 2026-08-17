#include <units/length.h>
#include <units/time.h>
#include <typeinfo>
#include <cstdio>
int main(){ using R=decltype(units::meters<double>(1)/units::seconds<double>(1));
  std::printf("WITHOUT velocity.h: %s\n", typeid(R).name()); }
