//
// File:    Tree.h
// Author:  Adam.Lewis@athens.edu
// Purpose:
// Use C++ templates, STL classes and functions, and smart pointers to
// implement a binary search tree.
//
// A binary search tree is defined as being either empty, or a 3-tuple
// (left.subtree, value, right.subtree) where left.subtree is a binary search
// tree that only contains values less than our value and right.subtree is a
// binary search tree that only contains values larger than our value.
//
// Note: We are strongly encouraged by the C++ Core Guidelines to strive
// for immutability in our data structures.  This means that we should not
// modify the tree in place, but rather create a new tree that is a modified
// version of the original tree.  This is a bit of a pain, but it is the
// right thing to do.  It allows us to reason about our code more easily, and
// it allows us to use the tree in a functional programming style.  
// 
// The downside of immutability is there are is a lot of copying of data.  This
// is mitigated by the use of smart pointers, which allow us to share ownership. But even so,  
// we should be careful about how we use this class to avoid unnecessary copies.
//
#pragma once

#include <memory>
#include <functional>
#include <cassert>
#include <initializer_list>
template<typename T>
class Tree
{
    // The inner struct Node represents one node of the tree.  This defines the
    // underlying structure implied by the mathematical definition of the Tree
    // ADT
    //
    struct Node
    {
        Node(std::shared_ptr<const Node>  lft
             , T val
             , std::shared_ptr<const Node>  rgt)
        : _lft(lft), _val(val), _rgt(rgt)
        {}

        std::shared_ptr<const Node> _lft;
        T _val;
        std::shared_ptr<const Node> _rgt;
    };

    //
    // And this private constructor defines how we keep track of the root of the
    // tree while not exposing that information to clients of this class.
    //
    explicit Tree(std::shared_ptr<const Node>  node)
      : _root(node) {}

public:
    //
    // The first two public constructors provide the protocol for creating a
    // Tree per the mathematical definition of the Tree ADT.
    //
    Tree(): _root(nullptr) {}
    Tree(Tree const & other) = default;
    Tree & operator=(Tree const & other) = default;
    ~Tree() = default;
    Tree(Tree && other) = default;
    Tree & operator=(Tree && other) = default;
    //

    Tree(Tree lft, T val, Tree  rgt)
    : _root(std::make_shared< Node>(lft._root, val, rgt._root))
    {}

    //
    // We add an additional constructor that we use to construct a Tree from
    // an initializer list.
    //
    Tree(std::initializer_list<T> init) {
        Tree t;
        for (T v: init) {
            t = t.insert(v);
        }
        _root = t._root;
    }

    //
    // The next portion of the protocol allows the client to query the state
    // of the Tree.   Again, note how we avoid exposing the state to the client.
    //
    bool isEmpty() const { return !_root; }

    size_t size() const {
        if (isEmpty()) return 0;
        return 1 + left().size() + right().size();
    }

    T root() const {
        assert(!isEmpty());
        return _root->_val;
    }


    Tree left() const {
        assert(!isEmpty());
        return Tree(_root->_lft);
    }

    Tree right() const {
        assert(!isEmpty());
        return Tree(_root->_rgt);
    }

    //
    // Now we manipulate the tree.
    // Note how insert operates... we don't try to "fix" an existing tree.
    // Rather we build a new tree, inserting the new value at the correct point.
    //
    // Note also the implicit assumption about the template class... it must
    // support the comparison operator, or mathematically, must be partially
    // ordered.
    //
    // Note that we are doing a bit of C++ trickery here.  We are using
    // the Compare type parameter to allow the user to specify how to
    // compare two values of type T.  The default is std::less<T>,
	// which means that we are building a binary search tree where the
	// left subtree contains values less than the root, and the right
	// subtree contains values greater than the root.
    //
    template <typename Compare=std::less<T>>
    Tree insert(T x, Compare comp=std::less<T>()) const {
        if (isEmpty())
            return Tree(Tree(), x, Tree());
        T y = root();
        if (comp(x, y))
            return Tree(left().insert(x, comp), y, right());
        else if (comp(y, x))
            return Tree(left(), y, right().insert(x, comp));
        else
            return *this; // no duplicates
    }

    // Continuing the use of the Compare type parameter, we provide a default
    // comparison function that uses std::less<T>.  This allows the user to
    // provide a callable object that defines how to compare two values of type T.
    template <typename Compare=std::less<T>>
    bool member(T x, Compare comp=std::less<T>()) const {
        if (isEmpty()) {
            return false;
        }
        T y = root();
        if (comp(x, y)) {
            return left().member(x, comp);
        } else if (comp(y, x)) {
            return right().member(x, comp);
        } else {
            return true;
        }   
    }

    //
    // This function finds a value in the tree, and returns true if it is found.
    // If it is found, the subtree where it was found is returned in the
    // second parameter.
    //
    // The Compare parameter is a function object that defines how to compare
    // two values of type T.  The default is std::less<T>, which means that
    // we are looking for a value that is greater than the current node's value.
    //
  
  template<typename Compare=std::less<T>>
  bool find(T x, Tree &subtreeWhereFound, Compare comp=std::less<T>()) const {
        if (isEmpty()) {
            subtreeWhereFound = Tree();
            return false;
        }   
        T y = root();
        if (comp(x, y))
            return left().find(x, subtreeWhereFound, comp);
        else if (comp(y, x))
            return right().find(x, subtreeWhereFound, comp);
        else {
          subtreeWhereFound = *this;
          return true;
        }
    }

    //
    // For each of traversal functions, we assume that the parameter is a
    // function pointer, object, or lambda expression that returns void and is
    // passed an object of type T.
    //
    void preorder(std::function<void(T)> visit) const {
        if (isEmpty())
            return;
        T contents = root();
        visit(contents);
        left().preorder(visit);
        right().preorder(visit);
    }

    void inorder(std::function<void(T)> visit) const {
        if (isEmpty()) return;
        left().inorder(visit);
        T contents = root();
        visit(contents);
        right().inorder(visit);
    }

    void postorder(std::function<void(T)> visit) const {
        if (isEmpty()) return;
        left().postorder(visit);
        right().postorder(visit);
        T contents = root();
        visit(contents);
    }

private:
    std::shared_ptr<const Node> _root;
};


// Overload the << operator to print the tree in a human-readable format   
// This is a simple implementation that prints the tree in a pre-order traversal.
// Note how the overload takes advantage of the preorder function to print the values in the tree
// and the ability to pass a callable object to the preorder function.
template<typename T>
std::ostream& operator<<(std::ostream& os, const Tree<T>& tree) {
	if (tree.isEmpty()) {
		os << "[]";
		return os;
	}
	os << "[";
	tree.inorder([&os](T val) { os << val << " "; });
	os << "]";
	return os;
}
