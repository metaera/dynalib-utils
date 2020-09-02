
#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

/*******************************************************************/
// class String
// String is a lightweight wrapper class for std::string
// (a.k.a typedef std::basic_string<...>)
//
// The article that describes this class is:
// "String - A lightweight wrapper for std::string"
// and can be found at:
// http://www.codeproject.com/
//
// Features:
// - String contains no templates in the interface (almost).
// - wrappers are provided for iterator and const_iterator
// - no namespace is used
// - the implementation mostly forwards calls to the underlying std::string
//
// the implementation is platform independent;
// _WIN32: automatic adaption to char or wchar_t (according to macro _UNICODE)
//
// introductory material to std::string:
// http://www.msoe.edu/eecs/cese/resources/stl/string.htm
// http://www.cppreference.com/cppstring/
//
// Windows specific:
// The Complete Guide to C++ Strings
// http://www.codeproject.com/string/CPPStringGuide1.asp
// http://www.codeproject.com/string/CPPStringGuide2.asp
/*******************************************************************/


#if defined (_MSC_VER) && _MSC_VER > 1000
#  pragma once
#  pragma warning (disable : 4786)
#endif 

#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <assert.h>
#include <sstream>
#include "IHashable.h"
#include "ICopyable.h"
#include "HashCoder.h"
#include "Utils.h"

//using namespace std;

#if defined (_WIN32)
#  include<tchar.h>  // _TCHAR
#endif

class String : IHashable<std::basic_string<char>>, ICopyable<String> {
// change this typedef to adapt String to any char type if desired
#if defined (_WIN32)
   typedef _TCHAR charT;
#else
   typedef char charT;
#endif
   typedef std::basic_string<charT> string_type;

public:
   // types:
   typedef string_type::traits_type     traits_type;
   typedef string_type::value_type      value_type;
   typedef string_type::allocator_type  allocator_type;
   typedef string_type::size_type       size_type;
   typedef string_type::difference_type difference_type;
   typedef string_type::reference       reference;
   typedef string_type::const_reference const_reference;
   typedef string_type::pointer         pointer;
   typedef string_type::const_pointer   const_pointer;

    int hashCode() const override {
        auto code = HashCoder();
        code.add(imp);
        return code.getCode();
    }
    bool operator== (const std::basic_string<char> &other) const override {
        return imp == other;
    }
    bool operator!= (const std::basic_string<char> &other) const {
        return !(imp == other);
    }

    String* copy() override {
        return new String(*this);
    }


// easily switch from the new iterators below to the 
// basic_string<...>::iterators (you also need to change function base_iter())
#if 0
   typedef string_type::iterator        iterator; //See 23.1
   typedef string_type::const_iterator  const_iterator; // See 23.1

//----- NEW ITERATORs BEGIN -----------------------------------------
#else
   struct iterator {
      typedef std::random_access_iterator_tag iterator_category;
      typedef string_type::value_type         value_type;
      typedef string_type::reference          reference;
      typedef string_type::pointer            pointer;
      typedef string_type::difference_type    difference_type;

      iterator() : iterImp (string_type::iterator()) {}
      iterator (const iterator& other) : iterImp (other.iterImp) {}
   private:
      iterator (const string_type::iterator& other) : iterImp (other) {}
   public:
      reference operator*()  const { return *iterImp; }
      pointer   operator->() const { return &(*iterImp); }
      
      iterator& operator++()   { ++iterImp; return *this; }
      iterator  operator++(int){ return iterImp++; }
      iterator& operator--()   { --iterImp; return *this; }
      iterator  operator--(int){ return iterImp--; }

      iterator  operator+  (difference_type n) const { return (iterImp + n); }
      iterator& operator+= (difference_type n) { iterImp += n; return *this; }
      iterator  operator-  (difference_type n) const  { return (iterImp - n); }
      iterator& operator-= (difference_type n) { iterImp -= n; return *this; }
      difference_type operator- (const iterator& other)  const { return (iterImp - other.iterImp); }
      reference operator[] (difference_type n) const { return iterImp[n]; }

