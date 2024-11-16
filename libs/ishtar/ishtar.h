/// Ishtar v1.0 - A single-file suite of tools for C++.
/// 
/// To include the library in your project, copy this file (ishtar.h) into the directory where you usually 
/// keep your libraries, make a new translation unit (ishtar.cpp, for example), use `#define ISHTAR_IMPL` before the include statement, 
/// and make sure to compile that translation unit with your project. 
/// 
/// For example: 
///
/// ```c++
/// #define ISHTAR_IMPL
/// #include "ishtar.h"
///
/// // ... 
/// ```
/// 
/// If you have any inquires or potential bugs, please contact me: https://frodoalaska.github.io/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace ishtar { // Start of ishtar

////////////////////////////////////////////////////////////////////

/// Typedefs 

/// char 
typedef char   i8; 

/// short
typedef short  i16;

/// int
typedef int    i32; 

/// long
typedef long   i64; 

/// unsigned char
typedef unsigned char  u8;

/// unsigned short
typedef unsigned short u16;

/// unsigned int
typedef unsigned int   u32;

/// unsigned long
typedef unsigned long  u64;

/// size_t
typedef size_t sizei;

/// Typedefs 

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// Function pointers 

/// Function for iterating over `LinkedList`, `Queue`, and `Stack`
template<typename T>
using ListForEachFn = void(*)(T& value);

/// Function for iterating over `DynamicArray` and `String`
template<typename T>
using ArrayForEachFn = void(*)(T& value, const sizei index);

/// Function for iterating over a `HashTable`
template<typename K, typename V>
using TableForEachFn = void(*)(const K& key, V& value);

/// Hashing function prototype
template<typename K>
using HashFn = const u64(*)(const K& key);

/// Allocation function prototype 
template<typename T>
using AllocFn = T*(*)(const sizei count, const sizei element_size);

/// Free function prototype 
template<typename T>
using FreeFn = void(*)(T* ptr);

/// Function pointers 

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// Defines 

/// Underflowing a `sizei` type to get a "non position" which can indicate 
/// a string error
#define STRING_NPOS            ((sizei)-1)

/// `HashTable` uses this to determine to grow its size 
#define HASH_TABLE_LOAD_FACTOR 0.7

/// Defines 

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// Node 
/// A generic node class of type `T`
template<typename T>
class Node {
  public:
    /// Default CTOR
    Node() = default;

    /// Assigns `val` to the node and as well as `next` and `previous`
    Node(const T& val, Node<T>* next = nullptr, Node<T>* previous = nullptr) {
      value          = val; 
      this->next     = next;
      this->previous = previous;
    }

  public:
    T value; 
    Node<T>* next;
    Node<T>* previous;
};
/// Node

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// LinedList 
template<typename T> 
class LinkedList {
  public:

    /// Takes in the given `head` as the start of the list
    LinkedList(Node<T>* head = nullptr) {
      this->head = head; 
      tail       = nullptr;
      count      = head != nullptr ? 1 : 0;
    }
   
    /// Will assign `val` to the head of the list
    LinkedList(const T& val) {
      head  = new Node<T>(val); 
      tail  = nullptr;
      count = 1;
    }

  public:
    Node<T>* head; 
    Node<T>* tail;
    sizei    count;

  public:
    /// Retrieves node at the given `index`. 
    /// NOTE: This function will assert if `index` is out of range
    Node<T>* get_at(const sizei index) {
      assert(index >= 0 && index < count);

      Node<T>* node = head;
      for(sizei i = 0; node && i < count; i++) {
        if(i == index) {
          return node;
        }

        node = node->next;
      }

      return nullptr;
    }

    /// Add the given `node` to the tail of the list 
    /// NOTE: If the given `node` is `nullptr` this function will do nothing
    void append(Node<T>* node) {
      // Not a valid node in the first place
      if(!node) {
        return;
      }
     
      // Still need to make sure the head is valid 
      if(!head) {
        prepend(node);
      }
      // Place as tail if there is no tail 
      else if(!tail) {
        node->previous = head; 
        node->next     = nullptr;
        tail           = node; 
        head->next     = tail;
      }
      // Otherwise, it's a normal node and append it to the tail 
      else {
        node->next     = nullptr; 
        node->previous = tail;
        tail->next     = node;

        // The new node is now the tail
        tail = node;
      }

      // New node added so increase the count
      count++;
    }

