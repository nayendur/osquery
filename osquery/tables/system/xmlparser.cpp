#include <boost/filesystem.hpp>

#include <osquery/flags.h>
#include <osquery/filesystem.h>
#include <osquery/tables.h>
#include <osquery/logger.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "XmlMap.h"

#define DELIM_XML_ATTR ','
#define DELIM_XML_NODE '/'

namespace osquery {
namespace tables {

QueryData genxmlparser(QueryContext& context) {
  QueryData results;
  auto paths = context.constraints["file"].getAll(EQUALS);
  std::string strFile, strSearchPattern, strData;
    // Iterate through the file paths, adding the hash results
  LOG(WARNING) << "xml file paths size " << paths.size();
  for (const auto& path_string : paths) {
	  strFile = path_string;
	  break;
    }
  for (const auto& path_string : paths) {
      LOG(WARNING) << "xml file path string in paths " << path_string;
  }
 auto findpattren = context.constraints["xmltag"].getAll(EQUALS);
 LOG(WARNING) << "xmltag size " << findpattren.size();
   for (const auto& pattren_string : findpattren) {
	  strSearchPattern = pattren_string;
	  break;
    }
   for (const auto& pattren_string : findpattren) {
       LOG(WARNING) << "xmltag " << pattren_string;
   }

    Row r;
  
    std::ifstream inFile;
    std::string strXmlData;
    std::string strXmlNode;
    std::string strXmlAttr;
    XmlMap xmlIterator;
    size_t currOffSet = 0;
    size_t prevOffSet = 0;

    strData.clear();
    inFile.open(strFile);
    if (!inFile.is_open())
    {
		LOG(WARNING) << "Failed to open xml file " << strFile;
        return results;
    }
    strXmlData.assign((std::istreambuf_iterator<char>(inFile)), (std::istreambuf_iterator<char>()));
    inFile.close();

    //Parse the xml data
    try
    {
        xmlIterator.setData(strXmlData.c_str());
    }
    catch (std::exception &e)
    {
		LOG(WARNING) << "ACAS: Could not parse XML data. Exception occured: " << e.what();
        return results;
    }

    //Parse the search pattern and accordingly read the xmlmap
    /* @TODO Check if there can be a way to use the xpath directly instead of parsing the entire xml*/

    /*Find the '/' and then ',' for leaf*/
    prevOffSet = strSearchPattern.length();
    if (std::string::npos != (currOffSet = strSearchPattern.find_last_of(DELIM_XML_ATTR, prevOffSet)))
    {
        //The pattern will be of format xmlNode,xmlAttribute
        strXmlAttr = strSearchPattern.substr(currOffSet + 1);
        prevOffSet = currOffSet;
        if (std::string::npos != (currOffSet = strSearchPattern.find_last_of(DELIM_XML_NODE, currOffSet)))
        {
            strXmlNode = strSearchPattern.substr(currOffSet + 1, prevOffSet - currOffSet - 1);
        }
        else
        {
            strXmlNode = strSearchPattern.substr(0, prevOffSet);
        }

        if (!xmlIterator.getAttribute(strXmlNode.c_str(), strXmlAttr.c_str(), strData))
        {
			LOG(WARNING) << "ACAS: Could not get Dver from xml ";
            return results;
        }
    }
    else if (std::string::npos != (currOffSet = strSearchPattern.find_last_of(DELIM_XML_NODE, prevOffSet)))
    {
        //No attribute in format
        try
        {
            currOffSet = 0;
            prevOffSet = 0;
            XmlMap xmlMapEnt = xmlIterator;
            while(std::string::npos != (currOffSet = strSearchPattern.find_first_of(DELIM_XML_NODE, prevOffSet)))
            {
                strXmlNode = strSearchPattern.substr(prevOffSet, (currOffSet - prevOffSet));
                prevOffSet = currOffSet + 1;
                xmlMapEnt = xmlMapEnt.getMap(strXmlNode.c_str());
            }
            if(prevOffSet < strSearchPattern.size())
            {
                strXmlNode = strSearchPattern.substr(prevOffSet);
                strData = xmlMapEnt[strXmlNode.c_str()];
            }
            else
            {
				LOG(WARNING) << "ACAS: Not a valid pattern" << strSearchPattern.c_str();
                return results;
            }
        }
        catch (XmlMapException &e)
        {
			LOG(WARNING) << "ACAS: Could not Get the Dver from xml. Exception: " << e.getDescription().c_str();
			return results;
        }
    }
    else
    {
		LOG(WARNING) << "ACAS: Not a valid pattern  " << strSearchPattern.c_str();
		return results;
    }
  if (!strData.empty())
  {
	  r["file"] = strFile;
	  r["xmltag"] = strSearchPattern;
	  r["value"] = strData;
	  LOG(WARNING) << "test log " << strFile << ": " << strSearchPattern << " " << strData;
	  results.push_back(r);
  }
  return results;
}

}
}