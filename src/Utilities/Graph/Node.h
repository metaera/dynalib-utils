#ifndef KAML_NODE_H
#define KAML_NODE_H

#include <chrono>
using namespace std;
using namespace std::chrono;

#include "../String.h"
#include "../DynaList.h"
#include "../DynaHashMap.h"
#include "../IntWrapper.h"
#include "../TimeUtils.h"

#define MAKE_LOCAL_REF_NODE Node(NodeType::UNTYPED, false)
#define NODE_GET_INIT Node _presult = Node(NodeType::UNTYPED, false)
#define NODE_GET_STR(N,K,V)     if (Node::present(N, K, _presult)) { V = _presult(); }
#define NODE_GET_SEQ(N,K,V)     if (Node::present(N, K, _presult)) { V = _presult(); }
#define NODE_GET_PROPER(N,K,V)  if (Node::present(N, K, _presult)) { V = _presult(); }
#define NODE_GET_INT(N,K,V)     if (Node::present(N, K, _presult)) { V = _presult.asInt(); }
#define NODE_GET_FLOAT(N,K,V)   if (Node::present(N, K, _presult)) { V = _presult.asFloat(); }
#define NODE_GET_BOOL(N,K,V)    if (Node::present(N, K, _presult)) { V = _presult.asBool(); }
#define NODE_GET_TIME(N,K,V)    if (Node::present(N, K, _presult)) { V = _presult.asTime(); }
#define NODE_GET_TS(N,K,V)      if (Node::present(N, K, _presult)) { V = _presult.asTimestamp(); }
#define NODE_GET_DATE(N,K,V)    if (Node::present(N, K, _presult)) { V = _presult.asDate(); }
#define NODE_GET_LIST(N,K,V)    if (Node::present(N, K, _presult)) { V = _presult.asList(); }
#define NODE_GET_MAP(N,K,V)     if (Node::present(N, K, _presult)) { V = _presult.asMap(); }

namespace Graph {
    class Node;

    MAKE_LISTTYPE_DEF(Node, Node);
    MAKE_MAPTYPE_DEF(String, Node, Node);

    enum class NodeType { UNTYPED, SCALAR, LIST, MAP };
    enum class ScalarType { NONE, BOOL, INT, FLOAT, STRING, SEQ, PROPER, TIME, TIMESTAMP, DATE };

    union NodeValue {
        bool        boolValue;
        long long   intValue;
        long double floatValue;
        String*     strValue = nullptr;
        system_clock::time_point* timeValue;
        NodeList*   listValue;
        NodeMap*    mapValue;
    };

    class Node : public IHashable<Node>, public ICopyable<Node> {
        bool        _ownsData;
        String*     _key;
        NodeValue   _value;
        NodeType    _nodeType;
        ScalarType  _scalarType;

    private:
        void  _ensureNodeType(NodeType nodeType);
        Node* _addToList(Node* node);
        Node* _addToMap(const String& key, Node* node, bool append = true);
        void  _setNode(const Node& other);
        void  _reset();

    public:
        static bool present(const Node& node, const String& fldName, Node& result);
        static int  getListLeadingScalarCount(const Node& node);

        Node();
        explicit Node(NodeType type, bool ownsData = true);
        Node(const char* value, bool ownsData = true);
        // explicit Node(char* value, bool ownsData = true);
        // explicit Node(String* value, bool ownsData = true);
        // explicit Node(const String& value, bool ownsData = true);
        // explicit Node(String& value, bool ownsData = true);
        Node(int value, bool ownsData = true);
        Node(long long value, bool ownsData = true);

        virtual ~Node();
        Node(const Node& other);
        Node*      copy() override;
        void       setOwnsData(bool ownsData);
        bool       isOwnsData();
        String*    key() const;
        NodeValue& value();
        const NodeType   type() const;
        const ScalarType scalarType() const;
        Node&       reset();

        NodeList&   list() const;
        NodeMap&    map()  const;
        Node*       listElem(int index, bool throwInvalid = false) const;
        Node*       mapElem(const String& key, bool throwInvalid = false) const;

        Node*        operator()(int index);
        const Node*  operator()(int index) const;
        Node*        operator()(const String& key);
        const Node*  operator()(const String& key) const;
        const String operator()() const;

        Node&       operator[](int index);
        const Node& operator[](int index) const;
        Node&       operator[](const String& key);
        const Node& operator[](const String& key) const;

        Node&       operator=(const char* value);
        Node&       operator=(const String& value);
        Node&       operator=(nullptr_t value);
        Node&       operator=(bool value);
        Node&       operator=(char value);
        Node&       operator=(int value);
        Node&       operator=(long long value);
        Node&       operator=(float value);
        Node&       operator=(long double value);
        Node&       operator=(system_clock::time_point value);
        Node&       operator=(Time value);
        Node&       operator=(Timestamp value);

        Node&       operator+=(const char* value);
        Node&       operator+=(const String& value);
        Node&       operator+=(nullptr_t value);
        Node&       operator+=(bool value);
        Node&       operator+=(char value);
        Node&       operator+=(int value);
        Node&       operator+=(long long value);
        Node&       operator+=(float value);
        Node&       operator+=(long double value);
        Node&       operator+=(system_clock::time_point value);
        Node&       operator+=(Time value);
        Node&       operator+=(Timestamp value);

        static Node* newUntypedNode();

        const bool  isUntyped() const;
        const bool  isScalar() const;
        const bool  isList() const;
        const bool  isMap() const;
        const bool  isBool() const;
        const bool  isString() const;
        const bool  isSeq() const;
        const bool  isProper() const;
        const bool  isInt() const;
        const bool  isFloat() const;
        const bool  isTime() const;
        const bool  isTimestamp() const;
        const bool  isDate() const;

        const String      asString() const;
        const bool        asBool() const;
        const long long   asInt() const;
        const long double asFloat() const;
        const String      asNullPtr() const;
        const system_clock::time_point asTime() const;
        const system_clock::time_point asTimestamp() const;
        const system_clock::time_point asDate() const;

        // These methods are used to set the type and value of this node
        void  setUntypedNode();
        void  setNode(const Node& other);
        void  setScalar(const char* value, ScalarType strType = ScalarType::STRING);
        void  setScalar(bool value);
        void  setScalar(long long value);
        void  setScalar(long double value);
        void  setScalar(system_clock::time_point& value, ScalarType timeType);
        void  setList();
        void  setMap();

        Node* addNode(Node* node);
        Node* addUntypedNode();
        Node* addNode(const String& key, Node* node, bool append = true);
        Node* addUntypedNode(const String& key, bool append = true);

        // These methods are used to add a value to this node, ensuring this node is a list
        Node* addScalar(const char* value, ScalarType strType = ScalarType::STRING);
        Node* addScalar(bool value);
        Node* addScalar(long long value);
        Node* addScalar(long double value);
        Node* addScalar(system_clock::time_point& value, ScalarType timeType);
        Node* addList();
        Node* addMap();

        // These methods are used to add a value to this node, ensuring this node is a map
        Node* addScalar(const String& key, const char* value, ScalarType strType = ScalarType::STRING);
        Node* addScalar(const String& key, bool value);
        Node* addScalar(const String& key, long long value);
        Node* addScalar(const String& key, long double value);
        Node* addScalar(const String& key, system_clock::time_point& value, ScalarType timeType);
        Node* addList(const String& key);
        Node* addMap(const String& key);

        int hashCode() const override;
        bool operator== (const Node &other) const override;
    };

}

#endif