    /// Add the given `node` to the head of the list. 
    /// NOTE: If the given `node` is `nullptr` this function will do nothing
    void prepend(Node<T>* node) {
      // Not a valid node in the first place
      if(!node) {
        return;
      }

      // The node becomes the new head if no head exists 
      if(!head) {
        head           = node;
        head->next     = tail;
        head->previous = nullptr;
      }
      // Just a plain simple prepend operation
      else {
        node->next     = head;
        node->previous = nullptr;
        head->previous = node; 

        // The node now becomes the new head 
        head = node;
      }
      
      // New node added so increase the count
      count++;
    }
   
    /// Insert the given `node` between `prev` and `next`
    /// NOTE: If the given `node` is `nullptr` this function will do nothing
    void insert(Node<T>* node, Node<T>* prev, Node<T>* next) {
      // Not a valid node in the first place
      if(!node) {
        return;
      }
      
      // Placing the node in between the `next` and `prev`
      node->previous = prev;
      node->next     = next; 

      // Relinking the nodes
      next->previous = node;
      prev->next     = node;

      // New node added so increase the count
      count++;
    }

    /// Remove the given `node` from the list completely. 
    /// NOTE: This function _will_ de-allocate the given node 
    void remove(Node<T>* node) {
      // Not a valid node in the first place or there's just nothing to remove
      if(!node || count == 0) {
        return;
      }
    
      // Relinking the nodes 
      if(node->next) {
        node->next->previous = node->previous;
      }

      if(node->previous) {
        node->previous->next = node->next;
      }

      // Some memory cleanup 
      free(node);
      
      // A node was removed so decrease the count (but don't go below zero)
      count -= count == 0 ? 0 : 1;
    }

    /// Remove the node at the given `index` from the list completely. 
    /// NOTE: This function _will_ de-allocate the given node 
    void remove_at(const sizei index) {
      // Must be a valid index
      assert(index >= 0 && index < count);
    
      // Just pop the head
      if(index == 0) {
        Node<T>* popped = pop_front();
        remove(popped);

        return;
      }
      // Just pop the tail 
      else if(index == (count - 1)) {
        Node<T>* popped = pop_back();
        remove(popped);

        return;
      }

      // Otherwise, just remove the node
      Node<T>* node = get_at(index);
      remove(node);
    }

    /// Prepend a new node with the value `val` to the front of the list 
    /// NOTE: This function will allocate `Node<T>` 
    void emplace_front(const T& val) {
      Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
      node->value   = val;

      prepend(node);
    }

    /// Append a new node with the value `val` to the back of the list 
    /// NOTE: This function will allocate `Node<T>` 
    void emplace_back(const T& val) {
      Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
      node->value   = val;

      append(node);
    }

    /// Insert a new node with value of `val` at the given `index`.   
    /// NOTE: This function will allocate `Node<T>`. 
    /// NOTE: This function will also assert if the `index` is out of range.
    void emplace_at(const T& val, const sizei index) {
      // Can't insert past the end 
      assert(index >= 0 && index <= count);
      
      Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
      node->value   = val;

      if(index == 0) {
        prepend(node);
        return;
      }
      else if(index == count) {
        append(node);
        return;
      }

      // Otherwise, find the node at `index` and insert 
      Node<T>* curr = get_at(index);
      insert(node, curr->previous, curr);
    }

    /// Remove and return a valid node at the head of the list 
    Node<T>* pop_front() {
      // No head exists in the first place
      if(!head) {
        return nullptr;
      }
      
      // A node was removed so decrease the count (but don't go below zero)
      count -= count == 0 ? 0 : 1;

      // Relocating the head
      Node<T>* old_head        = head;
      old_head->next->previous = nullptr;
      head                     = old_head->next;

      return old_head;
    }
    
    /// Remove and return a valid node at the tail of the list 
    Node<T>* pop_back() {
      // No tail exists in the first place
      if(!tail) {
        return nullptr;
      } 
      
      // A node was removed so decrease the count (but don't go below zero)
      count -= count == 0 ? 0 : 1;

      // Relocating the tail 
      Node<T>* old_tail        = tail; 
      old_tail->previous->next = nullptr; 
      tail                     = old_tail->previous;

      return old_tail;
    }

    /// Return the value of the node at the given `index`
    const T& peek_at(const sizei index) {
      return get_at(index)->value; 
    }
   
    /// De-allocate each node from the list completely
    void clear() {
      Node<T>* node = head;

      // Clearing all the memory of each node 
      while(node != nullptr) {
        // Moving the loop forwards
        Node<T>* old_node  = node; 
        node               = node->next;
        
        // Deleting the node from existence
        delete old_node;

        // One less node to worry about...
        count--;
      }
    }

