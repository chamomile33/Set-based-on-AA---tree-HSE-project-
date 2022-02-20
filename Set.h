#include <set>
#include <cstddef>
#include <memory>

template<class T>
struct Node {
    T value;
    size_t level;
    Node<T>* left_child = nullptr;
    Node<T>* right_child = nullptr;
    Node<T>* parent = nullptr;

    Node<T>(T val, size_t l, Node<T>* left, Node<T>* right, Node<T>* p)
            : value(val), level(l), left_child(left), right_child(right), parent(p) {};

    Node(T val, size_t l) : value(val), level(l) {};

    ~Node() = default;

    void clear();
};

template<class T>
void Node<T>::clear() {
    if (left_child != nullptr) {
        left_child->clear();
    }
    if (right_child != nullptr) {
        right_child->clear();
    }
    delete this;
}


template<class T>
class Set {
public:
    Set();

    ~Set();

    template<typename Iterator>
    Set(const Iterator first, const Iterator last);

    Set(const std::initializer_list<T>& elems);

    Set(Set<T>& set);

    size_t size() const;

    bool empty() const;

    void insert(const T& elem);

    void erase(const T& elem);

    Set<T>& operator=(const Set<T>& set);

    class iterator {
    public:
        friend Set<T>;

        iterator() = default;

        ~iterator();

        explicit iterator(const Set* pointer);

        iterator(const iterator& point);

        iterator& operator++();

        iterator& operator--();

        iterator& operator=(const iterator t);

        iterator operator++(int);

        iterator operator--(int);

        T& operator*() const;

        T* operator->() const;

        bool operator!=(const iterator& it) const;

        bool operator==(const iterator& it) const;

    private:
        const Set* set = nullptr;
        Node<T>* ptr = nullptr;
    };

    iterator begin() const;

    iterator end() const;

    iterator find(const T& elem) const;

    iterator lower_bound(const T& elem) const;

private:
    Node<T>* root = nullptr;
    size_t tree_size = 0;

    Node<T>* dfs(Node<T>* v, Node<T>* v1, Node<T>* par);

    Node<T>* deleteNode(const T& elem, Node<T>* v);

    Node<T>* Succesor(Node<T>* v);

    Node<T>* Predcessor(Node<T>* v);

    Node<T>* insert_element(const T& elem, Node<T>* v, Node<T>* parent);

    Node<T>* Skew(Node<T>* v);

    Node<T>* Split(Node<T>* v);

};

template<class T>
Set<T>::Set() {
    root = nullptr;
    tree_size = 0;
}

template<typename T>
template<typename Iterator>
Set<T>::Set(const Iterator first, const Iterator last) {
    tree_size = 0;
    Iterator curr = first;
    while (curr != last) {
        root = insert_element(*curr, root, nullptr);
        curr++;
    }
}

template<class T>
Set<T>::Set(const std::initializer_list<T>& elems) {
    tree_size = 0;
    for (const auto& i: elems) {
        root = insert_element(i, root, nullptr);
    }
}

template<class T>
size_t Set<T>::size() const {
    return tree_size;
}

template<class T>
bool Set<T>::empty() const {
    if (tree_size == 0) {
        return true;
    }
    return false;
}

template<class T>
void Set<T>::insert(const T& elem) {
    root = insert_element(elem, root, nullptr);
}

template<class T>
Node<T>* Set<T>::insert_element(const T& elem, Node<T>* v, Node<T>* parent) {
    if (v == nullptr) {
        tree_size++;
        return new Node<T>(elem, 1, nullptr, nullptr, parent);
    }
    if (elem < v->value) {
        v->left_child = insert_element(elem, v->left_child, v);
    } else if (v->value < elem) {
        v->right_child = insert_element(elem, v->right_child, v);
    }
    v = Skew(v);
    v = Split(v);
    return v;
}

template<class T>
Node<T>* Set<T>::Skew(Node<T>* v) {
    if (v == nullptr || v->left_child == nullptr || v->left_child->level != v->level) {
        return v;
    }
    Node<T>* temp = new Node<T>(v->value, v->level, v->left_child->right_child, v->right_child, nullptr);
    Node<T>* ans = new Node<T>(v->left_child->value, v->left_child->level, v->left_child->left_child, temp, v->parent);
    temp->parent = ans;

    if (v->right_child != nullptr) {
        v->right_child->parent = temp;
    }
    if (v->left_child->right_child != nullptr) {
        v->left_child->right_child->parent = temp;
    }
    if (v->left_child->left_child != nullptr) {
        v->left_child->left_child->parent = ans;
    }
    delete v->left_child;
    delete v;
    return ans;
}

