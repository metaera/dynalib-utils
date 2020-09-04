#ifndef DYNABTREE_H
#define DYNABTREE_H

#include <exception>
#include <type_traits>
#include <cstring>
#include "TypeDefs.h"
#include "ICopyable.h"
#include "DynaArray.h"
#include "DynaList.h"
#include "ICacheConst.h"

using namespace std;

#define MAKE_BTREETYPE_DEF(K,V,T) \
    typedef DynaBTree<K,V> T##BTree; \
    typedef DynaBTreeLeafNode<K,V> T##BTreeLeafNode; \
    typedef DynaBTreeInnerNode<K> T##BTreeInnerNode

template <typename K> class DynaBTreeNode;
template <typename K, typename V> class DynaBTreeLeafNode;

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

    DynaBTreeLeafNode<K,V>* _findLeafNodeForKey(K* key);

public:
    DynaBTree(int leafOrder, int innerOrder);
    virtual ~DynaBTree();

    void insert(K* key, V value);
    V search(K* key);
    void deleteEntry(K* key);    
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
    DynaList<K>*      _keys;
    int               _keyCount;
    DynaBTreeNode<K>* _parent;
    DynaBTreeNode<K>* _leftSibling;
    DynaBTreeNode<K>* _rightSibling;

public:
    DynaBTreeNode(TreeNodeType nodeType, int order);
    virtual ~DynaBTreeNode();
    DynaBTreeNode(const DynaBTreeNode<K>& other);
    DynaBTreeNode<K>* copy() override;

    int  getKeyCount();
    K*   getKey(uint index);
    void setKey(uint index, K* key);
    DynaBTreeNode<K>* getParent();
    void setParent(DynaBTreeNode<K>* parent);
    TreeNodeType getNodeType();

    virtual bool isOverflow();
    virtual DynaBTreeNode<K>* handleOverflow();

    virtual bool isUnderflow();
    virtual bool canGiveKey();

    DynaBTreeNode<K>* getLeftSibling();
    void setLeftSibling(DynaBTreeNode<K>* sibling);
    DynaBTreeNode<K>* getRightSibling();
    void setRightSibling(DynaBTreeNode<K>* sibling);
    DynaBTreeNode<K>* handleUnderflow();

    virtual int search(K* key);
    virtual DynaBTreeNode<K>* split();
    virtual DynaBTreeNode<K>* pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode);

    virtual void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex);
    virtual DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild);
    virtual void joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling);
    virtual K* transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex);
};

//----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief B-Tree Leaf Node
 * 
 * @tparam K 
 * @tparam V 
 */
template <typename K, typename V> class DynaBTreeLeafNode : public DynaBTreeNode<K> {
    DynaArray<V>* _values;

    void _insertAt(uint index, K* key, V value);
    void _deleteAt(uint index);

public:
    DynaBTreeLeafNode(int order);
    virtual ~DynaBTreeLeafNode();
    DynaBTreeLeafNode(const DynaBTreeLeafNode<K,V>& other);
    DynaBTreeNode<K>* copy() override;

    V    getValue(uint index);
    void setValue(uint index, V value);
    void insertKey(K* key, V value);
    bool deleteKey(K* key);

    int search(K* key) override;
    DynaBTreeNode<K>* split() override;
    DynaBTreeNode<K>* pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) override;

    void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) override;
    DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) override;
    void joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling) override;
    K* transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex) override;
};

//----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief B-Tree Inner Node
 * 
 * @tparam K 
 */
template <typename K> class DynaBTreeInnerNode : public DynaBTreeNode<K> {
    DynaList<DynaBTreeNode<K>>* _children;

    void _insertAt(uint index, K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild);
    void _deleteAt(uint index);

public:
    DynaBTreeInnerNode(int order);
    virtual ~DynaBTreeInnerNode();
    DynaBTreeInnerNode(const DynaBTreeInnerNode<K>& other);
    DynaBTreeNode<K>* copy() override;

    DynaBTreeNode<K>* getChild(uint index);
    void setChild(uint index, DynaBTreeNode<K>* child);

    int search(K* key) override;
    DynaBTreeNode<K>* split() override;
    DynaBTreeNode<K>* pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) override;

    void transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) override;
    DynaBTreeNode<K>* joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) override;
    void joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling) override;
    K* transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex) override;
};


#endif