    /// Call the given `func` with each node in the list
    void for_each(const ListForEachFn<T>& func) {
      if(!func) {
        return;
      }

      for(auto node = head; node != nullptr; node = node->next) {
        func(node->value);
      }
    }
};
/// LinedList

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// Queue 
/// A simple queue FIFO structure with nodes of type `T`
template<typename T>
class Queue {
  public:
    /// Default CTOR
    Queue() 
      :count(0), head(nullptr), tail(nullptr)
    {}
  
  public:
    sizei count;
    Node<T>* head;
    Node<T>* tail;
   
    /// Add `node` to the queue. 
    /// NOTE: This function will do nothing if `node` is a `nullptr`.
    void push(Node<T>* node) {
      // The given node is invalid 
      if(!node) {
        return;
      }

      // This is possibly the first element in the queue 
      if(!head) {
        head = node;
        head->next = nullptr;
      }
      // Place as tail if there is no tail 
      else if(!tail) {
        node->next     = nullptr;
        tail           = node; 
        head->next     = tail;
      }
      // Otherwise, it's a normal node and push it to the queue 
      else {
        tail->next     = node;
        node->next     = nullptr; 

        // The new node is now the tail
        tail = node;
      }

      // New node added so increase the count
      count++;
    }

    /// Remove and return the current head node of the queue.
    /// NOTE: This function will return a `nullptr` if the queue is empty
    Node<T>* pop() {
      // There's nothing in the queue 
      if(!head) {
        return nullptr;
      }
      
      // Don't go below zero when popping 
      count -= count == 0 ? 0 : 1;
    
      // Relocating the head and popping the old head
      Node<T>* old_head = head; 
      head              = head->next; 

      return old_head;
    }

    /// Push a new node with the value of `val` to the queue.
    /// NOTE: This function will allocate memory.
    void emplace(const T& val) {
      Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
      node->value   = val;

      push(node);
    }

    /// Iterate through each element in the queue and call the given `func` function.
    void for_each(const ListForEachFn<T>& func) {
      if(!func) {
        return;
      }

      for(auto node = head; node != nullptr; node = node->next) {
        func(node->value);
      }
    }
};
/// Queue

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// Stack 
/// A simple stack LIFO structure with nodes of type `T`
template<typename T>
class Stack {
  public:
    /// Default CTOR
    Stack() 
      :count(0), head(nullptr)
    {}
  
  public:
    sizei count;
    Node<T>* head;
   
    /// Push `node` to the back of the stack
    void push(Node<T>* node) {
      if(!node) {
        return;
      }

      // This is possibly the first element in the stack 
      if(!head) {
        head       = node;
        head->next = nullptr;
      }
      // Otherwise, it's a normal node and push it to the stack 
      else {
        node->next = head;
        head       = node;
      }

      // New node added so increase the count
      count++;
    }

    /// Remove and return the node at the back of the stack. 
    /// NODE: This function will return `nullptr` if the stack is empty.
    Node<T>* pop() {
      // There's nothing in the stack 
      if(count == 0) {
        return nullptr;
      }

      // Don't go below zero when popping 
      count -= count == 0 ? 0 : 1;
    
      // Relocating the head and popping the old head
      Node<T>* old_head = head; 
      head              = head->next;
      old_head->next    = nullptr;

      return old_head;
    }

    /// Push a new node with the value of `val` to the stack.
    /// NOTE: This function will allocate memory.
    void emplace(const T& val) {
      Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
      node->value   = val;

      push(node);
    }

    /// Iterate through each node in the stack and call the given `func` function.
    void for_each(const ListForEachFn<T>& func) {
      if(!func) {
        return;
      }

      for(auto node = head; node != nullptr; node = node->next) {
        func(node);
      }
    }
};
/// Stack

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// DynamicArray 
/// A dynamically resizable array with each element being of type `T`
template<typename T> 
class DynamicArray {
  public:

    /// Default CTOR. 
    /// NOTE: This CTOR will have a `capacity` of 5.
    DynamicArray() {
      capacity = 5; 
      size     = 0;
      data     = (T*)malloc(sizeof(T) * capacity);
    }

    /// Set the `capacity` member of the dynamic array to `initial_capacity`
    DynamicArray(const sizei initial_capacity) {
      capacity = initial_capacity; 
      size     = 0;
      data     = (T*)malloc(sizeof(T) * capacity);
    }
   
