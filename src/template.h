/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <vector>

#include "qcstring.h"
#include "containers.h"
#include "variant.h"

class TemplateListIntf;
class TemplateStructIntf;
class TemplateEngine;
class TextStream;

/** @defgroup template_api Template API
 *
 *  This is the API for a
 *  <a href="https://www.djangoproject.com/">Django</a>
 *  compatible template system written in C++.
 *  It is somewhat inspired by Stephen Kelly's
 *  <a href="http://www.gitorious.org/grantlee/pages/Home">Grantlee</a>.
 *
 *  A template is simply a text file.
 *  A template contains \b variables, which get replaced with values when the
 *  template is evaluated, and \b tags, which control the logic of the template.
 *
 *  Variables look like this: `{{ variable }}`
 *  When the template engine encounters a variable, it evaluates that variable and
 *  replaces it with the result. Variable names consist of any combination of
 *  alphanumeric characters and the underscore ("_").
 *  Use a dot (.) to access attributes of a structured variable.
 *
 *  One can modify variables for display by using \b filters, for example:
 *  `{{ value|default:"nothing" }}`
 *
 *  Tags look like this: `{% tag %}`. Tags are more complex than variables:
 *  Some create text in the output, some control flow by performing loops or logic,
 *  and some load external information into the template to be used by later variables.
 *
 *  To comment-out part of a line in a template, use the comment syntax:
 *  `{# comment text #}`.
 *
 *  Supported Django tags:
 *  - `for ... empty ... endfor`
 *  - `if ... else ... endif`
 *  - `block ... endblock`
 *  - `extend`
 *  - `include`
 *  - `with ... endwith`
 *  - `spaceless ... endspaceless`
 *  - `cycle`
 *
 *  Extension tags:
 *  - `create` which instantiates a template and writes the result to a file.
 *     The syntax is `{% create 'filename' from 'template' %}`.
 *  - `recursetree`
 *  - `markers`
 *  - `msg` ... `endmsg`
 *  - `set`
 *
 *  Supported Django filters:
 *  - `default`
 *  - `length`
 *  - `add`
 *  - `divisibleby`
 *
 *  Extension filters:
 *  - `stripPath`
 *  - `nowrap`
 *  - `prepend`
 *  - `append`
 *
 *  @{
 */

/** @brief Variant type which can hold one value of a fixed set of types. */
class TemplateVariant
{
  public:
    /** @brief Helper class to create a delegate that can store a function/method call. */
    class FunctionDelegate
    {
      public:
        /** Callback type to use when creating a delegate from a function. */
        typedef TemplateVariant (*StubType)(const void *obj, const std::vector<TemplateVariant> &args);

        FunctionDelegate() : m_objectPtr(0) , m_stubPtr(0) {}

        /** Creates a delegate given an object. The method to call is passed as a template parameter */
        template <class T, TemplateVariant (T::*TMethod)(const std::vector<TemplateVariant> &) const>
        static FunctionDelegate fromMethod(const T* objectPtr)
        {
          FunctionDelegate d;
          d.m_objectPtr = objectPtr;
          d.m_stubPtr   = &methodStub<T, TMethod>;
          return d;
        }
        /** Creates a delegate given an object, and a plain function. */
        static FunctionDelegate fromFunction(const void *obj,StubType func)
        {
          FunctionDelegate d;
          d.m_objectPtr = obj;
          d.m_stubPtr = func;
          return d;
        }

        /** Invokes the function/method stored in the delegate */
        TemplateVariant operator()(const std::vector<TemplateVariant> &args) const
        {
          return (*m_stubPtr)(m_objectPtr, args);
        }

      private:
        const void* m_objectPtr;
        StubType    m_stubPtr;

        template <class T, TemplateVariant (T::*TMethod)(const std::vector<TemplateVariant> &) const>
        static TemplateVariant methodStub(const void* objectPtr, const std::vector<TemplateVariant> &args)
        {
          T* p = (T*)(objectPtr);
          return (p->*TMethod)(args);
        }
    };

    /** Constructs an invalid variant. */
    TemplateVariant() {}

    /** Constructs a new variant with a boolean value \a b. */
    explicit TemplateVariant(bool b) { m_variant.set<bool>(b); }

    /** Constructs a new variant with a integer value \a v. */
    TemplateVariant(int v) { m_variant.set<int>(v); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const char *s,bool raw=FALSE) : m_raw(raw) { m_variant.set<QCString>(s); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const QCString &s,bool raw=FALSE) : m_raw(raw) { m_variant.set<QCString>(s); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const std::string &s,bool raw=FALSE) : m_raw(raw) { m_variant.set<QCString>(s); }

    /** Constructs a new variant with a struct value \a s.
     *  @note. The variant will hold a reference to the object.
     */
    TemplateVariant(TemplateStructIntf *s);

