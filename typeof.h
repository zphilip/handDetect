#ifndef _typeof_h_
#define _typeof_h_

// One of missing operators in the C++ standard is the typeof operator which returns
// the type-of given expression. It is often implemeted as a compiler extension, but
// is still missing from the Visual Studio's C++ compiler (7.1).

// However, there are few typeof implementations which could be used, probably all
// of them based on a very simple, but genious idea introduced by Bill Gibbons. In a 
// short (feel free to skip this paragraph to the end since it could be hard for you
// to understand this), each type that will be used with a typeof operation should be 
// separately registered, which partially specializes (by a unique index) a class 
// which contains registered type. Besides it, a new function overload is introduced, 
// which, as a parameter, receives the type that is registering and, as an output, it 
// returns an array that has a size of the unique index used in the previous class. A 
// typeof operator is now simple to implement: you have an expression you need a type 
// of; call function with the expression as a parameter; calculate a sizeof return 
// value; put the size in a class template parameter and receive a type stored there, 
// which is equal to the expression type! Besides it, all of this is calculated at the 
// compile time, so, no actual function definitions are needed, meaning, no additional 
// code is introduced! A brilliant idea which is useable with very small consequences 
// (every type to be used should be registered and there is a small hit on compiler 
// speed).

// Although it's not documented (or I haven't found it in documentation), it appears
// that Boost's MPL contains a typeof operator implementation that applies to the
// above pattern:

// BOOST_MPL_AUX_REGISTER_TYPE(123, abc);
// abc aa1;
// BOOST_MPL_AUX_TYPEOF(_, aa1) *aa2 = &aa1;

// The drawbacks of Boost' solution, in my opinion, are:
//  1. it's undocumented
//  2. the syntax is extemly ugly
//  3. you have to manually feed the registration process with an unique index for 
//     every type
//  4. you have to register pointer and constant types separatelly
//  5. you have to use typedefs if you want to register template instances, since 
//     colons cannot be put in define macro parameter
//  6. error messages are non-existent

// I've tried and, hopefully, succeeded in finding workarounds for all listed issues.

// Here we go:
// 1. You are just looking at it! :)
// 2. Syntax is much more clear now!
//    Type registration is done this way:
//       register_type hash_table<int, char *> for_typeof;
//    ...while the usage is as simple as this:
//       hash_table<int, char *> var;
//       typeof(&var) ptr = &var;
//       for(typeof(var)::iterator it = var.begin(); it != var.end(); it++)
//    You can see that the code becomes more readable, you have to type less and,
//    in the case type (especially collection classes) changes, you have to modify
//    only at the one place.
//
//    Note that that if you provide a typedef polluted with pointers, reference or 
//    const/volatile modifiers during the registration process, they will be removed
//    for you. Also, feel free to use the typeof operator wherever you want, for
//    local variables, global variables, class members, base classes and so on.
// 3. As it can be seen above, you only need a type during the registration! Unique
//    index is provided by a __COUNTER__ macro found in new Visual Studio. However,
//    don't think it makes the implementation trivial - Boost's preprocessor library
//    and template specialization have been heavily used since every usage of the
//    __COUNTER__ macro will give you the different number, while indice had to be
//    used on many places during the registration process!
// 4. By using Boost's preprocessor library (which is at the same time very powerful,
//    but very limiting), pointers for the type are automatically registered. A 
//    constant TYPEOF_POINTER_LEVELS (defaults to the value of three) defines how 
//    many pointers will be registered. In practice, the default value will be more 
//    than enough; higher values (50, for example) will make compiler very slow, while 
//    the limit is about 80. Also, a const modified at the lowest level will also be 
//    registered (an implementation for other combinations would be very complicated). 
//    To make this more clear, after registration, with default pointer_levels value, 
//    you'll be able to work with following types:
//       hash_table<int, char *>
//       hash_table<int, char *> *
//       const hash_table<int, char *> *
//       hash_table<int, char *> **
//       const hash_table<int, char *> **
//       hash_table<int, char *> ***
//       const hash_table<int, char *> ***
// 5. Fixed, without additional comment. :)
// 6. If you'll try to apply the typeof operator to an unregistered type, in a second
//    error message, the typeof_type_not_registered type will be mentioned, which
//    should make more clear what just happened.


// I'm looking forward to see comments, improvements, suggestions and more simple
// solutions!

// Requiremets:
//  - Visual Studio 7.1
//  - Boost 1.31

// (untested with previous versions)



// includes
namespace boost {
	namespace mpl {
		namespace aux {
			class typeof_type_not_registered;

