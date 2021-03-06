
#ifndef _XMLRPCVALUE_H_
#define _XMLRPCVALUE_H_
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
//
#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#include "rpcpp/RobotRpcDecl.h"

#ifndef MAKEDEPEND
# include <map>
# include <string>
# include <vector>
# include <time.h>
#endif

namespace RobotRpc {

  //! RPC method arguments and results are represented by Values
  //   should probably refcount them...
  // 数据类，用于通信，支持序列化与反序列化
  class ROBOTRPCPP_DECL XmlRpcValue 
  {

public:
    enum Type {
      TypeInvalid,
      TypeBoolean,
      TypeInt,
      TypeDouble,
      TypeString,
      TypeDateTime,
      TypeBase64,
      TypeArray,
      TypeStruct
    };

    //非原始类型
    typedef std::vector<char> BinaryData;
    typedef std::vector<XmlRpcValue> ValueArray;
    typedef std::map<std::string, XmlRpcValue> ValueStruct;
    typedef ValueStruct::iterator iterator;
    typedef ValueStruct::const_iterator const_iterator;

    //构造函数
    XmlRpcValue() : _type(TypeInvalid) { _value.asBinary = 0; }
    XmlRpcValue(bool value) : _type(TypeBoolean) { _value.asBool = value; }
    XmlRpcValue(int value)  : _type(TypeInt) { _value.asInt = value; }
    XmlRpcValue(double value)  : _type(TypeDouble) { _value.asDouble = value; }

    XmlRpcValue(std::string const& value) : _type(TypeString) 
    { _value.asString = new std::string(value); }

    XmlRpcValue(const char* value)  : _type(TypeString)
    { _value.asString = new std::string(value); }

    XmlRpcValue(struct tm* value)  : _type(TypeDateTime) 
    { _value.asTime = new struct tm(*value); }


    XmlRpcValue(void* value, int nBytes)  : _type(TypeBase64)
    {
      _value.asBinary = new BinaryData((char*)value, ((char*)value)+nBytes);
    }

    //! Construct from xml, beginning at *offset chars into the string, updates offset
    XmlRpcValue(std::string const& xml, int* offset) : _type(TypeInvalid)
    { if ( ! fromXml(xml,offset)) _type = TypeInvalid; }

    //! Copy
    XmlRpcValue(XmlRpcValue const& rhs) : _type(TypeInvalid) { *this = rhs; }

    //! Destructor (make virtual if you want to subclass)
    //此类不应集成 所以不写virtual
    /*virtual*/ ~XmlRpcValue() { invalidate(); }

    //! Erase the current value
    void clear() { invalidate(); }

    // Operators
    // 运算符重载
    XmlRpcValue& operator=(XmlRpcValue const& rhs);
    XmlRpcValue& operator=(bool const& rhs) { return operator=(XmlRpcValue(rhs)); }
    XmlRpcValue& operator=(int const& rhs) { return operator=(XmlRpcValue(rhs)); }
    XmlRpcValue& operator=(double const& rhs) { return operator=(XmlRpcValue(rhs)); }
    XmlRpcValue& operator=(const char* rhs) { return operator=(XmlRpcValue(std::string(rhs))); }

    bool operator==(XmlRpcValue const& other) const;
    bool operator!=(XmlRpcValue const& other) const;

    operator bool&()          { assertTypeOrInvalid(TypeBoolean); return _value.asBool; }
    operator int&()           { assertTypeOrInvalid(TypeInt); return _value.asInt; }
    operator double&()        { assertTypeOrInvalid(TypeDouble); return _value.asDouble; }
    operator std::string&()   { assertTypeOrInvalid(TypeString); return *_value.asString; }
    operator BinaryData&()    { assertTypeOrInvalid(TypeBase64); return *_value.asBinary; }
    operator struct tm&()     { assertTypeOrInvalid(TypeDateTime); return *_value.asTime; }

    XmlRpcValue const& operator[](int i) const { assertArray(i+1); return _value.asArray->at(i); }
    XmlRpcValue& operator[](int i)             { assertArray(i+1); return _value.asArray->at(i); }

    XmlRpcValue& operator[](std::string const& k) const { assertStruct(); return (*_value.asStruct)[k]; }
    XmlRpcValue& operator[](std::string const& k) { assertStruct(); return (*_value.asStruct)[k]; }
    XmlRpcValue& operator[](const char* k) const { assertStruct(); std::string s(k); return (*_value.asStruct)[s]; }
    XmlRpcValue& operator[](const char* k) { assertStruct(); std::string s(k); return (*_value.asStruct)[s]; }

    iterator begin() {assertStruct(); return (*_value.asStruct).begin(); }
    iterator end() {assertStruct(); return (*_value.asStruct).end(); }

    const_iterator begin() const {assertStruct(); return (*_value.asStruct).begin(); }
    const_iterator end() const {assertStruct(); return (*_value.asStruct).end(); }

    // Accessors
    //! Return true if the value has been set to something.
    bool valid() const { return _type != TypeInvalid; }

    //! Return the type of the value stored. \see Type.
    Type const &getType() const { return _type; }

    //! Return the size for string, base64, array, and struct values.
    int size() const;

    //! Specify the size for array values. Array values will grow beyond this size if needed.
    void setSize(int size)    { assertArray(size); }

    //! Check for the existence of a struct member by name.
    bool hasMember(const std::string& name) const;

    // 反序列化
    //! Decode xml. Destroys any existing value.
    bool fromXml(std::string const& valueXml, int* offset);

    // 序列化
    //! Encode the Value in xml
    std::string toXml() const;

    //! Write the value (no xml encoding)
    std::ostream& write(std::ostream& os) const;

    // Formatting
    //! Return the format used to write double values.
    static std::string const& getDoubleFormat() { return _doubleFormat; }

    //! Specify the format used to write double values.
    static void setDoubleFormat(const char* f) { _doubleFormat = f; }

protected:
    // Clean up
    void invalidate();

    // Type checking
    void assertTypeOrInvalid(Type t);
    void assertArray(int size) const;
    void assertArray(int size);
    void assertStruct() const;
    void assertStruct();

    // XML decoding
    bool boolFromXml(std::string const& valueXml, int* offset);
    bool intFromXml(std::string const& valueXml, int* offset);
    bool doubleFromXml(std::string const& valueXml, int* offset);
    bool stringFromXml(std::string const& valueXml, int* offset);
    bool timeFromXml(std::string const& valueXml, int* offset);
    bool binaryFromXml(std::string const& valueXml, int* offset);
    bool arrayFromXml(std::string const& valueXml, int* offset);
    bool structFromXml(std::string const& valueXml, int* offset);

    // XML encoding
    std::string boolToXml() const;
    std::string intToXml() const;
    std::string doubleToXml() const;
    std::string stringToXml() const;
    std::string timeToXml() const;
    std::string binaryToXml() const;
    std::string arrayToXml() const;
    std::string structToXml() const;

    // Format strings
    static std::string _doubleFormat;

    // Type tag and values
    Type _type;

    // At some point I will split off Arrays and Structs into
    // separate ref-counted objects for more efficient copying.
    union {
      bool          asBool;
      int           asInt;
      double        asDouble;
      struct tm*    asTime;
      std::string*  asString;
      BinaryData*   asBinary;
      ValueArray*   asArray;
      ValueStruct*  asStruct;
    } _value;

  };


}// namespace XmlRpc


std::ostream& operator<<(std::ostream& os, const RobotRpc::XmlRpcValue& v);


#endif // _XMLRPCVALUE_H_
