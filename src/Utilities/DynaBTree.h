#ifndef DYNABTREE_H
#define DYNABTREE_H

#include <exception>
#include <type_traits>
#include <cstring>
#include "TypeDefs.h"
#include "ICopyable.h"
#include "DynaArray.h"
#include "DynaList.h"
#include "DynaBuffer.h"
#include "ICacheConst.h"

using namespace std;

#define MAKE_BTREETYPE_DEF(K,V,T) \
    typedef DynaTreeInnerElem<K> T##TreeInnerElem; \
    typedef DynaTreeLeafElem<K,V> T##TreeLeafElem; \
    typedef DynaTreeInnerElem<K>::Data T##TreeInnerData; \
    typedef DynaTreeLeafElem<K,V>::Data T##TreeLeafData; \
    typedef DynaBTree<K,V> T##BTree; \
    typedef DynaBTreeLeafNode<K,V> T##BTreeLeafNode; \
    typedef DynaBTreeInnerNode<K> T##BTreeInnerNode

template <typename K> class DynaBTreeNode;
template <typename K, typename V> class DynaBTreeLeafNode;

template <typename T> T* ptr(T& obj) { return &obj; } // turn reference into pointer!
template <typename T> T* ptr(T* obj) { return obj; }  // obj is already pointer, return it!

//----------------------------------------------------------------------------------------------------------------------------------------------

enum class MatchType {FULL_KEY, PARTIAL_KEY, WITH_VALUE};

template <typename K, typename V> struct DynaTreeLeafElem {
    #pragma pack(push, 1)
    struct Data {
        K key   = "";
        V value = 0;
    } search;
    #pragma pack(pop)
    Data* data;
    virtual int  compareTo(DynaTreeLeafElem<K,V>* other, MatchType match);
    virtual void setKeyInSearch(K key);
    virtual void setValueInSearch(V value);
    virtual void setKeyInData(K key);
    virtual void setValueInData(V value);
    virtual void getKeyFromData(K& key);
    virtual void getValueFromData(V& value);
    int size() { return sizeof(this->search); }
    int keySize() { return sizeof(this->search.key); }
};

template <typename K> struct DynaTreeInnerElem {
    #pragma pack(push, 1)
    struct Data {
        K key   = "";
        index_t rightNodeIndex = 0;
    } search;
    #pragma pack(pop)
    Data* data;
    virtual int compareTo(DynaTreeInnerElem<K>* other, MatchType match);
    virtual void setKeyInSearch(K key);
    virtual void setKeyInData(K key);
    virtual void getKeyFromData(K& key);
    int size() { return sizeof(this->search); }
    int keySize() { return sizeof(this->search.key); }
};

//----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief B-Tree main class
 * 
 * @tparam K 
 * @tparam V 
 */
template <typename K, typename V> class DynaBTree {
    DynaBTreeNode<K>* _root;
    int               _leafOrder;
    int               _innerOrder;

    DynaBTreeLeafNode<K,V>* _findLeafNodeForKey(K& elem);

public:
    DynaBTree(int leafOrder, int innerOrder);
    virtual ~DynaBTree();

    static bool wasFound(int returnValue);
    static int  insertAt(int returnValue);

    void insert(K& key, V& value);
    int  search(K& key, V* value, MatchType match = MatchType::FULL_KEY);
    void deleteEntry(K& key, V* value);    
};

//----------------------------------------------------------------------------------------------------------------------------------------------

enum class TreeNodeType {NONE, LEAF, INNER};

/**
 * @brief Base Class for B-Tree nodes
 * 
 * @tparam K 
 */
