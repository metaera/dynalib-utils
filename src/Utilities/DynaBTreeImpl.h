#ifndef DYNABTREEIMPL_H
#define DYNABTREEIMPL_H

#include "DynaBTree.h"

#define MAKE_BTREETYPE_INSTANCE(K,V,T) \
    template class DynaBTree<K,V>; \
    template class DynaBTreeNode<K>; \
    template class DynaBTreeLeafNode<K,V>; \
    template class DynaBTreeInnerNode<K>; \
    typedef DynaBTree<K,V> T##BTree; \
    typedef DynaBTreeNode<K> T##BTreeNode; \
    typedef DynaBTreeLeafNode<K,V> T##BTreeLeafNode; \
    typedef DynaBTreeInnerNode<K> T##BTreeInnerNode

template <typename K, typename V> DynaBTreeLeafNode<K,V>* DynaBTree<K,V>::_findLeafNodeForKey(K* key) {
    DynaBTreeNode<K>* node = _root;

    while (node->getNodeType() == TreeNodeType::INNER) {
        node = ((DynaBTreeInnerNode<K>*)node)->getChild( node->search(key) );
    }
    
    return (DynaBTreeLeafNode<K,V>*)node;
}

template <typename K, typename V> DynaBTree<K,V>::DynaBTree(int leafOrder, int innerOrder) : _leafOrder(leafOrder), _innerOrder(innerOrder) {
    _root = new DynaBTreeLeafNode<K,V>(leafOrder);
}

template <typename K, typename V> DynaBTree<K,V>::~DynaBTree() {
    delete _root;
}

template <typename K, typename V> void DynaBTree<K,V>::insert(K* key, V value) {
    DynaBTreeLeafNode<K,V>* leaf = _findLeafNodeForKey(key);
    leaf->insertKey(key, value);
    
    if (leaf->isOverflow()) {
        DynaBTreeNode<K>* n = leaf->handleOverflow();
        if (n != nullptr)
            _root = n; 
    }
}

template <typename K, typename V> V DynaBTree<K,V>::search(K* key) {
    return 0;
}

template <typename K, typename V> void DynaBTree<K,V>::deleteEntry(K* key) {

} 

//----------------------------------------------------------------------------------------------------------------------------------------------


template <typename K> DynaBTreeNode<K>::DynaBTreeNode(TreeNodeType nodeType, int order) : _nodeType(nodeType) {
    _keys         = new DynaList<K>(order);
    _keyCount     = 0;
    _parent       = nullptr;
    _leftSibling  = nullptr;
    _rightSibling = nullptr;
}

template <typename K> DynaBTreeNode<K>::~DynaBTreeNode() {
    delete _keys;
}

