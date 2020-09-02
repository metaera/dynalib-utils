/**
 * @file DynaRTTI.h
 * @author Ken Kopelson (ken@metaera.com)
 * @brief Provides fast RTTI abilities in C++, loosely based off LLVM techniques
 * @version 0.1
 * @date 2019-12-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

/**
 * @brief Creates an enum of an entire class tree.
 * 
 * Each class is specified with two entries
 * Each branch starts with _FT_X and ends with _LT_X.
 * 
 * {_FT_Shape,
 *      _FT_Square,
 *          _FT_Rectangle, _LT_Rectangle,
 *      _LT_Square,
 *      _FT_Ellipse,
 *          _FT_Circle, _LT_Circle,
 *      _LT_Ellipse,
 *  _LT_Shape
 * }
 * 
 * This macro should be specified in the protected section of your base class 
 */
#define RTTI_BASE_CLASS_TREE(...) \
    enum RTTI{ __VA_ARGS__ }; \
    static RTTI _rttiTree;

#define RTTI_BASE_CLASS_PRIVATE \
    const RTTI _classType;

#define RTTI_BASE_CLASS_PROTECTED_CONSTRUCTOR(BASE_CLASS) \
    BASE_CLASS(RTTI classType) : _classType(classType) { \
    }


#define RTTI_BASE_CLASS_PUBLIC \
    const RTTI getClassType() const { return _classType; }


#define RTTI_PROTECTED_CONSTRUCTOR(PARENT_CLASS, THIS_CLASS) \
    THIS_CLASS(RTTI classType) : PARENT_CLASS(classType) { \
    }

#define RTTI_PUBLIC_DEFAULT_CONSTRUCTOR(THIS_CLASS, RTTI_TYPE) \
    THIS_CLASS() : THIS_CLASS(_FT_##RTTI_TYPE) { \
    }

/**
 * @brief This macro should be specified in the public section of all classes that
 * derive from the base class.
 * 
 * @param BASE_CLASS - This specifies the actual base class name
 * @param RTTI_TYPE - This specifies the root part of the RTTI part (without _FT_ or _LT_)
 * 
 */
#define RTTI_PUBLIC_METHODS(BASE_CLASS, RTTI_TYPE) \
    static bool isTypeOf(const BASE_CLASS *clazz) { \
        const RTTI rttiType = clazz->getClassType(); \
        return (rttiType >= _FT_##RTTI_TYPE && rttiType <= _LT_##RTTI_TYPE); \
    }