      friend bool operator== (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp == rhs.iterImp;
      }
      friend bool operator!= (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp != rhs.iterImp;
      }
      friend bool operator< (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp < rhs.iterImp;
      }
      friend bool operator> (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp > rhs.iterImp;
      }
      friend bool operator<= (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp <= rhs.iterImp;
      }
      friend bool operator>= (const iterator& lhs, const iterator& rhs) {
         return lhs.iterImp >= rhs.iterImp;
      }
   private:
      friend class String;
      friend struct const_iterator;
      string_type::iterator iterImp;
	};

    struct const_iterator {
        typedef std::random_access_iterator_tag iterator_category;
        typedef string_type::value_type         value_type;
        typedef string_type::const_reference    reference;
        typedef string_type::const_pointer      pointer;
        typedef string_type::difference_type    difference_type;

        const_iterator() : iterImp (string_type::const_iterator()) {}
        const_iterator (const const_iterator& other) : iterImp (other.iterImp) {};
        const_iterator (const iterator& other) : iterImp(other.iterImp) {};
    private:
        const_iterator (const string_type::const_iterator& other) : iterImp (other) {};
    public:
        reference operator*()  const { return *iterImp; }
        pointer   operator->() const { return &(*iterImp); }

        const_iterator& operator++()   { ++iterImp; return *this; }
        const_iterator  operator++(int){ return (const_iterator)iterImp++; }
        const_iterator& operator--()   { --iterImp; return *this; }
        const_iterator  operator--(int){ return (const_iterator)iterImp--; }

        const_iterator  operator+  (difference_type n) const { return (iterImp + n); }
        const_iterator& operator+= (difference_type n) { iterImp += n; return *this; }
        const_iterator  operator-  (difference_type n) const  { return (iterImp - n); }
        const_iterator& operator-= (difference_type n) { iterImp -= n; return *this; }
        difference_type operator- (const const_iterator& other)  const { return (iterImp - other.iterImp); }
        reference operator[] (difference_type n) const { return iterImp[n]; }

        friend bool operator== (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp == rhs.iterImp;
        }
        friend bool operator!= (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp != rhs.iterImp;
        }
        friend bool operator< (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp < rhs.iterImp;
        }
        friend bool operator> (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp > rhs.iterImp;
        }
        friend bool operator<= (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp <= rhs.iterImp;
        }
        friend bool operator>= (const const_iterator& lhs, const const_iterator& rhs) {
            return lhs.iterImp >= rhs.iterImp;
        }
    private:
        friend class String;
        string_type::const_iterator iterImp;
    };
#endif

//----- NEW ITERATORs END -------------------------------------------

    typedef string_type::reverse_iterator       reverse_iterator;
    typedef string_type::const_reverse_iterator const_reverse_iterator;

    enum { npos = size_type (-1) };

    // construct/copy/destroy:
    explicit String() {}
    String (const String& str, size_type pos = 0, size_type n = npos) : imp (str.imp, pos, n) {};
    String (const string_type& str, size_type pos = 0, size_type n = npos) : imp (str, pos, n) {};
    String (const charT* s, size_type n) : imp (s, n)  { assert (s); }
    String (const charT* s) : imp (s) { assert (s); }
    String (size_type n, charT c) : imp (n, c) {};
    template<class InputIterator> 
    String (InputIterator first, InputIterator last) : imp (first, last) {}
    ~String() {
    }
    
    String& operator= (const String& str)     { imp = str.imp; return *this; }
    String& operator= (const string_type& str){ imp = str; return *this; }
    String& operator= (const charT* s)        { assert (s); imp = s; return *this; }
    String& operator= (charT c)               { imp = c; return *this; }

    //   operator string_type() const { return imp; }

    // iterators:
    iterator       begin()                { return imp.begin(); }
    const_iterator begin() const          { return imp.begin(); }
    iterator       end()                  { return imp.end(); }
    const_iterator end() const            { return imp.end(); }
    reverse_iterator       rbegin()       { return imp.rbegin(); }
    const_reverse_iterator rbegin() const { return imp.rbegin(); }
    reverse_iterator       rend()         { return imp.rend(); }
    const_reverse_iterator rend() const   { return imp.rend(); }
    
