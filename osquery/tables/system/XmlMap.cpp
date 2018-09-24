/**************************************************************************
*       Copyright (c) 2017, Cisco Systems, All Rights Reserved
**************************************************************************/

/** @file XmlMap.cpp
 *  The implementation of XmlMap.
 */

#include "XmlMap.h"
#include "tinyxml.h"
#include "assert.h"

/** The implementation class of XmlMap. 
 *  So that XmlMap does not depend on any particular XMl parser.
 */
class XmlMapImpl
{
public:
    XmlMapImpl(const TiXmlElement* element);
    ~XmlMapImpl();

    const std::string& operator[](const char* str) const;
    std::vector<std::string> getArray(const char* str) ;
    XmlMapImpl* getMap(const char* str);
    std::vector< XmlMapImpl*> getMapArray(const char* str);
    bool  hasKey(const char* key) const ;
    bool getAttribute(const char* key, const char *Attrib, std::string &Value) const;
private:
    typedef std::pair <std::string, std::string> StrPair;
    typedef std::pair <std::string, XmlMapImpl>  StrMapPair;


    typedef std::multimap< std::string,  std::string> StrStrMap;
    typedef std::multimap< std::string,  XmlMapImpl>  StrMapMap;
    typedef std::multimap< std::string,  StrStrMap>  AttrStrMapMap;

    typedef std::pair <const std::string, const StrStrMap> AttrPair;

    StrStrMap m_strStrMap;
    StrMapMap m_strMapMap;
    AttrStrMapMap m_strAttrMap;

};

/**
 *
 */
XmlMap::XmlMap(const char* data)
{
   assert(data);
   parse(data);
}

void XmlMap::parse(const char* data)
{
   assert(data);
   TiXmlBase::SetCondenseWhiteSpace(false); 
   TiXmlDocument doc;
   doc.Parse(data);
   if (doc.Error())
       throw  XmlMapException(XmlMapException::ERROR_PARSE, doc.ErrorDesc());
 
   TiXmlElement*  element = doc.FirstChildElement();
   if (!element)
       throw XmlMapException(XmlMapException::ERROR_PARSE, "Null element");

   m_pImpl = new XmlMapImpl(element);
   if (!m_pImpl)
       XmlMapException(XmlMapException::ERROR_MEMORY, "Memory failure in creating new XmlMap Object");
}

XmlMap::XmlMap( XmlMapImpl* impl)
{
    m_pImpl = impl;
    m_copied = true;
}

XmlMap::~XmlMap()
{
    if (!m_copied && m_pImpl)
       delete m_pImpl;
}

XmlMap::XmlMap()
       : m_copied (false),
         m_pImpl (NULL)
{

}

bool  XmlMap::hasKey(const char* key) const
{
   assert(m_pImpl);
   return m_pImpl->hasKey(key);
}


bool  XmlMap::getAttribute(const char* key, const char *attrib, std::string &Value) const
{
   assert(m_pImpl);
   return m_pImpl->getAttribute(key,attrib,Value);
}


void  XmlMap::setData(const char* data)
{
   if (m_pImpl)
   {
       delete m_pImpl;
       m_pImpl = NULL;
   }
   parse(data);
}

const std::string& XmlMap::operator[](const char* str) const
{
   assert(m_pImpl);
   return (*m_pImpl)[str];
}

const std::string& XmlMap::GetOptional(const char* str) const
{
   assert(m_pImpl);
   if (hasKey(str))
       return (*m_pImpl)[str];                               
   return(m_EmptyString);
}


 std::vector< std::string> XmlMap::getArray(const char* str) 
{
   assert(m_pImpl);
   return m_pImpl->getArray(str);
}


const XmlMap XmlMap::getMap(const char* str) const
{
   assert(m_pImpl);
   return XmlMap(m_pImpl->getMap(str));
}

const std::vector< XmlMap> XmlMap::getMapArray(const char* str) const
{
   assert(m_pImpl);
   std::vector< XmlMapImpl*> impl = m_pImpl->getMapArray(str);
   std::vector< XmlMap> v;

   std::vector< XmlMapImpl*>::iterator it;
   for (it=impl.begin(); it != impl.end(); it++)
       v.push_back(XmlMap(*it));
   
   return v;
}

/**
 *
 */
