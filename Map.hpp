#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace kanec1994
{

template<typename Key_T, typename Mapped_T>
class Map
{
private:
    struct RBNode
    {
        int color;
        Key_T *key;
        Mapped_T *value;
        struct RBNode *parent;
        struct RBNode *left;
        struct RBNode *right;
        struct RBNode *next;
        struct RBNode *prev;
    };
    struct RBNode *first, *last;
    typedef std::pair<const Key_T, Mapped_T> ValueType;
    class RBTree
    {
        private:
            Map *owner;
            int black = 0, red = 1;
            struct RBNode *root;
            size_t num_nodes;
        public:
            /*RBTree constructor*/
            RBTree(Map *owned_by)
            {
                owner = owned_by;
                root = nullptr;
                num_nodes = 0;
            }
            /*RBTree destructor*/
            ~RBTree()
            {
                //calls delete on nodes until no nodes remaining
                while(root != nullptr)
                {
                    delete_node(*root->key);
                }
            }
            void delete_map()
            {
                while(root != nullptr)
                {
                    delete_node(*root->key);
                }
            }
            size_t size_tree() const
            {
                return num_nodes;
            }
            /*Perform left rotation on selected node*/
            void rotate_left(RBNode *&root, RBNode *&pivot)
            {
                RBNode *swap_node = pivot->right;

                //if it exists, make swap_node subtree child of pivot
                if(swap_node->left != nullptr)
                {
                    pivot->right = swap_node->left;
                    swap_node->left->parent = pivot;
                }
                else
                {
                    pivot->right = nullptr;
                }

                //if pivot is root, make swap_node root, else handle rest of swap
                if(*pivot->key == *root->key)
                {
                    swap_node->parent = nullptr;
                    root = swap_node;
                }
                else
                {
                    swap_node->parent = pivot->parent;
                    if(pivot->parent->left != nullptr && *pivot->key == *pivot->parent->left->key)
                    {
                        pivot->parent->left = swap_node;
                    }
                    else
                    {
                        pivot->parent->right = swap_node;
                    }
                }
                pivot->parent = swap_node;
                swap_node->left = pivot;
            }
            /*Perform right rotation on selected node*/
            void rotate_right(RBNode *&root, RBNode *&pivot)
            {
                RBNode *swap_node = pivot->left;

                //if it exists, make swap_node subtree child of pivot
                if(swap_node->right != nullptr)
                {
                    pivot->left = swap_node->right;
                    swap_node->right->parent = pivot;
                }
                else
                {
                    pivot->left = nullptr;
                }

                //if pivot is root, make swap_node root, else handle rest of swap
                if(*pivot->key == *root->key)
                {
                    swap_node->parent = nullptr;
                    root = swap_node;
                }
                else
                {
                    swap_node->parent = pivot->parent;
                    if(pivot->parent->left != nullptr && *pivot->key == *pivot->parent->left->key)
                    {
                        pivot->parent->left = swap_node;
                    }
                    else
                    {
                        pivot->parent->right = swap_node;
                    }
                }
                pivot->parent = swap_node;
                swap_node->right = pivot;
            }
            /*Rebalance Red-Black tree*/
            void fix_insert(RBNode *&root, RBNode *&node)
            {
                RBNode *parent = nullptr;
                RBNode *grand_parent = nullptr;

                //Continue looping while node != root, node is red, and parent color is red
                while((*node->key != *root->key) && (node->color != black) && (node->parent->color == red))
                {
                    parent = node->parent;
                    grand_parent = node->parent->parent;

                    //all possible cases when parent of node is left child of grand_parent
                    if(grand_parent->left != nullptr && *parent->key == *grand_parent->left->key)
                    {
                        //recolor if uncle is red
                        if(grand_parent->right != nullptr && grand_parent->right->color == red)
                        {
                            parent->color = black;
                            grand_parent->right->color = black;
                            grand_parent->color = red;
                            node = grand_parent;
                        }
                        //perform cases when uncle is black
                        else
                        {
                            int temp_color;

                            //left-right case: left rotation of parent needed
                            if(parent->right != nullptr && *node->key == *parent->right->key)
                            {
                                rotate_left(root, parent);
                                node = parent;
                                parent = node->parent;
                            }

                            //left-left case: right rotation of grand_parent and color swap
                            rotate_right(root, grand_parent);
                            temp_color = grand_parent->color;
                            grand_parent->color = parent->color;
                            parent->color = temp_color;
                            node = parent;
                        }
                    }
                    else
                    {
                        //recolor if uncle is red
                        if(grand_parent->left != nullptr && grand_parent->left->color == red)
                        {
                            parent->color = black;
                            grand_parent->left->color = black;
                            grand_parent->color = red;
                            node = grand_parent;
                        }
                        //perform cases when uncle is black
                        else
                        {
                            int temp_color;

                            //right-left case: right rotation of parent needed
                             if(parent->left != nullptr && *node->key == *parent->left->key)
                            {
                                rotate_right(root, parent);
                                node = parent;
                                parent = node->parent;
                            }

                            //right-right case: left rotation of grand_parent and color swap
                            rotate_left(root, grand_parent);
                            temp_color = grand_parent->color;
                            grand_parent->color = parent->color;
                            parent->color = temp_color;
                            node = parent;
                        }
                    }
                    //If current node is root, change color to black
                    if(*node->key == *root->key)
                    {
                        node->color = black;
                    }
                }
            }

            /*Performed iteratively in order to keep track of red
            and black node colors*/
            std::pair<RBNode *, bool> insert_node(Key_T key, Mapped_T value)
            {
                //create new_node to be inserted
                RBNode *new_node = new RBNode();
                new_node->key = new Key_T(key);
                new_node->value = new Mapped_T(value);
                new_node->left = nullptr;
                new_node->right = nullptr;
                new_node->parent = nullptr;
                new_node->next = nullptr;
                new_node->prev = nullptr;
                new_node->color = red;

                //if root does not already exist, new_node becomes root
                if(root == nullptr)
                {
                    root = new_node;
                    root->color = black;
                    owner->first = new_node;
                    owner->last = new_node;
                }
                else
                {
                    //find place to insert new_node
                    RBNode *curr = root;
                    RBNode *curr_parent = nullptr;
                    while(curr != nullptr)
                    {
                        if(key < *curr->key)
                        {
                            curr_parent = curr;
                            curr = curr->left;
                        }
                        else if(*curr->key < key)
                        {
                            curr_parent = curr;
                            curr = curr->right;
                        }
                        else
                        {
                            return {curr, false};
                        }
                    }

                    curr = new_node;
                    //curr to be inserted as curr_parent left child
                    if(key < *curr_parent->key)
                    {
                        curr_parent->left = curr;
                        curr->parent = curr_parent;
                        curr->next = curr_parent;
                        curr->prev = curr_parent->prev;
                        if(curr_parent->prev == nullptr)
                        {
                            curr_parent->prev = curr;
                            owner->first = curr;
                        }
                        else
                        {
                            curr_parent->prev->next = curr;
                            curr_parent->prev = curr;
                        }
                    }
                    //curr to be inserted as curr_parent right child
                    else if(*curr_parent->key < key)
                    {
                        curr_parent->right = curr;
                        curr->parent = curr_parent;
                        curr->prev = curr_parent;
                        curr->next = curr_parent->next;
                        if(curr_parent->next == nullptr)
                        {
                            curr_parent->next = curr;
                            owner->last = curr;
                        }
                        else
                        {
                            curr_parent->next->prev = curr;
                            curr_parent->next = curr;
                        }
                    }
                    fix_insert(root, curr);
                }
                num_nodes++;
                return {new_node, true};
            }
            /*Performed iteratively in order to keep track of red
            and black node colors*/
            bool delete_node(Key_T key)
            {
                //Find node to be deleted
                bool left;
                RBNode *curr = root;
                RBNode *curr_parent = nullptr;
                while(curr != nullptr)
                {
                    if(*curr->key == key)
                    {
                        break;
                    }
                    else if(key < *curr->key)
                    {
                        left = true;
                        curr_parent = curr;
                        curr = curr->left;
                    }
                    else
                    {
                        left = false;
                        curr_parent = curr;
                        curr = curr->right;
                    }
                }

                //If node not in tree, return appropriate response
                if(curr == nullptr)
                {
                    return false;
                }
                //Case 1: Node has no children
                if(curr->left == nullptr && curr->right == nullptr)
                {
                    //Handle root node
                    if(curr_parent == nullptr)
                    {
                        root = nullptr;
                    }
                    //Handle case when child is a leaf
                    else
                    {
                        if(left)
                        {
                            curr_parent->left = nullptr;
                        }
                        else
                        {
                            curr_parent->right = nullptr;
                        }
                    }
                }
                //Case 2: Node has 1 child
                else if(curr->left == nullptr || curr->right == nullptr)
                {
                    //Handle case when current node is the root
                    if(curr_parent == nullptr)
                    {
                        if(curr->left == nullptr)
                        {
                            curr->right->parent = nullptr;
                            root = curr->right;
                        }
                        else
                        {
                            curr->left->parent = nullptr;
                            root = curr->left;
                        }
                    }
                    //Handle all other cases of nodes with 1 child
                    else
                    {
                        if(curr->left == nullptr)
                        {
                            if(left)
                            {
                                curr->right->parent = curr_parent;
                                curr_parent->left = curr->right;
                            }
                            else
                            {
                                curr->right->parent = curr_parent;
                                curr_parent->right = curr->right;
                            }
                        }
                        else
                        {
                            if(left)
                            {
                                curr->left->parent = curr_parent;
                                curr_parent->left = curr->left;
                            }
                            else
                            {
                                curr->left->parent = curr_parent;
                                curr_parent->right = curr->left;
                            }
                        }
                    }
                }
                //Case 3: Node has 2 children
                else
                {
                    //get largest RBNode in left subtree
                    RBNode *replacement = curr->left;
                    while(replacement->right != nullptr)
                    {
                        replacement = replacement->right;
                    }

                    //Handle all dependencies of replacing node to be deleted
                    if(replacement->parent != curr)
                    {
                        if(replacement->left != nullptr)
                        {
                            replacement->left->parent = replacement->parent;
                            replacement->parent->right = replacement->left;
                        }
                        else
                        {
                            replacement->parent->right = nullptr;
                        }
                        replacement->left = curr->left;
                        replacement->right = curr->right;
                        curr->left->parent = replacement;
                        curr->right->parent = replacement;
                    }
                    else
                    {
                        replacement->right = curr->right;
                        curr->right->parent = replacement;
                    }

                    if(curr_parent == nullptr)
                    {
                        replacement->parent = nullptr;
                        root = replacement;
                    }
                    else
                    {
                        replacement->parent = curr_parent;
                        if(left)
                        {
                            curr_parent->left = replacement;
                        }
                        else
                        {
                            curr->parent->right = replacement;
                        }

                    }

                    //delete selected node

                }
                if(root == nullptr)
                {
                    owner->first = nullptr;
                    owner->last = nullptr;
                }
                else if(curr->prev == nullptr)
                {
                    curr->next->prev = nullptr;
                    owner->first = curr->next;
                }
                else if(curr->next == nullptr)
                {
                    curr->prev->next = nullptr;
                    owner->last = curr->prev;
                }
                else
                {
                    curr->next->prev = curr->prev;
                    curr->prev->next = curr->next;
                }
                delete curr->key;
                delete curr->value;
                delete curr;
                num_nodes--;
                return true;
            }
            Mapped_T &find_val(const Key_T &key)
            {
                RBNode *curr = root;
                while(curr != nullptr)
                {
                    if(*curr->key == key)
                    {
                        return *curr->value;
                    }
                    else if(key < *curr->key)
                    {
                        curr = curr->left;
                    }
                    else
                    {
                        curr = curr->right;
                    }
                }
                throw std::out_of_range("Item not in Map");
            }
            RBNode *find_node(const Key_T &key)
            {
                RBNode *curr = root;
                while(curr != nullptr)
                {
                    if(*curr->key == key)
                    {
                        return curr;
                    }
                    else if(key < *curr->key)
                    {
                        curr = curr->left;
                    }
                    else
                    {
                        curr = curr->right;
                    }
                }
                return nullptr;
            }
    };
    RBTree Curr_Map = RBTree(this);
    Mapped_T mapped;
public:
    class ConstIterator;
    class Iterator
    {
    private:
        RBNode *target;
        Map *owner;
    public:
        ValueType *val = nullptr;
        Iterator(RBNode *node, Map *owned_by)
        {
            target = node;
            owner = owned_by;
            if(node != nullptr)
            {
                val = new ValueType(*node->key, *node->value);
            }
        }
        Iterator(const Iterator &it)
        {
            target = it.target;
            owner = it.owner;
            if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
        }
        ~Iterator()
        {
            delete val;
        }
        Iterator &operator++()
        {
            assert(target != nullptr);
            target = target->next;
            if(target != nullptr)
            {

                ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        Iterator operator++(int num)
        {
            Iterator it(*this);
            operator++();
            return it;
        }
        Iterator &operator--()
        {
            if(target == nullptr)
            {
                target = owner->last;
            }
            else
            {
                target = target->prev;
            }
            if(target != nullptr)
            {
                ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        Iterator operator--(int num)
        {
            Iterator it(*this);
            operator--();
            return it;
        }
        ValueType &operator*() const
        {
            return *val;
        }
        ValueType *operator->() const
        {
            return *val;
        }
        bool operator==(const Iterator &it2)
        {
            return target == it2.target;
        }
        bool operator==(const ConstIterator &it2)
        {
            return target == it2.target;
        }
        bool operator!=(const Iterator &it2)
        {
            return target != it2.target;
        }
        bool operator!=(const ConstIterator &it2)
        {
            return target != it2.target;
        }
    };
    class ConstIterator
    {
    private:
        RBNode *target;
        const Map *owner;
    public:
        const ValueType *val = nullptr;
        ConstIterator(RBNode *node, const Map *owned_by)
        {
            target = node;
            owner = owned_by;
            if(node != nullptr)
            {
                val = new ValueType(*node->key, *node->value);
            }
        }
        ConstIterator(const ConstIterator &it)
        {
            target = it.target;
            owner = it.owner;
            if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
        }
        ConstIterator(const Iterator &it)
        {
            target = it.target;
            owner = it.owned;
            if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
        }
        ~ConstIterator()
        {
            delete val;
        }
        ConstIterator& operator=(const ConstIterator &it)
        {
            target = it.target;
            owner = it.owned;
            if(val != nullptr && it.target != nullptr)
            {
                ValueType *temp = val;
                val = it.val;
                delete temp;
            }
            else if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
        }
        ConstIterator &operator++()
        {
            assert(target != nullptr);
            target = target->next;
            if(target != nullptr)
            {
                const ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        ConstIterator operator++(int num)
        {
            ConstIterator it(*this);
            operator++();
            return it;
        }
        ConstIterator &operator--()
        {
            if(target == nullptr)
            {
                target = owner->last;
            }
            else
            {
                target = target->prev;
            }

            if(target != nullptr)
            {
                ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        ConstIterator operator--(int num)
        {
            ConstIterator it(*this);
            operator--();
            return it;
        }
        const ValueType &operator*() const
        {
            return *val;
        }
        const ValueType *operator->() const
        {
            return ValueType(*target->key, *target->value);
        }
        bool operator==(const Iterator &it2)
        {
            return target == it2.target;
        }
        bool operator==(const ConstIterator &it2)
        {
            return target == it2.target;
        }
        bool operator!=(const Iterator &it2)
        {
            return target != it2.target;
        }
        bool operator!=(const ConstIterator &it2)
        {
            return target != it2.target;
        }
    };
    class ReverseIterator
    {
    private:
        RBNode *target;
        Map *owner;
    public:
        ValueType *val = nullptr;
        ReverseIterator(RBNode *node, Map *owned_by)
        {
            target = node;
            owner = owned_by;
            if(node != nullptr)
            {
                val = new ValueType(*node->key, *node->value);
            }
        }
        ReverseIterator(const ReverseIterator &it)
        {
            target = it.target;
            owner = it.owner;
            if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
        }
        ~ReverseIterator()
        {
            delete val;
        }
        ReverseIterator& operator=(const ReverseIterator &it)
        {
            target = it.target;
            owner = it.owner;
            if(val != nullptr && it.target != nullptr)
            {
                ValueType *temp = val;
                val = it.val;
                delete temp;
            }
            else if(it.target != nullptr)
            {
                val = new ValueType(*it.target->key, *it.target->value);
            }
            return *this;
        }
        ReverseIterator &operator++()
        {
            assert(target != nullptr);
            target = target->prev;
            if(target != nullptr)
            {
                ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        ReverseIterator operator++(int num)
        {
            ReverseIterator it(*this);
            operator++();
            return it;
        }
        ReverseIterator &operator--()
        {
            if(target == nullptr)
            {
                target = owner->first;
            }
            else
            {
                target = target->next;
            }
            if(target != nullptr)
            {
                ValueType *temp = val;
                val = new ValueType(*target->key, *target->value);
                delete temp;
            }
            return *this;
        }
        ReverseIterator operator--(int num)
        {
            ReverseIterator it(*this);
            operator--();
            return it;
        }
        ValueType &operator*() const
        {
            return *val;
        }
        ValueType *operator->() const
        {
            return ValueType(*target->key, *target->value);
        }
        bool operator==(const ReverseIterator &it2)
        {
            return target == it2.target;
        }
        bool operator!=(const ReverseIterator &it2)
        {
            return target != it2.target;
        }
    };

    /*Map Class member functions begin here*/
    Map()
    {

    }

    Map(const Map &Map2)
    {
        auto it = Map2.begin();
        auto it2 = Map2.end();
        insert(it, it2);
    }
    Map &operator=(const Map &Map2)
    {
        this->clear();
        auto it = Map2.begin();
        auto it2 = Map2.end();
        for(; it != it2; it++)
        {
            insert(*it);
        }
    }
    Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> values)
    {
        for(auto element : values)
        {
            insert(element);
        }
    }
    ~Map()
    {

    }
    size_t size() const
    {
        return Curr_Map.size_tree();
    }
    bool empty() const
    {
        return Curr_Map.size_tree() == 0;
    }
    Iterator begin()
    {
        Iterator it = Iterator(first, this);
        return it;
    }
    Iterator end()
    {
        Iterator it = Iterator(nullptr, this);
        return it;
    }
    ConstIterator begin() const
    {
        ConstIterator it = ConstIterator(first, this);
        return it;
    }
    ConstIterator end() const
    {
        ConstIterator it = ConstIterator(nullptr, this);
        return it;
    }
    ReverseIterator rbegin()
    {
        ReverseIterator it = ReverseIterator(last, this);
        return it;
    }
    ReverseIterator rend()
    {
        ReverseIterator it = ReverseIterator(nullptr, this);
        return it;
    }
    Iterator find(const Key_T &key)
    {
        RBNode *temp = Curr_Map.find_node(key);
        return Iterator(temp, this);
    }
    ConstIterator find(const Key_T &key) const
    {
        RBNode *temp = Curr_Map.find_node(key);
        return ConstIterator(temp, this);
    }
    Mapped_T &at(const Key_T &key)
    {
        return Curr_Map.find_val(key);
    }
    const Mapped_T &at(const Key_T &key) const
    {
        return Curr_Map.find_val(key);
    }
    Mapped_T &operator[](const Key_T &key)
    {
        RBNode *temp = Curr_Map.find_node(key);
        if(temp != nullptr)
        {
            return *temp->value;
        }
        else
        {
            mapped = Mapped_T();
            return mapped;
        }
    }
    std::pair<Iterator, bool> insert(const ValueType &value)
    {
        std::pair<RBNode *, bool> ret = Curr_Map.insert_node(value.first, value.second);
        if(ret.second)
        {
            Iterator it = Iterator(ret.first, this);
            return {it, ret.second};
        }
        else
        {
            Iterator it = Iterator(ret.first, this);
            return {it, ret.second};
        }
    }
    template <typename IT_T>
    void insert(IT_T range_beg, IT_T range_end)
    {
        IT_T it(range_beg);
        for(; it != range_end; it++)
        {
            Curr_Map.insert_node((*it).first, (*it).second);
        }
    }
    void erase(Iterator pos)
    {
        Curr_Map.delete_node((*pos).first);
    }
    void erase(const Key_T &key)
    {
        Curr_Map.delete_node(key);
    }
    void clear()
    {
        Curr_Map.delete_map();
    }
    bool operator==(const Map &Map2)
    {
        auto iter = this->begin();
        auto iter2 = Map2.begin();
        if(this->size() == Map2.size()){
            for(; iter != this->end(); iter++)
            {
                if((*iter).second != (*iter2).second)
                {
                    return false;
                }
                iter2++;
            }
        }
        else{
            return false;
        }
        return true;
    }
    bool operator!=(const Map &Map2)
    {
        return !operator==(Map2);
    }
    bool operator<(const Map &Map2)
    {
        auto iter = this->begin();
        auto iter2 = Map2.begin();
        if(this->size() < Map2.size())
        {
            for(; iter != this->end(); iter++)
            {
                if((*iter).second < (*iter2).second)
                {
                    return true;
                }
                else if((*iter2).second < (*iter).second)
                {
                    return false;
                }
                iter2++;
            }
            return true;
        }
        else if(Map2.size() < this->size())
        {
            for(; iter2 != Map2.end(); iter2++)
            {
                if((*iter).second < (*iter2).second)
                {
                    return true;
                }
                else if((*iter2).second < (*iter).second)
                {
                    return false;
                }
                iter++;
            }
            return false;
        }
        else
        {
            for(; iter != this->end(); iter++)
            {
                if((*iter).second < (*iter2).second)
                {
                    return true;
                }
                else if((*iter2).second < (*iter).second)
                {
                    return false;
                }
                iter2++;
            }
            return false;
        }
        return true;
    }
};

#endif // MAP_HPP

}