template <typename K> class DynaBTreeNode : public ICopyable<DynaBTreeNode<K>> {
protected:
    TreeNodeType      _nodeType;
    int               _leafOrder;
    int               _innerOrder;
    DynaBuffer*       _buffer;
    index_t           _parentIndex;
    index_t           _leftSiblingIndex;
    index_t           _rightSiblingIndex;
    DynaBTreeNode<K>* _parent;
    DynaBTreeNode<K>* _leftSibling;
    DynaBTreeNode<K>* _rightSibling;

public:
    DynaBTreeNode(TreeNodeType nodeType, int maxKeys, int leafOrder, int innerOrder);
    virtual ~DynaBTreeNode();
    DynaBTreeNode(const DynaBTreeNode<K>& other);
    DynaBTreeNode<K>* copy() override;

    DynaBuffer*       getBuffer();
    DynaBTreeNode<K>* getParent();
    void              setParent(DynaBTreeNode<K>* parent);
    TreeNodeType      getNodeType();
    int               getKeyCount();

    bool wasFound(int returnValue);
    int  insertAt(int returnValue);

    virtual bool getKeyFromData(uint index, K& key);
    virtual void setKeyToData(uint index, K& key);

    virtual bool isOverflow();
    virtual DynaBTreeNode<K>* handleOverflow();

    virtual bool isUnderflow();
    virtual bool canGiveKey();

    DynaBTreeNode<K>* getLeftSibling();
    void setLeftSibling(DynaBTreeNode<K>* sibling);
    DynaBTreeNode<K>* getRightSibling();
    void setRightSibling(DynaBTreeNode<K>* sibling);
    DynaBTreeNode<K>* handleUnderflow();

    virtual int search(K& key, MatchType match = MatchType::FULL_KEY);

    virtual DynaBTreeNode<K>* split();
    virtual DynaBTreeNode<K>* pullUpKey(K& key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode);

    virtual void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex);
    virtual DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild);
    virtual void joinWithSibling(K& sinkKey, DynaBTreeNode<K>* rightSibling);
    virtual void transferFromSibling(K& sinkKey, K& upKey, DynaBTreeNode<K>* sibling, uint fromIndex);
};

//----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief B-Tree Leaf Node
 * 
 * @tparam K 
 * @tparam V 
 */
template <typename K, typename V> class DynaBTreeLeafNode : public DynaBTreeNode<K> {
    DynaTreeLeafElem<K,V>* _elemPtr;

    void _insertAt(uint index, DynaTreeLeafElem<K,V>& elem);
    void _deleteAt(uint index);

public:
    DynaBTreeLeafNode(int leafOrder, int innerOrder);
    virtual ~DynaBTreeLeafNode();
    DynaBTreeLeafNode(const DynaBTreeLeafNode<K,V>& other);
    DynaBTreeNode<K>* copy() override;

    void setElemDataPos(uint index, DynaTreeLeafElem<K,V>& elem);
    bool getKeyFromData(uint index, K& key) override;
    void setKeyToData(uint index, K& key) override;

    V*   getValue(uint index);
    DynaTreeLeafElem<K,V>* getElemIntoSearch(uint index, DynaTreeLeafElem<K,V>& elem);
    void setElemFromSearch(uint index, DynaTreeLeafElem<K,V>& elem);
    void insertElem(DynaTreeLeafElem<K,V>& elem);
    bool deleteElem(DynaTreeLeafElem<K,V>& elem);

    int search(K& key, MatchType match = MatchType::FULL_KEY) override;
    int search(DynaTreeLeafElem<K,V>& elem, MatchType match = MatchType::FULL_KEY);

    DynaBTreeNode<K>* split() override;
    DynaBTreeNode<K>* pullUpKey(K& key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) override;

    void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) override;
    DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) override;
    void joinWithSibling(K& sinkKey, DynaBTreeNode<K>* rightSibling) override;
    void transferFromSibling(K& sinkKey, K& upKey, DynaBTreeNode<K>* sibling, uint fromIndex) override;
};

//----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief B-Tree Inner Node
 * 
 * @tparam K 
 */
template <typename K> class DynaBTreeInnerNode : public DynaBTreeNode<K> {
    DynaTreeInnerElem<K>* _elemPtr;

    void _insertAt(uint index, K& elem, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild);
    void _deleteAt(uint index);

public:
    explicit DynaBTreeInnerNode(int order);
    virtual ~DynaBTreeInnerNode();
    DynaBTreeInnerNode(const DynaBTreeInnerNode<K>& other);
    DynaBTreeNode<K>* copy() override;

    void setElemDataPos(uint index, DynaTreeInnerElem<K>& elem);
    bool getKeyFromData(uint index, K& key) override;
    void setKeyToData(uint index, K& key) override;

    DynaBTreeNode<K>* getChild(uint index);
    void setChild(uint index, DynaBTreeNode<K>* child);

    int search(K& key, MatchType match = MatchType::FULL_KEY) override;

    DynaBTreeNode<K>* split() override;
    DynaBTreeNode<K>* pullUpKey(K& key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) override;

    void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) override;
    DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) override;
    void joinWithSibling(K& sinkKey, DynaBTreeNode<K>* rightSibling) override;
    void transferFromSibling(K& sinkKey, K& upKey, DynaBTreeNode<K>* sibling, uint fromIndex) override;
};


#endif