    /// Set the given `dat` with size of `dat_size` to the member `data` and `size`.
    /// NOTE: The `capacity` will be: `dat_size + (dat_size / 2)`.
    DynamicArray(T* dat, const sizei dat_size) {
      size     = dat_size; 
      capacity = size + (size / 2);
      data     = dat;
    }

  public:
    T* data;
    sizei capacity, size;

  public:
    /// Make room for `new_capacity` amount of elements in the array. 
    /// This will not add new elements in the array. It will only allocate enough 
    /// memory for `new_capacity` of elements to be added.
    /// NOTE: Keep in mind that if the given `new_capacity` is _smaller_ than
    /// the current `capacity`, the array will _shrink_ in size, naturally. 
    /// However, if the opposite is true, than the array will _bloat_ its size.
    void reserve(const sizei new_capacity) {
      // There's nothing to be done when the given `new_capacity` is 
      // the _same_ as the current `capacity`
      if(new_capacity == capacity) {
        return;
      }

      capacity = new_capacity;
      data     = (T*)realloc(data, sizeof(T) * capacity); 
    
      // Assert just in case the system runs out of memory 
      assert(data != nullptr);
    }

    /// Resize the array by `new_size`. This will add new elements to the array.
    /// NOTE: This function will call `reserve` if the `new_size` is greater than `capacity`.
    void resize(const sizei new_size) {
      sizei old_size = size;
      size = new_size;

      // Grow the array by half the new size if need be
      if(size >= capacity) {
        reserve(capacity + (size / 2));
      }

      // We set _only_ the new elements in the array to a default value. 
      // This way, we won't need to touch the already-existing values.
      data = memset(data + (sizeof(T) * old_size), 0, sizeof(T) * new_size);
    }

    /// Add a new element with a value of `val` to the array. 
    /// NOTE: This function will call `reserve` if the array has no more space for elements
    void append(const T& val) {
      size++;

      // Grow the array if need be
      if(size >= capacity) {
        reserve(capacity + (size / 2));
      }

      // Add the new value to the array
      data[size - 1] = val;
    }

    /// Remove and return the last added element in the array.
    T& pop_back() {
      // There's nothing in the array in the first place 
      if(size == 0) {
        return data[size];
      }

      // We pop the element from the back and ignore it for now
      T& element = data[size - 1];
      size--;

      return element;
    }

    /// Remove and return the first element in the array.
    T& pop_front() {
      T& element = data[0];

      // Reshuffle the entire array 
      for(sizei i = 1; i < size; i++) {
        data[i - 1] = data[i];
      }

      size--;
      return element;
    }

    /// Check the value in the beginning of the array.
    const T& peek_front() {
      return data[0];
    }
   
    /// Check the value at the end of the array.
    const T& peek_back() {
      return data[size - 1];
    }

    /// Completely remove the element at `index` from the array. 
    /// NOTE: This function will not do any memory de-allocations. 
    void remove(const sizei index) {
      for(sizei i = index; i < size; i++) {
        data[i] = data[i + 1]; 
      } 

      size--;
    }

    /// Create a new `DynamicArray` reaching from `begin` till `end` elements. 
    /// Both `begin` and `end` are inclusive.
    /// NOTE: This function will allocate memory and will create a copy.
    DynamicArray<T> slice(const sizei begin, const sizei end) {
      sizei new_data_size = (end - begin) + 1; // Make sure that `end` is _inclusive_
      T* new_data = (T*)malloc(sizeof(T) * new_data_size);

      // Copying over the correct data 
      for(sizei i = begin, j = 0; i < new_data_size || j < new_data_size; i++, j++) {
        new_data[j] = at(i);
      }

      return DynamicArray<T>(new_data, new_data_size);
    }

    /// Fill `count` amount of elements with values of `value`.
    void fill(const sizei count, const T& value) {
      for(sizei i = 0; i < count; i++) {
        append(value); 
      }
    }

    /// Free the underlying array buffer and reset the `capacity` and `size` members.
    void clear() {
      free(data);

      size     = 0; 
      capacity = 0;
    }
   
    /// Retrieve element at `index`. 
    /// NOTE: This function will assert if the `index` is out of bounds.
    const T& at(const sizei index) {
      // Index out of bounds
      assert((index >= 0 && index < size));

      // Otherwise, just return the value 
      return data[index];
    }

    /// Iterate through each element in the array and call the function `func`.
    void for_each(const ArrayForEachFn<T>& func) {
      if(!func) {
        return;
      }

      for(sizei i = 0; i < size; i++) {
        func(data[i], i);
      }
    }

    // Operator overloading for []
    const T& operator[](const sizei index) {
      return at(index);
    }