    /** Constructs a new variant with a list value \a l.
     *  @note. The variant will hold a reference to the object.
     */
    TemplateVariant(TemplateListIntf *l);

    /** Constructs a new variant which represents a method call
     *  @param[in] delegate FunctionDelegate object to invoke when
     *             calling call() on this variant.
     *  @note Use TemplateVariant::FunctionDelegate::fromMethod() and
     *  TemplateVariant::FunctionDelegate::fromFunction() to create
     *  FunctionDelegate objects.
     */
    TemplateVariant(const FunctionDelegate &delegate) { m_variant.set<FunctionDelegate>(delegate); }

    /** Destroys the Variant object */
    ~TemplateVariant();

    /** Constructs a copy of the variant, \a v,
     *  passed as the argument to this constructor.
     */
    TemplateVariant(const TemplateVariant &v);

    /** Moves the contents of variant \a v into this variant.
     *  variant \a v will become invalid
     */
    TemplateVariant(TemplateVariant &&v);

    /** Assigns the value of the variant \a v to this variant. */
    TemplateVariant &operator=(const TemplateVariant &v);

    /** Move the value of the variant \a v into this variant.
     *  Variant \a v will become invalid */
    TemplateVariant &operator=(TemplateVariant &&v);

    /** Compares this QVariant with v and returns true if they are equal;
     *  otherwise returns false.
     */
    bool operator==(TemplateVariant &other)
    {
      if (!m_variant.valid())
      {
        return FALSE;
      }
      if (isBool() && other.isBool())
      {
        return m_variant.get<bool>() == other.m_variant.get<bool>();
      }
      else if (isInt() && other.isInt())
      {
        return m_variant.get<int>() == other.m_variant.get<int>();
      }
      else if (isList() && other.isList())
      {
        return m_variant.get<TemplateListIntf*>() == other.m_variant.get<TemplateListIntf*>();
      }
      else if (isStruct() && other.isStruct())
      {
        return m_variant.get<TemplateStructIntf*>() == other.m_variant.get<TemplateStructIntf*>();
      }
      return toString()==other.toString();
    }

    QCString listToString() const;
    QCString structToString() const;

    /** Returns the variant as a string. */
    QCString toString() const;

    /** Returns the variant as a boolean. */
    bool toBool() const;

    /** Returns the variant as an integer. */
    int toInt() const;

    /** Returns TRUE if the variant holds a valid value, or FALSE otherwise */
    constexpr bool isValid()    const { return m_variant.valid(); }
    /** Returns TRUE if the variant holds a boolean value */
    constexpr bool isBool()     const { return m_variant.is<bool>(); }
    /** Returns TRUE if the variant holds an integer value */
    constexpr bool isInt()      const { return m_variant.is<int>(); }
    /** Returns TRUE if the variant holds a string value */
    constexpr bool isString()   const { return m_variant.is<QCString>(); }
    /** Returns TRUE if the variant holds a struct value */
    constexpr bool isStruct()   const { return m_variant.is<TemplateStructIntf*>(); }
    /** Returns TRUE if the variant holds a list value */
    constexpr bool isList()     const { return m_variant.is<TemplateListIntf*>(); }
    /** Returns TRUE if the variant holds a function value */
    constexpr bool isFunction() const { return m_variant.is<FunctionDelegate>(); }

    /** Returns the pointer to list referenced by this variant
     *  or 0 if this variant does not have list type.
     */
    TemplateListIntf   *toList()
    {
      return isList() ? m_variant.get<TemplateListIntf*>() : nullptr;
    }
    const TemplateListIntf *toList() const
    {
      return isList() ? m_variant.get<TemplateListIntf*>() : nullptr;
    }

    /** Returns the pointer to struct referenced by this variant
     *  or 0 if this variant does not have struct type.
     */
    TemplateStructIntf *toStruct()
    {
      return isStruct() ? m_variant.get<TemplateStructIntf*>() : nullptr;
    }
    const TemplateStructIntf *toStruct() const
    {
      return isStruct() ? m_variant.get<TemplateStructIntf*>() : nullptr;
    }

    /** Return the result of apply this function with \a args.
     *  Returns an empty string if the variant type is not a function.
     */
    TemplateVariant call(const std::vector<TemplateVariant> &args)
    {
      return isFunction() ? m_variant.get<FunctionDelegate>()(args) : TemplateVariant();
    }

    /** Sets whether or not the value of the Variant should be
     *  escaped or written as-is (raw).
     *  @param[in] b TRUE means write as-is, FALSE means apply escaping.
     */
    void setRaw(bool b) { m_raw = b; }

    /** Returns whether or not the value of the Value is raw.
     *  @see setRaw()
     */
    constexpr bool raw() const { return m_raw; }