    // capacity:
    size_type size() const             { return imp.size(); }
    size_type length() const           { return imp.length(); }
    size_type max_size() const         { return imp.max_size(); }
    void resize (size_type n, charT c) { imp.resize (n, c); }
    void resize (size_type n)          { imp.resize (n); }
    size_type capacity() const         { return imp.capacity(); }
    void reserve (size_type n = 0)     { imp.reserve (n); }
    void clear()                       { imp.erase (base_iter(begin()), base_iter(end())); } // no clear() in MSVC++ 6.0
    bool empty() const                 { return imp.empty(); }

    // element access:
    reference       operator[] (size_type pos)       { return imp[pos]; }
    const_reference operator[] (size_type pos) const { return imp[pos]; }
    reference       at (size_type n)                 { return imp.at(n); }
    const_reference at (size_type n) const           { return imp.at(n); }


    // modifiers:
    String& operator+= (const String& str) { imp += (str.imp); return *this; }
    String& operator+= (const charT* s)     { assert(s); imp += (s); return *this; }
    String& operator+= (charT c)            { imp += (c); return *this; }
    String& append (const String& str)     { imp.append(str.imp); return *this; }
    String& append (const String& str, size_type pos, size_type n) { imp.append(str.imp, pos, n); return *this; }
    String& append (const charT* s, size_type n) { assert(s); imp.append (s, n); return *this; }
    String& append (const charT* s)              { assert(s); imp.append (s); return *this; }
    String& append (size_type n, charT c)        { imp.append(n, c); return *this; }
    template<class InputIterator>
    String& append (InputIterator first, InputIterator last) { imp.append (first, last); return *this; }
    void push_back (const charT c)         { imp += (c); } // // no push_back() in MSVC++ 6.0
    
    String& assign (const String& str)   { imp.assign(str.imp); return *this; }
    String& assign (const String& str, size_type pos, size_type n)
                                { imp.assign(str.imp, pos, n); return *this; }
    String& assign (const charT* s, size_type n) { assert(s); imp.assign (s, n); return *this; }
    String& assign (const charT* s)              { assert(s); imp.assign (s); return *this; }
    String& assign (size_type n, charT c)        { imp.assign(n, c); return *this; }
    template<class InputIterator>
    String& assign (InputIterator first, InputIterator last)
                                { imp.assign(first, last); return *this; }

    String& insert (size_type pos1, const String& str)
                                { imp.insert(pos1, str.imp); return *this; }
    String& insert (size_type pos1, const String& str, size_type pos2, size_type n)
                                { imp.insert(pos1, str.imp, pos2, n); return *this; }
    String& insert (size_type pos, const charT* s, size_type n)
                                { assert(s); imp.insert(pos, s, n); return *this; }
    String& insert (size_type pos, const charT* s)
                                { assert(s); imp.insert(pos, s); return *this; }
    String& insert (size_type pos, size_type n, charT c)
                                { imp.insert(pos, n, c); return *this; }
    iterator insert (iterator p, charT c)
                                { return imp.insert(base_iter (p), c); }
    void insert (iterator p, size_type n, charT c)
                                { imp.insert(base_iter (p), n, c); }
    template<class InputIterator>
    void insert (iterator p, InputIterator first, InputIterator last)
                                { imp.insert(base_iter (p), first, last); }

