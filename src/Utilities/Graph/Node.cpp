#include "../Utils.h"
#include "../DynaListImpl.h"
#include "../DynaHashMapImpl.h"
#include "../Exception.h"
#include "../TimeUtils.h"
#include "Node.h"

using namespace Graph;

MAKE_LISTTYPE_INSTANCE(Node, Node);
MAKE_MAPTYPE_INSTANCE(String, Node, Node);

namespace Graph {

    bool Node::present(const Node& node, const String& fldName, Node& result) {
        try {
            result.setNode(node[fldName]);
            return true;
        }
        catch (Exception e) {
            return false;
        }
    }

    int Node::getListLeadingScalarCount(const Node& node) {
        int count = 0;
        if (node.isList()) {
            auto& list = node.list();
            for (auto* child : list) {
                if (child->isScalar()) {
                    ++count;
                }
                else {
                    break;
                }
            }
        }
        return count;
    }

    Node::Node(NodeType nodeType, bool ownsData) : _ownsData(true), _key(nullptr) {
        _nodeType   = NodeType::UNTYPED;
        _scalarType = ScalarType::NONE;
        setOwnsData(ownsData);
        _ensureNodeType(nodeType);
    }

    Node::~Node() {
        if (_ownsData) {
            delete _key;
        }
        _key = nullptr;
        _reset();
    }

    Node::Node(const Node& other) {
        _setNode(other);
    }

    Node* Node::copy() {
        return new Node(*this);
    }

    void Node::setOwnsData(bool ownsData) {
        _ownsData = ownsData;
    }
    bool Node::isOwnsData() {
        return _ownsData;
    }

    void Node::_setNode(const Node& other) {
        _key        = other._key != nullptr ? _ownsData ? other._key->copy() : other._key : nullptr;
        _nodeType   = other._nodeType;
        _scalarType = other._scalarType;
        switch (_nodeType) {
            case NodeType::UNTYPED: {
                break;
            }
            case NodeType::SCALAR: {
                switch (_scalarType) {
                    case ScalarType::NONE: {
                        break;
                    }
                    case ScalarType::BOOL: {
                        _value.boolValue = other._value.boolValue;
                        break;
                    }
                    case ScalarType::INT: {
                        _value.intValue = other._value.intValue;
                        break;
                    }
                    case ScalarType::FLOAT: {
                        _value.floatValue = other._value.floatValue;
                        break;
                    }
                    case ScalarType::STRING:
                    case ScalarType::SEQ:
                    case ScalarType::PROPER: {
                        _value.strValue = other._value.strValue != nullptr ?
                                          _ownsData ? other._value.strValue->copy() : other._value.strValue : nullptr;
                        break;
                    }
                    case ScalarType::TIME:
                    case ScalarType::TIMESTAMP:
                    case ScalarType::DATE: {
                        _value.timeValue = _ownsData ? new system_clock::time_point(*(other._value.timeValue)) : other._value.timeValue;
                    }
                }
                break;
            }
            case NodeType::LIST: {
                _value.listValue = other._value.listValue != nullptr ?
                                   _ownsData ? other._value.listValue->copy() : other._value.listValue : nullptr;
            }
            case NodeType::MAP: {
                _value.mapValue = other._value.mapValue != nullptr ?
                                   _ownsData ? other._value.mapValue->copy() : other._value.mapValue : nullptr;
            }
        }
    }

