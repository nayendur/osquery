#include <boost/filesystem.hpp>

#include <osquery/flags.h>
#include <osquery/filesystem.h>
#include <osquery/tables.h>
#include <osquery/logger.h>
#include <sstream>
#include <iostream>
#include <fstream>

namespace osquery {
namespace tables {

QueryData gensearchpattren(QueryContext& context) {
  QueryData results;
  auto paths = context.constraints["path"].getAll(EQUALS);
  std::string strFile, strPattren, strData;
    // Iterate through the file paths, adding the hash results
  LOG(WARNING) << "paths size " << paths.size();
  for (const auto& path_string : paths) {
	  strFile = path_string;
	  break;
    }
  for (const auto& path_string : paths) {
      LOG(WARNING) << "each paths in paths " << path_string;
  }
 auto findpattren = context.constraints["findstring"].getAll(EQUALS);
 LOG(WARNING) << "findpattren size " << findpattren.size();
   for (const auto& pattren_string : findpattren) {
	  strPattren = pattren_string;
	  break;
    }
   for (const auto& pattren_string : findpattren) {
       LOG(WARNING) << "each pattren in findpattren " << pattren_string;
   }
   int ncount = -1;
   //if (context.hasConstraint("noofchars", EQUALS))
   if (context.constraints["noofchars"].exists())
   {
       LOG(WARNING) << "No of characters to read is available changed ";
       auto noofchars = context.constraints["noofchars"].getAll<int>(EQUALS);
       for (const auto& count : noofchars) {
           ncount = count;
           break;
       }
       LOG(WARNING) << "No of characters to read " << ncount;
   }

  Row r;
  if(!boost::filesystem::exists(strFile))
  {
	  LOG(WARNING) << "cannot find the file " << strFile;
  }
  else{
	  std::ifstream ifs;

    strData.clear();

    ifs.open(strFile.c_str());
    if(!ifs.is_open())
    {
		LOG(WARNING) << "Failed to open file " << strFile;
        return results;
    }
    do
    {
        std::string strLine;
        size_t pos = 0;
        std::getline(ifs, strLine);
        if (std::string::npos != (pos = strLine.find(strPattren, 0)))
        {
            strData = strLine.substr(pos + strPattren.length());
            break;
        }
    } while (ifs);

    ifs.close();
  }
  if (!strData.empty())
  {
	  r["path"] = strFile;
	  r["findstring"] = strPattren;
	  r["value"] = ((ncount == -1) ? strData : strData.substr(0, ncount));
	  r["noofchars"] = INTEGER(ncount);
	  LOG(WARNING) << "test log " << strFile << ": " << strPattren << " " << strData;
	  results.push_back(r);
  }
  return results;
}

}
}