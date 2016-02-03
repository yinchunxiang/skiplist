/****************************************************************************
 * 
 * Copyright (C) 2016 Baidu.com, Inc. All rights reserved.
 * 
 ****************************************************************************/



/** 
 * @file test.cc
 * @author yinchunxiang(com@baidu.com)
 * @date 2016/02/02 14:21:39
 * @brief 
 * 
 **/ 
 
#include "skiplist.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <random>

int main(int argc, const char *argv[])
{
    //srand(time(NULL));
    //for (int i = 0; i < 10; ++i) {
    //    std::cout << rand() % 32 << std::endl;
    //}
    //return 0;
    std::random_device rd;
    SkipList<int> skip_list;
    skip_list.dump<std::ostream>(std::cout);
    for (int i = 0; i < 10; ++i) {
        //int r = rand() % 10;
        unsigned r = ((unsigned)rd()) % 33;
        std::cout << "insert value: " << r << std::endl;
        //r = 10 - i;
        skip_list.insert(r);
    }

    skip_list.dump<std::ostream>(std::cout);
    return 0;
}
 
 
 
 
 
 
 
 
 
 
 
 
 
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
