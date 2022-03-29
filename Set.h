#pragma once
#include <cstddef>
#include <memory>
#include <set>


template<class ValueType>
struct Node {
    ValueType value;
    size_t level;
    Node<ValueType>* left_child = nullptr;
    Node<ValueType>* right_child = nullptr;
    Node<ValueType>* parent = nullptr;

    Node<ValueType>(ValueType value, size_t level, Node<ValueType>* left, Node<ValueType>* right, Node<ValueType>* parent)
            : value(value), level(level), left_child(left), right_child(right), parent(parent) {};

    Node(ValueType value, size_t level) : value(value), level(level) {};

    ~Node() = default;

    void clear() {
        if (left_child != nullptr) {
            left_child->clear();
        }
        if (right_child != nullptr) {
            right_child->clear();
        }
        delete this;
    };
};


template<class ValueType>
class Set {
public:
    Set();

    ~Set();

    template<typename Iterator>
    Set(const Iterator first, const Iterator last);

    Set(const std::initializer_list<ValueType>& elements);

    Set(const Set<ValueType>& set);

    size_t size() const;

    bool empty() const;

    void insert(const ValueType& element);

    void erase(const ValueType& element);

    Set<ValueType>& operator=(const Set<ValueType>& set);

    class iterator {
    public:
        friend Set<ValueType>;

        iterator();

        ~iterator();

        explicit iterator(const Set* pointer);

        iterator(const iterator& point);

        iterator& operator++();

        iterator& operator--();

        iterator& operator=(const iterator& it);

        iterator operator++(int);

        iterator operator--(int);

        const ValueType& operator*() const;

        const ValueType* operator->() const;

        bool operator!=(const iterator& it) const;

        bool operator==(const iterator& it) const;

    private:
        const Set* set = nullptr;
        Node<ValueType>* ptr = nullptr;
    };

    iterator begin() const;

    iterator end() const;

    iterator find(const ValueType& element) const;

    iterator lower_bound(const ValueType& element) const;

private:
    Node<ValueType>* root = nullptr;
    size_t tree_size = 0;

    Node<ValueType>* Dfs(Node<ValueType>* copied_node, Node<ValueType>* node_to_copy, Node<ValueType>* parent);

    Node<ValueType>* DeleteNode(const ValueType& element, Node<ValueType>* current_node);

    Node<ValueType>* Succesor(Node<ValueType>* current_node);

    Node<ValueType>* Predcessor(Node<ValueType>* current_node);

    Node<ValueType>* Insert_element(const ValueType& element, Node<ValueType>* current_node, Node<ValueType>* parent);

    Node<ValueType>* Skew(Node<ValueType>* current_node);

    Node<ValueType>* Split(Node<ValueType>* current_node);

};

template<class ValueType>
Set<ValueType>::Set() {
    root = nullptr;
    tree_size = 0;
}

template<typename ValueType>
template<typename Iterator>
Set<ValueType>::Set(const Iterator first, const Iterator last) {
    tree_size = 0;
    for(Iterator current = first; current != last; ++current){
        root = Insert_element(*current, root, nullptr);
    }
}

template<class ValueType>
Set<ValueType>::Set(const std::initializer_list<ValueType>& elements) {
    tree_size = 0;
    for (const auto& i: elements) {
        root = Insert_element(i, root, nullptr);
    }
}

template<class ValueType>
size_t Set<ValueType>::size() const {
    return tree_size;
}

template<class ValueType>
bool Set<ValueType>::empty() const {
    return tree_size == 0;
}

