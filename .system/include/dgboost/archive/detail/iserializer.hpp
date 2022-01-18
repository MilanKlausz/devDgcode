#ifndef BOOST_ARCHIVE_DETAIL_ISERIALIZER_HPP
#define BOOST_ARCHIVE_DETAIL_ISERIALIZER_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#pragma inline_depth(511)
#pragma inline_recursion(on)
#endif

#if defined(__MWERKS__)
#pragma inline_depth(511)
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// iserializer.hpp: interface for serialization system.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <new>     // for placement new
#include <cstddef> // size_t, NULL

#include <dgboost/config.hpp>
#include <dgboost/detail/workaround.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::size_t; 
} // namespace std
#endif

#include <dgboost/static_assert.hpp>

#include <dgboost/mpl/eval_if.hpp>
#include <dgboost/mpl/identity.hpp>
#include <dgboost/mpl/greater_equal.hpp>
#include <dgboost/mpl/equal_to.hpp>
#include <dgboost/core/no_exceptions_support.hpp>

#ifndef BOOST_SERIALIZATION_DEFAULT_TYPE_INFO   
    #include <dgboost/serialization/extended_type_info_typeid.hpp>   
#endif
#include <dgboost/serialization/throw_exception.hpp>
#include <dgboost/serialization/smart_cast.hpp>
#include <dgboost/serialization/static_warning.hpp>

#include <dgboost/type_traits/is_pointer.hpp>
#include <dgboost/type_traits/is_enum.hpp>
#include <dgboost/type_traits/is_const.hpp>
#include <dgboost/type_traits/remove_const.hpp>
#include <dgboost/type_traits/remove_extent.hpp>
#include <dgboost/type_traits/is_polymorphic.hpp>

#include <dgboost/serialization/assume_abstract.hpp>

#if !defined(BOOST_MSVC) && \
    (BOOST_WORKAROUND(__IBMCPP__, < 1210) || \
    defined(__SUNPRO_CC) && (__SUNPRO_CC < 0x590))
    #define DONT_USE_HAS_NEW_OPERATOR 1
#else
    #define DONT_USE_HAS_NEW_OPERATOR 0
#endif

#if ! DONT_USE_HAS_NEW_OPERATOR
#include <dgboost/type_traits/has_new_operator.hpp>
#endif

#include <dgboost/serialization/serialization.hpp>
#include <dgboost/serialization/version.hpp>
#include <dgboost/serialization/level.hpp>
#include <dgboost/serialization/tracking.hpp>
#include <dgboost/serialization/type_info_implementation.hpp>
#include <dgboost/serialization/nvp.hpp>
#include <dgboost/serialization/void_cast.hpp>
#include <dgboost/serialization/collection_size_type.hpp>
#include <dgboost/serialization/singleton.hpp>
#include <dgboost/serialization/wrapper.hpp>
#include <dgboost/serialization/array_wrapper.hpp>

// the following is need only for dynamic cast of polymorphic pointers
#include <dgboost/archive/archive_exception.hpp>
#include <dgboost/archive/detail/basic_iarchive.hpp>
#include <dgboost/archive/detail/basic_iserializer.hpp>
#include <dgboost/archive/detail/basic_pointer_iserializer.hpp>
#include <dgboost/archive/detail/archive_serializer_map.hpp>
#include <dgboost/archive/detail/check.hpp>

#include <dgboost/core/addressof.hpp>