    String& erase (size_type pos = 0, size_type n = npos)
                                { imp.erase(pos, n); return *this; }
    iterator erase (iterator position)
                                { return imp.erase(base_iter(position)); }
    iterator erase (iterator first, iterator last)
                                { return imp.erase(base_iter(first), base_iter(last)); }
    String& replace (size_type pos1, size_type n1, const String& str)
                                { imp.replace(pos1, n1, str.imp); return *this; }
    String& replace (size_type pos1, size_type n1, const String& str, size_type pos2, size_type n2)
                                { imp.replace(pos1, n1, str.imp, pos2, n2); return *this; }
    String& replace (size_type pos, size_type n1, const charT* s, size_type n2)
                                { assert(s); imp.replace(pos, n1, s, n2); return *this; }
    String& replace (size_type pos, size_type n1, const charT* s)
                                { assert (s); imp.replace(pos, n1, s); return *this; }
    String& replace (size_type pos, size_type n1, size_type n2, charT c)
                                { imp.replace(pos, n1, n2, c); return *this; }
    String& replace (iterator& i1, iterator& i2, const String& str)
                                { imp.replace(base_iter (i1), base_iter(i2), str.imp); return *this; }
    String& replace (iterator& i1, iterator& i2, const charT* s, size_type n)
                                { assert(s); imp.replace(base_iter(i1), base_iter(i2), s, n); return *this; }
    String& replace (iterator& i1, iterator& i2, const charT* s)
                                { assert(s); imp.replace(base_iter(i1), base_iter(i2), s); return *this; }
    String& replace (iterator& i1, iterator& i2, size_type n, charT c)
                                { imp.replace(base_iter(i1), base_iter(i2), n, c); return *this; }
    template<class InputIterator>
    String& replace (iterator& i1, iterator& i2,InputIterator j1, InputIterator j2)
                                { imp.replace(base_iter(i1), base_iter(i2), j1, j2); return *this; }
    size_type copy (charT* s, size_type n, size_type pos = 0) const
                                { assert(s); return imp.copy(s, n, pos); }
    

    void swap (String& str)    { imp.swap(str.imp); }
    friend inline void swap (String& lhs, String& rhs) { lhs.swap (rhs); }
    
        // additional swap functions to exchange string contents with std::string (std::basic_string);
    void swap (string_type& str) { imp.swap (str); }
    friend inline void swap (String& lhs, string_type& rhs) { lhs.imp.swap(rhs); }
    friend inline void swap (string_type& lhs, String& rhs) { lhs.swap(rhs.imp); }

    // string operations:
    const charT* c_str() const { return imp.c_str(); } 
    const charT* data() const  { return imp.data(); }

    allocator_type get_allocator() const { return imp.get_allocator(); }

    size_type find (const String& str, size_type pos = 0) const
                                { return imp.find(str.imp, pos); }
    size_type find (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.find(s, pos, n); }
    size_type find (const charT* s, size_type pos = 0) const
                                { assert(s); return imp.find(s, pos); }
    size_type find (charT c, size_type pos = 0) const
                                { return imp.find(c, pos); }
    size_type rfind (const String& str, size_type pos = npos) const
                                { return imp.rfind(str.imp, pos); }
    size_type rfind (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.rfind(s, pos, n); }
    size_type rfind (const charT* s, size_type pos = npos) const
                                { assert(s); return imp.rfind(s, pos); }
    size_type rfind (charT c, size_type pos = npos) const
                                { return imp.rfind(c, pos); }

    size_type find_first_of (const String& str, size_type pos = 0) const
                                { return imp.find_first_of(str.imp, pos); }
    size_type find_first_of (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.find_first_of(s, pos, n); }
    size_type find_first_of (const charT* s, size_type pos = 0) const
                                { assert (s); return imp.find_first_of(s, pos); }
    size_type find_first_of (charT c, size_type pos = 0) const
                                { return imp.find_first_of(c, pos); }

    size_type find_last_of (const String& str, size_type pos = npos) const
                                { return imp.find_last_of(str.imp, pos); }
    size_type find_last_of (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.find_last_of(s, pos, n); }
    size_type find_last_of (const charT* s, size_type pos = npos) const
                                { assert(s); return imp.find_last_of(s, pos); }
    size_type find_last_of (charT c, size_type pos = npos) const
                                { return imp.find_last_of(c, pos); }

    size_type find_first_not_of (const String& str, size_type pos = 0) const
                                { return imp.find_first_not_of(str.imp, pos); }
    size_type find_first_not_of (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.find_first_not_of(s, pos, n); }
    size_type find_first_not_of (const charT* s, size_type pos = 0) const
                                { assert(s); return imp.find_first_not_of(s, pos); }
    size_type find_first_not_of (charT c, size_type pos = 0) const
                                { return imp.find_first_not_of(c, pos); }

