#include <functional>
#include <iostream>
#include <utility>
#include <memory>
#include <iterator>
#include <vector>
//
//
// ===============================================================
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   node
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ===============================================================
template <typename k_type, typename v_type>
  struct _node{
    std::pair<const k_type, v_type> pair;
    std::unique_ptr<_node> right;
    std::unique_ptr<_node> left;
    _node* parent=nullptr;
    
    _node() noexcept = default; 
    ~_node() noexcept = default;
    
    explicit _node(const std::pair<k_type, v_type>& p) 
      : pair{p} {}
    
    explicit _node (std::pair<k_type, v_type>&& p) 
      : pair{std::move(p)} {}
    
    //move semantics
    _node(_node&& n) noexcept = default;
    _node& operator=(_node&& n) noexcept = default;
  };


// ===============================================================
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   bst
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ===============================================================
template <typename k_type, typename v_type,
          typename OP = std::less<k_type> >
class bst{

  using node = _node<const k_type,v_type>;
  OP op = OP{};
  

  std::unique_ptr<node> head;  
  std::size_t _size{0};



public:



  bst() noexcept = default;
  ~bst() noexcept = default;

  std::size_t get_size() const noexcept {return _size;}
  
// ===============================================================
//   begin & end
// ===============================================================

  template <typename O> 
  class _iterator;
  using iterator = _iterator<std::pair<const k_type,v_type>>;//<node>;
  using const_iterator = _iterator<const std::pair<const k_type,v_type>>;//<const node>;

  /*
   * _b: to avoid code duplication
   * begin node: the one with the smallest k
   * namely the left-most
   */
  node* _b() const noexcept {
    auto tmp = head.get();
    while (tmp->left)
      tmp = tmp->left.get();
    return tmp;  
  }

  // define begin iterators
  iterator begin() noexcept { return iterator{_b()};}
  const_iterator begin() const noexcept { return const_iterator{_b()};}
  const_iterator cbegin() const noexcept { return const_iterator{_b()};} 

  // define end iterators
  iterator end() noexcept { return iterator{nullptr}; }
  const_iterator end() const noexcept { return const_iterator{nullptr}; }
  const_iterator cend() const noexcept { return const_iterator{nullptr}; }

  /*
   * _last: to avoid code duplication
   * last node: the one with the highest k, 
   * namely the right-most
   */
  node* _last() const noexcept { 
    auto tmp = head.get();
    while (tmp->right)
      tmp = tmp->right.get();
    return tmp; 
  }

  iterator last() noexcept { return iterator{_last()};}
  const_iterator last() const noexcept { return const_iterator{_last()};}

// ===============================================================
//   insert
// ===============================================================
 
  std::pair<iterator, bool> insert(const std::pair<k_type,v_type>& x) {return _insert(x);}
  std::pair<iterator, bool> insert(std::pair<k_type,v_type>&& x) {return _insert(std::move(x));}
  

  template <typename O>
  std::pair<iterator,bool> _insert(O&& v) {
    
    auto _node = new node{std::forward<O>(v)};
    auto tmp = head.get();
    bool found=1;
    _size++;


    if (!tmp){
      head.reset(_node);
      return std::pair<iterator,bool>{ begin() , found };
    }
 
    while (tmp || tmp == head.get()  ) {
      if (op(v.first, tmp->pair.first)) {
          
	  if (tmp->left)
	    tmp = tmp->left.get();
	  else {
            _node->parent = tmp;
	    tmp->left.reset(_node);
            return std::pair<iterator,bool>{ iterator(_node) , found };
	  }
      }

      else if (op(tmp->pair.first, v.first)) {

	  if (tmp->right)
	    tmp = tmp->right.get();
	  else {
            _node->parent = tmp;
	    tmp->right.reset(_node);
            return std::pair<iterator,bool>{ iterator(_node) , found };
          }
      }
      else {
        found = 0;
        _size--;
        return std::pair<iterator,bool>{ end() , found };  
      }
    }
    
    found = 0;
    _size--;
    return std::pair<iterator,bool>{ end() , found };  
  }
  /*std::pair<iterator,bool> _insert(O&& v) {
    auto _node = new node{std::forward<O>(v)};
    bool found=1;
    _size++;
    
    // if head empty, _node will be the head
    if (!(head.get())) { 
      head.reset(_node);
      return std::pair<iterator,bool> {begin(),found};
    }
    
    auto pos = end();
    auto i = begin();
    
    // loop over the nodes
    while(&(*i) != &(*end())) {
      
      // check if there is already such a key, and return in case 
      if (!op(v.first, i->pair.first) && !op(i->pair.first, v.first)){
        delete _node;
	found=0;
	_size--;
	return std::pair<iterator,bool> {end(),found};
	}

      // find the first node (pos) that has 
      // a key greater the one to insert
      if (op(v.first, i->pair.first)){

        pos = i;
	// if pos has not a left child, place the new element there
	if (!pos->left) {
	  (*pos).left.reset(_node);
          (*pos).left->parent = (&(*pos));
          return std::pair<iterator,bool>{pos,found};
          }
	// else, place the new node at the right of
	// the first free left-child position 
	// among nodes with smaller keys.
        else {
          pos--;
	  while (pos->right)
            pos--;
          (*pos).right.reset(_node);
          (*pos).right->parent = (&(*pos));
          return std::pair<iterator,bool>{pos,found};
          }
      }

    ++i;
    }

    // if we reached this point, we 
    // are inserting the new biggest key
    pos = last();
    (*pos).right.reset(_node);
    (*pos).right->parent = (&(*pos));
    return std::pair<iterator,bool> {pos,found};
    
  }
  */
// ===============================================================
//   move and copy ctrs and assignments
// ===============================================================