template <typename K> DynaBTreeNode<K>::DynaBTreeNode(const DynaBTreeNode<K>& other) {
    _nodeType = other._nodeType;
    _keys = other._keys != nullptr ? other._keys->copy() : nullptr;
    _keyCount = other._keyCount;
    _parent   = other._parent;
    _leftSibling = other._leftSibling;
    _rightSibling = other._rightSibling;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::copy() {
    throw MethodNotImplemented();
}

template <typename K> int DynaBTreeNode<K>::getKeyCount() {
    return _keyCount;
}

template <typename K> K* DynaBTreeNode<K>::getKey(uint index) {
    return (*_keys)[index];
}

template <typename K> void DynaBTreeNode<K>::setKey(uint index, K* key) {
    (*_keys)[index] = key;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::getParent() {
    return _parent;
}

template <typename K> void DynaBTreeNode<K>::setParent(DynaBTreeNode<K>* parent) {
    _parent = parent;
}

template <typename K> TreeNodeType DynaBTreeNode<K>::getNodeType() {
    return _nodeType;
}

template <typename K> bool DynaBTreeNode<K>::isOverflow() {
    return false;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::handleOverflow() {
    return nullptr;
}

template <typename K> bool DynaBTreeNode<K>::isUnderflow() {
    return false;
}

template <typename K> bool DynaBTreeNode<K>::canGiveKey() {
    return false;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::getLeftSibling() {
    return nullptr;
}

template <typename K> void DynaBTreeNode<K>::setLeftSibling(DynaBTreeNode<K>* sibling) {

}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::getRightSibling() {
    return nullptr;
}

template <typename K> void DynaBTreeNode<K>::setRightSibling(DynaBTreeNode<K>* sibling) {

}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::handleUnderflow() {
    return nullptr;
}

template <typename K> int DynaBTreeNode<K>::search(K* key) {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::split() {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling) {
    throw MethodNotImplemented();
}

template <typename K> K* DynaBTreeNode<K>::transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex) {
    throw MethodNotImplemented();
}


//----------------------------------------------------------------------------------------------------------------------------------------------

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::_insertAt(uint index, K* key, V value) {

}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::_deleteAt(uint index) {

}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::DynaBTreeLeafNode(int order) : DynaBTreeNode<K>(TreeNodeType::LEAF, order + 1) {
    _values   = new DynaArray<V>(order + 1);
}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::~DynaBTreeLeafNode() {
    delete _values;
}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::DynaBTreeLeafNode(const DynaBTreeLeafNode<K,V>& other) : DynaBTreeNode<K>(other) {
    _values = other._values != nullptr ? other._values->copy() : nullptr;
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::copy() {
    return new DynaBTreeLeafNode<K,V>(*this);
}

template <typename K, typename V> V DynaBTreeLeafNode<K,V>::getValue(uint index) {
    return (*_values)[index];
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::setValue(uint index, V value) {
    (*_values)[index] = value;
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::insertKey(K* key, V value) {

}

template <typename K, typename V> bool DynaBTreeLeafNode<K,V>::deleteKey(K* key) {
    return false;
}

template <typename K, typename V> int DynaBTreeLeafNode<K,V>::search(K* key) {
    return 0;
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::split() {
    return nullptr;
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    return nullptr;
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {

}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    return nullptr;
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling) {

}

template <typename K, typename V> K* DynaBTreeLeafNode<K,V>::transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex) {
    return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

template <typename K> void DynaBTreeInnerNode<K>::_insertAt(uint index, K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {

}

template <typename K> void DynaBTreeInnerNode<K>::_deleteAt(uint index) {

}

template <typename K> DynaBTreeInnerNode<K>::DynaBTreeInnerNode(int order) : DynaBTreeNode<K>(TreeNodeType::INNER, order + 1) {
    _children = new DynaList<DynaBTreeNode<K>>(order + 2);
}

template <typename K> DynaBTreeInnerNode<K>::~DynaBTreeInnerNode() {
    delete _children;
}

template <typename K> DynaBTreeInnerNode<K>::DynaBTreeInnerNode(const DynaBTreeInnerNode<K>& other) : DynaBTreeNode<K>(other) {
    _children = other._children != nullptr ? other._children->copy() : nullptr;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::copy() {
    return new DynaBTreeInnerNode<K>(*this);
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::getChild(uint index) {
    return (DynaBTreeNode<K>*)(*_children)[index];
}

template <typename K> void DynaBTreeInnerNode<K>::setChild(uint index, DynaBTreeNode<K>* child) {
    (*_children)[index] = child;
    if (child != nullptr) {
        child->setParent(this);
    }
}

template <typename K> int DynaBTreeInnerNode<K>::search(K* key) {
    return 0;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::split() {
    return nullptr;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::pullUpKey(K* key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    return nullptr;
}

template <typename K> void DynaBTreeInnerNode<K>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {

}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    return nullptr;
}

template <typename K> void DynaBTreeInnerNode<K>::joinWithSibling(K* sinkKey, DynaBTreeNode<K>* rightSibling) {

}

template <typename K> K* DynaBTreeInnerNode<K>::transferFromSibling(K* sinkKey, DynaBTreeNode<K>* sibling, uint toIndex) {
    return nullptr;
}


#endif