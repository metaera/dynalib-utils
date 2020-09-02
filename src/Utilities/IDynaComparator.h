/**
 * @file IDynaComparator.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-21
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef IDYNACOMPARATOR_H
#define IDYNACOMPARATOR_H

int const COMPARE_LESSER  = -1;
int const COMPARE_EQUAL   = 0;
int const COMPARE_GREATER = 1;

template <typename T> class IDynaComparator {
public:
    virtual int compare(T obj1, T obj2) = 0;
};

#endif //IDYNACOMPARATOR_H