    void Node::_reset() {
        switch (_nodeType) {
            case NodeType::UNTYPED: {
                break;
            }
            case NodeType::SCALAR: {
                switch (_scalarType) {
                    case ScalarType::STRING:
                    case ScalarType::SEQ:
                    case ScalarType::PROPER: {
                        if (_ownsData) {
                            delete _value.strValue;
                        }
                        _value.strValue = nullptr;
                        break;
                    }
                    case ScalarType::TIME:
                    case ScalarType::TIMESTAMP:
                    case ScalarType::DATE: {
                        if (_ownsData) {
                            delete _value.timeValue;
                        }
                        _value.timeValue = nullptr;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case NodeType::LIST: {
                delete _value.listValue;
                _value.listValue = nullptr;
                break;
            }
            case NodeType::MAP: {
                delete _value.mapValue;
                _value.mapValue = nullptr;
                break;
            }
        }
    }

    void Node::_ensureNodeType(NodeType nodeType) {
        if (_nodeType == NodeType::UNTYPED) {
            _nodeType   = nodeType;
            _scalarType = ScalarType::NONE;
            if (_nodeType == NodeType::LIST) {
                _value.listValue = new NodeList();
            }
            else if (_nodeType == NodeType::MAP) {
                _value.mapValue = new NodeMap();
            }
        }
        if (_nodeType != nodeType) {
            throw Exception("Attempted to add an invalid node type");
        }
    }

    Node* Node::newUntypedNode() {
        return new Node(NodeType::UNTYPED);
    }

    String* Node::key() const {
        return _key;
    }

    NodeValue& Node::value() {
        return _value;
    }

    const NodeType Node::type() const {
        return _nodeType;
    }

    const ScalarType Node::scalarType() const {
        return _scalarType;
    }

    Node& Node::reset() {
        setUntypedNode();
        return *this;
    }

    NodeList& Node::list() const {
        if (_nodeType == NodeType::LIST) {
            return *(_value.listValue);
        }
        throw InvalidValueException("Node is not a list");
    }
    NodeMap& Node::map() const {
        if (_nodeType == NodeType::MAP) {
            return *(_value.mapValue);
        }
        throw InvalidValueException("Node is not a map");
    }

    Node* Node::listElem(int index, bool throwInvalid) const {
        if (_nodeType == NodeType::LIST) {
            size_t count = _value.listValue->getCount();
            if (index < 0 || index >= count) {
                if (throwInvalid) {
                    throw InvalidValueException("List index " + tostr(index) + " is out of range");
                }
                return nullptr;
            }
            return (*(_value.listValue))[index];
        }
        throw InvalidValueException("Node is not a list");
    }

    Node* Node::mapElem(const String& key, bool throwInvalid) const {
        if (_nodeType == NodeType::MAP) {
            Node* node = (*(_value.mapValue))[key];
            if (node == nullptr && throwInvalid) {
                throw InvalidValueException("Key '" + key + "' not found in map");
            }
            return node;
        }
        throw InvalidValueException("Node is not a map");
    }

    Node* Node::operator()(int index) {
        return listElem(index);
    }
    const Node* Node::operator()(int index) const {
        return listElem(index);
    }
    Node* Node::operator()(const String& key) {
        return mapElem(key);
    }
    const Node* Node::operator()(const String& key) const {
        return mapElem(key);
    }
    const String Node::operator()() const {
        return asString();
    }

    Node& Node::operator[](int index) {
        _ensureNodeType(NodeType::LIST);
        auto* val = listElem(index);
        if (val == nullptr) {
            if (index == _value.listValue->getCount()) {
                val = addUntypedNode();
            }
            else {
                return *(listElem(index, true));
            }
        }
        return *val;
    }

    const Node& Node::operator[](int index) const {
        return *(listElem(index, true));
    }

    Node& Node::operator[](const String& key) {
        _ensureNodeType(NodeType::MAP);
        auto* val = mapElem(key);
        if (val == nullptr) {
            val = addUntypedNode(key);
        }
        return *val;
    }

    const Node& Node::operator[](const String& key) const {
        return *(mapElem(key, true));
    }

    Node& Node::operator=(const char* value) {
        setScalar(value);
        return *this;
    }

    Node& Node::operator=(const String& value) {
        setScalar(value.c_str());
        return *this;
    }

    Node& Node::operator=(nullptr_t value) {
        setUntypedNode();
        return *this;
    }

    Node& Node::operator=(bool value) {
        setScalar(value);
        return *this;
    }

    Node& Node::operator=(char value) {
        auto str = String(1, value);
        setScalar(str.c_str());
        return *this;
    }

    Node& Node::operator=(int value) {
        setScalar((long long)value);
        return *this;
    }

    Node& Node::operator=(long long value) {
        setScalar(value);
        return *this;
    }

    Node& Node::operator=(float value) {
        setScalar((double)value);
        return *this;
    }

    Node& Node::operator=(double value) {
        setScalar(value);
        return *this;
    }

    Node& Node::operator=(system_clock::time_point value) {
        setScalar(value, ScalarType::TIMESTAMP);
        return *this;
    }

    Node& Node::operator=(Time value) {
        setScalar(value.tp, ScalarType::TIME);
        return *this;
    }

    Node& Node::operator=(Timestamp value) {
        setScalar(value.tp, ScalarType::TIMESTAMP);
        return *this;
    }

    Node& Node::operator+=(const char* value) {
        addScalar(value);
        return *this;
    }

    Node& Node::operator+=(const String& value) {
        addScalar(value.c_str());
        return *this;
    }

    Node& Node::operator+=(nullptr_t value) {
        addUntypedNode();
        return *this;
    }

    Node& Node::operator+=(bool value) {
        addScalar(value);
        return *this;
    }

    Node& Node::operator+=(char value) {
        auto str = String(1, value);
        addScalar(str.c_str());
        return *this;
    }

    Node& Node::operator+=(int value) {
        addScalar((long long)value);
        return *this;
    }

    Node& Node::operator+=(long long value) {
        addScalar(value);
        return *this;
    }

    Node& Node::operator+=(float value) {
        addScalar((double)value);
        return *this;
    }

    Node& Node::operator+=(double value) {
        addScalar(value);
        return *this;
    }

    Node& Node::operator+=(system_clock::time_point value) {
        addScalar(value, ScalarType::TIMESTAMP);
        return *this;
    }

    Node& Node::operator+=(Time value) {
        addScalar(value.tp, ScalarType::TIME);
        return *this;
    }

    Node& Node::operator+=(Timestamp value) {
        addScalar(value.tp, ScalarType::TIMESTAMP);
        return *this;
    }

    Node* Node::_addToList(Node* node) {
        _ensureNodeType(NodeType::LIST);
        _value.listValue->append(node);
        return node;
    }

    Node* Node::_addToMap(const String& key, Node* node, bool append) {
        _ensureNodeType(NodeType::MAP);
        if (append) {
            if (_value.mapValue->containsKey(key)) {
                auto* valueNode = _value.mapValue->get(key);
                if (valueNode != nullptr) {
                    if (!valueNode->isList()) {
                        // Not a List, so encase the existing value into a new List
                        auto* listNode = newUntypedNode();
                        // Append the old value node to the new List
                        listNode->addNode(valueNode);
                        // Now point the valueNode variable to refer to the List
                        valueNode = listNode;
                    }
                    valueNode->addNode(node);
                    return valueNode;
                }
            }
        }
        _value.mapValue->put(key, node);
        return node;
    }

    const bool Node::isUntyped() const {
        return _nodeType == NodeType::UNTYPED;
    }

    const bool Node::isScalar() const {
        return _nodeType == NodeType::SCALAR;
    }

    const bool Node::isList() const {
        return _nodeType == NodeType::LIST;
    }

    const bool Node::isMap() const {
        return _nodeType == NodeType::MAP;
    }

    const bool Node::isBool() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::BOOL;
    }

    const bool Node::isString() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::STRING;
    }

    const bool Node::isSeq() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::SEQ;
    }

