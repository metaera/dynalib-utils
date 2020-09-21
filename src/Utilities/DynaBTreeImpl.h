#ifndef DYNABTREEIMPL_H
#define DYNABTREEIMPL_H

#include "DynaBTree.h"
#include <stdio.h>
#include <string.h>

#define MAKE_BTREETYPE_INSTANCE(K,V,T) \
    template struct DynaTreeInnerElem<K>; \
    template struct DynaTreeLeafElem<K,V>; \
    template class  DynaBTree<K,V>; \
    template class  DynaBTreeNode<K>; \
    template class  DynaBTreeLeafNode<K,V>; \
    template class  DynaBTreeInnerNode<K>; \
    typedef DynaTreeInnerElem<K> T##TreeInnerElem; \
    typedef DynaTreeLeafElem<K,V> T##TreeLeafElem; \
    typedef DynaTreeInnerElem<K>::Data T##TreeInnerData; \
    typedef DynaTreeLeafElem<K,V>::Data T##TreeLeafData; \
    typedef DynaBTree<K,V> T##BTree; \
    typedef DynaBTreeNode<K> T##BTreeNode; \
    typedef DynaBTreeLeafNode<K,V> T##BTreeLeafNode; \
    typedef DynaBTreeInnerNode<K> T##BTreeInnerNode

//----------------------------------------------------------------------------------------------------------------------------------------------

// template <typename K, typename V> DynaTreeLeafElem<K,V>::DynaTreeLeafElem::Data("", 0) {    
// }

template <typename K, typename V> int DynaTreeLeafElem<K,V>::compareTo(DynaTreeLeafElem<K,V>* other, MatchType match) {
    int result;
    if (match == MatchType::PARTIAL_KEY) {
        auto len = strlen(search.key);
        result   = strncmp(search.key, other->data->key, len);
    }
    else {
        result = strncmp(search.key, other->data->key, keySize());
        if (match == MatchType::WITH_VALUE && result == 0) {
            auto v1 = search.value;
            auto v2 = other->data->value;
            result = v1 < v2 ? -1 : v1 > v2 ? 1 : 0;
        }
    }
    return result;
}

template <typename K, typename V> void DynaTreeLeafElem<K,V>::setKeyInSearch(K key) {
    std::strncpy(search.key, key, keySize());
}
template <typename K, typename V> void DynaTreeLeafElem<K,V>::setValueInSearch(V value) {
    search.value = value;
}
template <typename K, typename V> void DynaTreeLeafElem<K,V>::setKeyInData(K key) {
    std::strncpy(data->key, key, keySize());
}
template <typename K, typename V> void DynaTreeLeafElem<K,V>::setValueInData(V value) {
    data->value = value;
}
template <typename K, typename V> void DynaTreeLeafElem<K,V>::getKeyFromData(K& key) {
    std::strncpy(key, data->key, keySize());
}
template <typename K, typename V> void DynaTreeLeafElem<K,V>::getValueFromData(V& value) {
    value = data->value;
}


template <typename K> int DynaTreeInnerElem<K>::compareTo(DynaTreeInnerElem<K>* other, MatchType match) {
    auto len    = match == MatchType::PARTIAL_KEY ? strlen(search.key) : keySize();
    auto result = std::strncmp(search.key, other->data->key, len);
    return result;
}

template <typename K> void DynaTreeInnerElem<K>::setKeyInSearch(K key) {
    std::strncpy(search.key, key, keySize());
}

template <typename K> void DynaTreeInnerElem<K>::setKeyInData(K key) {
    std::strncpy(data->key, key, keySize());
}

template <typename K> void DynaTreeInnerElem<K>::getKeyFromData(K& key) {
    std::strncpy(key, data->key, keySize());
}

//----------------------------------------------------------------------------------------------------------------------------------------------

template <typename K, typename V> DynaBTreeLeafNode<K,V>* DynaBTree<K,V>::_findLeafNodeForKey(K& key) {
    auto* node = _root;

    while (node->getNodeType() == TreeNodeType::INNER) {
        int result = node->search(key);
        node = ((DynaBTreeInnerNode<K>*)node)->getChild(DynaBTree::insertAt(result));
    }    
    return (DynaBTreeLeafNode<K,V>*)node;
}