    size_type find_last_not_of (const String& str, size_type pos = npos) const
                                { return imp.find_last_not_of(str.imp, pos); }
    size_type find_last_not_of (const charT* s, size_type pos, size_type n) const
                                { assert(s); return imp.find_last_not_of(s, pos, n); }
    size_type find_last_not_of (const charT* s, size_type pos = npos) const
                                { assert(s); return imp.find_last_not_of(s, pos); }
    size_type find_last_not_of (charT c, size_type pos = npos) const
                                { return imp.find_last_not_of(c, pos); }


    // String& ltrim_inplace() {
    //     imp.erase(imp.begin(), find_if(imp.begin(), imp.end(), [](int ch) { return !isspace(ch); }));
    //     return *this;
    // }

    // // trim from end (in place)
    // String& rtrim_inplace() {
    //     imp.erase(find_if(imp.rbegin(), imp.rend(), [](int ch) { return !isspace(ch); }).base(), imp.end());
    //     return *this;
    // }

    // // trim from both ends (in place)
    // String& trim_inplace() {
    //     ltrim_inplace();
    //     rtrim_inplace();
    //     return *this;
    // }

    String trim() const {
        auto wsfront=find_if_not(imp.begin(),imp.end(),[](int c) { return isspace(c);});
        auto wsback=find_if_not(imp.rbegin(),imp.rend(),[](int c) { return isspace(c);}).base();
        return (wsback <= wsfront ? String() : String(wsfront, wsback));
    }

    String* trim_copy() const {
        auto wsfront=find_if_not(imp.begin(),imp.end(),[](int c) { return isspace(c);});
        auto wsback=find_if_not(imp.rbegin(),imp.rend(),[](int c) { return isspace(c);}).base();
        return (wsback <= wsfront ? new String() : new String(wsfront, wsback));
    }

    String& trim_inplace() {
        imp.erase(imp.begin(), find_if(imp.begin(),imp.end(),[](int c) {
             return !isspace(c);
             }));
        imp.erase(find_if(imp.rbegin(),imp.rend(),[](int c) {
             return !isspace(c);
             }).base(), imp.end());
        return *this;
    }

    String tolower() const {
        if (!mapsInitFlag)
            initConvertMaps();
        String str2;
        for (char i : imp)
            str2.append(1, uToL[i]);
        return String(str2);
    }

    String* tolower_copy() const {
        if (!mapsInitFlag)
            initConvertMaps();
        auto str2 = new String();
        for (char i : imp)
            str2->append(1, uToL[i]);
        return str2;
    }

    String& tolower_inplace() {
        if (!mapsInitFlag)
            initConvertMaps();
        for (unsigned long i = 0, len = imp.length(); i < len; ++i)
            imp.replace(i, 1, 1, uToL[imp[i]]);
        return *this;
    }

    String toupper() const {
        if (!mapsInitFlag)
            initConvertMaps();
        String str2;
        for (char i : imp)
            str2.append(1, lToU[i]);
        return String(str2);
    }

    String* toupper_copy() const {
        if (!mapsInitFlag)
            initConvertMaps();
        auto str2 = new String();
        for (char i : imp)
            str2->append(1, lToU[i]);
        return str2;
    }

    String& toupper_inplace() {
        if (!mapsInitFlag)
            initConvertMaps();
        for (unsigned long i = 0, len = imp.length(); i < len; ++i)
            imp.replace(i, 1, 1, lToU[imp[i]]);
        return *this;
    }

    String substr (size_type pos = 0, size_type n = npos) const  {
        if (pos <= size()) { // see C++ Standard 21.3.6.7
            size_type rlen = n < size()-pos ? n : size()-pos;
            return String (c_str() + pos, rlen);
        } else {
            throw std::out_of_range("String::substr");
        }
    }

    std::vector<std::string> split (char delim) const {
        std::stringstream ss(imp);
        std::string item;
        std::vector<std::string> splittedString;
        while (getline(ss, item, delim)) {
            splittedString.push_back(item);
        }
        return splittedString;
    }