    const T& operator[](const sizei index) const {
      return at(index);
    }
};
/// DynamicArray

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////    

/// String
// ASCII string class 
class String {
  public:
    /// Default CTOR
    String() = default;

    /// Copy CTORs
    String(const String& other);
    String(String&& other);

    /// Make a `String` out of the given `str`
    String(const char* str);

    /// Take in the given `str` with the length `str_len` and create a new `String`.
    String(const char* str, const sizei str_len);

    ~String();

  public:
    sizei length = 0; 
    char* data      = nullptr;

  public: 
    /// Retrieve `char` at `index`. 
    /// NOTE: This function will assert if the `index` is out of bounds.
    const char& at(const sizei index) const {
      assert(index >= 0 && index < length);
      return data[index]; 
    }

    char& operator[](const sizei index) {
      return (char&)at(index); // Remove const because I'm mean
    }

    const char& operator[](const sizei index) const {
      return at(index); 
    }

    String& operator=(const String& other) {
      copy(other);
      return *this;
    } 
    
    const bool operator==(const String& other) {
      return compare(other);
    } 
    
    const bool operator!=(const String& other) {
      return !compare(other);
    } 

    /// Copy the given `str` with the length of `str_len`. 
    void copy(const char* str, const sizei str_len);
   
    /// Copy the given `String` `str`.
    void copy(const String& str);
   
    /// Copy the given C-string `str`.
    void copy(const char* str);
   
    /// Return `true` if the `String` is empty
    const bool is_empty();
   
    /// Add the given `other` `String` to the current `String`
    void append(const String& other);
   
    /// Add the given `other` C-string to the current `String`
    void append(const char* other);
   
    /// Add the given `ch` `char` to the end of the current `String`.
    void append(const char& ch);
   
    /// Add the given `ch` `char` at `index`. 
    void append_at(const sizei index, const char& ch);
   
    /// Create a new `String` with the contents of the current `String` 
    /// starting from `begin` till `end`. 
    /// NOTE: The given `begin` is inclusive while `end` is exclusive.
    String slice(const sizei begin, const sizei end);
   
    /// Return `true` the current `String` matches the `other` `String`.
    const bool compare(const String& other);
   
    /// Completely reverse the contents of the current `String`.
    /// NOTE: This will allocate a temporary buffer and free it. 
    void reverse();
   
    /// Starting at `start`, go through the `String` and try to find `ch`.
    /// Return the index of `ch` if it was found. 
    /// Otherwise, return `STRING_NPOS`.
    const sizei find(const char& ch, const sizei start = 0);
   
    /// Start at the end of the `String` and find the last instance of `ch`.
    const sizei find_last_of(const char& ch);
   
    /// Start at the start of the `String` and find the first instance of `ch`.
    const sizei find_first_of(const char& ch);
   
    /// Completely remove all of the `char`s starting from `begin` till `end`. 
    /// NOTE: The given `begin` is inclusive while `end` is exclusive.
    void remove(const sizei begin, const sizei end);

    /// Equivalent to `String[index] = ch`. 
    void replace_at(const sizei index, const char& ch);
   
    /// Go through the entire `String`, find `ch1`, and replace it with `ch2`.
    /// NOTE: If `ch1` was not found, this function will do nothing.
    void replace(const char& ch1, const char& ch2);
   
    /// Replace every instance of `ch1` in the `String` with `ch2`.
    /// NOTE: If `ch1` was not found, this function will do nothing.
    void replace_all_of(const char& ch1, const char& ch2);
    
    /// Return `true` if `ch` was found in the current `String`.
    const bool has(const char& ch);
    
    /// Return `true` if `ch` was found at exactly `index`.
    const bool has_at(const sizei index, const char& ch);
   
    /// Convert the `String` into a C-string format.
    const char* c_str() const;
   
    /// Fill `len` amount of elements with `ch`.
    void fill(const sizei len, const char& ch);
   
    /// Iterate over each `char` in the `String` and call the given 
    /// `func` function.
    void for_each(const ArrayForEachFn<char>& func);

    /// Clear the underlying buffer and reset the string.
    /// NOTE: This function does not de-allocate memory. It only resets the buffer to `0`
    void clear();

  private:
    const sizei string_length(const char* str) {
      return strlen(str);
    } 
};
/// String

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// HashTable functions

/// Return a scrambled (hashed) `u64` number from `str`. 
const u64 hash_key(const char* str);

/// Return a scrambled (hashed) `u64` number from `str`. 
const u64 hash_key(const String& str);

