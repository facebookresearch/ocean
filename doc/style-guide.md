# Style Guide

Last change: 2017-05-30

Author(s): Jan Herling, Nils Plath

The following sections define the C++ style guide for the Ocean framework. Though these rules are
written for C++, they can and should be adapted to other languages as well.

## Documentation

The documentation of code is as important as the actual implementation itself and is an essential
part of responsible software development. The fundamental rule is: the documentation isn't (only)
intended for the original author but rather for his fellow developers who are neither familiar with
the code nor with the idea behind it.

On average, documenting code takes as long as writing the actual implementation. In general, every
function, every class, and every variable in header files must be documented clearly and
unambiguously. The documentation of functions must state the valid range of values of all of its
parameters (especially in cases when the range of valid values is only a subrange of the range of
all possible values). Since the original author of a function may no longer be available, a detailed
and correct documentation of these ranges of values is an absolute imperative in order to avoid
errors or crashes of programs.

Classes and functions which are hard to document even for its author are a possible sign of bad code
design and implementation, e.g. because the code is hard to use or because of a complex structure of
dependencies. If this situation arises, it is the responsibility of the author to develop and
implement possible alternatives for the suboptimal code, which are as efficient as the original
implementation but easier to document and to maintain.

By convention, code in source files (`*.cpp`) will not be documented. Instead the declaration in the
corresponding header file must detail what the code does and how it does it. It is, however,
advisable to add a short and precise documentation inside the source files if code blocks are
complex and would otherwise be hard to understand. These code blocks should be documented using
single-line comments (`// ...`). Comments for assignments of variables can also be added at the end
of a line of code (also using single-line comments).

**Example**

```cpp
void function()
{
    // first we determine the number of ...
    const double value = complexFunction(5, 10, 0.318309886); // 0.3183 = 1 / PI
    // ...
}
```

## Structure and layout

A good code structure and good documentation of code are the fundamental building block of
successful software development. A clear code structure is intended to help fellow developers to
understand code that they didn't write themselves. Additionally, a clear structure makes debugging
easier and more efficient. Code that is logically linked together should be written in consecutive
lines. Empty lines between code blocks separate these code blocks from each other and define the
structure of the code.

### Hierarchy

Code hierarchy is defined by indenting lines of code. With every new level in the hierarchy, another
level of indentation will be added. Only use tabs to indent code; do not use spaces to indent code,
which appears to be the default in many editors. The width that is used to visualize tabs can be
chosen by each developer individually without affecting the preference of others.

