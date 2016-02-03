/****************************************************************************
 * 
 * Copyright (C) 2016 Baidu.com, Inc. All rights reserved.
 * 
 ****************************************************************************/



/** 
 * @file skiplist_internal.h
 * @author yinchunxiang(com@baidu.com)
 * @date 2016/02/01 15:15:04
 * @brief 
 * 
 **/ 

#include <iostream>
#include <assert.h>
#include <vector>
#include <iterator>
#include <random>
 
namespace internal {

#define output(message) \
    do {\
        std::cout << __FILE__ << ":" << __func__ << ":" << __LINE__ << " [" << message << "]" << std::endl;\
    }while(0)

/*
void output(const std::string &message) {
    std::cout << __FILE__ << ":" << __func__ << ":" << __LINE__ << " [" << message << "]" << std::endl;
}
*/

template <typename T>
class SkipListImpl;

template <typename T>
class SkipListIterator;

template <typename T>
class SkipListConstIterator;


const unsigned kMagicGood   = 0x01020304;
const unsigned kMagicBad    = 0xfefefefe;

template <typename T> 
bool operator==(const T &lhs, const T &rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
}

template <typename T>
bool operator<=(const T &lhs, const T &rhs) {
    return !(rhs < lhs);
}

/// =============== SkipListNode ======================

template <typename T> 
struct SkipListNode {
    typedef SkipListNode<T> SelfType;

    SkipListNode(const T &v, unsigned level) 
    : value_(v),  level_(level) {
    }

    ~SkipListNode() {
        if (magic_ != kMagicGood) {
            std::cout << "Magic is not good" << std::endl;
            return;
        }
        magic_ = kMagicBad;
        for (unsigned i = 0; i < level_ + 1; ++i) {
            next_[i] = nullptr;
        }
        prev_ = nullptr;
    }

    unsigned    magic_  = kMagicGood;
    T           value_;
    unsigned    level_;
    SelfType    *prev_  = nullptr;
    std::vector<SelfType*>  next_ = std::vector<SelfType*>(level_ + 1, nullptr);
};


/// ========= SkipListImpl ==============

template <typename T> 
class SkipListImpl {
public:
    typedef T                                       ValueType;
    typedef SkipListNode<T>                         NodeType;
    typedef std::allocator<T>                       Allocator;
    typedef typename Allocator::size_type           SizeType;
    typedef typename Allocator::difference_type     DifferenceType;
    typedef typename Allocator::const_reference     ConstReference;
    typedef typename Allocator::reference           Reference;
    typedef typename Allocator::const_pointer       ConstPointer;
    typedef typename Allocator::pointer             Pointer;

    static const unsigned kMaxLevel = 4;

    SkipListImpl() {
        for (unsigned i = 0; i < kMaxLevel; ++i) {
            head_ptr_->next_[i] = tail_ptr_;
            tail_ptr_->next_[i] = nullptr;
        }
        tail_ptr_->prev_ = head_ptr_;
    }

    ~SkipListImpl() {
        //TODO: destruct all item
    }

    unsigned new_level() {
        /// make sure levels_ <= 32

        //srand(time(NULL));
        std::random_device rd;
        unsigned level = 0;
        //int number = rand();
        int number = rd();
        for (int i = 0; i < kMaxLevel - 1; ++i) {
            if (number & 1) {
                level += 1;
            }
            number >> 1;
        }
        if (level >= levels_) {
            level = levels_;
            levels_ += 1;
        }
        return level;
    }

    bool valid_ptr(const NodeType *node) {
        return node && node != head_ptr_ && node != tail_ptr_;
    }

    NodeType *insert(const ValueType &value, NodeType* hint = nullptr) {
        output("========> insert start ...");
        const int insert_level = new_level();
        std::cout << "new_level: " << insert_level << std::endl;
        NodeType *new_node = new (std::nothrow)NodeType(value, insert_level);
        if (nullptr == new_node) {
            std::cout << "new node failed" << std::endl;
            return new_node;
        }
        output("=============> 1111111111111");
        const bool good_hint = valid_ptr(hint) && hint->level_ == levels_ - 1;
        
        output("=============> 2222222222222");
        NodeType *insert_point = good_hint ? hint : head_ptr_;
        {
            std::string msg = "levels:" + std::to_string(levels_) + " insert_level:" + std::to_string(insert_level);
            output(msg);
        }
        for (int level_index = levels_ - 1; level_index >= 0; --level_index) {
            std::string msg = "start to insert at level" + std::to_string(level_index);
            output(msg);
            while (insert_point->next_[level_index] != tail_ptr_ 
                    && insert_point->next_[level_index]->value_ < value) {
                insert_point = insert_point->next_[level_index];
            }
            
            /// 所有小于等于insert_level的层都要进行插入
            //assert(level_index <= insert_level);
            if (level_index <= insert_level) {
                NodeType *next = insert_point->next_[level_index];
                new_node->next_[level_index] = next;
                insert_point->next_[level_index] = new_node;
            }
        }
        output("=============> 33333333333");

        /// set prev
        assert(insert_point->next_[0] == new_node);
        NodeType *next = new_node->next_[0];
        new_node->prev_ = insert_point;
        next->prev_ = new_node;
        item_count_ += 1;
        output("========> insert end ...");
        return new_node;
    }