template <typename K, typename V> DynaBTree<K,V>::DynaBTree(int leafOrder, int innerOrder) : _leafOrder(leafOrder), _innerOrder(innerOrder) {
    _root = new DynaBTreeLeafNode<K,V>(leafOrder, innerOrder);
}

template <typename K, typename V> DynaBTree<K,V>::~DynaBTree() {
    delete _root;
}

template <typename K, typename V> bool DynaBTree<K,V>::wasFound( int returnValue ) {
    return returnValue >= 0;
}
    
template <typename K, typename V> int DynaBTree<K,V>::insertAt( int returnValue ) {
    return returnValue >= 0 ? returnValue : -(returnValue + 1);
}

template <typename K, typename V> void DynaBTree<K,V>::insert(K& key, V& value) {
    auto* leaf = _findLeafNodeForKey(key);
    DynaTreeLeafElem<K,V> elem;
    elem.setKeyInSearch(key);
    elem.setValueInSearch(value);
    leaf->insertElem(elem);

    if (leaf->isOverflow()) {
        auto* n = leaf->handleOverflow();
        if (n != nullptr)
            _root = n; 
    }
}

template <typename K, typename V> int DynaBTree<K,V>::search(K& key, V* value, MatchType match) {
    auto* leaf = _findLeafNodeForKey(key);
    DynaTreeLeafElem<K,V> tempElem;
    tempElem.setKeyInSearch(key);
    if (value != nullptr) {
        tempElem.setValueInSearch(*value);
    }
    return leaf->search(tempElem, match);
}

template <typename K, typename V> void DynaBTree<K,V>::deleteEntry(K& key, V* value) {
    auto* leaf = _findLeafNodeForKey(key);
    DynaTreeLeafElem<K,V> tempElem;
    tempElem.setKeyInSearch(key);
    if (value != nullptr) {
        tempElem.setValueInSearch(*value);
    }
    if (leaf->deleteElem(tempElem) && leaf->isUnderflow()) {
        auto* n = leaf->handleUnderflow();
        if (n != nullptr)
            _root = n; 
    }
} 

//----------------------------------------------------------------------------------------------------------------------------------------------


template <typename K> DynaBTreeNode<K>::DynaBTreeNode(TreeNodeType nodeType, int maxKeys, int leafOrder, int innerOrder) :
         _nodeType(nodeType), _leafOrder(leafOrder), _innerOrder(innerOrder) {
    _buffer       = new DynaBuffer(maxKeys);
    _parent       = nullptr;
    _leftSibling  = nullptr;
    _rightSibling = nullptr;
}

template <typename K> DynaBTreeNode<K>::~DynaBTreeNode() {
    delete _buffer;
}