XmlMapImpl::XmlMapImpl(const TiXmlElement* root)
{
   assert(root);
   const TiXmlElement* element = root;
   //Parse the attributes for root
   std::string Name, Value;
   const TiXmlAttribute *Attrib = element->FirstAttribute();
   StrStrMap tmpAttrPair;

   while (Attrib != NULL)
   {
       Name = Attrib->Name();
       Value = Attrib->Value();
       tmpAttrPair.insert(StrPair(Name, Value));
       Attrib = Attrib->Next();
   }
   m_strAttrMap.insert(AttrPair(element->Value(), tmpAttrPair));

   for( element = element->FirstChildElement(); element; element = element->NextSiblingElement())
   {
        const TiXmlNode* node = element->FirstChild();
        std::string str;
        if (node) 
        {
            if (node->NoChildren() && node->ToText()) // "<data>1</data>"
            {
                m_strStrMap.insert(StrPair(element->Value(), node->Value()));
            }
            else if (!node->NoChildren() && node->ToElement())
            {
                m_strMapMap.insert(StrMapPair(element->Value(), 
                                   XmlMapImpl(element)));
            }
        }
        else // <data/>
        {
           m_strStrMap.insert(StrPair(element->Value(), ""));
        }
        //Parse the attributes
        std::string Name, Value;
        const TiXmlAttribute *Attrib = element->FirstAttribute();
        StrStrMap tmpAttrPair;

        while (Attrib != NULL)
        {
            Name = Attrib->Name();
            Value = Attrib->Value();
            tmpAttrPair.insert(StrPair(Name, Value));
            Attrib = Attrib->Next();
        }
        m_strAttrMap.insert(AttrPair(element->Value(), tmpAttrPair));
   }

}


XmlMapImpl::~XmlMapImpl()
{
    m_strStrMap.clear();
    m_strMapMap.clear();
}

const std::string& XmlMapImpl::operator[](const char* str) const
{
    StrStrMap::const_iterator it = m_strStrMap.find(str);
    if ( it == m_strStrMap.end())
        throw XmlMapException(XmlMapException::ERROR_KEY, str);
    return it->second;
}

std::vector< std::string> XmlMapImpl::getArray(const char* str) 
{
    StrStrMap::iterator it = m_strStrMap.find(str);
    if ( it == m_strStrMap.end())
        throw XmlMapException(XmlMapException::ERROR_KEY, str);

    std::vector< std::string> v;
    for (; it!=m_strStrMap.end(); it++)
    {
        if (it->first != str)
            break;
        v.push_back(it->second);
    }
    return v;
}

 XmlMapImpl* XmlMapImpl::getMap(const char* str)
{
    StrMapMap::iterator it = m_strMapMap.find(str);
    if ( it == m_strMapMap.end())
        throw XmlMapException(XmlMapException::ERROR_KEY, str);
    return &(it->second);
}

 std::vector< XmlMapImpl*> XmlMapImpl::getMapArray(const char* str)
{
    StrMapMap::iterator it = m_strMapMap.find(str);
    if ( it == m_strMapMap.end())
        throw XmlMapException(XmlMapException::ERROR_KEY, str);

    std::vector< XmlMapImpl*> v;
    for (; it!=m_strMapMap.end(); it++)
    {
        if (it->first != str)
            break;
        v.push_back(&(it->second));
    }
    return v;
}


bool XmlMapImpl::hasKey(const char* key) const
{
    StrMapMap::const_iterator it = m_strMapMap.find(key);
    if ( it != m_strMapMap.end())
        return true;
    StrStrMap::const_iterator it2 = m_strStrMap.find(key);
    if ( it2 != m_strStrMap.end())
        return true;
   return false;
}


bool XmlMapImpl::getAttribute(const char* key, const char *Attrib, std::string &Value) const
{
    AttrStrMapMap::const_iterator it = m_strAttrMap.find(key);
 
    if ( it != m_strAttrMap.end())
    {
       StrStrMap LocalAttribList = it->second; // get the attribute table
       StrStrMap::const_iterator itLocalAttribute = LocalAttribList.find(Attrib);
       if (itLocalAttribute != LocalAttribList.end())
       {
           Value = itLocalAttribute->second;
           return(true);
       }
    }

   return false;
}


const std::string XmlMapException::getDescription() const 
{ 
    std::string str;
    switch(m_error)
    {
    case XmlMapException::ERROR_KEY:
        str="missing key \'"+m_description+"\'";
        break;
    case XmlMapException::ERROR_MEMORY:
        str="memory error";
        break;
    case XmlMapException::ERROR_PARSE:
        str="parse error "+m_description;
        break;
    }
    return str; 
};

