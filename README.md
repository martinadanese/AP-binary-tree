# AP_project
Repo for Advanced Programming final project

## C++ part

This part consists of the implementation of a template binary search tree (bst). The classes implemented are:

- `bst<k_type,v_type,OP>` which defines a pointer to the root of the bst (`head`), 
the comparison operator (`op`), which is used to compare two keys (`std::less<k_type`>is set as 
default choice, and the number of nodes present in the tree (`_size`).

- `node`, a structure that describes a node in the bst. It has a `std::pair<k_type,v_type>` pair, two 
`std::unique_ptr<node>` pointing towards left and right child and a pointer to the parent node.

- `_iterator` that implements a bidirectional iterator templaten on the node.

### constructors and member functions

The `node` structure presents only constructors, while the main constructors and member functions of
bst class are:

#### begin(), last() and end()
both in the constant and non-constant version, together with `cend()` and `cbegin()`. 
To avoid copde duplication, begin and last rely on `_b` and `_last`, two utilities 
that identify the biggest and smallest node respectively.


#### get_size()

Returns `_size`
```
std::size_t get_size() const noexcept;
```


#### insert
The following:
```
std::pair<iterator, bool> insert(const std::pair<k_type,v_type>& x)
std::pair<iterator, bool> insert(std::pair<k_type,v_type>&& x)
```
rely on the utility:
```
template <typename O>
std::pair<iterator,bool> _insert(O&& v) {
```
that actually implements the insertion. Given a new pair, a loop is performed over the bst,
until an element with a key bigger than the one to be inserted is found. If free, the new elements 
becomes the left child of the identified node, otherwise it becomes the right child of the node with 
no right child that
has the greatest key among those with a key smaller than the new node.
Repetitions are avoided.

#### move and copy semantics
move constructor and assignment are the default ones, while copy constructor and assignment are
implemented to perform a deep copy of the bsty.



#### find()

Loop over the bst until the target key is found. In case there is no such value, they return a null
pointer

```
iterator find(const k_type& x) noexcept
const_iterator find(const k_type& x) const noexcept
```




#### emplace
relies on the `_insert` utility.
```
template< class... Types >
std::pair<iterator,bool> emplace(Types&&... args)
```




#### clear()

erases the bst, resetting the root and so letting out of scop all the pointers

```
void clear() noexcept
```




#### balance()

to balance the tree all items are stored in a `std::vector`
in ascending order thanks to the iterator. The head
is reset to a nullptr and then stored elements are insered
again recursively splitting into half the vector, thanks to 
the `_balance` utility.

```
void balance() noexcept
```



#### erase()
to erase a node, first it is swapped with its successor
repeatedly until a leaves level is found. If the node
of interest is the last one, the predecessor is usend
instead of the successor.
```
void erase(const k_type& x) noexcept {
```



#### reverse_print()
prints the bst backwards.



#### operator overloading
The put-to operator is overloaded on order to print the bst in the correct order.
The subscripting operator is overloaded in order to take a key as input and to
print the corrsponding value, if present, or to insert that key in the bst if not
present.


## Python part

A reverse dicrionary is created exploiting comprehensions.