/// Return a scrambled (hashed) `u64` number from `key` of type `T`. 
template<typename K> 
const u64 hash_key(const K& key) {
  u32 hash = 2166136261u;

  hash ^= key;
  hash *= 1677719;

  return hash;
}

/// HashTable functions 

/// HashTable
/// A hash table with key of type `K` and value of type `V`.
template<typename K, typename V> 
class HashTable {
  public:
    /// Default CTOR.
    /// Starts off with a `capacity` of 5.
    HashTable() 
      :size(0), capacity(5), entries(new TableEntry*[capacity]), hash_fn(hash_key)
    {}

    /// Copy CTORs.
    HashTable(const HashTable&) = default;
    HashTable(HashTable&&) = default;
   
    /// Create a new `HashTable` with the `capacity` memeber set to `initial_capacity`, using 
    /// the `hash_fn` to hash each entry's key.
    HashTable(const sizei initial_capacity, const HashFn<K>& hash_fn = hash_key) {
      size            = 0; 
      capacity        = initial_capacity;
      entries         = new TableEntry*[capacity]; 
      this->hash_fn   = hash_fn;
    }

  private:
    struct TableEntry {
      K key; 
      V value; 
      TableEntry* next = nullptr;
      sizei index = 0;
    };

  public:
    sizei size; 
    sizei capacity;
    TableEntry** entries = nullptr;
    HashFn<K> hash_fn;

  public:
    /// De-allocate any memory used by the `HashTable`.
    void clear() {
      if(!entries) {
        return;
      }

      for(sizei i = 0; i < capacity; i++) {
        while(entries[i]) {
          TableEntry* entry = entries[i];
          entries[i] = entry->next;

          delete entry;
        }
      }
      
      size     = 0;
      capacity = 0;
    }

    /// Set the value of `key` to `value`.
    /// NOTE: If an entry with a key `key` was not found in the `HashTable`, a new entry will be created.
    /// NOTE: This might grow the `HashTable` if there is not enough space.
    void set(const K& key, const V& value) {
      // One more element to worry about... 
      size++;

      // Grow the table when the load factor is reached 
      if(size >= (capacity * HASH_TABLE_LOAD_FACTOR)) {
        grow(capacity + (size / 2));
      }

      u64 hashed_key = hash_fn(key);
      sizei index = (hashed_key % capacity);

      TableEntry* entry = entries[index];

      if(!entry) {
        entries[index] = new TableEntry{key, value, nullptr, index};
        return;
      }

      while(entry->next) {
        entry = entry->next;
      }
      
      if(entry->key == key) {
        entry->value = value;
        size--;

        return;
      }
        
      entry->next = new TableEntry{key, value, nullptr, index};
    }

    /// Return the value of entry with the key `key`.
    /// NOTE: This function will add a new entry if an entry with key `key` was not found in the `HashTable`.
    V& get(const K& key) {
      TableEntry* entry = get_entry(key);
      if(!entry) {
        set(key, {});
        entry = get_entry(key);
      }

      return entry->value; 
    }

    /// Returns `true` if an entry with key `key` was found in the `HashTable`. Otherwise, returns `false`.
    const bool has(const K& key) {
      return get_entry(key) != nullptr;
    }

    /// Completely remove an entry with key `key` from the `HashTable`.
    /// NOTE: This function will de-allocate memory.
    void remove(const K& key) {
      TableEntry* entry = get_entry(key);
      if(!entry) {
        return;
      } 

      size--;

      entry = entries[entry->index];
      if(entry->key == key) {
        entries[entry->index] = entry->next;
        delete entry;

        return;
      }

      TableEntry* prev_entry = entry;
      while(entry && entry->key != key) {
        prev_entry = entry;
        entry      = entry->next;
      }

      prev_entry->next = entry->next;
      entry->next      = nullptr;
      delete entry;
    }

    /// Itereate through each entry in the `HashTable` and call the given `func` function.
    void for_each(const TableForEachFn<K, V>& func) {
      if(!func) {
        return;
      }

      for(sizei i = 0; i < capacity; i++) {
        TableEntry* entry = entries[i];

        while(entry) {
          func(entry->key, entry->value);
          entry = entry->next;
        } 
      }
    }

  private:
    TableEntry* get_entry(const K& key) {
      // Get the appropriate index
      u64 hashed_key = hash_fn(key);
      sizei index = (hashed_key % capacity);

      TableEntry* entry = entries[index];

      while(entry && entry->key != key) {
        entry = entry->next;
      }
      
      return entry;
    }

