#ifndef BOOST_ARCHIVE_OSERIALIZER_HPP
#define BOOST_ARCHIVE_OSERIALIZER_HPP

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
// oserializer.hpp: interface for serialization system.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <dgboost/assert.hpp>
#include <cstddef> // NULL

#include <dgboost/config.hpp>

#include <dgboost/static_assert.hpp>
#include <dgboost/detail/workaround.hpp>

#include <dgboost/mpl/eval_if.hpp>
#include <dgboost/mpl/equal_to.hpp>
#include <dgboost/mpl/greater_equal.hpp>
#include <dgboost/mpl/identity.hpp>
#include <dgboost/mpl/bool_fwd.hpp>

#ifndef BOOST_SERIALIZATION_DEFAULT_TYPE_INFO   
    #include <dgboost/serialization/extended_type_info_typeid.hpp>   
#endif
#include <dgboost/serialization/throw_exception.hpp>
#include <dgboost/serialization/smart_cast.hpp>
#include <dgboost/serialization/assume_abstract.hpp>
#include <dgboost/serialization/static_warning.hpp>

#include <dgboost/type_traits/is_pointer.hpp>
#include <dgboost/type_traits/is_enum.hpp>
#include <dgboost/type_traits/is_const.hpp>
#include <dgboost/type_traits/is_polymorphic.hpp>
#include <dgboost/type_traits/remove_extent.hpp>

#include <dgboost/serialization/serialization.hpp>
#include <dgboost/serialization/version.hpp>
#include <dgboost/serialization/level.hpp>
#include <dgboost/serialization/tracking.hpp>
#include <dgboost/serialization/type_info_implementation.hpp>
#include <dgboost/serialization/nvp.hpp>
#include <dgboost/serialization/void_cast.hpp>
#include <dgboost/serialization/collection_size_type.hpp>
#include <dgboost/serialization/array_wrapper.hpp>

#include <dgboost/serialization/singleton.hpp>

#include <dgboost/archive/archive_exception.hpp>
#include <dgboost/archive/detail/basic_oarchive.hpp>
#include <dgboost/archive/detail/basic_oserializer.hpp>
#include <dgboost/archive/detail/basic_pointer_oserializer.hpp>
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
class save_access {
public:
    template<class Archive>
    static void end_preamble(Archive & ar){
        ar.end_preamble();
    }
    template<class Archive, class T>
    static void save_primitive(Archive & ar, const  T & t){
        ar.end_preamble();
        ar.save(t);
    }
};

