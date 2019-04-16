* Btree
* BtreeHelper (will be deprecated I think)
* NodeBase
  * LeafNode
  * MidddleNode
* Proxy(temporary)

(Only show the class mainly related to B+ tree, other class or function like utility will not be shown.)

* Btree

**compareFunc**
public for self and other to use to compare two Key

**Btree(compareFunction, pairArray)**

Left-value version Constructor of Btree,

Dependency:

    * std::sort

​    * <a href="#user-content-cnh">Btree::constructNodeHierarchy</a>

    * LeafNode constructor

*TODO*
- [ ] should think about the difference between left-value and right-value
- [ ] duplicate check is using compareFunction && compareFunction. If return true, that means two Key is the same. Maybe here is another method. Maybe related keyword: *强相等*

**Btree(compareFunction, pairAarray)**

RIght-value version of Btree constructor

<span id="cnh"> **constructNodeHierarchy**</span>

Recursive method to construct Nodes Hierarchy, the first call is different from the subsequent call. Because the fist call is to construct the LeafNode, the other is to construct the MidddleNode. First, use Key-Value pair to Construct a LeafNode level then put it into a strict type Node array. Then use this level array to construct a higher level Node, put it into a strict type Node array too. Use FirstCall template argument to distinguish different call.

Dependency:

    * LeafNode constructor

    * MidddleNode constructor

*TODO*
- [ ] Need to think about setting the next node of the LeafNode
- [ ] Should add some debug code to monitor these Node constructing to ensure the Node hierarchy

**search**

Search the given Key in the Btree. Return Value pointer when found the LeafNode corresponding the Key. Or return nullptr.

Dependency:

    * middle of NodeBase

    * checkOut

    * operator[] of the LeafNode

*TODO*
- [ ] Return value type is not correspond to the pointer type


**add**

add the Key-Value pair into the Btree

Dependency:

    * middle of NodeBase

    * checkOut

    * have of NodeBase

    * add of LeafNode

    * static function maxLeaf

*TODO*
- [ ] Need to attention the position of adding the new pair. Could make the move pair logic out the Node.

**modify**

modify the existed Value of the Key

Dependency:

    * middle of NodeBase

    * checkOut

    * have of NodeBase(TODO: should belong to LeafNode?)

    * operator[] of LeafNode

**explore**

use traverseLeaf to get Key of LeafNode

Dependency:

    * traverseLeaf

    * allKey of LeafNode

**remove**

remove the correspond Node of the Key

Dependency:

    * checkOut

    * have of NodeBase

    * remvoe of LeafNode(TODO: in the current this logic is belong to NodeBase, should modify)

**have**

User before changing the btree should use have **have** to check the Key's existence

Dependency:

    * checkOut

    * middle of NodeBase

    * have of NodeBase(TODO: this logic shoud belong to LeafNode? Think of it.)