    std::vector<std::string> split (const string_type& delim) const {
        std::vector<std::string> splittedString;
        size_type startIndex = 0;
        size_type endIndex = 0;
        while ( (endIndex = imp.find(delim, startIndex)) < imp.size() ) {
            std::string val = imp.substr(startIndex, endIndex - startIndex);
            splittedString.push_back(val);
            startIndex = endIndex + delim.size();
        }
        if (startIndex < imp.size()) {
            std::string val = imp.substr(startIndex);
            splittedString.push_back(val);
        }
        return splittedString;
    }

    int compare (const String& str) const
                                { return imp.compare(str.imp); }
    int compare (size_type pos1, size_type n1, const String& str) const
                                { return imp.compare(pos1, n1, str.imp); }
    int compare (size_type pos1, size_type n1, const String& str, size_type pos2, size_type n2) const
                                { return imp.compare(pos1, n1, str.imp, pos2, n2); }
    int compare (const charT* s) const
                                { assert(s); return imp.compare(s); }
    int compare (size_type pos1, size_type n1, const charT* s, size_type n2 = npos) const
                                { assert(s); return imp.compare(pos1, n1, s, n2); }
    
    friend inline String operator+ (const String& lhs, const String& rhs) {
        return String (lhs, rhs, false);
    }
    friend inline String operator+ (const charT* lhs, const String& rhs) {
        assert(lhs);
        return String (lhs, rhs);
    }
    friend inline String operator+ (charT lhs, const String& rhs) {
        return String(lhs, rhs);
    }
    friend inline String operator+ (const String& lhs, const charT* rhs) {
        assert(rhs);
        return String(lhs, rhs);
    }
    friend inline String operator+ (const String& lhs, charT rhs) {
        return String(lhs, rhs);
    }

    friend inline bool operator== (String& lhs, String& rhs) { return lhs.imp == rhs.imp; }
    friend inline bool operator== (const charT* lhs, String& rhs) {
        assert(lhs);
        return lhs == rhs.imp;
    }
    friend inline bool operator== (String& lhs, const charT* rhs) {
        assert(rhs);
        return lhs.imp == rhs;
    }
    friend inline bool operator<  (String& lhs, String& rhs) { return lhs.imp < rhs.imp; }
    friend inline bool operator<  (String& lhs, const charT* rhs) {
        assert(rhs);
        return lhs.imp < rhs;
    }
    friend inline bool operator<  (const charT* lhs, String& rhs) {
        assert(lhs);
        return lhs < rhs.imp;
    }
    friend inline bool operator== (const String& lhs, const String& rhs) { return lhs.imp == rhs.imp; }
    friend inline bool operator== (const charT* lhs, const String& rhs) {
        assert(lhs);
        return lhs == rhs.imp;
    }
    friend inline bool operator== (const String& lhs, const charT* rhs) {
        assert(rhs);
        return lhs.imp == rhs;
    }
    friend inline bool operator<  (const String& lhs, const String& rhs) { return lhs.imp < rhs.imp; }
    friend inline bool operator<  (const String& lhs, const charT* rhs) {
        assert(rhs);
        return lhs.imp < rhs;
    }
    friend inline bool operator<  (const charT* lhs, const String& rhs) {
        assert(lhs);
        return lhs < rhs.imp;
    }

    friend inline bool operator!= (String& lhs, String& rhs)        { return !(lhs == rhs); }
    friend inline bool operator!= (const charT* lhs, String& rhs)   { return !(lhs == rhs); }
    friend inline bool operator!= (String& lhs, const charT* rhs)   { return !(lhs == rhs); }
    friend inline bool operator>  (String& lhs, String& rhs)        { return rhs < lhs; }
    friend inline bool operator>  (String& lhs, const charT* rhs)   { return rhs < lhs; }
    friend inline bool operator>  (const charT* lhs, String& rhs)   { return rhs < lhs; }
    friend inline bool operator<= (String& lhs, String& rhs)        { return !(rhs < lhs); }
    friend inline bool operator<= (String& lhs, const charT* rhs)   { return !(rhs < lhs); }
    friend inline bool operator<= (const charT* lhs, String& rhs)   { return !(rhs < lhs); }
    friend inline bool operator>= (String& lhs, String& rhs)        { return !(lhs < rhs); }
    friend inline bool operator>= (String& lhs, const charT* rhs)   { return !(lhs < rhs); }
    friend inline bool operator>= (const charT* lhs, String& rhs)   { return !(lhs < rhs); }