    void remove(NodeType *node) {
        if (!valid_ptr(node)) {
            return;
        }

        node->next[0]->prev_ = node->prev_;
        NodeType *cur = head_ptr_;
        for (unsigned level_index = levels_ - 1; level_index >= 0; --level_index) {
            if (level_index > cur->level_) {
                return;
            }
            while (cur->next_[level_index] != tail_ptr_ 
                    && cur->next_[level_index]->value_ < node->value_) {
                cur = cur->next_[level_index];
            }
            NodeType *temp = cur;
            while (temp != tail_ptr_) {
                NodeType *next = temp->next_[level_index];
                if (next == tail_ptr_) {
                    break;
                }
                if (next == node) {
                    cur = temp;
                    break;
                }
                if (temp->next_[level_index]->value_ == node->value_) {
                    temp = next; //继续查找
                } else {
                    break;
                }
            }
            if (cur->next_[level_index] == node) {
                cur->next_[level_index] = node->next_[level_index];
            }
        }
        delete node;
        item_count_ -= 1;
        return;
    }

    void remove_between(NodeType *begin, NodeType *end) {
        if (!valid_ptr(begin) || !valid_ptr(end)) {
            return;
        }
        NodeType * const prev = begin->prev_;
        NodeType * const next_of_end = end->next_[0];
        const ValueType &begin_value = begin->value_;
        const ValueType &end_value = end->value_;
        next_of_end->prev_ = prev;
        NodeType *cur = head_ptr_;
        for (unsigned level = levels_ - 1; level >= 0; --level) {
            assert(level <= cur->level_);
            while (cur->next_[level] != tail_ptr_
                    && cur->next_[level]->value_ < begin_value) {
                cur = cur->next_[level];
            }
            if (cur->next_[level] != tail_ptr_
                    && cur->next_[level]->value_ <= end_value) {
                NodeType *last = cur->next_[level];
                while (last != tail_ptr_ && last->value_ <= end_value) {
                    last = last->next_[level];
                }
                cur->next_[level] = last;
            }
        }
    }

    void swap(SkipListImpl &other) {
        std::swap(head_node_, other.head_node_);
        std::swap(head_ptr_, other.head_ptr_);
        std::swap(tail_node_, other.tail_node_);
        std::swap(tail_ptr_, other.tail_ptr_);
        std::swap(item_count_, other.item_count_);
        std::swap(levels_, other.levels_);
    }

    void clear() {
        NodeType *node = head_ptr_->next_[0];
        while (node != tail_ptr_) {
            NodeType *next = node->next[0];
            if (node) {
                delete node;
            }
            node = next;
        }
        for (unsigned level = 0; level < kMaxLevel; ++level) {
            head_ptr_->next_[level] = tail_ptr_;
        }
        tail_ptr_->prev_ = head_ptr_;
        item_count_ = 0;
        return;
    }


    NodeType *find(const ValueType &value) {
        NodeType *node = &head_ptr_;
        for (unsigned level = levels_ - 1; level >= 0; --level) {
            while (node->next_[level] != &tail_ptr_) {
                const ValueType &current_value = node->next_[level].value_;
                if (current_value < value) {
                    node = node->next_[level];
                }
            }
        }
        return node;
    }

    NodeType *find_first(const ValueType &value) {
        NodeType *node = find(value);
        while (node != &head_ptr_ && node->prev_->value_ == value) {
            node = node->prev_;
        }
        if (node != &tail_ptr_ && node->value_ < value) {
            node = node->next_[0];
        }
        return node;
    }

    SizeType size() const {
        return item_count_;
    }

    unsigned levels() const {
        return levels_;
    }

