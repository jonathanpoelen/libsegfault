#include <cassert>

void foo(int*p)
{
  assert(p);
  *p = 0;
}

int main()
{
  foo(nullptr);
}