    void grow(const sizei cap) {
      TableEntry** temp_entries = new TableEntry*[cap];
      memcpy(temp_entries, entries, sizeof(TableEntry*) * capacity);

      capacity = cap;

      delete[] entries;
      entries = new TableEntry*[capacity];
      memcpy(entries, temp_entries, sizeof(TableEntry*) * capacity);

      delete[] temp_entries;
    }
};
/// HashTable 

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

/// ArenaAllocater functions

/// Allocate `count` amount of elements each with size of `element_size`.
template<typename T>
T* default_alloc(const sizei count, const sizei element_size) {
  return (T*)calloc(count, element_size);
}

/// De-allocate the given `ptr`.
template<typename T>
void default_free(T* ptr) {
  free(ptr); 
}

/// ArenaAllocator functions

/// ArenaAllocator 
// A generic arena allocator with each element's type being `T`. 
template<typename T> 
class ArenaAllocator {
  private:
    T* m_buffer           = nullptr;
    sizei m_element_size  = 0;
    sizei m_max_allocs    = 0;
    sizei m_total_alloc   = 0;
    sizei m_total_free    = 0;
    AllocFn<T> alloc_func = default_alloc;
    FreeFn<T> free_func   = default_free; 

  public:
    /// Default CTOR. 
    ArenaAllocator()                       = default;
    
    /// Copy CTORs.
    ArenaAllocator(const ArenaAllocator&)  = default;
    ArenaAllocator(ArenaAllocator&&)       = default;

    /// Allocates a new buffer using `alloc_fn` with a size of `count * element_size`.
    /// The `free_fn` function will be used later to clear the buffer.
    ArenaAllocator(const sizei count, const sizei element_size, const AllocFn<T>& alloc_fn = default_alloc, const FreeFn<T>& free_fn = default_free) {
      alloc_func = alloc_fn; 
      free_func  = free_fn;

      m_element_size = element_size;
      m_max_allocs   = count;

      m_buffer = alloc_func(count, element_size);
      memset(m_buffer, 0, sizeof(count) * element_size);
    }

  public:
    /// Return/allocate a new pointer from the arena buffer. 
    /// NOTE: This function will assert if there's no more sufficient space in the buffer.
    T* alloc() {
      // Make sure there's enough memory
      assert(m_total_alloc <= m_max_allocs);
      
      m_total_alloc++;

      return &m_buffer[m_total_alloc - 1];
    }

    /// De-allocate the given `ptr` from the arena buffer.
    /// NOTE: This function will assert if the given `ptr` is `nullptr`
    void free(T* ptr) {
      // The given `ptr` needs to be valid
      assert(ptr);

      ptr = nullptr;

      m_total_alloc--;
      m_total_free++;
    }

    /// De-allocate the underlying buffer using `free_fn`.
    void clear() {
      free_func(m_buffer);

      m_total_alloc = 0;
      m_total_free  = 0;
    }

    /// Return the total number of allocations made so far.
    const sizei get_alloc_count() {
      return m_total_alloc + m_total_free;
    }
    
    /// Return the total number of de-allocations made so far.
    const sizei get_free_count() {
      return m_total_free;
    }
};
/// ArenaAllocater 

////////////////////////////////////////////////////////////////////

} // End of ishtar


/// Ishtar implementation 
////////////////////////////////////////////////////////////////////
#ifdef ISHTAR_IMPL // ISHTAR_IMPL