  bst(bst&&) noexcept = default;
  bst& operator=(bst&&) noexcept = default;


  bst(const bst& b){
    if (b.head) {
      auto _node = new node{b.head->pair};
      head.reset(_node);
      _size = b._size;
      for(const auto &x : b)
	insert(x);
      }
    }
  
  bst operator=(const bst& b){
    head.reset();              
    auto tmp = b;              
    (*this) = std::move(tmp);  
    return *this;
  }



// ===============================================================
//   find
// ===============================================================

  /*
   * loop over all nodes until the desired one is found.
   */
  /*
  node* _find(node* next, const k_type& x) {
    
    if (!next)
      return nullptr;

    if (op(next->pair.first,x)){
      next = next->right.get();
      next = _find(next,x);
      }
    else if (op(x, head->pair.first)){
      next = next->left.get();
      next = _find(next,x);
      }
    
     return next;
  }
*/
  iterator find(const k_type& x) noexcept {   
    
    auto next = head.get();
    while (next) {
      if (op(next->pair.first, x))
        next = next->right.get();
      else if (op(x, next->pair.first))
        next = next->left.get();
      else if (!op(x, next->pair.first) && !op(next->pair.first, x))
        return iterator(next);
    }

    //if not found
    return iterator(end());
  }

// ===============================================================
  const_iterator find(const k_type& x) const noexcept {   
    
    auto next = head.get();
    while (next) {
      if (op(next->pair.first, x))
        next = next->right.get();
      else if (op(x, next->pair.first))
        next = next->left.get();
      else if (!op(x, next->pair.first) && !op(next->pair.first, x))
        return const_iterator(next);
    }

    //if not found
    return iterator(end());
  }

// ===============================================================
  node* _find(const k_type& x) noexcept {   
    
    auto next = head.get();
    while (next) {
      if (op(next->pair.first, x))
        next = next->right.get();
      else if (op(x, next->pair.first))
        next = next->left.get();
      else if (!op(x, next->pair.first) && !op(next->pair.first, x))
        return next;
    }

    //if not found
    return nullptr;
  }
/*
  iterator find(const k_type& x) noexcept {   
    auto first = begin();
    while(&(*first) != &(*end())) {
      if (first->pair.first == x) {
        return first;
      }
      ++first;
    }
    // if not found
    return end();
  }

  const_iterator find(const k_type& x) const noexcept {   
    auto first = begin();
    while(&(*first) != &(*end())) {
      if (first->pair.first == x) {
        return first;
      }
      ++first;
    }
    // if not found
    return end();
  }
*/

// ===============================================================
//   emplace
// ===============================================================

  template< class... Types >
  std::pair<iterator,bool> emplace(Types&&... args){
    return _insert(std::make_pair(std::forward<Types>(args)...));
  }

// ===============================================================
//   clear
// ===============================================================
  
  void clear() noexcept {
    head.reset();
    return;
  }

// ===============================================================
//   balance
// ===============================================================
  
  void balance() noexcept {
    /*
     * to balance the tree all items are stored in a std::vector
     * in ascending order thanks to the iterator, then the head
     * is reset to a nullptr and then stored elements are insered 
     * again recursively splitting into half the vector.
     */

    std::vector<std::pair<k_type,v_type>> tmp;
    for (const auto &x : *this)
      tmp.push_back(x);
    
    head.reset(nullptr);

    _size =0;
    _balance(tmp, 0, tmp.size());
    return;
  }

// ===============================================================
  
  void _balance(std::vector<std::pair<k_type,v_type>>& tmp, std::size_t sidx, std::size_t eidx) noexcept {
    
    _insert(tmp[(eidx+sidx)/2]);
    
    if ((eidx+sidx)/2 != sidx)
      _balance(tmp, sidx, (eidx+sidx)/2);
    if ((eidx+sidx)/2+1 != eidx)
      _balance(tmp, (eidx+sidx)/2+1, eidx);
  
  }
    
// ===============================================================
//   erase
// ===============================================================
 
