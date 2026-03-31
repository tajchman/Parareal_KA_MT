#include "arguments.hxx"
#include <cstring>
#include <iomanip>
#include <sstream>


std::ostream & operator<<(std::ostream & f, const Arguments & args)
{
  for (auto &[key, val] : args._arguments)
  {
    const std::any & a = val.first;
    f << std::setw(12) << key << " ";
    if (a.type() == typeid(bool))
        f << "(bool)   : " << std::any_cast<bool>(a);
    if (a.type() == typeid(int))
        f << "(int)    : " << std::any_cast<int>(a);
    if (a.type() == typeid(unsigned long))
        f << "(unsigned long)    : " << std::any_cast<unsigned long>(a);
    else if (a.type() == typeid(std::string))
        f << "(string) : " << std::any_cast<std::string>(a);
    else if (a.type() == typeid(double))
        f << "(double) : " << std::any_cast<double>(a);
    else
        f << "(type ?) : ";
    f << std::endl;
  }
  return f;
}

void Arguments::parse(int argc, char **argv)
{
  std::ostringstream s;

  try {
    for( int iarg=1; iarg<argc; iarg++)
    {
      const char *option = argv[iarg];
      if (option[0] != '-')
      {
          s << "Error : " << option << " error: each option must start with '-'";
          throw std::invalid_argument(s.str());
      }

      if (strcmp(option, "-h") == 0 || strcmp(option, "-help") == 0)
      {
        help(argv[0]);
        exit(0);
      }

      auto pos = _arguments.find(option + 1);
      if (pos == _arguments.end())
      {
        s << "Error : " << option << " is not an existing option";
        throw std::invalid_argument(s.str());
      }

     std::any & a = pos->second.first;

      if (iarg == argc-1 || argv[iarg+1][0] == '-') {
        if (a.type() == typeid(bool)) {
          a = true;
          continue;
        }
        else
        {
          s << "Error : " << option << " is not a boolean option";
          throw std::invalid_argument(s.str());
        }
      }
      
      auto val = argv[++iarg];
      char * end;
      if (a.type() == typeid(int))
      {
        int v = int(strtol(val, &end, 10));
        if (*end == '\0')
        {
          a = v;
          continue;
        }
        else 
        {
          s << "Error : " << val << " is not a integer value";
          throw std::domain_error(s.str());
        }
      }

      if (a.type() == typeid(unsigned long))
      {
        unsigned long v = (unsigned long) strtol(val, &end, 10);
        if (*end == '\0')
        {
          a = v;
          continue;
        }
        else 
        {
          s << "Error : " << val << " is not a non negative integer value";
          throw std::domain_error(s.str());
        }
      }

      if (a.type() == typeid(float)) 
      {
        float v = strtof(val, &end);
        if (*end == '\0')
        {
          a = v;
          continue;
        }
        else
        {
          s << "Error : " << val << " is not a double value";
          throw std::domain_error(s.str());
        } 
      }

      if (a.type() == typeid(double)) 
      {
        double v = strtod(val, &end);
        if (*end == '\0')
        {
          a = v;
          continue;
        }
        else
        {
          s << "Error : " << val << " is not a double value";
          throw std::domain_error(s.str());
        } 
      }

      if (a.type() == typeid(const char *))
      {
        a = std::string(val);
        continue;
      }
    }
  }
  catch (std::exception & e)
  {
    std::cerr << std::endl << e.what() << std::endl;
    help(argv[0]);
    exit(-1);
  }

}

void Arguments::help(const char * progName)
{
  std::cout << "\nUsage " << progName;

  std::cout << " [-h|-help]";
  for (auto &[key, val] : _arguments)
  {
    std::cout << " [-" << key;
    if (val.first.type() != typeid(bool))
      std::cout << " value";
    std::cout << "]";
  }

  std::cout << "\nOptions\n" << std::endl;
  for (auto &[key, val] : _arguments)
  {
    std::any & a = val.first;
    std::string & help = val.second;

    std::cout << " -" << std::left << std::setw(12) << key;
    if (a.type() == typeid(int))
      std::cout << help << " (integer, current value: " << std::any_cast<int>(a) << ")";
    else if (a.type() == typeid(unsigned long))
      std::cout << help << " (unsigned integer, current value: " << std::any_cast<double>(a) << ")";
    else if (a.type() == typeid(double))
      std::cout << help << " (double, current value: " << std::any_cast<double>(a) << ")";
    else if (a.type() == typeid(bool)) {
      bool b = std::any_cast<bool>(a);
      std::cout << help << " (bool, current value: " << (b ? "true" : "false") << ")";
    }
    else if (a.type() == typeid(const char *)) {
      std::cout << help << " (string, current value: \"" << std::any_cast<const char *>(a) << "\")";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}