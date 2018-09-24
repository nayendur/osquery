/**************************************************************************
*       Copyright (c) 2017, Cisco Systems, All Rights Reserved
**************************************************************************/

/** @file XmlMap.h
 * A XML accessor utility. 
 */

#ifndef _XMLMAP_H_
#define _XMLMAP_H_


#include <map>
#include <string>
#include <vector>

class XmlMapImpl;


/** Parse XML blob and access the data in a dictionary style.
 *
 * This tool is not for parsing any XML data, but only for tagged elements.
 * Attributes and comments will be ignored. All the XML elements are represented 
 * with std::string and are accessible through keys in the form of const char*.
 *
 * Assuming the following XML blob is parsed into a XmlMap object named xm:
 *
 *   <cleanmachines>
 *   <version>2</version>
 *   <item>i1</item>
 *   <item>i2</item>
 *   <nested>
 *      <data>nested data</data>
 *   </nested>
 *   <package>
 *      <id>9</id>
 *      <type>3</type>
 *   </package>
 *   <package>
 *       <id>8</id>
 *       <type>1</type>
 *   </package>
 *   </cleanmachines>
 * 
 * To parse this blob:
 *     
 *    //declare variables to receive parsed data
 *    std::string version;
 *    std::vector<std::string> items;
 *    XmlMap nested;
 *    std::vector<XmlMap> packages;
 *    
 *    try {
 *       XmlMap xm = XmlMap(blob);
 *       version = xm["version"];
 *       items = xm.getArray["item"];
 *       nested = xm.getMap("nested");
 *       packages = xm.getMapArray("package");
 *    } 
 *    catch (XmlMapException& error)
 *    {
 *      log (error.getDescription());
 *      return;
 *    }
 *    // process data.  
 * 
 * Further explanations:
 * - Top level leaves can be accessed with []. The "version" element can be
 *   retrieved with xm["version"].
 * - Top level leaves with the same name can be accessed with getArray(key).
 *   the return value is a vector of const string. For example, xm["item"]
 *   returns a vector for the two "items" in the XML blob.
 * - Non-leaf elements (who have child elements) are stored as a XmlMap 
 *   objects inside the parent XmlMap object. Use getMap(key) to retrieve 
 *   the object:
 * 
 *       XmlMap nested = xm.getMap("nested");
 *       std::string data = nested["data"];
 *   or just
 *       std::string data = xm.getMap("nested")["data"];
 * - If multiple non-leaf elements share the same name, these elements
 *   can be retrieved with getMapArray(key) as a vector of XmlMap objects:
 *
 *       std::vector<XmlMap> packages = xm.getMapArray("package");
 *       std::string typeOfPackage2 = packages[1]["type"]
 *   
 *   You can also use getMap(key) to retrieve the first element of the array.
 *
 * Notes:
 * - The name of the root elements ("cleanmachines" in this case) is not 
 *   represented in the XmlMap object.
 * - Leaf elements and non-leaf elements are stored different in XmlMap. Use
 *   [] or getArray() to retrieve non-leaf elements, or use getMap() getMapArray()
 *   to retrieve leaf elements will cause exceptions thrown.
 * - If the key is not available, an exception will be thrown. The exception 
 *   has the reason causing the exception (getError()), and the description 
 *   of the error (getDescription()).
 * - Try/catch is recommended if the data you want to retrieve are mandatory.
 *   For optional data you can still use try/catch, or use hasKey() to check 
 *   the availability first, and no exception will be thrown in this case. 
 *   Note: There is also a GetOptional feature that will not assert if the key is not found.
 *         This will return an empty string that is no different than the one returned if the key
 *         is found but empty.  Use only if this is not an issue for you.
 */

class XmlMap
{
public:
    XmlMap();
    XmlMap(const char* data);
    ~XmlMap();
    
    /** Set the data of XmlMap.
     *  Previous data (if any) in XmlMap will be erased. Exceptions will 
     *  be thrown if new data is not parsable.
     */
    void setData(const char* data);

    /** Retrieve the text of a leaf element.
     *  Exceptions will be thrown if the key is invalid.
     */
    const std::string& operator[](const char* key) const;

    /** Retrieve an array of leaf elements with the same key.
     *  Exceptions will be thrown if the key is invalid. Or you can use []
     *  to access the first element of the key without calling getArray.
     */
      std::vector< std::string> getArray(const char* key) ;

    /** Retrieve the first XmlMap object for a non-leaf element.
     * 
     */
    const XmlMap getMap(const char* key) const;

    /** Retrieve an array of XmlMap objects for the non-leaf elements sharing 
     *  the same key.
     * 
     */
    const std::vector< XmlMap> getMapArray(const char* key) const;

    /** Check if XmlMap has data for the key.
     *  No exceptions will be thrown if there is no matching data for the key.
     *  Note that the function does not tell if the key is a leaf or non-leaf.
     *  If you have leaves and non-leaves sharing the same key name, you should
     *  access them with the methods above and use try/catch.
     */
    bool hasKey(const char* key) const;
    bool hasAttribute(const char* key,const char *attribute) const;
    bool getAttribute(const char* key,const char *attribute, std::string &Value) const;
    const std::string& GetOptional(const char* str) const;

private:
    XmlMap( XmlMapImpl* impl);
    XmlMapImpl* m_pImpl;
    std::string m_EmptyString;
    void parse(const char* data);
    bool  m_copied;
    friend class XmlMapImpl;
};

/** The exceptions the could be generated by XmlMapException.
 *  Three types of possible errors: ERROR_PARSE, ERROR_KAY and ERROR_MEMORY.
 *  If the error is caused by ERROR_KEY, the error decription contains the key.
 *  
 */
class XmlMapException
{
public:
    enum Error
    {
        ERROR_PARSE,
        ERROR_KEY,
        ERROR_MEMORY
    };
    XmlMapException(Error error, const std::string& description);
    XmlMapException(const XmlMapException& e);
    const Error getError() const;
    const std::string getDescription() const;
private:
    const Error        m_error;
    const std::string  m_description;
};

inline XmlMapException::XmlMapException(const XmlMapException& e)
                       :m_error(e.m_error),
                        m_description(e.m_description){};
inline XmlMapException::XmlMapException(Error error, const std::string& description) 
                       :m_error(error),
                        m_description(description) {};
inline const XmlMapException::Error XmlMapException::getError() const 
{ return m_error; };


#endif