template<class T>
Node<T>* Set<T>::Split(Node<T>* v) {
    if (v == nullptr || v->right_child == nullptr || v->right_child->right_child == nullptr ||
        v->right_child->right_child->level != v->level) {
        return v;
    }
    Node<T>* temp = new Node<T>(v->value, v->level, v->left_child, v->right_child->left_child, nullptr);
    Node<T>* ans = new Node<T>(v->right_child->value, v->right_child->level + 1, temp, v->right_child->right_child,
                               v->parent);
    temp->parent = ans;
    v->right_child->right_child->parent = ans;
    if (v->right_child->left_child != nullptr) {
        v->right_child->left_child->parent = temp;
    }
    if (v->left_child != nullptr) {
        v->left_child->parent = temp;
    }
    delete v->right_child;
    delete v;
    return ans;
}

template<class T>
void Set<T>::erase(const T& elem) {
    root = deleteNode(elem, root);
}

template<class T>
Node<T>* Set<T>::deleteNode(const T& elem, Node<T>* v) {
    if (v == nullptr) {
        return v;
    }
    if (elem < v->value) {
        v->left_child = deleteNode(elem, v->left_child);
    } else if (v->value < elem) {
        v->right_child = deleteNode(elem, v->right_child);
    } else if (v->right_child == nullptr && v->left_child == nullptr) {
        delete v;
        tree_size--;
        return nullptr;
    } else if (v->left_child == nullptr) {
        T temp_val = Succesor(v)->value;
        v->right_child = deleteNode(temp_val, v->right_child);
        v->value = temp_val;
    } else {
        T temp_val = Predcessor(v)->value;
        v->left_child = deleteNode(temp_val, v->left_child);
        v->value = temp_val;
    }
    size_t depth;
    if (v->left_child != nullptr && v->right_child != nullptr) {
        depth = std::min(v->right_child->level, v->left_child->level) + 1;
        if (depth < v->level) {
            v->level = depth;
            if (depth < v->right_child->level) {
                v->right_child->level = depth;
            }
        }
    }
    v = Skew(v);
    v->right_child = Skew(v->right_child);
    if (v->right_child != nullptr) {
        v->right_child->right_child = Skew(v->right_child->right_child);
    }
    v = Split(v);
    v->right_child = Split(v->right_child);
    return v;
}

template<class T>
typename Set<T>::iterator Set<T>::begin() const {
    iterator iter(this);
    Node<T>* cur;
    cur = iter.set->root;
    while (cur != nullptr && cur->left_child != nullptr) {
        cur = cur->left_child;
    }
    iter.ptr = cur;
    return iter;
}

template<class T>
typename Set<T>::iterator Set<T>::end() const {
    iterator iter(this);
    iter.ptr = nullptr;
    return iter;
}

template<class T>
typename Set<T>::iterator Set<T>::find(const T& elem) const {
    iterator iter(this);
    iter = lower_bound(elem);
    if (!(iter.ptr->value < elem) && !(elem < iter.ptr->value)) {
        return iter;
    }
    return end();
}

template<class T>
typename Set<T>::iterator Set<T>::lower_bound(const T& elem) const {
    iterator iter(this);
    Node<T>* cur = root;
    iter.ptr = cur;
    while (cur) {
        if (elem < cur->value) {
            if (cur->left_child == nullptr) {
                iter.ptr = cur;
                return iter;
            }
            cur = cur->left_child;
        } else if (cur->value < elem) {
            if (cur->right_child == nullptr) {
                iter.ptr = cur;
                ++iter;
                return iter;
            }
            cur = cur->right_child;
        } else {
            iter.ptr = cur;
            return iter;
        }
    }
    return iter;
}

template<class T>
Set<T>& Set<T>::operator=(const Set& set) {
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
    root = dfs(root, set.root, nullptr);
    tree_size = set.tree_size;
    return (*this);
}

