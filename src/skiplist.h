/****************************************************************************
 * 
 * Copyright (C) 2016 Baidu.com, Inc. All rights reserved.
 * 
 ****************************************************************************/



/** 
 * @file skiplist.h
 * @author yinchunxiang(com@baidu.com)
 * @date 2016/02/01 14:06:08
 * @brief 
 * 
 **/ 
 
#pragma once

#include "skiplist_internal.h"

namespace internal {
    template <typename T> class SkipListImpl;
    template <typename T> class SkipListIterator;
    template <typename T> class SkipListConstIterator;
};

template <typename T>
class SkipList {
    protected:
        typedef typename internal::SkipListImpl<T> impl_type;
        typedef typename impl_type::NodeType node_type;
        template <typename T1> friend class internal::SkipListIterator;
        template <typename T1> friend class internal::SkipListConstIterator;

    public:
        typedef T                                                       value_type;
        typedef typename impl_type::SizeType                            size_type;
        typedef typename internal::SkipListIterator<impl_type>           iterator;
        typedef typename internal::SkipListConstIterator<impl_type>      const_iterator;
        typedef std::reverse_iterator<iterator>                         reverse_iterator;
        typedef std::reverse_iterator<const_iterator>                   const_reverse_iterator;
        typedef typename impl_type::Reference                           reference;
        typedef typename impl_type::ConstReference                      const_reference;
        typedef typename impl_type::Pointer                             pointer;
        typedef typename impl_type::ConstPointer                        const_pointer;

        typedef std::pair<iterator, bool> insert_by_value_result;


        /// 默认构造函数
        SkipList() {
            //TODO
        }

        /// 
        SkipList(const SkipList &other) {
            //TODO
        }

        SkipList operator= (const SkipList &other) {
            //TODO
        }

        insert_by_value_result insert(const T& value) {
            //TODO
            node_type *node = impl_.insert(value, nullptr);
            return std::make_pair(iterator(&impl_, node), impl_.valid_ptr(node));
            
        }

        iterator erase(const T& value) {
            //TODO
        }

        iterator erase(const_iterator position) {
            //TODO
        }

        iterator erase(const_iterator first, const_iterator last) {
            //TODO
        }

        size_type size() const {
            return impl_.size();
        }

        unsigned levels() const {
            return impl_.levels();
        }

        void swap(SkipList &other) {
            impl_.swap(other.impl_);
        }

        friend void swap(SkipList &lhs, SkipList &rhs) {
            lhs.swap(rhs);
        }

        template<typename Stream>
        void dump(Stream &s) {
            impl_.dump(s);
        }

    protected:
        impl_type impl_;

    private:

};
 
 
 
 
 
 
 
 
 
 
 
 
 
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
