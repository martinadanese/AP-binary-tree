# AP_project
Repo for Advanced Programming final project

## C++ part

This part consists of the implementation of a template binary search tree (bst). The classes implemented are:

- `bst<const k_type,v_type,OP>` which defines a pointer to the root of the bst (`head`), 
the comparison operator (`op`), which is used to compare two keys (`std::less<k_type`>is set as 
default choice, and the number of nodes present in the tree (`_size`).

- `node`, a structure that describes a node in the bst. It has a `std::pair<const k_type,v_type>` pair, two 
`std::unique_ptr<node>` pointing towards left and right child and a raw pointer to the parent node.

- `_iterator` that implements a bidirectional iterator templated on the bst pair.

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
which implements the insertion. Given a new pair, the tree is vertically crossed with O(log(n)) complexity,
until a free spot is found.

#### move and copy semantics
move constructor and assignment are the default ones, while copy constructor and assignment are
implemented to perform a deep copy of the bst.



#### find()

Vertically loops over the bst until the target key is found with O(log(n)) complexity. In case there is no such value, they return a null
pointer. Relies on the `_find` utility, which returns a raw pointer to the node.

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
the one of the node branches is attached to the parent node while the other is
re-insered exploiting the `_balance` utility.
```
void erase(const k_type& x) noexcept {
```


#### operator overloading
The put-to operator is overloaded on order to print the bst in the correct order.
The subscripting operator is overloaded in order to take a key as input and to
print the corrsponding value, if present, or to insert that key in the bst if not
present.


## Python part

A reverse dicrionary is created exploiting comprehensions.