    const bool Node::isProper() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::PROPER;
    }

    const bool Node::isInt() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::INT;
    }

    const bool Node::isFloat() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::FLOAT;
    }

    const bool Node::isTime() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::TIME;
    }

    const bool Node::isTimestamp() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::TIMESTAMP;
    }

    const bool Node::isDate() const {
        return _nodeType == NodeType::SCALAR && _scalarType == ScalarType::DATE;
    }

    const String Node::asString() const {
        String result = "";
        if (_nodeType == NodeType::UNTYPED) {
            result = "null";
        }
        else if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? "true" : "false";
                    break;
                }
                case ScalarType::INT: {
                    result = tostr(_value.intValue);
                    break;
                }
                case ScalarType::FLOAT: {
                    result = tostr(_value.floatValue);
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    result = *(_value.strValue);
                    break;
                }
                case ScalarType::TIME: {
                    result = TimeUtils::getLocalTime(*(_value.timeValue), "%FT%T");
                    break;
                }
                case ScalarType::TIMESTAMP: {
                    result = TimeUtils::getUTCTimestamp(*(_value.timeValue));
                    break;
                }
                case ScalarType::DATE: {
                    result = TimeUtils::getLocalTime(*(_value.timeValue), "%F", false);
                    break;
                }
            }
        }
        return result;
    }

    const bool Node::asBool() const {
        bool result = false;
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue;
                    break;
                }
                case ScalarType::INT: {
                    result = _value.intValue != 0L;
                    break;
                }
                case ScalarType::FLOAT: {
                    result = _value.floatValue != 0.0;
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    String str = (*(_value.strValue)).tolower();
                    if (str == "false" || str == "no" || str == "0" || str == "") {
                        return false;
                    }
                    else {
                        return true;
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = TimeUtils::getEpochMillis(*(_value.timeValue)) != 0L;
                    break;
                }
            }
        }
        return result;
    }

    const long long Node::asInt() const {
        long long result = 0L;
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? -1L : 0L;
                    break;
                }
                case ScalarType::INT: {
                    result = _value.intValue;
                    break;
                }
                case ScalarType::FLOAT: {
                    result = static_cast<long long>(_value.floatValue);
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    try {
                        result = atol(_value.strValue->c_str());
                    }
                    catch (Exception e) {
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = TimeUtils::getEpochMillis(*(_value.timeValue));
                    break;
                }
            }
        }
        return result;
    }
 
    const double Node::asFloat() const {
        double result = 0.0;
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? -1.0 : 0.0;
                    break;
                }
                case ScalarType::INT: {
                    result = static_cast<double>(_value.intValue);
                    break;
                }
                case ScalarType::FLOAT: {
                    result = _value.floatValue;
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    try {
                        result = atof(_value.strValue->c_str());
                    }
                    catch (Exception e) {                            
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = static_cast<double>(TimeUtils::getEpochMillis(*(_value.timeValue)));
                    break;
                }
            }
        }
        return result;
    }

    const String Node::asNullPtr() const {
        String result = "";
        if (_nodeType == NodeType::UNTYPED) {
            result = "null";
        }
        return result;
    }

    const system_clock::time_point Node::asTime() const {
        system_clock::time_point result = TimeUtils::getTime(0L);
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? TimeUtils::getNow() : TimeUtils::getTime(0L);
                    break;
                }
                case ScalarType::INT: {
                    result = TimeUtils::getTime(static_cast<long>(_value.intValue));
                    break;
                }
                case ScalarType::FLOAT: {
                    result = TimeUtils::getTime(static_cast<long>(_value.floatValue));
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    try {
                        result = TimeUtils::parseLocalTime(_value.strValue->c_str());
                    }
                    catch (Exception e) {                            
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = *(_value.timeValue);
                    break;
                }
            }
        }
        return result;
    }

    const system_clock::time_point Node::asTimestamp() const {
        system_clock::time_point result = TimeUtils::getTime(0L);
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? TimeUtils::getNow() : TimeUtils::getTime(0L);
                    break;
                }
                case ScalarType::INT: {
                    result = TimeUtils::getTime(_value.intValue);
                    break;
                }
                case ScalarType::FLOAT: {
                    result = TimeUtils::getTime(static_cast<long>(_value.floatValue));
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    try {
                        result = TimeUtils::parseTimestamp(_value.strValue->c_str());
                    }
                    catch (Exception e) {                            
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = *(_value.timeValue);
                    break;
                }
            }
        }
        return result;
    }

    const system_clock::time_point Node::asDate() const {
        system_clock::time_point result = TimeUtils::getTime(0L);
        if (_nodeType == NodeType::SCALAR) {
            switch (_scalarType) {
                case ScalarType::NONE: {
                    break;
                }
                case ScalarType::BOOL: {
                    result = _value.boolValue ? TimeUtils::getNow() : TimeUtils::getTime(0L);
                    break;
                }
                case ScalarType::INT: {
                    result = TimeUtils::getTime(_value.intValue);
                    break;
                }
                case ScalarType::FLOAT: {
                    result = TimeUtils::getTime(static_cast<long>(_value.floatValue));
                    break;
                }
                case ScalarType::STRING:
                case ScalarType::SEQ:
                case ScalarType::PROPER: {
                    try {
                        result = TimeUtils::parseLocalDate(_value.strValue->c_str());
                    }
                    catch (Exception e) {                            
                    }
                    break;
                }
                case ScalarType::TIME:
                case ScalarType::TIMESTAMP:
                case ScalarType::DATE: {
                    result = *(_value.timeValue);
                    break;
                }
            }
        }
        return result;
    }

    void Node::setUntypedNode() {
        _reset();
        _nodeType   = NodeType::UNTYPED;
        _scalarType = ScalarType::NONE;
    }

    void Node::setNode(const Node& other) {
        _reset();
        _setNode(other);
    }

    void Node::setScalar(const char* value, ScalarType strType) {
        _reset();
        _nodeType   = NodeType::SCALAR;
        _scalarType = strType;
        _value.strValue = new String(value);
    }

    void Node::setScalar(bool value) {
        _reset();
        _nodeType   = NodeType::SCALAR;
        _scalarType = ScalarType::BOOL;
        _value.boolValue = value;
    }

    void Node::setScalar(long long value) {
        _reset();
        _nodeType   = NodeType::SCALAR;
        _scalarType = ScalarType::INT;
        _value.intValue = value;
    }

    void Node::setScalar(double value) {
        _reset();
        _nodeType   = NodeType::SCALAR;
        _scalarType = ScalarType::FLOAT;
        _value.floatValue = value;
    }

    void Node::setScalar(system_clock::time_point& value, ScalarType timeType) {
        _reset();
        _nodeType   = NodeType::SCALAR;
        _scalarType = timeType;
        _value.timeValue = new system_clock::time_point(value);
    }

    void Node::setList() {
        _reset();
        _nodeType   = NodeType::LIST;
        _scalarType = ScalarType::NONE;        
        _value.listValue = new NodeList();
    }

    void Node::setMap() {
        _reset();
        _nodeType   = NodeType::MAP;
        _scalarType = ScalarType::NONE;
        _value.mapValue = new NodeMap();
    }

    Node* Node::addNode(Node* node) {
        return _addToList(node);
    }

    Node* Node::addUntypedNode() {
        return _addToList(newUntypedNode());
    }

    Node* Node::addNode(const String& key, Node* node, bool append) {
        return _addToMap(key, node, append);
    }

    Node* Node::addUntypedNode(const String& key, bool append) {
        return _addToMap(key, newUntypedNode(), append);
    }

    Node* Node::addScalar(const char* value, ScalarType strType) {
        auto* node = newUntypedNode();
        node->setScalar(value, strType);
        return addNode(node);
    }

    Node* Node::addScalar(long long value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(node);
    }

    Node* Node::addScalar(double value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(node);
    }

    Node* Node::addScalar(bool value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(node);
    }

    Node* Node::addScalar(system_clock::time_point& value, ScalarType timeType) {
        auto* node = newUntypedNode();
        node->setScalar(value, timeType);
        return addNode(node);
    }

    Node* Node::addList() {
        auto* node = newUntypedNode();
        node->setList();
        return addNode(node);
    }

    Node* Node::addMap() {
        auto* node = newUntypedNode();
        node->setMap();
        return addNode(node);
    }

    Node* Node::addScalar(const String& key, const char* value, ScalarType strType) {
        auto* node = newUntypedNode();
        node->setScalar(value, strType);
        return addNode(key, node);
    }

    Node* Node::addScalar(const String& key, long long value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(key, node);
    }

    Node* Node::addScalar(const String& key, double value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(key, node);
    }

    Node* Node::addScalar(const String& key, bool value) {
        auto* node = newUntypedNode();
        node->setScalar(value);
        return addNode(key, node);
    }

    Node* Node::addScalar(const String& key, system_clock::time_point& value, ScalarType timeType) {
        auto* node = newUntypedNode();
        node->setScalar(value, timeType);
        return addNode(key, node);
    }

    Node* Node::addList(const String& key) {
        auto* node = newUntypedNode();
        node->setList();
        return addNode(key, node);
    }

    Node* Node::addMap(const String& key) {
        auto* node = newUntypedNode();
        node->setMap();
        return addNode(key, node);
    }

    int Node::hashCode() const {
        auto code = HashCoder();
        code.add(_key);
        return code.getCode();
    }

    bool Node::operator== (const Node &other) const {
        return _key == other.key();
    }
}