 /*
  * to erase a node, first we swap it with its successor
  * repeatedly until a leaves level is found. If the node 
  * of interest is the last one, the predecessor is consi-
  * dered.
  *
 */
 void _swap(node* a, node* b){
   /*  
   k_type tmp_k{std::move(a->pair.first)};
   v_type tmp_v{std::move(a->pair.second)};
   a->pair.first = std::move(b->pair.first);
   a->pair.second = std::move(b->pair.second);
   b->pair.first = std::move(tmp_k);
   b->pair.second = std::move(tmp_v);
   */
   auto tmp{std::move(a)};
   a= std::move(b);
   b= std::move(tmp);
 }

// ===============================================================
 void erase(const k_type& x) noexcept {
   
   auto to_remove = _find(x);//ptr to node
   auto e = end();
   bool left;
   if (to_remove == nullptr){
     std::cout << "no such value is present" << std::endl;
     return;}
   //auto tmp = to_remove;
   
   while(to_remove->left || to_remove->right){
   //finché ha figli, vai a destra. Se non ha il nodo di destra, ma quello di sinistra, vai a sinistra
     if (to_remove->right){
       auto to_move = to_remove->right.get();
       //swap pair
       _swap(to_move, to_remove);
       //change node to point
       to_remove=std::move(to_move);
       left =0;
       }
     else{ 
       auto to_move = to_remove->left.get();
       //swap pair
       _swap(to_move, to_remove);
       //change node to point
       to_remove=std::move(to_move);
       left =1;
       }
   }
   if(left)
     to_remove->parent->left.reset();
   else
     to_remove->parent->right.reset();

   _size = _size-1;










   
 }

// ===============================================================
//   reverse print
// ===============================================================
  
/*
  void reverse_print() noexcept {

    auto it = last();
    for(std::size_t i{0}; i<_size; i++){
      std::cout << (*it).pair.first << " ";
      it--;
    }
   std::cout << std::endl;

  }

*/
// ===============================================================
//   overloading
// ===============================================================
  
  //subscripting operator
  v_type& operator[](const k_type& x) noexcept  {
    auto it = find(x);
    auto e = end();
    //if ( it == end()){
    if ( it == e){
      v_type v = "inserted new value (iwso)";// inserted with subscripting operator
      emplace(x,v);
      it = find(x);
    }
    return it->second;
  }

// ===============================================================
  
  v_type& operator[](k_type& x) noexcept  {
    auto it = find(x);
    auto e = end();
    if ( it == e){
      v_type v = "inserted new value (iwso)";// inserted with subscripting operator
      emplace(x,v);
      it = find(x);
    }
    return it->second;
  }
  
// ===============================================================
  friend
  // put-to operator
  std::ostream &operator<<(std::ostream &os, const bst& b) noexcept {
    if (!b.head){
      os << " ";
      return os;}
    for (const auto &x : b)
      os << x.first << " ";
    return os;
  }
};
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



// ===============================================================
//   iterator
// ===============================================================

  template <typename k_type, typename v_type,
            typename OP >
  template <typename O> 
  class bst<k_type,v_type,OP>::_iterator
  {
  using node = typename bst<k_type,v_type,OP>::node;
  node *current; // using node allows avoid typename (s 45)

  public:
    using value_type = O;// std::pair<O,T>; // O is a std::pair // 
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t; 
    using iterator_category = std::bidirectional_iterator_tag;   //forward_iterator_tag;
    // iterator ctr
    explicit _iterator(node *p) : current{p} {}
  
    // const refers to itr itselft -> the status is not changed
    reference operator*() const { return current->pair; }// when dereference I shall return pair
    pointer operator->() const { return &**this; }
    
    // pre-increment
    _iterator &operator++() {
      
      // check if there are sub-right trees
      if (current->right){
        
	current = current->right.get();
	while(current->left){
	  current = current->left.get();
        }

      } else {// go up and use the parent only if coming from right
        
	auto tmp_p = current->parent;
	
	if (!tmp_p){//if node is null ptr the end is reached and must return 
	  current = tmp_p;
	  return *this;
	  }
	  
        

	while (tmp_p->parent && tmp_p->right.get() == current) {
	  current = tmp_p;
	  tmp_p = tmp_p->parent;
	  }
	if( tmp_p->right.get() == current) 
	  tmp_p = tmp_p->parent;
	
	current = tmp_p;
      }
      return *this;
    }
    
    // post-increment
    _iterator operator++(int) {
      auto tmp{*this};
      ++(*this);
      return tmp;
    }


    _iterator &operator--() {
      
      // check if there are sub-right trees
      if (current->left){
        
	current = current->left.get();
	while(current->right){
	  current = current->right.get();
        }

      } else {// go up and use the parent only if coming from right
        
	auto tmp_p = current->parent;
	
	if (!tmp_p){//if node is null ptr the end is reached and must return 
	  current = tmp_p;
	  return *this;
	  }
	  
        

	while (tmp_p->parent && tmp_p->left.get() == current) {
	  current = tmp_p;
	  tmp_p = tmp_p->parent;
	  }
	if( tmp_p->left.get() == current) 
	  tmp_p = tmp_p->parent;
	
	current = tmp_p;
      }
      return *this;
    }

    _iterator operator--(int) {
      auto tmp{*this};
      --(*this);
      return tmp;
    }
    // fine because comparing the pointers, not the keys
    friend bool operator==(_iterator &a, _iterator &b) {
      return (a.current == b.current);
    }
    friend bool operator==(_iterator&& a, _iterator&& b) {
      return (a.current == b.current);
    }
    friend bool operator!=(_iterator &a, _iterator &b) { 
      return !(a == b);
    }
};