    /** Symbolic names for the possible types that this variant can hold. */
    enum class Type : size_t
    {
      None     = std::string::npos,
      Bool     = 0,
      Int      = 1,
      String   = 2,
      Struct   = 3,
      List     = 4,
      Function = 5
    };

    /** Returns the type held by this variant */
    constexpr Type type() const { return static_cast<Type>(m_variant.index()); }

    /** Return a string representation of the type of the value stored in the variant */
    constexpr const char *typeAsString() const
    {
      switch (type())
      {
        case Type::None:       return "invalid";
        case Type::Bool:       return "bool";
        case Type::Int:        return "integer";
        case Type::String:     return "string";
        case Type::Struct:     return "struct";
        case Type::List:       return "list";
        case Type::Function:   return "function";
      }
      return "invalid";
    }

  private:
    using VariantT = Variant<bool,                  // index==0: Type::Bool
                             int,                   // index==1: Type::Int
                             QCString,              // index==2: Type::String
                             TemplateStructIntf*,   // index==3: Type::Struct
                             TemplateListIntf*,     // index==4: Type::List
                             FunctionDelegate       // index==5: Type::Function
                            >;
    VariantT              m_variant;
    bool                  m_raw = false;
};

//------------------------------------------------------------------------

template<class T> class TemplateAutoRef
{
  public:
    TemplateAutoRef(T *obj) : m_obj(obj)
    {
      m_obj->addRef();
    }
   ~TemplateAutoRef()
    {
      m_obj->release();
    }
    T &operator*() const { return *m_obj; }
    T *operator->() const { return m_obj; }
    T *get() const { return m_obj; }

  private:
   T *m_obj;
};

//------------------------------------------------------------------------

/** @brief Abstract read-only interface for a context value of type list.
 *  @note The values of the list are TemplateVariants.
 */
class TemplateListIntf
{
  public:
    /** @brief Abstract interface for a iterator of a list. */
    class ConstIterator
    {
      public:
        /** Destructor for the iterator */
        virtual ~ConstIterator() {}
        /** Moves iterator to the first element in the list */
        virtual void toFirst() = 0;
        /** Moves iterator to the last element in the list */
        virtual void toLast() = 0;
        /** Moves iterator to the next element in the list */
        virtual void toNext() = 0;
        /** Moves iterator to the previous element in the list */
        virtual void toPrev() = 0;
        /* Returns TRUE if the iterator points to a valid element
         * in the list, or FALSE otherwise.
         * If TRUE is returned, the value pointed to be the
         * iterator is assigned to \a v.
         */
        virtual bool current(TemplateVariant &v) const = 0;
    };

    /** Destroys the list */
    virtual ~TemplateListIntf() {}

    /** Returns the number of elements in the list */
    virtual uint count() const = 0;

    /** Returns the element at index position \a index. */
    virtual TemplateVariant  at(uint index) const = 0;

    /** Creates a new iterator for this list.
     *  @note the user should call delete on the returned pointer.
     */
    virtual TemplateListIntf::ConstIterator *createIterator() const = 0;

    /** Increase object's reference count */
    virtual int addRef() = 0;

    /** Decreases object's reference count, destroy object if 0 */
    virtual int release() = 0;
};

/** @brief Default implementation of a context value of type list. */
class TemplateList : public TemplateListIntf
{
  public:
    // TemplateListIntf methods
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef();
    virtual int release();

    /** Creates an instance with ref count set to 0 */
    static TemplateList *alloc();

    /** Appends element \a v to the end of the list */
    virtual void append(const TemplateVariant &v);

  private:
    /** Creates a list */
    TemplateList();
    /** Destroys the list */
   ~TemplateList();

    friend class TemplateListConstIterator;
    class Private;
    Private *p;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a context value of type struct. */
class TemplateStructIntf
{
  public:
    /** Destroys the struct */
    virtual ~TemplateStructIntf() {}

    /** Gets the value for a field name.
     *  @param[in] name The name of the field.
     */
    virtual TemplateVariant get(const QCString &name) const = 0;

    /** Return the list of fields. */
    virtual StringVector fields() const = 0;

    /** Increase object's reference count */
    virtual int addRef() = 0;

    /** Decreases object's reference count, destroy object if 0 */
    virtual int release() = 0;
};


/** @brief Default implementation of a context value of type struct. */
class TemplateStruct : public TemplateStructIntf
{
  public:
    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef();
    virtual int release();

    /** Creates an instance with ref count set to 0. */
    static TemplateStruct *alloc();

    /** Sets the value the field of a struct
     *  @param[in] name The name of the field.
     *  @param[in] v The value to set.
     */
    virtual void set(const QCString &name,const TemplateVariant &v);


  private:
    /** Creates a struct */
    TemplateStruct();
    /** Destroys the struct */
    virtual ~TemplateStruct();