    friend inline bool operator!= (const String& lhs, const String& rhs)  { return !(lhs == rhs); }
    friend inline bool operator!= (const charT* lhs, const String& rhs)   { return !(lhs == rhs); }
    friend inline bool operator!= (const String& lhs, const charT* rhs)   { return !(lhs == rhs); }
    friend inline bool operator>  (const String& lhs, const String& rhs)  { return rhs < lhs; }
    friend inline bool operator>  (const String& lhs, const charT* rhs)   { return rhs < lhs; }
    friend inline bool operator>  (const charT* lhs, const String& rhs)   { return rhs < lhs; }
    friend inline bool operator<= (const String& lhs, const String& rhs)  { return !(rhs < lhs); }
    friend inline bool operator<= (const String& lhs, const charT* rhs)   { return !(rhs < lhs); }
    friend inline bool operator<= (const charT* lhs, const String& rhs)   { return !(rhs < lhs); }
    friend inline bool operator>= (const String& lhs, const String& rhs)  { return !(lhs < rhs); }
    friend inline bool operator>= (const String& lhs, const charT* rhs)   { return !(lhs < rhs); }
    friend inline bool operator>= (const charT* lhs, const String& rhs)   { return !(lhs < rhs); }

    friend inline std::basic_ostream<charT, string_type::traits_type>&
    operator<< (std::basic_ostream<charT, string_type::traits_type>& os, const String& str) {
        return os << str.imp;
    }

    friend inline std::basic_istream <charT, string_type::traits_type>&
    operator>> (std::basic_istream<charT, string_type::traits_type>& is, String& str) {
        is >> str.imp;
        return is;
    }

    friend inline std::basic_istream<charT, string_type::traits_type>&
    getline(std::basic_istream<charT, string_type::traits_type>& is, String& str, charT delim) {
        std::getline(is, str.imp, delim);
        return is;
    }

    friend inline std::basic_istream<charT, string_type::traits_type>&
        getline(std::basic_istream<charT, string_type::traits_type>& is, String& str) {
        return getline(is, str.imp, is.widen('\n'));
    }

private:
    // private constructors, used only to facilitate RVO for operator+() 
    
    // the third argument is not used, it disambiguates parsing for older compilers
    explicit String (const String& lhs, const String& rhs, bool/*not used*/) {
        imp.reserve(lhs.length() + rhs.length());
            imp.append(lhs.imp).append (rhs.imp);
    }
    explicit String (const charT* lhs, const String& rhs) {
        assert(lhs);
        size_type lhsLen = std::char_traits<charT>::length (lhs);
        imp.reserve(lhsLen + rhs.length());
        imp.append(lhs, lhsLen).append (rhs.imp);
    }
    explicit String (charT lhs, const String& rhs) {
        imp.reserve(1 + rhs.length());
        imp.append(1, lhs).append (rhs.imp);
    }
    explicit String (const String& lhs, const charT* rhs) {
        assert(rhs);
        size_type rhsLen = std::char_traits<charT>::length (rhs);
        imp.reserve(lhs.length() + rhsLen);
        imp.append(lhs.imp).append (rhs, rhsLen);
    }
    explicit String (const String& lhs, charT rhs) {
        imp.reserve(lhs.length() + 1);
        imp.append(lhs.imp).append (1, rhs);
    }

    string_type::iterator base_iter (iterator iter)  { 
#if 0 // for basic_string<...>::iterator 
      return iter; 
#else // for new lstring::iterator
    return iter.iterImp; 
#endif
   }
   
   string_type imp;
};

// _UNICODE related Macros
#if !defined(_T)
#  if defined (_WIN32) && defined (_UNICODE)
#    define _T(x) L##x
#  else
#    define _T(x) x
#  endif
#endif


#endif // STRING_H_INCLUDED