template<class ValueType>
void Set<ValueType>::insert(const ValueType& element) {
    root = Insert_element(element, root, nullptr);
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Insert_element(const ValueType& element, Node<ValueType>* current_node, Node<ValueType>* parent) {
    if (current_node == nullptr) {
        ++tree_size;
        return new Node<ValueType>(element, 1, nullptr, nullptr, parent);
    }
    if (element < current_node->value) {
        current_node->left_child = Insert_element(element, current_node->left_child, current_node);
    } else if (current_node->value < element) {
        current_node->right_child = Insert_element(element, current_node->right_child, current_node);
    }
    current_node = Skew(current_node);
    current_node = Split(current_node);
    return current_node;
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Skew(Node<ValueType>* current_node) {
    if (current_node == nullptr || current_node->left_child == nullptr || current_node->left_child->level != current_node->level) {
        return current_node;
    }
    Node<ValueType>* temp = new Node<ValueType>(current_node->value, current_node->level, current_node->left_child->right_child, current_node->right_child, nullptr);
    Node<ValueType>* ans = new Node<ValueType>(current_node->left_child->value, current_node->left_child->level, current_node->left_child->left_child, temp, current_node->parent);
    temp->parent = ans;

    if (current_node->right_child != nullptr) {
        current_node->right_child->parent = temp;
    }
    if (current_node->left_child->right_child != nullptr) {
        current_node->left_child->right_child->parent = temp;
    }
    if (current_node->left_child->left_child != nullptr) {
        current_node->left_child->left_child->parent = ans;
    }
    delete current_node->left_child;
    delete current_node;
    return ans;
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Split(Node<ValueType>* current_node) {
    if (current_node == nullptr || current_node->right_child == nullptr || current_node->right_child->right_child == nullptr ||
            current_node->right_child->right_child->level != current_node->level) {
        return current_node;
    }
    Node<ValueType>* temp = new Node<ValueType>(current_node->value, current_node->level, current_node->left_child, current_node->right_child->left_child, nullptr);
    Node<ValueType>* ans = new Node<ValueType>(current_node->right_child->value, current_node->right_child->level + 1, temp, current_node->right_child->right_child,
                                               current_node->parent);
    temp->parent = ans;
    current_node->right_child->right_child->parent = ans;
    if (current_node->right_child->left_child != nullptr) {
        current_node->right_child->left_child->parent = temp;
    }
    if (current_node->left_child != nullptr) {
        current_node->left_child->parent = temp;
    }
    delete current_node->right_child;
    delete current_node;
    return ans;
}

template<class ValueType>
void Set<ValueType>::erase(const ValueType& element) {
    root = DeleteNode(element, root);
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::DeleteNode(const ValueType& element, Node<ValueType>* current_node) {
    if (current_node == nullptr) {
        return current_node;
    }
    if (element < current_node->value) {
        current_node->left_child = DeleteNode(element, current_node->left_child);
    } else if (current_node->value < element) {
        current_node->right_child = DeleteNode(element, current_node->right_child);
    } else if (current_node->right_child == nullptr && current_node->left_child == nullptr) {
        delete current_node;
        tree_size--;
        return nullptr;
    } else if (current_node->left_child == nullptr) {
        ValueType temp_value = Succesor(current_node)->value;
        current_node->right_child = DeleteNode(temp_value, current_node->right_child);
        current_node->value = temp_value;
    } else {
        ValueType temp_value = Predcessor(current_node)->value;
        current_node->left_child = DeleteNode(temp_value, current_node->left_child);
        current_node->value = temp_value;
    }
    size_t depth;
    if (current_node->left_child != nullptr && current_node->right_child != nullptr) {
        depth = std::min(current_node->right_child->level, current_node->left_child->level) + 1;
        if (depth < current_node->level) {
            current_node->level = depth;
            if (depth < current_node->right_child->level) {
                current_node->right_child->level = depth;
            }
        }
    }
    current_node = Skew(current_node);
    current_node->right_child = Skew(current_node->right_child);
    if (current_node->right_child != nullptr) {
        current_node->right_child->right_child = Skew(current_node->right_child->right_child);
    }
    current_node = Split(current_node);
    current_node->right_child = Split(current_node->right_child);
    return current_node;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::begin() const {
    iterator iter(this);
    Node<ValueType>* current;
    current = iter.set->root;
    while (current != nullptr && current->left_child != nullptr) {
        current = current->left_child;
    }
    iter.ptr = current;
    return iter;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::end() const {
    iterator iter(this);
    iter.ptr = nullptr;
    return iter;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::find(const ValueType& element) const {
    iterator iter(this);
    iter = lower_bound(element);
    if (!(iter.ptr->value < element) && !(element < iter.ptr->value)) {
        return iter;
    }
    return end();
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::lower_bound(const ValueType& element) const {
    iterator iter(this);
    Node<ValueType>* current = root;
    iter.ptr = current;
    while (current) {
        if (element < current->value) {
            if (current->left_child == nullptr) {
                iter.ptr = current;
                return iter;
            }
            current = current->left_child;
        } else if (current->value < element) {
            if (current->right_child == nullptr) {
                iter.ptr = current;
                ++iter;
                return iter;
            }
            current = current->right_child;
        } else {
            iter.ptr = current;
            return iter;
        }
    }
    return iter;
}

template<class ValueType>
Set<ValueType>& Set<ValueType>::operator=(const Set& set) {
    if (&set == this) {
        return *this;
    }
    if (root != nullptr) {
        root->clear();
        root = nullptr;
    }
    tree_size = 0;
    if (set.root == nullptr) {
        return (*this);
    }
    root = Dfs(root, set.root, nullptr);
    tree_size = set.tree_size;
    return (*this);
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Dfs(Node<ValueType>* copied_node, Node<ValueType>* node_to_copy, Node<ValueType>* parent) {
    copied_node = new Node<ValueType>(node_to_copy->value, node_to_copy->level);
    copied_node->parent = parent;
    if (node_to_copy->left_child != nullptr) {
        copied_node->left_child = Dfs(copied_node->left_child, node_to_copy->left_child, copied_node);
    }
    if (node_to_copy->right_child != nullptr) {
        copied_node->right_child = Dfs(copied_node->right_child, node_to_copy->right_child, copied_node);
    }
    return copied_node;
}

template<class ValueType>
Set<ValueType>::Set(const Set<ValueType>& set) {
    tree_size = set.tree_size;
    if (set.root == nullptr) {
        root = nullptr;
    } else {
        root = Dfs(root, set.root, nullptr);
    }
}

template<class ValueType>
Set<ValueType>::~Set() {
    if (root != nullptr) {
        root->clear();
        root = nullptr;
    }
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Predcessor(Node<ValueType>* current_node) {
    Node<ValueType>* current = current_node;
    current = current->left_child;
    while (current != nullptr && current->right_child != nullptr) {
        current = current->right_child;
    }
    return current;
}

template<class ValueType>
Node<ValueType>* Set<ValueType>::Succesor(Node<ValueType>* current_node) {
    Node<ValueType>* current = current_node;
    current = current->right_child;
    while (current != nullptr && current->left_child != nullptr) {
        current = current->left_child;
    }
    return current;
}


template<class ValueType>
typename Set<ValueType>::iterator& Set<ValueType>::iterator::operator++() {
    Node<ValueType>* current = ptr;
    if (current->right_child != nullptr) {
        current = current->right_child;
        while (current->left_child != nullptr) {
            current = current->left_child;
        }
        ptr = current;
        return *this;
    } else if (ptr->parent != nullptr) {
        if (ptr->value < ptr->parent->value) {
            ptr = ptr->parent;
            return *this;
        } else {
            while (current->parent == nullptr || current->parent->value < current->value) {
                if (current->parent == nullptr) {
                    ptr = current->parent;
                    return *this;
                }
                current = current->parent;
            }
            ptr = current->parent;
            return *this;
        }
    }
    ptr = nullptr;
    return *this;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator++(int) {
    iterator temp(*this);
    ++(*this);
    return temp;
}

template<class ValueType>
typename Set<ValueType>::iterator& Set<ValueType>::iterator::operator--() {
    Node<ValueType>* current(ptr);
    if (ptr == nullptr) {
        current = set->root;
        while (current->right_child != nullptr) {
            current = current->right_child;
        }
        ptr = current;
        return *this;
    }
    if (current->left_child != nullptr) {
        current = current->left_child;
        while (current->right_child != nullptr) {
            current = current->right_child;
        }
        ptr = current;
        return *this;
    } else if (ptr->parent != nullptr) {
        if (ptr->parent->value < ptr->value) {
            ptr = ptr->parent;
            return *this;
        } else {
            while (current->parent == nullptr || current->value < current->parent->value) {
                if (current->parent == nullptr) {
                    ptr = current->parent;
                    return *this;
                }
                current = current->parent;
            }
            ptr = current;
            return *this;
        }
    } else {
        ptr = nullptr;
        return *this;
    }
}

template<class ValueType>
Set<ValueType>::iterator::iterator(const Set* pointer) {
    set = pointer;
}

template<class ValueType>
typename Set<ValueType>::iterator Set<ValueType>::iterator::operator--(int) {
    iterator temp(*this);
    --(*this);
    return temp;
}

template<class ValueType>
const ValueType& Set<ValueType>::iterator::operator*() const {
    return ptr->value;
}

template<class ValueType>
const ValueType* Set<ValueType>::iterator::operator->() const {
    return &ptr->value;
}

template<class ValueType>
bool Set<ValueType>::iterator::operator!=(const Set::iterator& it) const {
    return ptr != it.ptr;
}

template<class ValueType>
bool Set<ValueType>::iterator::operator==(const Set::iterator& it) const {
    return (ptr == it.ptr && it.set == set);
}

template<class ValueType>
Set<ValueType>::iterator::~iterator() {
}

template<class ValueType>
typename Set<ValueType>::iterator& Set<ValueType>::iterator::operator=(const Set::iterator& it) {
    if (&it == this) {
        return *this;
    }
    set = it.set;
    ptr = it.ptr;
    return *this;
}

template<class ValueType>
Set<ValueType>::iterator::iterator(const Set::iterator& point) {
    ptr = point.ptr;
    set = point.set;
}

template<class ValueType>
Set<ValueType>::iterator::iterator() = default;