namespace detail {

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

template<class Archive, class T>
class oserializer : public basic_oserializer
{
private:
    // private constructor to inhibit any existence other than the 
    // static one
public:
    explicit BOOST_DLLEXPORT oserializer() :
        basic_oserializer(
            dgboost::serialization::singleton<
                typename 
                dgboost::serialization::type_info_implementation< T >::type
            >::get_const_instance()
        )
    {}
    virtual BOOST_DLLEXPORT void save_object_data(
        basic_oarchive & ar,    
        const void *x
    ) const BOOST_USED;
    virtual bool class_info() const {
        return dgboost::serialization::implementation_level< T >::value 
            >= dgboost::serialization::object_class_info;
    }
    virtual bool tracking(const unsigned int /* flags */) const {
        return dgboost::serialization::tracking_level< T >::value == dgboost::serialization::track_always
            || (dgboost::serialization::tracking_level< T >::value == dgboost::serialization::track_selectively
                && serialized_as_pointer());
    }
    virtual version_type version() const {
        return version_type(::dgboost::serialization::version< T >::value);
    }
    virtual bool is_polymorphic() const {
        return dgboost::is_polymorphic< T >::value;
    }
    virtual ~oserializer(){}
};

#ifdef BOOST_MSVC
#  pragma warning(pop)
#endif

template<class Archive, class T>
BOOST_DLLEXPORT void oserializer<Archive, T>::save_object_data(
    basic_oarchive & ar,    
    const void *x
) const {
    // make sure call is routed through the highest interface that might
    // be specialized by the user.
    BOOST_STATIC_ASSERT(dgboost::is_const< T >::value == false);
    dgboost::serialization::serialize_adl(
        dgboost::serialization::smart_cast_reference<Archive &>(ar),
        * static_cast<T *>(const_cast<void *>(x)),
        version()
    );
}

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

template<class Archive, class T>
class pointer_oserializer :
    public basic_pointer_oserializer
{
private:
    const basic_oserializer & 
    get_basic_serializer() const {
        return dgboost::serialization::singleton<
            oserializer<Archive, T>
        >::get_const_instance();
    }
    virtual BOOST_DLLEXPORT void save_object_ptr(
        basic_oarchive & ar,
        const void * x
    ) const BOOST_USED;
public:
    pointer_oserializer();
    ~pointer_oserializer();
};

#ifdef BOOST_MSVC
#  pragma warning(pop)
#endif

template<class Archive, class T>
BOOST_DLLEXPORT void pointer_oserializer<Archive, T>::save_object_ptr(
    basic_oarchive & ar,
    const void * x
) const {
    BOOST_ASSERT(NULL != x);
    // make sure call is routed through the highest interface that might
    // be specialized by the user.
    T * t = static_cast<T *>(const_cast<void *>(x));
    const unsigned int file_version = dgboost::serialization::version< T >::value;
    Archive & ar_impl 
        = dgboost::serialization::smart_cast_reference<Archive &>(ar);
    dgboost::serialization::save_construct_data_adl<Archive, T>(
        ar_impl, 
        t, 
        file_version
    );
    ar_impl << dgboost::serialization::make_nvp(NULL, * t);
}

template<class Archive, class T>
pointer_oserializer<Archive, T>::pointer_oserializer() :
    basic_pointer_oserializer(
        dgboost::serialization::singleton<
            typename 
            dgboost::serialization::type_info_implementation< T >::type
        >::get_const_instance()
    )
{
    // make sure appropriate member function is instantiated
    dgboost::serialization::singleton<
        oserializer<Archive, T> 
    >::get_mutable_instance().set_bpos(this);
    archive_serializer_map<Archive>::insert(this);
}

template<class Archive, class T>
pointer_oserializer<Archive, T>::~pointer_oserializer(){
    archive_serializer_map<Archive>::erase(this);
}

template<class Archive>
struct save_non_pointer_type {
    // note this bounces the call right back to the archive
    // with no runtime overhead
    struct save_primitive {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            save_access::save_primitive(ar, t);
        }
    };
    // same as above but passes through serialization
    struct save_only {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            // make sure call is routed through the highest interface that might
            // be specialized by the user.
            dgboost::serialization::serialize_adl(
                ar, 
                const_cast<T &>(t), 
                ::dgboost::serialization::version< T >::value
            );
        }
    };
    // adds class information to the archive. This includes
    // serialization level and class version
    struct save_standard {
        template<class T>
        static void invoke(Archive &ar, const T & t){
            ar.save_object(
                dgboost::addressof(t),
                dgboost::serialization::singleton<
                    oserializer<Archive, T>
                >::get_const_instance()
            );
        }
    };



    // adds class information to the archive. This includes
    // serialization level and class version
    struct save_conditional {
        template<class T>
        static void invoke(Archive &ar, const T &t){
            //if(0 == (ar.get_flags() & no_tracking))
                save_standard::invoke(ar, t);
            //else
            //   save_only::invoke(ar, t);
        }
    };


    template<class T>
    static void invoke(Archive & ar, const T & t){
        typedef 
            typename mpl::eval_if<
            // if its primitive
                mpl::equal_to<
                    dgboost::serialization::implementation_level< T >,
                    mpl::int_<dgboost::serialization::primitive_type>
                >,
                mpl::identity<save_primitive>,
            // else
            typename mpl::eval_if<
                // class info / version
                mpl::greater_equal<
                    dgboost::serialization::implementation_level< T >,
                    mpl::int_<dgboost::serialization::object_class_info>
                >,
                // do standard save
                mpl::identity<save_standard>,
            // else
            typename mpl::eval_if<
                    // no tracking
                mpl::equal_to<
                    dgboost::serialization::tracking_level< T >,
                    mpl::int_<dgboost::serialization::track_never>
                >,
                // do a fast save
                mpl::identity<save_only>,
            // else
                // do a fast save only tracking is turned off
                mpl::identity<save_conditional>
            > > >::type typex; 
        check_object_versioning< T >();
        typex::invoke(ar, t);
    }
    template<class T>
    static void invoke(Archive & ar, T & t){
        check_object_level< T >();
        check_object_tracking< T >();
        invoke(ar, const_cast<const T &>(t));
    }
};

template<class Archive>
struct save_pointer_type {
    struct abstract
    {
        template<class T>
        static const basic_pointer_oserializer * register_type(Archive & /* ar */){
            // it has? to be polymorphic
            BOOST_STATIC_ASSERT(dgboost::is_polymorphic< T >::value);
            return NULL;
        }
    };

    struct non_abstract
    {
        template<class T>
        static const basic_pointer_oserializer * register_type(Archive & ar){
            return ar.register_type(static_cast<T *>(NULL));
        }
    };

    template<class T>
    static const basic_pointer_oserializer * register_type(Archive &ar, T* const /*t*/){
        // there should never be any need to save an abstract polymorphic 
        // class pointer.  Inhibiting code generation for this
        // permits abstract base classes to be used - note: exception
        // virtual serialize functions used for plug-ins
        typedef 
            typename mpl::eval_if<
                dgboost::serialization::is_abstract< T >,
                mpl::identity<abstract>,
                mpl::identity<non_abstract>       
            >::type typex;
        return typex::template register_type< T >(ar);
    }

    struct non_polymorphic
    {
        template<class T>
        static void save(
            Archive &ar, 
            T & t
        ){
            const basic_pointer_oserializer & bpos = 
                dgboost::serialization::singleton<
                    pointer_oserializer<Archive, T>
                >::get_const_instance();
            // save the requested pointer type
            ar.save_pointer(& t, & bpos);
        }
    };