namespace ishtar { // Start of ishtar

////////////////////////////////////////////////////////////////////

// String functions 

String::String(const String& other) {
  copy(other);
}

String::String(String&& other) {
  copy(other);
}

String::String(const char* str) {
  copy(str);
}

String::String(const char* str, const sizei str_len) {
  copy(str, str_len);
}

String::~String() {
  length = 0;

  if(data) {
    free(data);
  }
}
    
void String::copy(const char* str, const sizei str_len) {
  if(!str) {
    return;
  }

  // The string's new size
  length = str_len; 

  // We don't need the old data
  if(data) {
    free(data);
  }

  // Make a new array
  data = (char*)malloc(length);

  // Copy the string over
  memcpy(data, str, length + 1);
}

void String::copy(const String& str) {
  copy(str.data, str.length);
}

void String::copy(const char* str) {
  copy(str, string_length(str));
}

const bool String::is_empty() {
  return length == 0;
}

void String::append(const String& other) {
  data = (char*)realloc(data, sizeof(char) * (length + other.length));
  memcpy(data + length, other.data, other.length);

  length += (other.length + 1);
}

void String::append(const char* other) {
  append((String)other);
}

void String::append(const char& ch) {
  length += 1;
  data    = (char*)realloc(data, length);
  data[length - 1] = ch;
}

void String::append_at(const sizei index, const char& ch) {
  length += 1;
  data    = (char*)realloc(data, sizeof(char) * length);

  // Shift all characters to fit the new given `ch` 
  for(sizei i = length - 1; i > index; i--) {
    data[i] = at(i - 1); 
  }

  // Insert the new given `ch` in the empty slot
  data[index] = ch;
}

String String::slice(const sizei begin, const sizei end) {
  sizei new_data_size = (end - begin) + 1; // Make sure that `end` is _inclusive_
  char* new_data = (char*)malloc(sizeof(char) * new_data_size);

  // Copying over the correct data 
  for(sizei i = begin, j = 0; i < new_data_size || j < new_data_size; i++, j++) {
    new_data[j] = at(i);
  }

  return String(new_data);
}

const bool String::compare(const String& other) {
  // Can't be the same if they are of different sizes
  if(length != other.length) {
    return false; 
  }

  for(sizei i = 0; i < length; i++) {
    // Even if a _single_ character is different then the strings are _not_ the same
    if(at(i) != other[i]) {
      return false;
    }
  }

  return true;
}

void String::reverse() {
  char* reversed_str = (char*)malloc(sizeof(char) * length);

  for(sizei i = length - 1, j = 0; i >= 0 && j < length; i--, j++) {
    reversed_str[j] = at(i); 
  }

  copy(reversed_str);
  free(reversed_str);
}

const sizei String::find(const char& ch, const sizei start) {
  for(sizei i = start; i < length; i++) {
    if(at(i) == ch) {
      return i;
    }
  }

  // Couldn't find the given `ch` in `str`
  return STRING_NPOS;
}

const sizei String::find_last_of(const char& ch) {
  for(sizei i = length - 1; i > 0; i--) {
    if(at(i) == ch) {
      return i;
    }
  } 

  return STRING_NPOS;
}

const sizei String::find_first_of(const char& ch) {
  return find(ch);
}

void String::remove(const sizei begin, const sizei end) {
  sizei new_len = (end - begin) * sizeof(char);
  char* temp_str = (char*)malloc(new_len);

  for(sizei i = begin, j = 0; i < end && j < end; i++, j++) {
    temp_str[j] = at(i); 
  }

  copy(temp_str, new_len);
}

void String::replace_at(const sizei index, const char& ch) {
  data[index] = ch;
}

void String::replace(const char& ch1, const char& ch2) {
  sizei index = 0;

  for(sizei i = 0; i < length; i++) {
    if(at(i) != ch1) {
      continue;
    }
     
    // The char was found!
    index = i;
    replace_at(index, ch2);

    // Leave the function
    return;
  }
}

void String::replace_all_of(const char& ch1, const char& ch2) {
  for(sizei i = 0; i < length; i++) {
    if(at(i) == ch1) {
      replace_at(i, ch2);
    }
  }
}

const bool String::has(const char& ch) {
  return find(ch) != STRING_NPOS;
}

const bool String::has_at(const sizei index, const char& ch) {
  return data[index] == ch;
}

const char* String::c_str() const {
  return data;
}

void String::fill(const sizei len, const char& ch) {
  length = len;
  data = (char*)realloc(data, sizeof(char) * length);

  for(sizei i = 0; i < length; i++) {
    data[i] = ch;
  }
}

void String::for_each(const ArrayForEachFn<char>& func) {
  if(!func) {
    return;
  }

  for(sizei i = 0; i < length; i++) {
    func((char&)at(i), i);
  }
}

void String::clear() {
  if(data) {
    memset(data, 0, length);
  }

  length = 0;
}

// String functions 

/// HashTable functions

const u64 hash_key(const char* str) {
  u32 hash  = 2166136261u;
  sizei len = strlen(str);

  for(sizei i = 0; i < len; i++) {
    hash ^= (u8)str[i];
    hash *= 1677719;
  }

  return hash;
}

const u64 hash_key(const String& str) {
  u32 hash  = 2166136261u;
  sizei len = str.length;

  for(sizei i = 0; i < len; i++) {
    hash ^= (u8)str[i];
    hash *= 1677719;
  }

  return hash;
}

/// HashTable functions

////////////////////////////////////////////////////////////////////

} // End of ishtar

#endif // ISHTAR_IMPL
////////////////////////////////////////////////////////////////////
/// Ishtar implementation 