Namespaces do not represent levels of hierarchy, i.e. namespaces will not be indented. Generally,
code in each file should be contained in a single namespace or in a hierarchy of namespaces, which
uniquely identifies the namespace the code belongs to (cf. [Namespaces](#namespaces)).

Preprocessor directives which are used to activate or deactivate code blocks, e.g. using `#if` or
`#ifdef`, are independent of general code hierarchies. Because of that preprocessor directives will
be left-aligned and will not be indented. In contrast, nested preprocessor directives may be
indented in order to improve the readability of the code.

**Example**

```cpp
void function()
{
    int value = 3;

#ifdef _WINDOWS
    callWindowsFunction(value);
#else
    callNonWindowsFunction(value);
#endif
}

void function()
{
    int value = 3;

#ifdef _WINDOWS

    #ifdef PLATFORM64BIT
        callWindowsFunction64(value);
    #else
        callWindowsFunction32(value);
    #endif

    callWindowsFunction();

#endif
}
```

### Empty spaces

Empty spaces improve the readability of code. Except for unary operators (e.g. `!`) an empty space
is added before and after an operator. Do not add spaces after the opening of braces or before
closing it again. Two or more consecutive spaces are not allowed.

**Example**

```cpp
int a = 7;
bool v = result == 9;

if (value[x] == 3)
{
    int array[5] = {0, 1, 2, 3, 4};
    // ...
}

int b = -5; // Unary operators, no space
bool v = !a;
```

### Lines of code

Only write a single expression per line. A semicolon, which denotes the end of an expression, also
denotes the end of a line. Expressions following flow control statements, e.g. `if`-`else`, `for`,
`do`, `while`, etc., are always written into separate lines. Because lines containing two or more
expressions are not easy to debug they should be avoided.

The resolution of modern computer displays is high enough to display far more than 150 characters
per line. Because of that, expressions should be written into a single line rather than breaking
them into multiple lines. If expressions are split into multiple lines they could appear as separate
calls or expressions which may decrease the readability and cause misunderstandings. Lines of code
*exceeding the length of 160 characters* may be split into multiple lines in order to avoid the need
to scroll horizontally. Use at least one additional level of indentation at the beginning of the
following lines to hint that the block of lines are related to the previous ones. Lines should be
split *before* binary operators. Writing the operator at the beginning of the next line also implies
that the current line belongs to the previous one. Conversely, it is easy to miss operators at the
end of a line.

Long lines containing non-critical code, e.g. the definition of functions with only trivial
parameters, do not have to be split despite the limit mentioned above. Many code editors support
automatic line wrapping, which can be configured by each developer individually without affecting
the preference of other developers.

**Example**

```cpp
if (true)
{
    int value = value0 + value1 + value2 + value3 + value4 + value5
        + value6 + value7 + value8 + value9 + value10;
    // ...
}
```

### Comparison operators

If a class implements an equality operator (`operator==(...)`) a corresponding inequality operator
(`operator!=(...)`) must also be implemented. This avoids undesired side-effects, i.e. the implicit
creation of the inequality operator by the compiler using a cast operator.

**Example**

```cpp
class Classname
{
    public:

        // ...
        bool operator==(const Classname& other) const;
        bool operator!=(const Classname& other) const;
        // ...
};

// ...

bool Classname::operator!=(const Classname& other) const
{
    return !(*this == other);
}
```

### Ternary operator (a ? b : c)

Similar to `if`-`else` clauses, the ternary operator allows the execution of branching code. Because
it is less readable than regular `if`-`else` clauses and because it is harder to debug, the ternary
operator should only be used in special cases. These include the initialization of (constant)
variables, member variables in constructors, or when passing arguments into a function. More complex
use cases should be an exception. Avoid nesting of ternary operators.

### Macros

The use of macros should be an exception (an exception of this exception are third party libraries
which rely on the existence of certain macros or which help with its integration). Macros cannot be
debugged, which is the main reason to refrain from using them.

### Code blocks

The begin and end of a code block should always be written on separate lines, i.e. the curly braces
after `if` or `switch` statements or a `class` declaration. The corresponding code block inside the
curly braces has to be indented by one extra tab. The connection between the opening and closing curly
braces is clearly visible because of the identical level of indention. Additionally, this format
conveys a sense of which code lines belong to which block, i.e. it visually separates blocks from
each other. There is no empty line before the first and after the last statement inside a code
block.

In order to visually separate them from function calls, a space should be added between flow control
statements like `if`, `switch`, `for`, `while`, etc. and their corresponding condition written
inside braces (cf. [Function](#functions)). However, a confusion between flow control statements and
regular statements, e.g. a function call, shouldn't be a problem in practice since most editors
support syntax highlighting for C/C++ keywords.

**Example**

```cpp
if (true)
{
    // the code block
}

for (unsigned int n = 0u; n < 10u; n++)
{
    // the code block
}
```

## <a name="variables"></a> Variables

By convention, names of variables start with a lower case letter. For names consisting of compound
words, use camel case, i.e. except for the first word all words begin with a capital letter, for
example `thisIsCamelCase`. Do not use underscores (`_`) to separate the words as they are reversed
for special purposes. Names of variables should be understandable and meaningful to other
developers, too. In general, write out the words in the variables name and refrain from using
abbreviations. To be able to faster and better understand code that was written by others, choose
meaningful names for variables. Do not include the type of the variable in its name. Due to the
multitude of different data types a simple and consistent notation simply doesn't exist.

**Example**

```cpp
int variable = 0;
double startTimestamp = 0.0;
std::string currentImageFilename = std::string("image.bmp");
```

If a data type exists as signed as well as unsigned, then choose the one that is more suitable for
the specific situation. For example, using `unsigned int` to describe the width and height of
regular 2D images (assuming negative dimensions don't have a logical meaning), this choice of the
data type already eliminates one possible source of errors, i.e. negative values.

Except for a few special cases, applications or libraries do not use global variables. The use of
global variables indicate problems in the structure of the code. Instead make us of singletons to
replace global variables. This also avoids implicit dependencies or missing definition of global
variables.

### Constness

Make sure to add the keyword `const` to the definition of a variable, if its values doesn't change
anymore during its lifetime. If variables are declared as constant, compilers may be able to apply
optimization to improve the overall performance. It also helps other developers to understand the
code as they don't have to watch out for places where the value of a variable may or may not change.

Memory that cannot be changed, i.e. constant memory, must be defined as `const` and must be also be
passed as `const` in function calls.

Pointers to constant memory which never change the address they point to must also be declared as
constant, i.e. two `const`  statements must be used in their definition.

**Example**

```cpp
const unsigned char* const constantPointerWithConstantData = ...;
```

### References

References should be preferred over pointers. For example, if the return value of a function is a
pointer it isn't clear who owns the memory, i.e. who is responsible to release the memory after use.
When using references as return value the question of ownership doesn't even arise because it is
clear for the caller that it isn't his responsibility.

In declarations of references the symbol `&` is appended to the right side of the data type, without
any space. This avoids accidental confusion with the bitwise and-operator (`&`).

**Example**

```cpp
const std::string& stringReference = ...;
bool& valueReference = ...;
```

### Pointer

For the initialization, assignment, or comparison of pointers with the default pointer address `0`
always use the statement `NULL`. This avoids confusion with *counter* variables. Conversely, the
statement `NULL` must not be used with non-pointer variables.

In declarations of pointers the symbol `*` is appended to the right side of the data, without any
space. This avoid accidental confusion with the multiplication operator (`*`).

**Example**

```cpp
const std::string* stringPointer = ...;
bool* value = NULL;
```

The *address-of* operator (`&`) and dereferencing operator (`*`) are prepended to the variable in
order to avoid accidental confusion with other operators.

**Example**

```cpp
std::string* stringPointer = &stringObject;
std::string stringObject2 = *stringPointer;
```

### Suffices

Numeric literals must always be accompanied by their corresponding suffices. Failing to add the type
suffix may results in compiler warnings, lower performance, or even overflows.

| Data type          | Suffix | Example                                  |
|--------------------|-------:|------------------------------------------|
| long               |    `l` | `long v = 17;`                           |
| unsigned int       |    `u` | `unsigned int v = 5u;`                   |
| unsigned long long |  `ull` | `unsigned long long v = 7ull;`           |
| float              |    `f` | `float v = 6.3f;`                        |

## <a name="functions"></a> Functions

Similar to the [naming convention of variables](variables), names of functions begin with a lower
case letter. If the name consists of compound words, use the camel case convention.

The name of a function must be unambiguous and indicate what the function is intended to do. The use
of abbreviations in function names is not allowed. Choosing suitable function names and writing out
the name in full helps other developers to understand the code.

**Example**

```cpp
void determineValue();
unsigned int countElements();
double findInKeywordDatabase();
```

In declarations, definitions, and function calls avoid spaces between the name of the function and
its lists of parameters/arguments in parentheses `(...)`. This avoids accidental confusion with flow
control statements, which must have a space between the statement and its condition.

Except for a few cases, applications and libraries should not contain global functions. In general,
global functions are indicators for bad design and bad code structure. Instead use classes with
static functions, which are globally accessible as well. Classes with only (public) static functions
is practically equivalent to a namespace, but avoid the creation of artificial hierarchies of
namespaces.

In the case of inline functions, use the statement `inline` in the declaration of the function as
well as in its definition. This helps to avoid confusion and linker errors. Note that functions can
never be made `virtual` and `inline` at the same time because the two statements are mutually
exclusive.

With the exception of constructors (which may not need to do anything) and virtual functions that
have no logical meaning in a certain implementations, avoid empty functions bodies. In order to make
a distinction between function bodies which are left empty intentionally and those left empty
accidentally, e.g.  because the implementation hasn't been written yet or forgotten, always leave a
comment to notify other developers, e.g. `// nothing to do here`. This makes it clear that the
responsible developer intentionally didn't add an implementation.

**Example**

```cpp
ExampleClass::ExampleClass(const unsigned int value0, const bool value1) :
    classMemberValue0(value0),
    classMemberValue1(value1)
{
    // nothing to do here
}

void ExampleClass::virtualFunctionWithoutImplementation()
{
    // nothing to do here
}
```

## Include files

Every library must define a common or base include file, which has to be included in all other
header files of that library. This base include file allows to share definitions specific to that
library.

In general, all included header files from the same namespace or the same third party must be
blocked together and each block must be sorted alphabetically, which helps to quickly get an
overview of all dependencies and identify and avoid duplicate include statements. Blocks include
statements are separated by an empty line. First include header files from the current namespace,
then those of other namespaces of the Ocean framework. After that, include header files of third
party libraries. Use quotation marks (`#include "..."`) for include files of the Ocean framework and
use angle brackets for all others (`#include <...>"`).

**Example**

```cpp
#include “ocean/rendering/advanced/AdvancedRendering.h”
#include “ocean/rendering/advanced/AdvancedObject.h”

#include “ocean/base/String.h”
#include “ocean/base/Thread.h”

#include “ocean/cv/FrameConverter.h”

#include “ocean/media/Movie.h”

#include “ocean/rendering/Object.h”

#include <vector>

#include <windows.h>
```

## Libraries

Dependencies to framework libraries or third party libraries must not be defined in the code but
rather in the corresponding project or build files, which are the central and only location to
define these dependencies.

When using objects from other libraries be aware of possible circular dependencies and make sure to
avoid or break them. If a library `B` depends on a library `A`, then no object from library `B` is
permitted to be used in library `A`. This rule preserves a linear structure of dependencies and
avoid circular dependencies.

The list of link libraries must be sorted alphabetically (first all libraries from the Ocean
framework, then library `A`, library `B`, etc.) in order to be able to easily identify missing or
incorrect libraries.

## <a name="namespaces"></a> Namespaces

Namespaces contain sets of classes, which share a certain logical relationship. The use of
namespaces helps to avoid duplicate or ambiguous function names, variables, classes or `typedefs`.
Except for rare occasions, each library must define its own namespace. The name of the library must
match that of the corresponding namespace. Each library must define a unique namespace and every
code file contains exactly one namespace or one hierarchy of namespaces.

In general, avoid `using` statements in the context of namespaces. This is particularly true for
third party libraries, e.g. the C++ Standard Library. If absolutely required, the `using` statement
is allowed inside source files for namespaces of the Ocean framework or those specific to Ocean
applications. Don't employ the `using` statement inside header files, i.e. specifically avoid `using
namespace XY`.

## Templates

Denote parameters of a template classes or function with a leading `T` for types and with a leading
`t` for variables. In case of multiple template parameters, each parameter must begin with `T` or
`t`.

**Example**

```cpp
template <typename T>
class Classname;

template <typename TValue, typename TKey, unsigned int tSize>
class Classname;

template <unsigned int tSize, unsigned char tValue>
void function();
```

## Enums

Enumerations begin with a capital letter and use the camel case convention if the name consists of
compound words. The actual enumerators consists of only capital letters. A prefix constructed from
the name of the `enum` is prepended to every enumerator, which avoids confusion with other similarly
or identically named enumerators,. Separate compound names of enumerators (and its prefix) using
underscores (`_`).

**Example**

```cpp
enum ConfigurationParameter
{
    /// Undefined configuration.
    CP_UNDEFINED,
    /// Fast configuration, with special properties ...
    CP_FAST,
    /// Slow configuration, with special properties ...
    CP_SLOW
};
```

## Defines

Preprocessor defines should be avoided, since they have to be defined at compile time. However, in
case a define is unavoidable, its name must consist of only capital letters and must also include
the name of the application, library, or platform. This avoids ambiguous names and possible clashes
with defines from third party libraries.

**Example**

```cpp
#define OCEAN_RUNTIME_STATIC
#define OCEAN_TRACKING_ITERATIONS 9
```

## Asserts

Assertions define an expectation which must be true. Usually `assert` statements are only evaluated
in debug builds (except if the macro `NDEBUG` is defined on Windows) and have no affect on release
builds. Assertions help to detect and avoid programming errors of the developer. They cannot,
however, avoid incorrect use of code. Because of that, `assert` should be used generously and
frequently, e.g. to check if the values of variables is in the expected range of values and allow
the developer to react accordingly and as early as possible.

Even if an `assert` isn't a valid substitute for an `if`-`else`-clause to check the validity of a
variable, it may be preferable to use `assert` in certain performance critical parts of code.

As a general rule of thumb (here, the terms *developer* and *user* are used interchangeably):

* public interface: if a user passes a variable into a program, use an `if`-clause to check for its
  correctness, e.g. to check if the value is inside the valid range of values, because invalid
  values are an example of incorrect use of a program and not a logical programming error
  (`assert`). This ensures that only valid values are passed into the program.

* private interface: for code that is not directly accessible by users, employ `assert` to check the
  correctness of parameters, i.e. the top of a function should list as many `assert` statements as
  the function has parameters with a limited range of values. Additional `if`-clauses may be added
  to handle incorrect values gracefully, in case the developer sees fit. This assessment should be
  based on how performance critical the function is and on the expected computational impact of such
  `if`-clause.

Independent of the use of `if`-clauses versus `assert`, the documentation must clearly state valid
ranges of values of its parameters as well as common sources of errors.

**Example**

```cpp
void mouseMove(const unsigned int x, const unsigned int y)
{
    if (x == 0u || y == 0u)
        return;

    // ...
}

void timeCriticalFunction(const unsigned int value)
{
    assert(value != 0u);
    unsigned int intermediate = 128u / value;

    // ...
}
```

If the value of a variable can be already computed at compile time, which often the case for
template implementation, the correctness must be evaluated with `static_assert()`. Since this
statement was not supported by all compilers at the time of writing of the initial version of this
document, the preprocessor macro `OCEAN_STATIC_ASSERT()` can be used, too.

## Classes

Use a separate file for each class. The file name is identical to the name of the class it contains
(except for the file extension, of course). For groups of classes that are logically closely
related, their declaration and definition can be placed in the same pair of header and source files,
cf. the classes `Ocean::Lock` and `Ocean::ScopedLock`.

Class names begin with a capital letter. For classes consisting of compound words the camel case
notation is used, cf. the section on [variables](#variables) and [functions](#functions).

The declaration and the definition of a class are strictly separate. This rule also comprises
`inline` functions and other short functions (*one-liners*). Declarations of classes are only
allowed in header files (with a few exceptions). The implementations of functions declared as
`inline` and template functions are allowed in the header file as well. These implementations follow
directly after the declaration of the class. This rule enforces that the interface of classes is
always visible.

### Block structure

A declaration of a class can be split into three main parts or blocks. The first block contains
declarations of data types or other nested classes. The second block lists all member functions of
the class. The last block details the member variables of the class. If applicable, each of these
main blocks is split in up to three sub-blocks depending on the access specifiers `public`,
`protected`, and `private` (and in that order). The common rules for [access specifiers][bs_01] in
classes apply.

Overloaded operators are declared *after* the declaration of the regular member functions. The
declarations of static functions follow after the declaration of the overloaded operators.

A block structure of class declarations like the one described above makes it easy for developers to
get familiar with classes that were written by other developers.

In order to reduce algorithmic complexity and to improve the performance of the code, member
functions which do not access or alter the state of a class instance must be declared as `static`.

Friendship declarations (`friend`) don't affect class inheritance and can be safely written at the
very top of the declaration, which avoids missing these declarations and improves the overall
structure of the declaration.

**Example**

```cpp
class Classname
{
    friend class FriendClassname;

    /// First, the block of definitions (with order: public, protected, private):

    public:

        typedef int PublicIntDefinition;

    protected:

        typedef int ProtectedIntDefinition;


    /// Second, the block of functions (with order: public, protected, private):

    public:

        Classname();

        inline double value() const;

        bool setValue(const double value);

        bool operator==(const Classname& object) const;

        static void staticFunction();

    private:

        void privateFunction();


    /// Third, the block of variables (with order: protected, private):

    protected:

        unsigned int protectedMember;

    private:

        void* privatePointer;
};
```

[bs_01]: http://en.cppreference.com/w/cpp/language/access

### Getters and setters

Classes must not have public member variables. To change the value of member variables add
corresponding getter and setter functions. If possible, getter functions should be added as `const`
functions, which is considered safer and may help improve the overall performance. Very simple
getter and setter functions should be declared as `inline` functions.

The name of setter functions must have the prefix `set`. In contrast, avoid the prefix `get` for
getter functions (not counting a few exceptions to this rule) because the term *get* implies a
simple computational task, e.g. a direct access to a member variable and passing its value back to
the caller. In general, this behavior cannot be guaranteed.

Conversely, avoid the prefix `get` for functions which do not provide direct access to member
variables because the function may perform a more expensive operation than the term `get` implies.
In these cases alternative terms should be preferred, e.g. `determine`, `calculate`, `compute`,
`find`, etc.

**Example**

```cpp
class NotCorrect
{
    public:

        inline int& getValueA();
        inline double getValueB() const;
        float getAverageValue() const;
};

class Correct
{
    public:

        inline int& valueA();
        inline double valueB() const;
        float determineAverageValue() const;
};
```

### Inheritance

In case of inheritance, write the access specifiers (`public`, `protected`, `private`) and the base
class into the same line as the declaration of derived class. The access specifier must be stated
explicitly.

**Example**

```cpp
class Classname : public BaseClassName
{
    public:

        // ...
};
```

In case of multiple inheritance, add a line break after the colon (`:`) and add a new line for each
base class each of which -- except for the last base class -- ends with a comma (`,`). The access
specifier of each base class must be stated explicitly.

**Example**

```cpp
class Classname :
    public BaseClassName0,
    protected BaseClassName1,
    public BaseClassName2
{
    // ...
};
```

## C++ Standard

The entire code base must be in compliance with the [C++ standard][cpp_01]. The results of code
that violates this rule may vary on different platforms and compilers may be unable to translate
that code.

For that reason, avoid the use of pre-compiled headers and preprocessor directives such as `#pragma
once` or `#pragma comment(lib, ...)`. Because equivalent alternatives exist for these directives,
those should be preferred.

In case non-compliant C++ code *has to be* added, make sure it is surrounded by preprocessor
directives (`#if defined ...`) adapted to the specific platform/compiler/CPU/etc.

[cpp_01]: https://isocpp.org/std/the-standard

## Code correctness

As long as the compiler produces error messages, never commit the current state of the code base
into the version control system (VCS). The violation of this rule may disrupt the work of other
developers, when they fetch the code fragments containing erroneous code.

Compiler warnings provide insights about problematic code which may under some circumstances lead to
incorrect results. As a consequence, it is an imperative to take these warnings seriously and
correct the code correspondingly. Only then it is allowed to commit the changed code fragments to
the VCS.

From the perspective of a compiler, the code may be regarded as *correct*, if the entire code base
builds without errors or warnings for all possible build types, platforms, and processor
architecture. Because an exhaustive check of all possible build configurations may not be feasible,
the minimum configuration that *must be* tested before the code may be committed to the VCS is
currently defined as follows:

* 32- and 64-bit CPU architectures, with
* dynamically and statically linked Ocean libraries, and
* release and debug configurations.

For an official release of the framework, *all* possible configurations have to be tested and
validated before the release. Additionally, all unit and performance tests have to pass.