template<class T>
Node<T>* Set<T>::dfs(Node<T>* v, Node<T>* v1, Node<T>* par) {
    v = new Node<T>(v1->value, v1->level);
    v->parent = par;
    if (v1->left_child != nullptr) {
        v->left_child = dfs(v->left_child, v1->left_child, v);
    }
    if (v1->right_child != nullptr) {
        v->right_child = dfs(v->right_child, v1->right_child, v);
    }
    return v;
}

template<class T>
Set<T>::Set(Set<T>& set) {
    tree_size = set.tree_size;
    if (set.root == nullptr) {
        root = nullptr;
    } else {
        root = dfs(root, set.root, nullptr);
    }
}

template<class T>
Set<T>::~Set() {
    if (root != nullptr) {
        root->clear();
        root = nullptr;
    }
}

template<class T>
Node<T>* Set<T>::Predcessor(Node<T>* v) {
    Node<T>* cur = v;
    cur = cur->left_child;
    while (cur != nullptr && cur->right_child != nullptr) {
        cur = cur->right_child;
    }
    return cur;
}

template<class T>
Node<T>* Set<T>::Succesor(Node<T>* v) {
    Node<T>* cur = v;
    cur = cur->right_child;
    while (cur != nullptr && cur->left_child != nullptr) {
        cur = cur->left_child;
    }
    return cur;
}


template<class T>
typename Set<T>::iterator& Set<T>::iterator::operator++() {
    Node<T>* cur = ptr;
    if (cur->right_child != nullptr) {
        cur = cur->right_child;
        while (cur->left_child != nullptr) {
            cur = cur->left_child;
        }
        ptr = cur;
        return *this;
    } else if (ptr->parent != nullptr) {
        if (ptr->value < ptr->parent->value) {
            ptr = ptr->parent;
            return *this;
        } else {
            while (cur->parent == nullptr || cur->parent->value < cur->value) {
                if (cur->parent == nullptr) {
                    ptr = cur->parent;
                    return *this;
                }
                cur = cur->parent;
            }
            ptr = cur->parent;
            return *this;
        }
    }
    ptr = nullptr;
    return *this;
}

template<class T>
typename Set<T>::iterator Set<T>::iterator::operator++(int) {
    iterator temp(*this);
    ++(*this);
    return temp;
}

template<class T>
typename Set<T>::iterator& Set<T>::iterator::operator--() {
    Node<T>* cur(ptr);
    if (ptr == nullptr) {
        cur = set->root;
        while (cur->right_child != nullptr) {
            cur = cur->right_child;
        }
        ptr = cur;
        return *this;
    }
    if (cur->left_child != nullptr) {
        cur = cur->left_child;
        while (cur->right_child != nullptr) {
            cur = cur->right_child;
        }
        ptr = cur;
        return *this;
    } else if (ptr->parent != nullptr) {
        if (ptr->parent->value < ptr->value) {
            ptr = ptr->parent;
            return *this;
        } else {
            while (cur->parent == nullptr || cur->value < cur->parent->value) {
                if (cur->parent == nullptr) {
                    ptr = cur->parent;
                    return *this;
                }
                cur = cur->parent;
            }
            ptr = cur;
            return *this;
        }
    } else {
        ptr = nullptr;
        return *this;
    }
}

template<class T>
Set<T>::iterator::iterator(const Set* pointer) {
    set = pointer;
}

template<class T>
typename Set<T>::iterator Set<T>::iterator::operator--(int) {
    iterator temp(*this);
    --(*this);
    return temp;
}

template<class T>
T& Set<T>::iterator::operator*() const {
    return ptr->value;
}

template<class T>
T* Set<T>::iterator::operator->() const {
    return &ptr->value;
}

template<class T>
bool Set<T>::iterator::operator!=(const Set::iterator& it) const {
    return ptr != it.ptr;
}

template<class T>
bool Set<T>::iterator::operator==(const Set::iterator& it) const {
    return (ptr == it.ptr && it.set == set);
}

template<class T>
Set<T>::iterator::~iterator() {
}

template<class T>
typename Set<T>::iterator& Set<T>::iterator::operator=(const Set::iterator t) {
    if (&t == this) {
        return *this;
    }
    set = t.set;
    ptr = t.ptr;
    return *this;
}

template<class T>
Set<T>::iterator::iterator(const Set::iterator& point) {
    ptr = point.ptr;
    set = point.set;
}
