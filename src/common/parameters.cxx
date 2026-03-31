#include "parameters.hxx"
#include "arguments.hxx"

Parameters::Parameters() {

  _args.add("log", true, "logging");
  _args.add("tmin", 0.0, "initial time");
  _args.add("tmax", 1.0, "final time");
  _args.add("output", 0.0, "output frequency (0 = never)");
  _args.add("verbose", false, "output frequency (0 = never)");

}