namespace dgboost {} namespace boost = dgboost; namespace dgboost {

namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {

// an accessor to permit friend access to archives.  Needed because
// some compilers don't handle friend templates completely
class load_access {
public:
    template<class Archive, class T>
    static void load_primitive(Archive &ar, T &t){
        ar.load(t);
    }
};

namespace detail {

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

template<class Archive, class T>
class iserializer : public basic_iserializer
{
private:
    virtual void destroy(/*const*/ void *address) const {
        dgboost::serialization::access::destroy(static_cast<T *>(address));
    }
public:
    explicit iserializer() :
        basic_iserializer(
            dgboost::serialization::singleton<
                typename 
                dgboost::serialization::type_info_implementation< T >::type
            >::get_const_instance()
        )
    {}
    virtual BOOST_DLLEXPORT void load_object_data(
        basic_iarchive & ar,
        void *x, 
        const unsigned int file_version
    ) const BOOST_USED;
    virtual bool class_info() const {
        return dgboost::serialization::implementation_level< T >::value 
            >= dgboost::serialization::object_class_info;
    }
    virtual bool tracking(const unsigned int /* flags */) const {
        return dgboost::serialization::tracking_level< T >::value 
                == dgboost::serialization::track_always
            || ( dgboost::serialization::tracking_level< T >::value 
                == dgboost::serialization::track_selectively
                && serialized_as_pointer());
    }
    virtual version_type version() const {
        return version_type(::dgboost::serialization::version< T >::value);
    }
    virtual bool is_polymorphic() const {
        return dgboost::is_polymorphic< T >::value;
    }
    virtual ~iserializer(){};
};

#ifdef BOOST_MSVC
#  pragma warning(pop)
#endif

template<class Archive, class T>
BOOST_DLLEXPORT void iserializer<Archive, T>::load_object_data(
    basic_iarchive & ar,
    void *x, 
    const unsigned int file_version
) const {
    // note: we now comment this out. Before we permited archive
    // version # to be very large.  Now we don't.  To permit
    // readers of these old archives, we have to suppress this 
    // code.  Perhaps in the future we might re-enable it but
    // permit its suppression with a runtime switch.
    #if 0
    // trap case where the program cannot handle the current version
    if(file_version > static_cast<const unsigned int>(version()))
        dgboost::serialization::throw_exception(
            archive::archive_exception(
                dgboost::archive::archive_exception::unsupported_class_version,
                get_debug_info()
            )
        );
    #endif
    // make sure call is routed through the higest interface that might
    // be specialized by the user.
    dgboost::serialization::serialize_adl(
        dgboost::serialization::smart_cast_reference<Archive &>(ar),
        * static_cast<T *>(x), 
        file_version
    );
}

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

// the purpose of this code is to allocate memory for an object
// without requiring the constructor to be called.  Presumably
// the allocated object will be subsequently initialized with
// "placement new". 
// note: we have the boost type trait has_new_operator but we
// have no corresponding has_delete_operator.  So we presume
// that the former being true would imply that the a delete
// operator is also defined for the class T.

template<class T>
struct heap_allocation {
    // dgboost::has_new_operator< T > doesn't work on these compilers
    #if DONT_USE_HAS_NEW_OPERATOR
        // This doesn't handle operator new overload for class T
        static T * invoke_new(){
            return static_cast<T *>(operator new(sizeof(T)));
        }
        static void invoke_delete(T *t){
            (operator delete(t));
        }
    #else
        // note: we presume that a true value for has_new_operator
        // implies the existence of a class specific delete operator as well
        // as a class specific new operator.
        struct has_new_operator {
            static T * invoke_new() {
                return static_cast<T *>((T::operator new)(sizeof(T)));
            }
            static void invoke_delete(T * t) {
                // if compilation fails here, the likely cause that the class
                // T has a class specific new operator but no class specific
                // delete operator which matches the following signature.
                // note that this solution addresses the issue that two
                // possible signatures.  But it doesn't address the possibility
                // that the class might have class specific new with NO
                // class specific delete at all.  Patches (compatible with
                // C++03) welcome!
                (operator delete)(t);
            }
        };
        struct doesnt_have_new_operator {
            static T* invoke_new() {
                return static_cast<T *>(operator new(sizeof(T)));
            }
            static void invoke_delete(T * t) {
                // Note: I'm reliance upon automatic conversion from T * to void * here
                (operator delete)(t);
            }
        };
        static T * invoke_new() {
            typedef typename
                mpl::eval_if<
                    dgboost::has_new_operator< T >,
                    mpl::identity<has_new_operator >,
                    mpl::identity<doesnt_have_new_operator >    
                >::type typex;
            return typex::invoke_new();
        }
        static void invoke_delete(T *t) {
            typedef typename
                mpl::eval_if<
                    dgboost::has_new_operator< T >,
                    mpl::identity<has_new_operator >,
                    mpl::identity<doesnt_have_new_operator >    
                >::type typex;
            typex::invoke_delete(t);
        }
    #endif
    explicit heap_allocation(){
        m_p = invoke_new();
    }
    ~heap_allocation(){
        if (0 != m_p)
            invoke_delete(m_p);
    }
    T* get() const {
        return m_p;
    }

    T* release() {
        T* p = m_p;
        m_p = 0;
        return p;
    }
private:
    T* m_p;
};

template<class Archive, class T>
class pointer_iserializer :
    public basic_pointer_iserializer
{
private:
    virtual void * heap_allocation() const {
        detail::heap_allocation<T> h;
        T * t = h.get();
        h.release();
        return t;
    }
    virtual const basic_iserializer & get_basic_serializer() const {
        return dgboost::serialization::singleton<
            iserializer<Archive, T>
        >::get_const_instance();
    }
    BOOST_DLLEXPORT virtual void load_object_ptr(
        basic_iarchive & ar, 
        void * x,
        const unsigned int file_version
    ) const BOOST_USED;
public:
    // this should alway be a singleton so make the constructor protected
    pointer_iserializer();
    ~pointer_iserializer();
};

#ifdef BOOST_MSVC
#  pragma warning(pop)
#endif

// note: BOOST_DLLEXPORT is so that code for polymorphic class
// serialized only through base class won't get optimized out
template<class Archive, class T>
BOOST_DLLEXPORT void pointer_iserializer<Archive, T>::load_object_ptr(
    basic_iarchive & ar, 
    void * t,
    const unsigned int file_version
) const
{
    Archive & ar_impl = 
        dgboost::serialization::smart_cast_reference<Archive &>(ar);

    // note that the above will throw std::bad_alloc if the allocation
    // fails so we don't have to address this contingency here.

    // catch exception during load_construct_data so that we don't
    // automatically delete the t which is most likely not fully
    // constructed
    BOOST_TRY {
        // this addresses an obscure situation that occurs when 
        // load_constructor de-serializes something through a pointer.
        ar.next_object_pointer(t);
        dgboost::serialization::load_construct_data_adl<Archive, T>(
            ar_impl,
            static_cast<T *>(t),
            file_version
        );
    }
    BOOST_CATCH(...){
        // if we get here the load_construct failed.  The heap_allocation
        // will be automatically deleted so we don't have to do anything
        // special here.
        BOOST_RETHROW;
    }
    BOOST_CATCH_END

    ar_impl >> dgboost::serialization::make_nvp(NULL, * static_cast<T *>(t));
}

template<class Archive, class T>
pointer_iserializer<Archive, T>::pointer_iserializer() :
    basic_pointer_iserializer(
        dgboost::serialization::singleton<
            typename 
            dgboost::serialization::type_info_implementation< T >::type
        >::get_const_instance()
    )
{
    dgboost::serialization::singleton<
        iserializer<Archive, T>
    >::get_mutable_instance().set_bpis(this);
    archive_serializer_map<Archive>::insert(this);
}

template<class Archive, class T>
pointer_iserializer<Archive, T>::~pointer_iserializer(){
    archive_serializer_map<Archive>::erase(this);
}

template<class Archive>
struct load_non_pointer_type {
    // note this bounces the call right back to the archive
    // with no runtime overhead
    struct load_primitive {
        template<class T>
        static void invoke(Archive & ar, T & t){
            load_access::load_primitive(ar, t);
        }
    };
    // note this bounces the call right back to the archive
    // with no runtime overhead
    struct load_only {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            // short cut to user's serializer
            // make sure call is routed through the higest interface that might
            // be specialized by the user.
            dgboost::serialization::serialize_adl(
                ar, 
                const_cast<T &>(t), 
                dgboost::serialization::version< T >::value
            );
        }
    };

    // note this save class information including version
    // and serialization level to the archive
    struct load_standard {
        template<class T>
        static void invoke(Archive &ar, const T & t){
            void * x = dgboost::addressof(const_cast<T &>(t));
            ar.load_object(
                x, 
                dgboost::serialization::singleton<
                    iserializer<Archive, T>
                >::get_const_instance()
            );
        }
    };

    struct load_conditional {
        template<class T>
        static void invoke(Archive &ar, T &t){
            //if(0 == (ar.get_flags() & no_tracking))
                load_standard::invoke(ar, t);
            //else
            //    load_only::invoke(ar, t);
        }
    };

    template<class T>
    static void invoke(Archive & ar, T &t){
        typedef typename mpl::eval_if<
                // if its primitive
                mpl::equal_to<
                    dgboost::serialization::implementation_level< T >,
                    mpl::int_<dgboost::serialization::primitive_type>
                >,
                mpl::identity<load_primitive>,
            // else
            typename mpl::eval_if<
            // class info / version
            mpl::greater_equal<
                        dgboost::serialization::implementation_level< T >,
                        mpl::int_<dgboost::serialization::object_class_info>
                    >,
            // do standard load
            mpl::identity<load_standard>,
        // else
        typename mpl::eval_if<
            // no tracking
                    mpl::equal_to<
                        dgboost::serialization::tracking_level< T >,
                        mpl::int_<dgboost::serialization::track_never>
                >,
                // do a fast load
                mpl::identity<load_only>,
            // else
            // do a fast load only tracking is turned off
            mpl::identity<load_conditional>
        > > >::type typex;
        check_object_versioning< T >();
        check_object_level< T >();
        typex::invoke(ar, t);
    }
};

template<class Archive>
struct load_pointer_type {
    struct abstract
    {
        template<class T>
        static const basic_pointer_iserializer * register_type(Archive & /* ar */){
            // it has? to be polymorphic
            BOOST_STATIC_ASSERT(dgboost::is_polymorphic< T >::value);
            return static_cast<basic_pointer_iserializer *>(NULL);
         }
    };

    struct non_abstract
    {
        template<class T>
        static const basic_pointer_iserializer * register_type(Archive & ar){
            return ar.register_type(static_cast<T *>(NULL));
        }
    };

    template<class T>
    static const basic_pointer_iserializer * register_type(Archive &ar, const T* const /*t*/){
        // there should never be any need to load an abstract polymorphic 
        // class pointer.  Inhibiting code generation for this
        // permits abstract base classes to be used - note: exception
        // virtual serialize functions used for plug-ins
        typedef typename
            mpl::eval_if<
                dgboost::serialization::is_abstract<const T>,
                dgboost::mpl::identity<abstract>,
                dgboost::mpl::identity<non_abstract>  
            >::type typex;
        return typex::template register_type< T >(ar);
    }

    template<class T>
    static T * pointer_tweak(
        const dgboost::serialization::extended_type_info & eti,
        void const * const t,
        const T &
    ) {
        // tweak the pointer back to the base class
        void * upcast = const_cast<void *>(
            dgboost::serialization::void_upcast(
                eti,
                dgboost::serialization::singleton<
                    typename 
                    dgboost::serialization::type_info_implementation< T >::type
                >::get_const_instance(),
                t
            )
        );
        if(NULL == upcast)
            dgboost::serialization::throw_exception(
                archive_exception(archive_exception::unregistered_class)
            );
        return static_cast<T *>(upcast);
    }

    template<class T>
    static void check_load(T * const /* t */){
        check_pointer_level< T >();
        check_pointer_tracking< T >();
    }

    static const basic_pointer_iserializer *
    find(const dgboost::serialization::extended_type_info & type){
        return static_cast<const basic_pointer_iserializer *>(
            archive_serializer_map<Archive>::find(type)
        );
    }

    template<class Tptr>
    static void invoke(Archive & ar, Tptr & t){
        check_load(t);
        const basic_pointer_iserializer * bpis_ptr = register_type(ar, t);
        const basic_pointer_iserializer * newbpis_ptr = ar.load_pointer(
            // note major hack here !!!
            // I tried every way to convert Tptr &t (where Tptr might
            // include const) to void * &.  This is the only way
            // I could make it work. RR
            (void * & )t,
            bpis_ptr,
            find
        );
        // if the pointer isn't that of the base class
        if(newbpis_ptr != bpis_ptr){
            t = pointer_tweak(newbpis_ptr->get_eti(), t, *t);
        }
    }
};

template<class Archive>
struct load_enum_type {
    template<class T>
    static void invoke(Archive &ar, T &t){
        // convert integers to correct enum to load
        int i;
        ar >> dgboost::serialization::make_nvp(NULL, i);
        t = static_cast< T >(i);
    }
};

template<class Archive>
struct load_array_type {
    template<class T>
    static void invoke(Archive &ar, T &t){
        typedef typename remove_extent< T >::type value_type;
        
        // convert integers to correct enum to load
        // determine number of elements in the array. Consider the
        // fact that some machines will align elements on boundries
        // other than characters.
        std::size_t current_count = sizeof(t) / (
            static_cast<char *>(static_cast<void *>(&t[1])) 
            - static_cast<char *>(static_cast<void *>(&t[0]))
        );
        dgboost::serialization::collection_size_type count;
        ar >> BOOST_SERIALIZATION_NVP(count);
        if(static_cast<std::size_t>(count) > current_count)
            dgboost::serialization::throw_exception(
                archive::archive_exception(
                    dgboost::archive::archive_exception::array_size_too_short
                )
            );
        // explict template arguments to pass intel C++ compiler
        ar >> serialization::make_array<
            value_type,
            dgboost::serialization::collection_size_type
        >(
            static_cast<value_type *>(&t[0]),
            count
        );
    }
};

} // detail

template<class Archive, class T>
inline void load(Archive & ar, T &t){
    // if this assertion trips. It means we're trying to load a
    // const object with a compiler that doesn't have correct
    // function template ordering.  On other compilers, this is
    // handled below.
    detail::check_const_loading< T >();
    typedef
        typename mpl::eval_if<is_pointer< T >,
            mpl::identity<detail::load_pointer_type<Archive> >
        ,//else
        typename mpl::eval_if<is_array< T >,
            mpl::identity<detail::load_array_type<Archive> >
        ,//else
        typename mpl::eval_if<is_enum< T >,
            mpl::identity<detail::load_enum_type<Archive> >
        ,//else
            mpl::identity<detail::load_non_pointer_type<Archive> >
        >
        >
        >::type typex;
    typex::invoke(ar, t);
}

} // namespace archive
} // namespace dgboost

#endif // BOOST_ARCHIVE_DETAIL_ISERIALIZER_HPP
