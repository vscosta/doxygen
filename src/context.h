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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "types.h"
#include "template.h"
#include "classdef.h"
#include "searchindex.h"
#include "memberlist.h"

class Definition;

class PageDef;
class PageLinkedMap;
class PageLinkedRefMap;

class GroupDef;
class GroupLinkedMap;
class GroupList;

class ConceptDef;
class ConceptLinkedMap;
class ConceptLinkedRefMap;

class NamespaceDef;
class NamespaceLinkedMap;
class NamespaceLinkedRefMap;

class FileNameLinkedMap;
class ClassLinkedMap;
class MemberNameInfoLinkedMap;

class DirDef;
class DirLinkedMap;
class DirList;

class FileDef;
class FileList;

struct IncludeInfo;
class IncludeInfoList;

class MemberList;
class MemberDef;
struct Argument;
class ArgumentList;
class MemberInfo;
class DotNode;
class DotGfxHierarchyTable;

class MemberGroup;
class MemberGroupList;
class MemberGroupRefList;
class MemberVector;

//----------------------------------------------------

#define DEBUG_REF 0

/** @brief Helper class to support reference counting */
#if DEBUG_REF
class RefCountedContext
{
  public:
    RefCountedContext(const char *className) : m_refCount(0)
    {
      m_className=className;
      m_insideRelease = FALSE;
    }
    virtual ~RefCountedContext()
    {
      if (!m_insideRelease) abort();
    }
    int addRef()
    {
      ++s_totalCount;
      printf("%p:%s::addRef()=%d\n",this,qPrint(m_className),m_refCount);
      return ++m_refCount;
    }
    int release()
    {
      --s_totalCount;
      printf("%p:%s::release()=%d\n",this,qPrint(m_className),m_refCount-1);
      int count = --m_refCount;
      if (count<=0)
      {
        m_insideRelease=TRUE;
        delete this;
      }
      return count;
    }
  private:
    int m_refCount;
    QCString m_className;
    bool m_insideRelease;
  public:
    static int s_totalCount;
};

#else // release version

class RefCountedContext
{
  public:
    RefCountedContext(const char *) : m_refCount(0) {}
    virtual ~RefCountedContext() {}
    int addRef() { return ++m_refCount; }
    int release()
    {
      int count = --m_refCount;
      if (count<=0)
      {
        delete this;
      }
      return count;
    }
  private:
    int m_refCount;
};
#endif


//----------------------------------------------------

class ConfigContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ConfigContext *alloc() { return new ConfigContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ConfigContext();
   ~ConfigContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class DoxygenContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static DoxygenContext *alloc() { return new DoxygenContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DoxygenContext();
    ~DoxygenContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class TranslateContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static TranslateContext *alloc() { return new TranslateContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    TranslateContext();
   ~TranslateContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class UsedFilesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static UsedFilesContext *alloc(const ClassDef *cd) { return new UsedFilesContext(cd); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addFile(const FileDef *fd);

  private:
    UsedFilesContext(const ClassDef *cd);
   ~UsedFilesContext();

    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static IncludeInfoContext *alloc(const IncludeInfo *info,SrcLangExt lang)
    { return new IncludeInfoContext(info,lang); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    IncludeInfoContext(const IncludeInfo *,SrcLangExt lang);
   ~IncludeInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class IncludeInfoListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static IncludeInfoListContext *alloc(const IncludeInfoList &list,SrcLangExt lang)
    { return new IncludeInfoListContext(list,lang); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    IncludeInfoListContext(const IncludeInfoList &list,SrcLangExt lang);
   ~IncludeInfoListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassContext *alloc(const ClassDef *cd) { return new ClassContext(cd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassContext(const ClassDef *);
   ~ClassContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class ConceptContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ConceptContext *alloc(const ConceptDef *cd) { return new ConceptContext(cd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ConceptContext(const ConceptDef *);
   ~ConceptContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceContext *alloc(const NamespaceDef *nd) { return new NamespaceContext(nd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceContext(const NamespaceDef *);
   ~NamespaceContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static FileContext *alloc(const FileDef *fd) { return new FileContext(fd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileContext(const FileDef *);
   ~FileContext();
    class Private;
    Private *p;
};
//----------------------------------------------------

class DirContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static DirContext *alloc(const DirDef *dd) { return new DirContext(dd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DirContext(const DirDef *);
   ~DirContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class PageContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static PageContext *alloc(const PageDef *pd,bool isMainPage,bool isExample) { return new PageContext(pd,isMainPage,isExample); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    PageContext(const PageDef *,bool isMainPage,bool isExample);
   ~PageContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberContext *alloc(const MemberDef *md) { return new MemberContext(md); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberContext(const MemberDef *);
   ~MemberContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class ModuleContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleContext *alloc(const GroupDef *gd) { return new ModuleContext(gd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleContext(const GroupDef *);
   ~ModuleContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ClassListContext *alloc() { return new ClassListContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassListContext();
   ~ClassListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassIndexContext *alloc() { return new ClassIndexContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassIndexContext();
   ~ClassIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritanceGraphContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritanceGraphContext *alloc(DotGfxHierarchyTable *hierarchy,DotNode *n,int id)
    { return new InheritanceGraphContext(hierarchy,n,id); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceGraphContext(DotGfxHierarchyTable *hierarchy,DotNode *n,int id);
   ~InheritanceGraphContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassInheritanceNodeContext *alloc(const ClassDef *cd)
    { return new ClassInheritanceNodeContext(cd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addChildren(const BaseClassList &bcl,bool hideSuper);

  private:
    ClassInheritanceNodeContext(const ClassDef *);
   ~ClassInheritanceNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassInheritanceContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ClassInheritanceContext *alloc() { return new ClassInheritanceContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassInheritanceContext();
   ~ClassInheritanceContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassHierarchyContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassHierarchyContext *alloc() { return new ClassHierarchyContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassHierarchyContext();
   ~ClassHierarchyContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

enum class ContextTreeType
{
  Namespace,           // NamespaceTreeContext
  ClassInheritance,    // ClassHierarchyContext
  ClassNesting,        // ClassTreeContext
  Module,              // ModuleTreeContext
  File,                // FileTreeContext
  Page,                // PageTreeContext
  Concept,             // ContextTreeContext
  Example              // ExampleTreeContext
};

class NestingNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NestingNodeContext *alloc(const NestingNodeContext *parent,ContextTreeType type,
                                     const Definition *def,
                                     int index,int level,
                                     bool addClasses,bool addConcepts,
                                     bool inherit,bool hideSuper,
                                     ClassDefSet &visitedClasses)
    { return new NestingNodeContext(parent,type,def,index,level,addClasses,addConcepts,
                                    inherit,hideSuper,visitedClasses); }

    QCString id() const;

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NestingNodeContext(const NestingNodeContext *parent,
                       ContextTreeType type,
                       const Definition *,int index,int level,
                       bool addClasses,bool addConcepts,
                       bool inherit,bool hideSuper,
                       ClassDefSet &visitedClasses);
   ~NestingNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NestingContext : public RefCountedContext, public TemplateListIntf
{
  public:

    static NestingContext *alloc(const NestingNodeContext *parent,ContextTreeType type,int level)
    { return new NestingContext(parent,type,level); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addNamespaces(const NamespaceLinkedMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses);
    void addNamespaces(const NamespaceLinkedRefMap &nsLinkedMap,bool rootOnly,bool addClasses,bool addConcepts,ClassDefSet &visitedClasses);
    void addClasses(const ClassLinkedMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addClasses(const ClassLinkedRefMap &clLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addConcepts(const ConceptLinkedMap &cnLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addConcepts(const ConceptLinkedRefMap &cnLinkedMap,bool rootOnly,ClassDefSet &visitedClasses);
    void addDirs(const DirLinkedMap &,ClassDefSet &visitedClasses);
    void addDirs(const DirList &,ClassDefSet &visitedClasses);
    void addFiles(const FileNameLinkedMap &,ClassDefSet &visitedClasses);
    void addFiles(const FileList &,ClassDefSet &visitedClasses);
    void addPages(const PageLinkedMap &pages,bool rootOnly,ClassDefSet &visitedClasses);
    void addPages(const PageLinkedRefMap &pages,bool rootOnly,ClassDefSet &visitedClasses);
    void addModules(const GroupLinkedMap &modules,ClassDefSet &visitedClasses);
    void addModules(const GroupList &modules,ClassDefSet &visitedClasses);
    void addClassHierarchy(const ClassLinkedMap &clLinkedMap,ClassDefSet &visitedClasses);
    void addDerivedClasses(const BaseClassList &bcl,bool hideSuper,ClassDefSet &visitedClasses);
    void addMembers(const MemberVector &mv,ClassDefSet &visitedClasses);

  private:
    NestingContext(const NestingNodeContext *parent,ContextTreeType type,int level);
   ~NestingContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassTreeContext *alloc() { return new ClassTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassTreeContext();
   ~ClassTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ConceptListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ConceptListContext *alloc() { return new ConceptListContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ConceptListContext();
   ~ConceptListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static NamespaceListContext *alloc() { return new NamespaceListContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceListContext();
   ~NamespaceListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceTreeContext *alloc() { return new NamespaceTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceTreeContext();
   ~NamespaceTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class DirListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static DirListContext *alloc() { return new DirListContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    DirListContext();
   ~DirListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static FileListContext *alloc() { return new FileListContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileListContext();
   ~FileListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class FileTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static FileTreeContext *alloc() { return new FileTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    FileTreeContext();
   ~FileTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static PageListContext *alloc(const PageLinkedMap &pages) { return new PageListContext(pages); }

    // TemplateListIntf methods
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addPages(const PageLinkedMap &pages);

  private:
    PageListContext(const PageLinkedMap &pages);
   ~PageListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class PageTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static PageTreeContext *alloc(const PageLinkedMap &pages) { return new PageTreeContext(pages); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    PageTreeContext(const PageLinkedMap &pages);
   ~PageTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleNodeContext *alloc(const GroupDef *gd) { return new ModuleNodeContext(gd); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleNodeContext(const GroupDef *);
   ~ModuleNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ModuleListContext *alloc() { return new ModuleListContext(); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

    void addModules(const GroupLinkedMap &);
    void addModules(const GroupList &);

  private:
    ModuleListContext();
   ~ModuleListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ConceptTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ConceptTreeContext *alloc() { return new ConceptTreeContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ConceptTreeContext();
   ~ConceptTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ModuleTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ModuleTreeContext *alloc() { return new ModuleTreeContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ModuleTreeContext();
   ~ModuleTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ExampleListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ExampleListContext *alloc() { return new ExampleListContext; }

    // TemplateListIntf methods
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ExampleListContext();
   ~ExampleListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------


class ExampleTreeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ExampleTreeContext *alloc() { return new ExampleTreeContext; }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ExampleTreeContext();
   ~ExampleTreeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class GlobalsIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static GlobalsIndexContext *alloc() { return new GlobalsIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    GlobalsIndexContext();
   ~GlobalsIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ClassMembersIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ClassMembersIndexContext *alloc() { return new ClassMembersIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ClassMembersIndexContext();
   ~ClassMembersIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NamespaceMembersIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NamespaceMembersIndexContext *alloc() { return new NamespaceMembersIndexContext(); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NamespaceMembersIndexContext();
   ~NamespaceMembersIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class NavPathElemContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static NavPathElemContext *alloc(const Definition *def) { return new NavPathElemContext(def); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    NavPathElemContext(const Definition *def);
   ~NavPathElemContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class InheritanceNodeContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritanceNodeContext *alloc(const ClassDef *cd,const QCString &name)
    { return new InheritanceNodeContext(cd,name); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceNodeContext(const ClassDef *cd,const QCString &name);
   ~InheritanceNodeContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritanceListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static InheritanceListContext *alloc(const BaseClassList &list,bool baseClasses)
    { return new InheritanceListContext(list,baseClasses); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritanceListContext(const BaseClassList &list,bool baseClasses);
   ~InheritanceListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static MemberListContext *alloc()
    { return new MemberListContext; }
    static MemberListContext *alloc(const MemberList *ml)
    { return new MemberListContext(ml); }
    static MemberListContext *alloc(const MemberVector &ml)
    { return new MemberListContext(ml); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberListContext();
    MemberListContext(const MemberList *ml);
    MemberListContext(const MemberVector &ml);
   ~MemberListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberGroupInfoContext *alloc(const Definition *def,const QCString &relPath,const MemberGroup *mg)
    { return new MemberGroupInfoContext(def,relPath,mg); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberGroupInfoContext(const Definition *def,const QCString &relPath,const MemberGroup *mg);
   ~MemberGroupInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberGroupListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static MemberGroupListContext *alloc()
    { return new MemberGroupListContext; }
    static MemberGroupListContext *alloc(const Definition *def,const QCString &relPath,const MemberGroupRefList &list)
    { return new MemberGroupListContext(def,relPath,list); }
    static MemberGroupListContext *alloc(const Definition *def,const QCString &relPath,const MemberGroupList &list,bool subGrouping)
    { return new MemberGroupListContext(def,relPath,list,subGrouping); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberGroupListContext();
    MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupRefList &list);
    MemberGroupListContext(const Definition *def,const QCString &relPath,const MemberGroupList &list,bool subGrouping);
   ~MemberGroupListContext();
    class Private;
    Private *p;
};


//----------------------------------------------------

class MemberListInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberListInfoContext *alloc(const Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString())
    { return new MemberListInfoContext(def,relPath,ml,title,subtitle); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberListInfoContext(const Definition *def,const QCString &relPath,
                          const MemberList *ml,const QCString &title,
                          const QCString &subtitle=QCString());
   ~MemberListInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class MemberInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static MemberInfoContext *alloc(const MemberInfo *mi) { return new MemberInfoContext(mi); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    MemberInfoContext(const MemberInfo *mi);
   ~MemberInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static InheritedMemberInfoContext *alloc(const ClassDef *cd,const MemberList *ml,const QCString &title)
    { return new InheritedMemberInfoContext(cd,ml,title); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritedMemberInfoContext(const ClassDef *cd,const MemberList *ml,const QCString &title);
   ~InheritedMemberInfoContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class InheritedMemberInfoListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static InheritedMemberInfoListContext *alloc() { return new InheritedMemberInfoListContext; }
    void addMemberList(const ClassDef *cd,MemberListType lt,const QCString &title,bool additionalList=TRUE);

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    InheritedMemberInfoListContext();
   ~InheritedMemberInfoListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class AllMembersListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static AllMembersListContext *alloc(const MemberNameInfoLinkedMap &ml)
    { return new AllMembersListContext(ml); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    AllMembersListContext(const MemberNameInfoLinkedMap &ml);
   ~AllMembersListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static ArgumentContext *alloc(const Argument &arg,const Definition *def,const QCString &relPath)
    { return new ArgumentContext(arg,def,relPath); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ArgumentContext(const Argument &arg,const Definition *def,const QCString &relPath);
   ~ArgumentContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class ArgumentListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static ArgumentListContext *alloc() { return new ArgumentListContext; }
    static ArgumentListContext *alloc(const ArgumentList &al,const Definition *def,const QCString &relPath)
    { return new ArgumentListContext(al,def,relPath); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    ArgumentListContext();
    ArgumentListContext(const ArgumentList &al,const Definition *def,const QCString &relPath);
   ~ArgumentListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolContext *alloc(const Definition *def,const Definition *prev,const Definition *next)
    { return new SymbolContext(def,prev,next); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolContext(const Definition *def,const Definition *prev,const Definition *next);
   ~SymbolContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolListContext *alloc(const SearchIndexList::const_iterator &start,
                                    const SearchIndexList::const_iterator &end)
    { return new SymbolListContext(start,end); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolListContext(const SearchIndexList::const_iterator &start,
                      const SearchIndexList::const_iterator &end);
   ~SymbolListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolGroupContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolGroupContext *alloc(const SearchIndexList::const_iterator &start,
                                     const SearchIndexList::const_iterator &end)
    { return new SymbolGroupContext(start,end); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolGroupContext(const SearchIndexList::const_iterator &start,
                       const SearchIndexList::const_iterator &end);
   ~SymbolGroupContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolGroupListContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolGroupListContext *alloc(const SearchIndexList &sil)
    { return new SymbolGroupListContext(sil); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolGroupListContext(const SearchIndexList &sil);
   ~SymbolGroupListContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SymbolIndexContext *alloc(const std::string &letter,
                                     const SearchIndexList &sl,const QCString &name)
    { return new SymbolIndexContext(letter,sl,name); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolIndexContext(const std::string &letter,const SearchIndexList &sl,const QCString &name);
   ~SymbolIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SymbolIndicesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SymbolIndicesContext *alloc(const SearchIndexInfo &info)
    { return new SymbolIndicesContext(info); }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SymbolIndicesContext(const SearchIndexInfo &info);
   ~SymbolIndicesContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SearchIndexContext : public RefCountedContext, public TemplateStructIntf
{
  public:
    static SearchIndexContext *alloc(const SearchIndexInfo &info)
    { return new SearchIndexContext(info); }

    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SearchIndexContext(const SearchIndexInfo &info);
   ~SearchIndexContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

class SearchIndicesContext : public RefCountedContext, public TemplateListIntf
{
  public:
    static SearchIndicesContext *alloc() { return new SearchIndicesContext; }

    // TemplateListIntf
    virtual uint count() const;
    virtual TemplateVariant at(uint index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef()  { return RefCountedContext::addRef(); }
    virtual int release() { return RefCountedContext::release(); }

  private:
    SearchIndicesContext();
   ~SearchIndicesContext();
    class Private;
    Private *p;
};

//----------------------------------------------------

void generateOutputViaTemplate();
void generateTemplateFiles(const QCString &templateDir);

#endif