template <typename K> DynaBTreeNode<K>::DynaBTreeNode(const DynaBTreeNode<K>& other) {
    _nodeType     = other._nodeType;
    _leafOrder    = other._leafOrder;
    _innerOrder   = other._innerOrder;
    _buffer       = other._buffer != nullptr ? other._buffer->copy() : nullptr;
    _parent       = other._parent;
    _leftSibling  = other._leftSibling;
    _rightSibling = other._rightSibling;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::copy() {
    throw MethodNotImplemented();
}

template <typename K> DynaBuffer* DynaBTreeNode<K>::getBuffer() {
    return _buffer;
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

template <typename K> int DynaBTreeNode<K>::getKeyCount() {
    return _buffer->getElemCount();
}

template <typename K> bool DynaBTreeNode<K>::getKeyFromData(uint index, K& key) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::setKeyToData(uint index, K& key) {
    throw MethodNotImplemented();
}

template <typename K> bool DynaBTreeNode<K>::isOverflow() {
    return _buffer->isFull();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::handleOverflow() {
    int   midIndex = getKeyCount() / 2;
    K     upKey;
    getKeyFromData(midIndex, upKey);    
    auto* newRNode = split();
    if (getParent() == nullptr) {
        setParent(new DynaBTreeInnerNode<K>(this->_innerOrder));
    }
    newRNode->setParent(getParent());
    
    // maintain links of sibling nodes
    newRNode->setLeftSibling(this);
    newRNode->setRightSibling(_rightSibling);
    if (getRightSibling() != nullptr)
        getRightSibling()->setLeftSibling(newRNode);
    setRightSibling(newRNode);
    
    // push up a key to parent internal node
    return getParent()->pullUpKey(upKey, this, newRNode);
}

template <typename K> bool DynaBTreeNode<K>::isUnderflow() {
    return getKeyCount() < (_buffer->getElemCapacity() / 2);
}

template <typename K> bool DynaBTreeNode<K>::canGiveKey() {
    return getKeyCount() > (_buffer->getElemCapacity() / 2);
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::getLeftSibling() {
    if (_leftSibling != nullptr && _leftSibling->getParent() == getParent())
        return _leftSibling;
    return nullptr;
}

template <typename K> void DynaBTreeNode<K>::setLeftSibling(DynaBTreeNode<K>* sibling) {
    _leftSibling = sibling;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::getRightSibling() {
    if (_rightSibling != nullptr && _rightSibling->getParent() == getParent())
        return _rightSibling;
    return nullptr;
}

template <typename K> void DynaBTreeNode<K>::setRightSibling(DynaBTreeNode<K>* sibling) {
    _rightSibling = sibling;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::handleUnderflow() {
    if (getParent() == nullptr)
        return nullptr;

    // try to take a key from sibling
    auto* leftSibling = getLeftSibling();
    if (leftSibling != nullptr && leftSibling->canGiveKey()) {
        getParent()->transferChildren(this, leftSibling, leftSibling->getKeyCount() - 1);
        return nullptr;
    }

    auto* rightSibling = getRightSibling();
    if (rightSibling != nullptr && rightSibling->canGiveKey()) {
        getParent()->transferChildren(this, rightSibling, 0);
        return nullptr;
    }

    // Can not take a key from any sibling, so do join with sibling
    if (leftSibling != nullptr) {
        return getParent()->joinChildren(leftSibling, this);
    }
    else {
        return getParent()->joinChildren(this, rightSibling);
    }
}

template <typename K> int DynaBTreeNode<K>::search(K& key, MatchType match) {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::split() {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::pullUpKey(K& key, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {
    throw MethodNotImplemented();
}

template <typename K> DynaBTreeNode<K>* DynaBTreeNode<K>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::joinWithSibling(K& sinkElem, DynaBTreeNode<K>* rightSibling) {
    throw MethodNotImplemented();
}

template <typename K> void DynaBTreeNode<K>::transferFromSibling(K& sinkElem, K& upKey, DynaBTreeNode<K>* sibling, uint toIndex) {
    throw MethodNotImplemented();
}


//----------------------------------------------------------------------------------------------------------------------------------------------

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::_insertAt(uint index, DynaTreeLeafElem<K,V>& elem) {
    this->getBuffer()->insertElem(index, *((uint8_t*)&elem.search));
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::_deleteAt(uint index) {
    this->getBuffer()->deleteElem(index);
}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::DynaBTreeLeafNode(int leafOrder, int innerOrder) :
        DynaBTreeNode<K>(TreeNodeType::LEAF, leafOrder + 1, leafOrder, innerOrder) {
}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::~DynaBTreeLeafNode() {
}

template <typename K, typename V> DynaBTreeLeafNode<K,V>::DynaBTreeLeafNode(const DynaBTreeLeafNode<K,V>& other) : DynaBTreeNode<K>(other) {
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::copy() {
    return new DynaBTreeLeafNode<K,V>(*this);
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::setElemDataPos(uint index, DynaTreeLeafElem<K,V>& elem) {
    elem.data = (typename DynaTreeLeafElem<K,V>::Data*)(this->_buffer->getInternalTypedArrayAtPos(this->_buffer->getElemPos(index)));
}

template <typename K, typename V> bool DynaBTreeLeafNode<K,V>::getKeyFromData(uint index, K& key) {
    DynaTreeLeafElem<K,V> tempElem;
    setElemDataPos(index, tempElem);
    tempElem.getKeyFromData(key);
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::setKeyToData(uint index, K& key) {
    DynaTreeLeafElem<K,V> tempElem;
    setElemDataPos(index, tempElem);
    tempElem.setKeyInData(key);
}


template <typename K, typename V> DynaTreeLeafElem<K,V>* DynaBTreeLeafNode<K,V>::getElemIntoSearch(uint index, DynaTreeLeafElem<K,V>& elem) {
    if (this->_buffer->getElem(index, *((uint8_t*)&elem.search))) {
        return &elem;
    }
    return nullptr;
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::setElemFromSearch(uint index, DynaTreeLeafElem<K,V>& elem) {
    this->_buffer->setElem(index, *((uint8_t*)&elem.search));
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::insertElem(DynaTreeLeafElem<K,V>& elem) {
    auto index = search(elem);
    if (!DynaBTree<K,V>::wasFound(index)) {
        index = DynaBTree<K,V>::insertAt(index);
        if (index < this->getKeyCount()) {
            _insertAt(index, elem);
        }
    }
}

template <typename K, typename V> bool DynaBTreeLeafNode<K,V>::deleteElem(DynaTreeLeafElem<K,V>& elem) {
    auto index = search(elem);
    if (DynaBTree<K,V>::wasFound(index)) {
        this->_buffer->deleteElems(index, index);
        return true;
    }
    return false;
}


template <typename K, typename V> int DynaBTreeLeafNode<K,V>::search(K& key, MatchType match) {
    int low  = 0;
    int high = this->getBuffer()->getElemCount() - 1;
    DynaTreeLeafElem<K,V> searchElem;
    searchElem.setKeyInSearch(key);
    DynaTreeLeafElem<K,V> tempElem;

    while ( low <= high ) {
        uint middle = ( low + high ) >> 1;
        setElemDataPos(middle, tempElem);
        int result = searchElem.compareTo(&tempElem, match);
        if ( result < 0 ) {
            high = middle - 1;
        }
        else if ( result > 0 ) {
            low = middle + 1;
        }
        else {
            return middle;
        }
    }
    return -( low + 1 );
}

template <typename K, typename V> int DynaBTreeLeafNode<K,V>::search(DynaTreeLeafElem<K,V>& elem, MatchType match) {
    int low  = 0;
    int high = this->getBuffer()->getElemCount() - 1;
    DynaTreeLeafElem<K,V> tempElem;

    while ( low <= high ) {
        uint middle = ( low + high ) >> 1;
        setElemDataPos(middle, tempElem);
        int result = elem.compareTo(&tempElem, match);
        if ( result < 0 ) {
            high = middle - 1;
        }
        else if ( result > 0 ) {
            low = middle + 1;
        }
        else {
            return middle;
        }
    }
    return -( low + 1 );
}

// template <typename K, typename V> int DynaBTreeLeafNode<K,V>::search(K& elem) {
//     return 0;
// }

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::split() {
    int count    = this->getKeyCount();
    int midIndex = count / 2;
    auto* newRNode = new DynaBTreeLeafNode<K,V>(this->_leafOrder, this->_innerOrder);
    this->_buffer->moveElems(midIndex, count - 1, newRNode->getBuffer(), 0);
    return newRNode;
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::pullUpKey(K& elem, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    throw MethodNotSupported();
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {
    throw MethodNotSupported();
}

template <typename K, typename V> DynaBTreeNode<K>* DynaBTreeLeafNode<K,V>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    throw MethodNotSupported();
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::joinWithSibling(K& sinkKey, DynaBTreeNode<K>* rightSibling) {
    auto* siblingLeaf = rightSibling;
    int   thisCount   = this->getKeyCount();
    int   sibCount    = siblingLeaf->getKeyCount();
    siblingLeaf->getBuffer()->moveElems(0, sibCount - 1, this->getBuffer(), thisCount);
}

template <typename K, typename V> void DynaBTreeLeafNode<K,V>::transferFromSibling(K& sinkKey, K& upKey, DynaBTreeNode<K>* sibling, uint fromIndex) {
    auto* siblingNode = (DynaBTreeLeafNode<K,V>*)sibling;

    DynaTreeLeafElem<K,V> searchElem;
    siblingNode->getElemIntoSearch(fromIndex, searchElem);
    insertElem(searchElem);
    siblingNode->_deleteAt(fromIndex);

    if (fromIndex == 0) {
        sibling->getKeyFromData(0, upKey);
    }
    else {
        this->getKeyFromData(0, upKey);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------

template <typename K> void DynaBTreeInnerNode<K>::_insertAt(uint index, K& elem, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {

}

template <typename K> void DynaBTreeInnerNode<K>::_deleteAt(uint index) {

}

template <typename K> DynaBTreeInnerNode<K>::DynaBTreeInnerNode(int order) :
        DynaBTreeNode<K>(TreeNodeType::INNER, order + 1, 0, order) {
}

template <typename K> DynaBTreeInnerNode<K>::~DynaBTreeInnerNode() {
}

template <typename K> DynaBTreeInnerNode<K>::DynaBTreeInnerNode(const DynaBTreeInnerNode<K>& other) : DynaBTreeNode<K>(other) {
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::copy() {
    return new DynaBTreeInnerNode<K>(*this);
}

template <typename K> void DynaBTreeInnerNode<K>::setElemDataPos(uint index, DynaTreeInnerElem<K>& elem) {
    elem.data = (typename DynaTreeInnerElem<K>::Data*)(this->_buffer->getInternalTypedArrayAtPos(this->_buffer->getElemPos(index)));
}

template <typename K> bool DynaBTreeInnerNode<K>::getKeyFromData(uint index, K& key) {
    DynaTreeInnerElem<K> tempElem;
    setElemDataPos(index, tempElem);
    tempElem.getKeyFromData(key);
}

template <typename K> void DynaBTreeInnerNode<K>::setKeyToData(uint index, K& key) {
    DynaTreeInnerElem<K> tempElem;
    setElemDataPos(index, tempElem);
    tempElem.setKeyInData(key);
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::getChild(uint index) {
    // return (DynaBTreeNode<K>*)(*_children)[index];
    return nullptr;
}

template <typename K> void DynaBTreeInnerNode<K>::setChild(uint index, DynaBTreeNode<K>* child) {
    // (*_children)[index] = child;
    // if (child != nullptr) {
    //     child->setParent(this);
    // }
}

template <typename K> int DynaBTreeInnerNode<K>::search(K& key, MatchType match) {
    int low  = 0;
    int high = this->_buffer->getElemCount() - 1;
    DynaTreeInnerElem<K> searchElem;
    searchElem.setKeyInSearch(key);
    DynaTreeInnerElem<K> tempElem;

    while ( low <= high ) {
        uint middle = ( low + high ) >> 1;
        setElemDataPos(middle, tempElem);
        int result = searchElem.compareTo(&tempElem, match);
        if ( result < 0 ) {
            high = middle - 1;
        }
        else if ( result > 0 ) {
            low = middle + 1;
        }
        else {
            return middle;
        }
    }
    return -( low + 1 );
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::split() {
    return nullptr;
}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::pullUpKey(K& elem, DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightNode) {
    return nullptr;
}

template <typename K> void DynaBTreeInnerNode<K>::transferChildren(DynaBTreeNode<K>* fromNode, DynaBTreeNode<K>* toNode, uint toIndex) {

}

template <typename K> DynaBTreeNode<K>* DynaBTreeInnerNode<K>::joinChildren(DynaBTreeNode<K>* leftChild, DynaBTreeNode<K>* rightChild) {
    return nullptr;
}

template <typename K> void DynaBTreeInnerNode<K>::joinWithSibling(K& sinkElem, DynaBTreeNode<K>* rightSibling) {

}

template <typename K> void DynaBTreeInnerNode<K>::transferFromSibling(K& sinkKey, K& upKey, DynaBTreeNode<K>* sibling, uint toIndex) {
}


#endif