			template<typename T>
			typeof_type_not_registered type_index(T const&);
		}
	}
}

#include <boost\type_traits.hpp>
#include <boost\mpl\eval_if.hpp>
#include <boost\mpl\aux_\config\typeof.hpp>
#include <boost\preprocessor\repeat.hpp>
#include <boost\preprocessor\repeat_from_to.hpp>
#include <boost\preprocessor\arithmetic\add.hpp>
#include <boost\preprocessor\arithmetic\div.hpp>
#include <boost\preprocessor\arithmetic\mod.hpp>
#include <boost\preprocessor\arithmetic\mul.hpp>
#include <boost\preprocessor\arithmetic\sub.hpp>
#include <boost\preprocessor\facilities\empty.hpp>
#include <boost\preprocessor\facilities\expand.hpp>
#include <boost\preprocessor\list\adt.hpp>
#include <boost\preprocessor\list\cat.hpp>
#include <boost\preprocessor\slot\slot.hpp>


// helper class
template<typename TYPE>
class remove_all {
private:
	typedef typename remove_reference<TYPE>::type STEP1;
	typedef typename remove_cv<STEP1>::type STEP2;
	typedef typename remove_bounds<STEP2>::type STEP3;
	typedef typename remove_pointer<STEP3>::type STEP4;

	template<typename TYPE2>
	struct wrapper {
		typedef TYPE2 type;
	};

public:
	typedef typename mpl::apply_if_c<is_same<STEP4, TYPE>::value, wrapper<TYPE>, typename remove_all<STEP4> >::type type;
};



// configuration
#define TYPEOF_POINTER_LEVELS		3
#define TYPEOF_FIRST_ID				1000


// implementation
#define BOOST_PP_VALUE	(2 + TYPEOF_POINTER_LEVELS * 2)
#include BOOST_PP_ASSIGN_SLOT(1)

#define _typeof_user_type		1
#define _typeof_builtin_type	2

namespace boost {
	namespace mpl {
		namespace aux {
			template<int S> struct _typeof_temptype;
		}
	}
}

#define _typeof_temp_type(n)								\
						::boost::mpl::aux::_typeof_temptype<__COUNTER__ - BOOST_PP_MUL(n, 2)>::type
#define _typeof_const_modifier(n)							\
						BOOST_PP_IF(BOOST_PP_MOD(n, 2), BOOST_PP_EMPTY(), const)
#define _typeof_star(z, n, data)							\
						*
#define _typeof_generate_pointers(n)						\
						BOOST_PP_REPEAT(BOOST_PP_DIV(n, 2), _typeof_star, 0)
#define _typeof_id											\
						(__COUNTER__ + TYPEOF_FIRST_ID)
#define _typeof_declare_type(z, n, skip_counter)			\
						BOOST_MPL_AUX_REGISTER_TYPE(_typeof_id, _typeof_const_modifier(n) _typeof_temp_type(BOOST_PP_SUB(n, skip_counter)) _typeof_generate_pointers(n))
#define _typeof_final_iterator								\
						BOOST_PP_SLOT(1)
#define _typeof_for_typeof(builtin)							\
						>::type type;};}}}					\
						BOOST_PP_REPEAT_FROM_TO(builtin, _typeof_final_iterator, _typeof_declare_type, BOOST_PP_SUB(builtin, 1))
#define for_builtin_typeof									\
						_typeof_for_typeof(_typeof_builtin_type)



// public interface
#define register_type										\
						namespace boost {					\
							namespace mpl {					\
								namespace aux {				\
									template<> struct _typeof_temptype<__COUNTER__> { typedef remove_all<
#define for_typeof											\
						_typeof_for_typeof(_typeof_user_type)
#define typeof(exp)		::boost::mpl::aux::typeof_c<sizeof(::boost::mpl::aux::type_index(exp))>::type
//#define ttypeof(exp)	typename typeof(exp)


register_type bool for_builtin_typeof;
register_type signed char for_builtin_typeof;
register_type unsigned char for_builtin_typeof;
register_type char for_builtin_typeof;
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
register_type wchar_t for_builtin_typeof;
#endif
register_type short for_builtin_typeof;
register_type unsigned short for_builtin_typeof;
register_type int for_builtin_typeof;
register_type unsigned int for_builtin_typeof;
register_type long for_builtin_typeof;
register_type unsigned long for_builtin_typeof;
register_type float for_typeof;
register_type double for_typeof;
register_type long double for_typeof;
//register_type void for_builtin_typeof;

#endif
