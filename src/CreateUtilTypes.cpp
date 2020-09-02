/**
 * @file CreateUtilTypes.cpp
 * @author Ken Kopelson (ken@metaera.com)
 * @brief 
 * @version 0.1
 * @date 2020-01-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Utilities/String.h"
#include "Utilities/IntWrapper.h"
#include "Utilities/DynaListImpl.h"
#include "Utilities/DynaLinkedListImpl.h"
#include "Utilities/DynaArrayImpl.h"
#include "Utilities/DynaHashMapImpl.h"
#include "Utilities/DynaHashSetImpl.h"

class String;

MAKE_MAPTYPE_INSTANCE(String, Integer, StringInt);
MAKE_MAPTYPE_INSTANCE(Char, Integer, CharInt);
MAKE_MAPTYPE_INSTANCE(Index, Index, IndexRef);

MAKE_SETTYPE_INSTANCE(Char, Char);
