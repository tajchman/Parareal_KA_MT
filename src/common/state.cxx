#include "state.hxx"

#include <cstring>
#include <cassert>

State::State() 
    : owner(true) {
}

State::~State() {
  clear();
}

void State::clear() {
  for (auto &c : components) {
    if (owner && c.data)
      delete [] c.data;
    c.data = nullptr;
    c.n = 0L;
  }
  owner = true;
}

State & State::operator=(const State &other)
{
  size_t i, n = components.size();
  assert(n == other.components.size());

  for (i=0; i<n; i++)
  {
    auto & c1 = components[i];
    const auto & c2 = other.components[i];
    assert(c1.n == c2.n);
    memcpy(c1.data, c2.data, c2.n*sizeof(double));
  }
  return *this;
}

std::ostream & operator<< (std::ostream & f, const State & S)
{
  for (auto c:S.components) {
    f << std::endl << c.name << "\n";
    for (size_t i=0; i<c.n; i++) {
      if (i % 5L == 0)
        f << "\n" << std::setw(7) << std::right << i << " ";
      f << " " << std::setw(24) << std::setprecision(17) << c.data[i];
    }
    f << "\n" << std::flush;
  }
  f << "\n" << std::flush;
  return f;
}
