/*
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Copyright(c) 2010-2014 Louis Delacroix
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef PPRINT_PPRINT98_H_
#define PPRINT_PPRINT98_H_


#include <ostream>
#include <utility>
#include <iterator>
#include <set>

#ifndef NO_TR1
#  include <tr1/tuple>
#  include <tr1/unordered_set>
#endif

namespace pprint
{

    template <bool, typename S, typename T> struct conditional { };
    template <typename S, typename T> struct conditional<true,  S, T> { typedef S type; };
    template <typename S, typename T> struct conditional<false, S, T> { typedef T type; };

    template <bool, typename T> struct enable_if { };
    template <typename T> struct enable_if<true, T> { typedef T type; };

    // SFINAE type trait to detect whether T::const_iterator exists.

    template<typename T>
    struct has_const_iterator
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;

        template<typename C> static yes test(typename C::const_iterator*);
        template<typename C> static no  test(...);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
        typedef T type;
    };

    // SFINAE type trait to detect whether "T::const_iterator T::begin/end() const" exist.

    template <typename T>
    struct has_begin_end
    {
        struct Dummy { typedef void const_iterator; };
        typedef typename conditional<has_const_iterator<T>::value, T, Dummy>::type TType;
        typedef typename TType::const_iterator iter;

        struct Fallback { iter begin() const; iter end() const; };
        struct Derived : TType, Fallback { };

        template<typename C, C> struct ChT;

        template<typename C> static char (&f(ChT<iter (Fallback::*)() const, &C::begin>*))[1];
        template<typename C> static char (&f(...))[2];
        template<typename C> static char (&g(ChT<iter (Fallback::*)() const, &C::end>*))[1];
        template<typename C> static char (&g(...))[2];

        static bool const beg_value = sizeof(f<Derived>(0)) == 2;
        static bool const end_value = sizeof(g<Derived>(0)) == 2;
    };

    // Basic is_container template; specialize to have value "true" for all desired container types

    template<typename T> struct is_container { static const bool value = has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value; };

    template<typename T, std::size_t N> struct is_container<T[N]> { static const bool value = true; };

    template<std::size_t N> struct is_container<char[N]> { static const bool value = false; };


    // Holds the delimiter values for a specific character type

    template<typename TChar>
    struct delimiters_values
    {
        typedef TChar char_type;
        const TChar * prefix;
        const TChar * delimiter;
        const TChar * postfix;
    };


    // Defines the delimiter values for a specific container and character type

    template<typename T, typename TChar>
    struct delimiters
    {
        typedef delimiters_values<TChar> type;
        static const type values;
    };


    // Default delimiters

    template<typename T> struct delimiters<T, char> { static const delimiters_values<char> values; };
    template<typename T> const delimiters_values<char> delimiters<T, char>::values = { "[", ", ", "]" };
    template<typename T> struct delimiters<T, wchar_t> { static const delimiters_values<wchar_t> values; };
    template<typename T> const delimiters_values<wchar_t> delimiters<T, wchar_t>::values = { L"[", L", ", L"]" };


    // Delimiters for (multi)set and unordered_(multi)set

    template<typename T, typename TComp, typename TAllocator>
    struct delimiters< ::std::set<T, TComp, TAllocator>, char> { static const delimiters_values<char> values; };

    template<typename T, typename TComp, typename TAllocator>
    const delimiters_values<char> delimiters< ::std::set<T, TComp, TAllocator>, char>::values = { "{", ", ", "}" };

    template<typename T, typename TComp, typename TAllocator>
    struct delimiters< ::std::set<T, TComp, TAllocator>, wchar_t> { static const delimiters_values<wchar_t> values; };

    template<typename T, typename TComp, typename TAllocator>
    const delimiters_values<wchar_t> delimiters< ::std::set<T, TComp, TAllocator>, wchar_t>::values = { L"{", L", ", L"}" };

    template<typename T, typename TComp, typename TAllocator>
    struct delimiters< ::std::multiset<T, TComp, TAllocator>, char> { static const delimiters_values<char> values; };

    template<typename T, typename TComp, typename TAllocator>
    const delimiters_values<char> delimiters< ::std::multiset<T, TComp, TAllocator>, char>::values = { "{", ", ", "}" };

    template<typename T, typename TComp, typename TAllocator>
    struct delimiters< ::std::multiset<T, TComp, TAllocator>, wchar_t> { static const delimiters_values<wchar_t> values; };

    template<typename T, typename TComp, typename TAllocator>
    const delimiters_values<wchar_t> delimiters< ::std::multiset<T, TComp, TAllocator>, wchar_t>::values = { L"{", L", ", L"}" };

#ifndef NO_TR1
    template<typename T, typename THash, typename TEqual, typename TAllocator>
    struct delimiters< ::std::tr1::unordered_set<T, THash, TEqual, TAllocator>, char> { static const delimiters_values<char> values; };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    const delimiters_values<char> delimiters< ::std::tr1::unordered_set<T, THash, TEqual, TAllocator>, char>::values = { "{", ", ", "}" };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    struct delimiters< ::std::tr1::unordered_set<T, THash, TEqual, TAllocator>, wchar_t> { static const delimiters_values<wchar_t> values; };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    const delimiters_values<wchar_t> delimiters< ::std::tr1::unordered_set<T, THash, TEqual, TAllocator>, wchar_t>::values = { L"{", L", ", L"}" };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    struct delimiters< ::std::tr1::unordered_multiset<T, THash, TEqual, TAllocator>, char> { static const delimiters_values<char> values; };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    const delimiters_values<char> delimiters< ::std::tr1::unordered_multiset<T, THash, TEqual, TAllocator>, char>::values = { "{", ", ", "}" };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    struct delimiters< ::std::tr1::unordered_multiset<T, THash, TEqual, TAllocator>, wchar_t> { static const delimiters_values<wchar_t> values; };

    template<typename T, typename THash, typename TEqual, typename TAllocator>
    const delimiters_values<wchar_t> delimiters< ::std::tr1::unordered_multiset<T, THash, TEqual, TAllocator>, wchar_t>::values = { L"{", L", ", L"}" };
#endif


    // Delimiters for pair (reused for tuple, see below)

    template<typename T1, typename T2> struct delimiters< ::std::pair<T1, T2>, char> { static const delimiters_values<char> values; };
    template<typename T1, typename T2> const delimiters_values<char> delimiters< ::std::pair<T1, T2>, char>::values = { "(", ", ", ")" };
    template<typename T1, typename T2> struct delimiters< ::std::pair<T1, T2>, wchar_t> { static const delimiters_values<wchar_t> values; };
    template<typename T1, typename T2> const delimiters_values<wchar_t> delimiters< ::std::pair<T1, T2>, wchar_t>::values = { L"(", L", ", L")" };


    // Iterator microtrait class to handle C arrays uniformly

    template <typename T> struct get_iterator { typedef typename T::const_iterator iter; };
    template <typename T, std::size_t N> struct get_iterator<T[N]> { typedef const T * iter; };

    template <typename T> typename enable_if<has_const_iterator<T>::value, typename T::const_iterator>::type begin(const T & c) { return c.begin(); }
    template <typename T> typename enable_if<has_const_iterator<T>::value, typename T::const_iterator>::type end(const T & c) { return c.end(); }
    template <typename T, size_t N> const T * begin(const T(&x)[N]) { return &x[0];     }
    template <typename T, size_t N> const T * end  (const T(&x)[N]) { return &x[0] + N; }

    // Functor to print containers. You can use this directly if you want to specificy a non-default delimiters type.

    template<typename T, typename TChar = char, typename TCharTraits = ::std::char_traits<TChar>, typename TDelimiters = delimiters<T, TChar> >
    struct print_container_helper
    {
        typedef TChar char_type;
        typedef TDelimiters delimiters_type;
        typedef std::basic_ostream<TChar, TCharTraits> ostream_type;
        typedef typename get_iterator<T>::iter TIter;

        print_container_helper(const T & container)
        : _container(container)
        {
        }

        inline void operator()(ostream_type & stream) const
        {
            if (delimiters_type::values.prefix != NULL)
                stream << delimiters_type::values.prefix;

            if (begin(_container) != end(_container))
            for (TIter it = begin(_container), it_end = end(_container); ; )
            {
                stream << *it;

                if (++it == it_end) break;

                if (delimiters_type::values.delimiter != NULL)
                    stream << delimiters_type::values.delimiter;
            }

            if (delimiters_type::values.postfix != NULL)
                stream << delimiters_type::values.postfix;
        }

    private:
        const T & _container;
    };


    // Type-erasing helper class for easy use of custom delimiters.
    // Requires TCharTraits = std::char_traits<TChar> and TChar = char or wchar_t, and MyDelims needs to be defined for TChar.
    // Usage: "cout << pprint::custom_delims<MyDelims>(x)".

    struct custom_delims_base
    {
        virtual ~custom_delims_base() { }
        virtual ::std::ostream & stream(::std::ostream &) = 0;
        virtual ::std::wostream & stream(::std::wostream &) = 0;
    };

    template<typename T, typename Delims>
    struct custom_delims_wrapper : public custom_delims_base
    {
        custom_delims_wrapper(const T & t_) : t(t_) { }

        ::std::ostream & stream(::std::ostream & s)
        {
          return s << ::pprint::print_container_helper<T, char, ::std::char_traits<char>, Delims>(t);
        }
        ::std::wostream & stream(::std::wostream & s)
        {
          return s << ::pprint::print_container_helper<T, wchar_t, ::std::char_traits<wchar_t>, Delims>(t);
        }

    private:
        const T & t;
    };

    template<typename Delims>
    struct custom_delims
    {
        template<typename Container> custom_delims(const Container & c) : base(new custom_delims_wrapper<Container, Delims>(c)) { }
        ~custom_delims() { delete base; }
        custom_delims_base * base;
    };

} // namespace pprint

template<typename TChar, typename TCharTraits, typename Delims>
inline std::basic_ostream<TChar, TCharTraits> & operator<<(std::basic_ostream<TChar, TCharTraits> & s, const pprint::custom_delims<Delims> & p)
{
    return p.base->stream(s);
}

// Template aliases for char and wchar_t delimiters
// Enable these if you have compiler support
//
// Implement as "template<T, C, A> const sdelims::type sdelims<std::set<T,C,A>>::values = { ... }."

//template<typename T> using pp_sdelims = pprint::delimiters<T, char>;
//template<typename T> using pp_wsdelims = pprint::delimiters<T, wchar_t>;


namespace std
{
    // Prints a print_container_helper to the specified stream.

    template<typename T, typename TChar, typename TCharTraits, typename TDelimiters>
    inline basic_ostream<TChar, TCharTraits> & operator<<(basic_ostream<TChar, TCharTraits> & stream,
                                                          const ::pprint::print_container_helper<T, TChar, TCharTraits, TDelimiters> & helper)
    {
        helper(stream);
        return stream;
    }

    // Prints a container to the stream using default delimiters

    template<typename T, typename TChar, typename TCharTraits>
    inline typename ::pprint::enable_if< ::pprint::is_container<T>::value, basic_ostream<TChar, TCharTraits>&>::type
    operator<<(basic_ostream<TChar, TCharTraits> & stream, const T & container)
    {
        return stream << ::pprint::print_container_helper<T, TChar, TCharTraits>(container);
    }

    // Prints a pair to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    inline basic_ostream<TChar, TCharTraits> & operator<<(basic_ostream<TChar, TCharTraits> & stream, const pair<T1, T2> & value)
    {
        if (::pprint::delimiters<pair<T1, T2>, TChar>::values.prefix != NULL)
            stream << ::pprint::delimiters<pair<T1, T2>, TChar>::values.prefix;

        stream << value.first;

        if (::pprint::delimiters<pair<T1, T2>, TChar>::values.delimiter != NULL)
            stream << ::pprint::delimiters<pair<T1, T2>, TChar>::values.delimiter;

        stream << value.second;

        if (::pprint::delimiters<pair<T1, T2>, TChar>::values.postfix != NULL)
            stream << ::pprint::delimiters<pair<T1, T2>, TChar>::values.postfix;

        return stream;
    }
} // namespace std


#ifndef NO_TR1

// Prints a tuple to the stream using delimiters from delimiters<std::pair<tuple_dummy_t, tuple_dummy_t>>.

namespace pprint
{
    struct tuple_dummy_t { }; // Just if you want special delimiters for tuples.

    typedef std::pair<tuple_dummy_t, tuple_dummy_t> tuple_dummy_pair;

    template<typename Tuple, size_t N, typename TChar, typename TCharTraits>
    struct pretty_tuple_helper
    {
        static inline void print(::std::basic_ostream<TChar, TCharTraits> & stream, const Tuple & value)
        {
            pretty_tuple_helper<Tuple, N - 1, TChar, TCharTraits>::print(stream, value);

            if (delimiters<tuple_dummy_pair, TChar>::values.delimiter != NULL)
                stream << delimiters<tuple_dummy_pair, TChar>::values.delimiter;

            stream << std::tr1::get<N - 1>(value);
        }
    };

    template<typename Tuple, typename TChar, typename TCharTraits>
    struct pretty_tuple_helper<Tuple, 1, TChar, TCharTraits>
    {
        static inline void print(::std::basic_ostream<TChar, TCharTraits> & stream, const Tuple & value)
        {
            stream << ::std::tr1::get<0>(value);
        }
    };
} // namespace pprint


/* The following macros allow us to write "template <TUPLE_PARAMAS> std::tuple<TUPLE_ARGS>"
 * uniformly in C++0x compilers and in MS Visual Studio 2010.
 * Credits to STL: http://channel9.msdn.com/Shows/Going+Deep/C9-Lectures-Stephan-T-Lavavej-Advanced-STL-6-of-n
 */

