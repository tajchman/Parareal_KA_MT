
#ifndef _GLOBAL_HXX
#define _GLOBAL_HXX

#define LOGSTREAM(C) \
  (C).logStream() << std::endl << __FILE__ << " (" << __LINE__ << ") "


#include "deal.II/lac/vector.h"
typedef std::vector<dealii::Vector<double>> sComponents;

#endif