// ===============================================================
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   main
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ===============================================================

int main(){
 
  std::cout << "       _ _\n" ;
  std::cout << "    /~~   ~~\\           Code for Binary Search Trees \n" ;
  std::cout << " /~~         ~~\\          \n";
  std::cout << "{    ~~         }               by M.Danese\n";
  std::cout << " \\__     ~~  __/\n";
  std::cout << "    ~~\\   /~~  \n";
  std::cout << "       | |     \n";
  std::cout << "       | |    \n" ;
  std::cout << "  ,,,,/   \\,,,, " << std::endl; 
     
  std::cout << "=================================================================\n" ;

  std::pair<int, std::string> p1{1,"c"};
  p1.second = "c";
  
  // 
  
  
  std::cout << "\nAs a starting point, the following tree is created \nand printed step by step" << std::endl; 
  std::cout << "        8          \n"; 
  std::cout << "      /   \\       \n"; 
  std::cout << "     3     10      \n";
  std::cout << "    / \\      \\   \n";
  std::cout << "   1   6      14   \n";
  std::cout << "      / \\    /    \n";
  std::cout << "     4   7  13     \n";
  
  
  
  std::cout << "\nCreating a BST, b.\n";
  bst<int,std::string> b;
  p1.first = 8;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 3;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 10;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 1;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 6;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 4;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 7;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 14;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  p1.first = 13;
  std::cout << "\n--- inserting "<< p1.first <<" ---"  << std::endl;   
  b.insert(p1);
  std::cout << "\nprinting b"  << std::endl;   
  std::cout << b << std::endl;
  std::cout << "=================================================================\n" ;
  std::cout << "\nTo show the property of the BST class \nin the following we exploit some options and modifications:\n";

  std::cout << "\ninserting 100,h with emplace, the result is:\n";
  b.emplace(100,"h");
  std::cout << b << std::endl;
  std::cout << "\nprinting value of b[25], the result is:\n";
  std::cout << b[25] << std::endl;
  std::cout << b << std::endl;
  std::cout << "\nprinting value of b[100], the result is:\n";
  std::cout << b[100] << std::endl;
  
  std::cout << "\nerasing 7, the result is:\n";
  b.erase(7);
  std::cout << b << std::endl;
  std::cout << "\nseraching for 13:\n";
  auto result = b.find(13);
  auto bend = b.end();
  (result != bend)
     ? std::cout << "the BST contains the searched number\n"
     : std::cout << "the BST does not contain the searched number\n"; 
  
  std::cout << "\nseraching for 98:\n";
  result = b.find(98);
  (result != bend)
     ? std::cout << "the BST contains the searched number\n"
     : std::cout << "the BST does not contain the searched number\n"; 
  
  std::cout << "\nbalancing the tree, the result is:\n";
  b.balance();
  std::cout << b << std::endl;
  
  //std::cout << "\nprinting the tree in reversed order, the result is:\n";
  //b.reverse_print();
 
  std::cout << "\n -------- \n";
  std::cout << "\nNow the tree is copied into a new one, b1. The result is:\n";
  bst<int,std::string> b1{b};
  std::cout << b1 << std::endl;
  
  std::cout << "\nadding key 17 to b1 with the overloaded operator, the result is:\n";
  std::cout << b1[17] << std::endl;
  std::cout << b1 << std::endl;
  
  std::cout << "\nWhile b is left unchanged:\n";
  std::cout << b << std::endl;
  b.clear();
  
  std::cout << "\nNow b1 is moved into b2. The result is:\n";
  bst<int,std::string> b2{std::move(b1)};
  std::cout << b2 << std::endl;
  std::cout << "\nwhile b1 is:\n";
  std::cout << b1 << std::endl;
  
  std::cout << "\nFinally b2 is cleared:\n";
  b2.clear();
  std::cout << b2 << std::endl;
  
  return 0;
}
