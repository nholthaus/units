#include <cstdio>
double from_a(); double from_b();
int main(){ double a=from_a(),b=from_b(); std::printf("a=%.1f b=%.1f %s\n",a,b,(a==b?"consistent":"*** DIVERGES ***")); }