    struct polymorphic
    {
        template<class T>
        static void save(
            Archive &ar, 
            T & t
        ){
            typename 
            dgboost::serialization::type_info_implementation< T >::type const
            & i = dgboost::serialization::singleton<
                typename 
                dgboost::serialization::type_info_implementation< T >::type
            >::get_const_instance();

            dgboost::serialization::extended_type_info const * const this_type = & i;

            // retrieve the true type of the object pointed to
            // if this assertion fails its an error in this library
            BOOST_ASSERT(NULL != this_type);

            const dgboost::serialization::extended_type_info * true_type =
                i.get_derived_extended_type_info(t);

            // note:if this exception is thrown, be sure that derived pointer
            // is either registered or exported.
            if(NULL == true_type){
                dgboost::serialization::throw_exception(
                    archive_exception(
                        archive_exception::unregistered_class,
                        "derived class not registered or exported"
                    )
                );
            }

            // if its not a pointer to a more derived type
            const void *vp = static_cast<const void *>(&t);
            if(*this_type == *true_type){
                const basic_pointer_oserializer * bpos = register_type(ar, &t);
                ar.save_pointer(vp, bpos);
                return;
            }
            // convert pointer to more derived type. if this is thrown
            // it means that the base/derived relationship hasn't be registered
            vp = serialization::void_downcast(
                *true_type, 
                *this_type, 
                static_cast<const void *>(&t)
            );
            if(NULL == vp){
                dgboost::serialization::throw_exception(
                    archive_exception(
                        archive_exception::unregistered_cast,
                        true_type->get_debug_info(),
                        this_type->get_debug_info()
                    )
                );
            }

            // since true_type is valid, and this only gets made if the 
            // pointer oserializer object has been created, this should never
            // fail
            const basic_pointer_oserializer * bpos
                = static_cast<const basic_pointer_oserializer *>(
                    dgboost::serialization::singleton<
                        archive_serializer_map<Archive>
                    >::get_const_instance().find(*true_type)
                );
            BOOST_ASSERT(NULL != bpos);
            if(NULL == bpos)
                dgboost::serialization::throw_exception(
                    archive_exception(
                        archive_exception::unregistered_class,
                        "derived class not registered or exported"
                    )
                );
            ar.save_pointer(vp, bpos);
        }
    };

    template<class T>
    static void save(
        Archive & ar, 
        const T & t
    ){
        check_pointer_level< T >();
        check_pointer_tracking< T >();
        typedef typename mpl::eval_if<
            is_polymorphic< T >,
            mpl::identity<polymorphic>,
            mpl::identity<non_polymorphic>
        >::type type;
        type::save(ar, const_cast<T &>(t));
    }

    template<class TPtr>
    static void invoke(Archive &ar, const TPtr t){
        register_type(ar, t);
        if(NULL == t){
            basic_oarchive & boa 
                = dgboost::serialization::smart_cast_reference<basic_oarchive &>(ar);
            boa.save_null_pointer();
            save_access::end_preamble(ar);
            return;
        }
        save(ar, * t);
    }
};

template<class Archive>
struct save_enum_type
{
    template<class T>
    static void invoke(Archive &ar, const T &t){
        // convert enum to integers on save
        const int i = static_cast<int>(t);
        ar << dgboost::serialization::make_nvp(NULL, i);
    }
};

template<class Archive>
struct save_array_type
{
    template<class T>
    static void invoke(Archive &ar, const T &t){
        typedef typename dgboost::remove_extent< T >::type value_type;
        
        save_access::end_preamble(ar);
        // consider alignment
        std::size_t c = sizeof(t) / (
            static_cast<const char *>(static_cast<const void *>(&t[1])) 
            - static_cast<const char *>(static_cast<const void *>(&t[0]))
        );
        dgboost::serialization::collection_size_type count(c);
        ar << BOOST_SERIALIZATION_NVP(count);
        // explict template arguments to pass intel C++ compiler
        ar << serialization::make_array<
            const value_type,
            dgboost::serialization::collection_size_type
        >(
            static_cast<const value_type *>(&t[0]),
            count
        );
    }
};

} // detail

template<class Archive, class T>
inline void save(Archive & ar, /*const*/ T &t){
    typedef 
        typename mpl::eval_if<is_pointer< T >,
            mpl::identity<detail::save_pointer_type<Archive> >,
        //else
        typename mpl::eval_if<is_enum< T >,
            mpl::identity<detail::save_enum_type<Archive> >,
        //else
        typename mpl::eval_if<is_array< T >,
            mpl::identity<detail::save_array_type<Archive> >,
        //else
            mpl::identity<detail::save_non_pointer_type<Archive> >
        >
        >
        >::type typex;
    typex::invoke(ar, t);
}

} // namespace archive
} // namespace dgboost

#endif // BOOST_ARCHIVE_OSERIALIZER_HPP
