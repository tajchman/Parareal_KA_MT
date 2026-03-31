
#include "probe.hxx"
#include <sstream>
#include <iomanip>

void Probe::start(const std::string &prefix, int order)
{
  std::ostringstream s;
  s << prefix << "/probe";
  if (_name.size() > 0) s << "_" << _name;
  if (order >= 0) s << "_" << order;
  if (_rank >= 0)  s << "_" << _rank;
  s << "_" << _n << ".dat";
  _f.open(s.str().c_str());
}

void Probe::record(double t, const sComponents &v)
{
  _f << std::setprecision(10) << std::setw(15) << t << " " 
     << std::setprecision(12) << std::setw(17) << v[_c][_n] << std::endl;
}

void Probe::stop() {
  if (_f.is_open()) _f.close();
}