    class Private;
    Private *p;
};

//------------------------------------------------------------------------

/** @brief Interface used to escape characters in a string */
class TemplateEscapeIntf
{
  public:
    virtual ~TemplateEscapeIntf() {}
    /** Returns the \a input after escaping certain characters */
    virtual QCString escape(const QCString &input) = 0;
    /** Setting tabbing mode on or off (for LaTeX) */
    virtual void enableTabbing(bool b) = 0;
};

//------------------------------------------------------------------------

/** @brief Interface used to remove redundant spaces inside a spaceless block */
class TemplateSpacelessIntf
{
  public:
    virtual ~TemplateSpacelessIntf() {}
    /** Returns the \a input after removing redundant whitespace */
    virtual QCString remove(const QCString &input) = 0;
    /** Reset filter state */
    virtual void reset() = 0;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a template context.
 *
 *  A Context consists of a stack of dictionaries.
 *  A dictionary consists of a mapping of string keys onto TemplateVariant values.
 *  A key is searched starting with the dictionary at the top of the stack
 *  and searching downwards until it is found. The stack is used to create
 *  local scopes.
 *  @note This object must be created by TemplateEngine::createContext()
 */
class TemplateContext
{
  public:
    virtual ~TemplateContext() {}

    /** Push a new scope on the stack. */
    virtual void push() = 0;

    /** Pop the current scope from the stack. */
    virtual void pop() = 0;

    /** Sets a value in the current scope.
     *  @param[in] name The name of the value; the key in the dictionary.
     *  @param[in] v The value associated with the key.
     *  @note When a given key is already present,
     *  its value will be replaced by \a v
     */
    virtual void set(const QCString &name,const TemplateVariant &v) = 0;

    /** Gets the value for a given key
     *  @param[in] name The name of key.
     *  @returns The value, which can be an invalid variant in case the
     *  key was not found.
     */
    virtual TemplateVariant get(const QCString &name) const = 0;

    /** Returns a pointer to the value corresponding to a given key.
     *  @param[in] name The name of key.
     *  @returns A pointer to the value, or 0 in case the key was not found.
     */
    virtual const TemplateVariant *getRef(const QCString &name) const = 0;

    /** When files are created (i.e. by {% create ... %}) they written
     *  to the directory \a dir.
     */
    virtual void setOutputDirectory(const QCString &dir) = 0;

    /** Sets the interface that will be used for escaping the result
     *  of variable expansion before writing it to the output.
     */
    virtual void setEscapeIntf(const QCString &extension, TemplateEscapeIntf *intf) = 0;

    /** Sets the interface that will be used inside a spaceless block
     *  to remove any redundant whitespace.
     */
    virtual void setSpacelessIntf(TemplateSpacelessIntf *intf) = 0;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a template.
 *  @note Must be created and is deleted by the TemplateEngine
 */
class Template
{
  public:
    /** Destructor */
    virtual ~Template() {}

    /** Renders a template instance to a stream.
     *  @param[in] ts The text stream to write the results to.
     *  @param[in] c The context containing data that can be used
     *  when instantiating the template.
     */
    virtual void render(TextStream &ts,TemplateContext *c) = 0;
};

//------------------------------------------------------------------------

/** @brief Engine to create templates and template contexts. */
class TemplateEngine
{
  public:
    /** Create a template engine. */
    TemplateEngine();

    /** Destroys the template engine. */
   ~TemplateEngine();

    /** Creates a new context that can be using to render a template.
     *  @see Template::render()
     */
    TemplateContext *createContext() const;

    /** Destroys a context created via createContext().
     *  @param[in] ctx The context.
     */
    void destroyContext(TemplateContext *ctx);

    /** Creates a new template whose contents are in a file.
     *  @param[in] fileName The name of the file containing the template data
     *  @param[in] fromLine The line number of the statement that triggered the load
     *  @return the new template, the engine will keep ownership of the object.
     */
    Template *loadByName(const QCString &fileName,int fromLine);

    /** Indicates that template \a t is no longer needed. The engine
     *  may decide to delete it.
     */
    void unload(Template *t);

    /** Prints the current template file include stack */
    void printIncludeContext(const QCString &fileName,int line) const;

    /** Sets the search directory where to look for template files */
    void setTemplateDir(const QCString &dirName);

  private:
    friend class TemplateNodeBlock;
    friend class TemplateNodeCreate;

    void enterBlock(const QCString &fileName,const QCString &blockName,int line);
    void leaveBlock();

    /** Sets the extension of the output file. This is used to control the
     *  format of 'special' tags in the template
     */
    void setOutputExtension(const QCString &extension);

    /** Returns the output extension, set via setOutputExtension() */
    QCString outputExtension() const;

    class Private;
    Private *p;
};

/** @} */

#endif
