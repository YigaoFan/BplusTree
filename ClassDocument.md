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

	std::sort
​	<a href="#cnh">Btree::constructNodeHierarchy</a>	

- [ ] should think about the difference between left-value and right-value

**Btree(compareFunction, pairAarray)**
RIght-value version of Btree constructor

<span id="cnh"> **constructNodeHierarchy**</span>