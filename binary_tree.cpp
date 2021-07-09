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
    using pair_type = std::pair<const k_type, v_type>;
    pair_type pair;
    std::unique_ptr<_node> right;
    std::unique_ptr<_node> left;
    _node* parent=nullptr;
    
    _node() noexcept = default; 
    ~_node() noexcept = default;
    
    explicit _node(const pair_type& p) 
      : pair{p} {}
    
    explicit _node (pair_type&& p) noexcept 
      : pair{std::move(p)} {}
    
    _node(const pair_type& p, _node* nptr) noexcept 
      : pair{p} {
      if (nptr->left){
        left.reset(new _node(nptr->left->pair, nptr->left.get()));
	left->parent = this;
	}
      if (nptr->right){
        right.reset(new _node(nptr->right->pair, nptr->right.get()));
	right->parent = this;
        }
      }
  };

// ===============================================================
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   iterator
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ===============================================================

  //oppure node_type e no linea 64
  template <typename k_type, typename v_type, typename O> 
  class _iterator
  {
  using node = _node<k_type,v_type>;
  node *current; // using node allows avoid typename (s 45)

  public:
    using value_type = O;// std::pair<O,T>; // O is a std::pair // 
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t; 
    using iterator_category = std::bidirectional_iterator_tag;   //forward_iterator_tag;
    // iterator ctr
    explicit _iterator(node *p) noexcept: current{p} {}
  
    // const refers to itr itselft -> the status is not changed
    reference operator*() const noexcept { return current->pair; }// when dereference I shall return pair
    pointer operator->() const noexcept{ return &**this; }
    
    // pre-increment
    _iterator &operator++() noexcept{
      
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
	  
        

	while (tmp_p->parent && (tmp_p->right.get() == current)) {
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
	  
        

	while (tmp_p->parent && (tmp_p->left.get() == current)) {
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
    friend bool operator==(const _iterator &a, const _iterator &b) {
      return (a.current == b.current);
    }
    friend bool operator!=(const _iterator &a, const _iterator &b) { 
      return !(a == b); /// !!! era forward
    }
};

// ===============================================================
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   bst
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ===============================================================
template <typename k_type, typename v_type,
          typename OP = std::less<k_type> >
class bst{

  using node = _node<k_type,v_type>;//const è effettivamente ripetitivo
  //using node = _node<k_type,v_type>
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

  using iterator = _iterator<k_type, v_type, typename node::pair_type>;//<node>;
  //using iterator = _iterator<std::pair<k_type,v_type>>;//<node>;
  using const_iterator = _iterator<k_type, v_type, const typename node::pair_type>;//<const node>;
  //using const_iterator = _iterator<const std::pair<k_type,v_type>>;//<const node>;

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
 
  std::pair<iterator, bool> insert(const typename node::pair_type& x) {return _insert(x);}
  std::pair<iterator, bool> insert(typename node::pair_type&& x) {return _insert(std::move(x));}
  

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
        delete _node;
        return std::pair<iterator,bool>{ end() , found };  
      }
    }
    
    found = 0;
    _size--;
    delete _node;
    return std::pair<iterator,bool>{ end() , found };  
  }

// ===============================================================
//   move and copy ctrs and assignments
// ===============================================================

  bst(bst&&) noexcept = default;
  bst& operator=(bst&&) noexcept = default;


  bst(const bst& b){
    if (b.head) {
      auto _node = new node{b.head->pair, b.head.get()};
      head.reset(_node);
      _size = b._size;
      // alternativa -> != linked list. Convenienza?
     /* 
      for(const auto &x : b)
	_insert(x);
      */
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
  iterator find(const k_type& x) noexcept {   
    return iterator{_find(x)};  
  }

// ===============================================================
  const_iterator find(const k_type& x) const noexcept {   
    
    return const_iterator{_find(x)};
  }

// ===============================================================
  node* _find(const k_type& x) const noexcept {   
    
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

    std::vector<typename node::pair_type> tmp;
    for (const auto &x : *this)
      tmp.push_back(x);
    
    head.reset(nullptr);

    _size =0;
    _balance(tmp, 0, tmp.size());
    return;
  }

// ===============================================================
  
  void _balance(std::vector<typename node::pair_type>& tmp, std::size_t sidx, std::size_t eidx) noexcept {
    
    _insert(tmp[(eidx+sidx)/2]);
    
    if ((eidx+sidx)/2 != sidx)
      _balance(tmp, sidx, (eidx+sidx)/2);
    if ((eidx+sidx)/2+1 != eidx)
      _balance(tmp, (eidx+sidx)/2+1, eidx);
  
  }
    
// ===============================================================
//   erase
// ===============================================================
 

  void erase(const k_type& x) noexcept {
    auto to_remove = _find(x);//ptr to node
    bool left=0;
    std::vector<typename node::pair_type> tmp;
    auto it = iterator(to_remove);
    auto e = end();
   
    if (it == end()){
      std::cout << "the value is not present" << std::endl;
      return;}

    //head
    if (to_remove == head.get()){
      ++it;
      while (it != e){
	tmp.push_back(*it);
        ++it;}
      
      head.reset(head->left.release());
      head->parent=nullptr;
      for (auto &&x : tmp)
        _insert(x);
      return;
    }


    // if not the head
    if(to_remove->parent->left.get() == to_remove)
      left=1;

    if (left){
      ++it;
      //append the whole right branch of node to_remove to a std::vector
      //the right branch will be identified as the one between to_remove 
      //and its parents bc of bst properties
      //while (it != iterator(to_remove->parent)){
      while (op(it->first, to_remove->parent->pair.first) && to_remove->right){
	tmp.push_back(*it);
        ++it;}
    }
    else {
      --it;
      //append the whole left branch of node to_remove to a std::vector
      //the left branch will be identified as the one between to_remove 
      //and its parents bc of bst properties
      while (op(to_remove->parent->pair.first, it->first) && to_remove->left){
        tmp.push_back(*it);
        --it;}
    }


    if(left) {//(to_remove->parent->left.get() == to_remove)//if to remove is left child
      auto par = to_remove->parent;
      par->left.reset(to_remove->left.release());
      if(par->left)//if it did not become a null ptr
        par->left->parent = par;
      }
    else {//if to remove is right child
      // use par to avoid dangling pointers
      auto par = to_remove->parent;
      par->right.reset(to_remove->right.release());
      if(par->right)//if it did not become a null ptr
        par->right->parent = par;
    }

    for (auto &&x : tmp)
      _insert(std::move(x));
  }
 /*

 void erase(const k_type& x) noexcept {
   
   auto to_remove = _find(x);//ptr to node
   auto to_move = to_remove;
   bool left;
   if (to_remove == nullptr){
     std::cout << "no such value is present" << std::endl;
     return;}
   //auto tmp = to_remove;
   
   while(to_remove->left || to_remove->right){
   //finché ha figli, vai a destra. Se non ha il nodo di destra, ma quello di sinistra, vai a sinistra
     if (to_remove->right){
       to_move = to_remove->right.get();
       //swap pair
       _swap(to_move, to_remove);
       //change node to point
       to_remove = to_move;
       left =0;
       }
     else{ 
       to_move = to_remove->left.get();
       //swap pair
       _swap(to_move, to_remove);
       //change node to point
       to_remove = to_move;
       left =1;
       }
   }
   if(to_remove->parent->left.get() == to_remove)
     to_remove->parent->left.reset();
   else
     to_remove->parent->right.reset();

   _size = _size-1;

   
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
  //b.erase(6);
  //b.erase(14);
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
/*
*/  
  return 0;
}