#define TUPLE_PARAMS \
    typename T0, typename T1, typename T2, typename T3, typename T4,      \
    typename T5, typename T6, typename T7, typename T8, typename T9
#define TUPLE_ARGS T0, T1, T2, T3, T4, T5, T6, T7, T8, T9


namespace std
{
    template<typename TChar, typename TCharTraits, TUPLE_PARAMS>
    inline basic_ostream<TChar, TCharTraits> & operator<<(basic_ostream<TChar, TCharTraits> & stream, const tr1::tuple<TUPLE_ARGS> & value)
    {
        if (::pprint::delimiters< ::pprint::tuple_dummy_pair, TChar>::values.prefix != NULL)
            stream << ::pprint::delimiters< ::pprint::tuple_dummy_pair, TChar>::values.prefix;

        ::pprint::pretty_tuple_helper<const tr1::tuple<TUPLE_ARGS> &, tr1::tuple_size<tr1::tuple<TUPLE_ARGS> >::value, TChar, TCharTraits>::print(stream, value);

        if (::pprint::delimiters< ::pprint::tuple_dummy_pair, TChar>::values.postfix != NULL)
            stream << ::pprint::delimiters< ::pprint::tuple_dummy_pair, TChar>::values.postfix;

        return stream;
    }
} // namespace std

#endif // NO_TR1


// A wrapper for raw C-style arrays. Usage: int arr[] = { 1, 2, 4, 8, 16 };  std::cout << wrap_array(arr) << ...

namespace pprint
{
    template<typename T>
    struct array_wrapper_n
    {
        typedef const T * const_iterator;
        typedef T value_type;

        array_wrapper_n(const T * const a, size_t n) : _array(a), _n(n) { }
        inline const_iterator begin() const { return _array; }
        inline const_iterator end() const { return _array + _n; }

    private:
        const T * const _array;
        size_t _n;
    };
} // namespace pprint

template<typename T>
inline pprint::array_wrapper_n<T> pretty_print_array(const T * const a, size_t n)
{
  return pprint::array_wrapper_n<T>(a, n);
}


#endif  // PPRINT_PPRINT98_H_
