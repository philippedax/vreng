/************************************************************************
 *
 *  udoc.hpp: Document Element
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#ifndef _udoc_hpp_
#define _udoc_hpp_ 1
#include <list>
#include <map>
#include <ubit/udefs.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uboxes.hpp>
namespace ubit {
  
  class UDocCreator;
  class UDocAttachment;
  
  /** UDocAttachment List.
   */
  typedef std::list<UDocAttachment*> UDocAttachments;
  
  /* ==================================================== ====== ======= */
  /** Document Factory: use this class to read documents.
   * See class UDoc for info on "documents".
   */
  class UDocFactory {
  public:
    UDocFactory();
    virtual ~UDocFactory();
    
    static UDocFactory& getDefaultFactory();
    /**< returns the default factory for reading/creating documents.
     */
    
    virtual UDoc* create(const UStr& name);
    /**< creates an empty document.
     * creates an instance of the appropriate UDoc subclass depending
     * on 'name' suffix.
     */
    
    virtual UDoc* load(const UStr& name, const UStr& buffer);
    /**< loads a document from a buffer in memory.
     * creates an instance of the appropriate UDoc subclass depending
     * on 'name' suffix.
     */
    
    virtual UDoc* read(const UStr& filename);
    /**< reads a document from a file.
     * reads and creates an instance of the appropriate UDoc subclass
     *  depending on 'filename' suffix.
     */
    
    virtual int getStatus() const;
    /**< status of the last (read/load/create) operation.
     * value is:
     * - > 0 if the document could be created
     * - < 0 if the file cant be read (see UFilestat)
     * - = 0 if the type of the document is unknown (see getCreator, addCreator)
     */ 
    
    virtual void saveErrors(bool);
    virtual UStr* getErrors() const; 
    /**< return last errors messages if saveErrors() was set to true.
     * errors are printed on stderr otherwise.
     */
    
    virtual UDocCreator* getCreator(const UStr& type);
    /**< returns the creator for this type (if any, null otherwise).
     * by convention, the 'type' is the suffix of the file
     * (e.g. "html", "txt", "cpp" ...)
     *
     * Note: certains creators (such as the HTML or XML creators)
     * have specific options. This function provides a mean to retrieve
     * the actual creator and to change its options.
     */
    
    virtual UDocCreator* getDefaultCreator();
    ///< returns the default document creator.
    
    virtual void addCreator(const UStr& type, UDocCreator&);
    ///< adds a document creator.
    
    virtual void setDefaultCreator(UDocCreator&);
    ///< changes the creator that is used when the document type is unknown.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    struct Comp {
      bool operator()(const UStr* s1, const UStr* s2) const;
    };
    typedef std::map<const UStr*, UDocCreator*, Comp> Map;
    Map rmap;
    UDocCreator* default_creator;
    int stat;
    uptr<UStr> errors;
  };
  

  /* ==================================================== ====== ======= */
  /** Generic Document
   *
   * a UDoc is just a tree (or acyclic graph) of various Ubit objects.
   * It can be modified in the same way as other Ubit instance trees.
   *
   * UDoc can't be instantiated directly, only appropriate subclasses
   * can be (such as UXmlDocument, etc.) 
   * 
   * UDocFactory::read() reads a document file and creates an instance 
   * of the appropriate UDoc subclass depending on the file suffix
   */
  class UDoc : public UBox {
  public:
    UCLASS(UDoc)
    
    static UDocFactory& getDefaultFactory();
    ///< returns the default factory (for reading/creating documents).
    
    virtual ~UDoc();
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual bool isEditable() const;
    virtual void setEditable(bool state = true);
    
    virtual const UStr& getPath() const {return *ppath;}
    ///< returns the pathname of the document.
    
    virtual void setPath(const UStr& path);
    virtual void makePath(UStr& fullpath, const UStr& url);
    /**< creates a full pathname from an attachment or an URL.
     * returns true if the fullname is a local filename.
     * returns false otherwise (typically is the fullname starts with http:)
     */
    
    virtual const UDocAttachments* getAttachments() const {return null;}
    ///< returns the objects (images, stylesheets, etc) that are attached to this document.
    
    virtual int loadAttachments(bool reload = false) {return 0;}
    /**< loads the attachments.
     * - if 'reload' is false: load the attachments that are not already loaded
     * - if 'reload' is true: reload all the attachments
     * - returns: the number of attachments that are still unloaded
     */
    
    virtual bool loadAttachment(UDocAttachment*, bool reload = false) {return false;}
    ///< loads this attachment.
    
    virtual void addLinkCallback(const UChild& cond_slash_callback);
    /**< adds a callback that observes events in the document's hyperlinks.
     * 'cond_slash_callback' is condition/callback expression such as: 
     * <pre>   UOn::action / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback (ie. 'foo') will be fired when the condition (ie. 'UOn::action')
     * occurs in a link (typically a href anchor) of this document.
     */
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void linkEventCB(UInputEvent&, const UStr* path);
    
  protected:
    UDoc();
    UDoc(const UStr& path);
    ///< NB: the constr is protected: use read() or a subclass to create a UDoc.
    
  private:
    uptr<UStr> ppath;
    uptr<UElem> call_list;
    UDoc(const UDoc&);             // these operators are forbidden
    UDoc& operator=(const UDoc&);
  };
  
  /* ==================================================== ======== ======= */
  /** Document attachment (image, stylesheet, etc.)
   */
  class UDocAttachment {
  public:
    virtual ~UDocAttachment() {}
    virtual const UStr& getUrl() const = 0;
    virtual const UStr& getType() const = 0;
    virtual bool isLoaded() const = 0;
    virtual int  load(UDoc*) = 0;
  };
  
  /* ==================================================== ======== ======= */
  /** Document source (file, buffer, etc.).
   */
  struct UDocSource {
    enum From {PATHNAME, BUFFER, SCRATCH};
    
    UDocSource(int from, const UStr* path, UStr* errors, const UStr* buffer = null);
    ///< note that path and errors are not duplicated.
  
    int from;
    int stat;
    uptr<const UStr> path, fullpath;
    const UStr* buffer;
    UStr* errors;
  };
  
  
  /* ==================================================== ======== ======= */
  /** Document creator interface (intended to be subclassed).
   */
  class UDocCreator {
  public:
    virtual ~UDocCreator() {}
    virtual UDoc* create(UDocSource&) = 0;
    ///< create or load the document specified by the UDocSource.
  };
  
  
  /* ==================================================== ======== ======= */
  /**< Base of flat documents that just contains flat text or an image
   * see also: UXmlDocument for XML "hierarchical" documents.
   */
  class UFlatDoc : public UDoc {
  public:
    UFlatDoc(const UStr& _pathname);
    virtual bool isEditable() const;
    virtual void setEditable(bool state = true);
    uptr<class UEdit> edit;
  };
  
  
  /* ==================================================== ======== ======= */
  /**< Documents that needs a plugin to be displayed. 
   */
  class UPluginDoc : public UDoc {
  public:
    UPluginDoc(const UStr& _pathname);
    //virtual void setEditable(bool state = true) {}
  };
  
}
#endif