    template <typename Stream>
    void dump(Stream &s) {
        s << "SkipList(size=" << item_count_ << ", levels=" << levels_ << ")\n";
        for (unsigned level = 0; level < levels_; ++level) {
            s << "  [" << level << "]";
            NodeType *node = head_ptr_;
            while (node) {
                NodeType *next = node->next_[level];
                bool prev_ok = false;
                if (next) {
                    if (next->prev_ == node) prev_ok = true;
                }
                if (valid_ptr(node)) {
                    s << node->value_;
                } else {
                    s << "*";
                }
                if (valid_ptr(node)) {
                    if (next != tail_ptr_ && node->value_ > next->value_) {
                        s << "*XXXXXXXXX*";
                    } else {
                        if (next != tail_ptr_ && next->value_ >= node->value_) {
                            s << "*XXXXXXXXX*";
                        }
                    }
                }
                s << "> ";
                if (valid_ptr(node)) {
                    s << (prev_ok ? "<" : "-");
                }
                node = next;
            }
            s << "\n";
        }
    }


    SizeType count(const ValueType &value) {
        //TODO
        return 0;
    }


private:
    SkipListImpl(const SkipListImpl &other);
    SkipListImpl operator=(const SkipListImpl &other);

    NodeType    head_node_      = NodeType(T(), kMaxLevel);
    NodeType    tail_node_      = NodeType(T(), kMaxLevel);
    NodeType    *head_ptr_      = &head_node_;
    NodeType    *tail_ptr_      = &tail_node_;

    SizeType    item_count_     = 0;
    int         levels_         = 0;
};



template <typename T>
class SkipListIterator : public std::iterator<std::bidirectional_iterator_tag, int> {
public:
    typedef T                                       ImplType;
    typedef typename ImplType::NodeType             NodeType;
    typedef SkipListIterator<ImplType>              SelfType;
    typedef typename ImplType::ConstReference       ConstReference;
    typedef typename ImplType::ConstPointer         ConstPointer;
    typedef SkipListConstIterator<ImplType>                 ConstIterator;

    SkipListIterator() {
    }

    SkipListIterator(ImplType *impl, NodeType *node) : impl_(impl), node_(node) {
    }

    SelfType &operator++() {
        node_ = node_->next_[0];
        return *this;
    }

    SelfType operator++(int) {
        SelfType old(*this);
        node_ = node_->next[0];
        return old;
    }

    SelfType &operator--() {
        node_ = node_->prev_;
        return *this;
    }

    SelfType operator--(int) {
        SelfType old(*this);
        node_ = node_->prev_;
        return old;
    }

    ConstReference operator*() {
        return node_->value_;
    }

    ConstPointer operator->() {
        return node_->value_;
    }

    bool operator==(const SelfType &other) {
        return node_ == other.node_;
    }

    bool operator!=(const SelfType &other) {
        return !operator==(other);
    }

    bool operator==(const ConstIterator &other) {
        return node_ == other.node();
    }

    bool operator!=(const ConstIterator &other) {
        return !operator==(other);
    }

    const ImplType *impl() {
        return impl_;
    }
    const NodeType *node() {
        return node_;
    }

private:
    ImplType *impl_ = nullptr;
    NodeType *node_ = nullptr;
};

template <typename T>
class SkipListConstIterator : public std::iterator<std::bidirectional_iterator_tag, int> {

public:
    typedef const T                             ImplType;
    typedef SkipListIterator<ImplType>          Iterator;
    typedef const typename ImplType::NodeType   NodeType;
    typedef SkipListConstIterator<ImplType>     SelfType;
    typedef typename ImplType::ConstReference   ConstReference;
    typedef typename ImplType::ConstPointer     ConstPointer;

    SkipListConstIterator() {
    }

    SkipListConstIterator(const Iterator &other) : impl_(other.impl()), node_(other.node()){
    }

    SkipListConstIterator(ImplType *impl, NodeType* node) : impl_(impl), node_(node){
    }

    SelfType &operator++() {
        node_ = node_->next_[0];
        return *this;
    }

    SelfType operator++(int) {
        SelfType old(*this);
        node_ = node_->next[0];
        return old;
    }

    SelfType &operator--() {
        node_ = node_->prev_;
        return *this;
    }

    SelfType operator--(int) {
        SelfType old(*this);
        node_ = node_->prev_;
        return old;
    }

    ConstReference operator*() {
        return node_->value_;
    }

    ConstPointer operator->() {
        return node_->value_;
    }

    bool operator==(const SelfType &other) {
        return node_ == other.node_;
    }

    bool operator!=(const SelfType &other) {
        return !operator==(other);
    }

    bool operator==(const Iterator &other) {
        return node_ == other.node();
    }

    bool operator!=(const Iterator &other) {
        return !operator==(other);
    }

    ImplType *impl() {
        return impl_;
    }

    NodeType *node() {
        return node_;
    }

private:
    ImplType *impl_ = nullptr;
    NodeType *node_ = nullptr;
};



} //end of namespace internal
 
 
 
 
 
 
 
 
 
 
 
 
 
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
