/************************************************************************
 *
 *  ufile.hpp
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

#ifndef _ufile_hpp_
#define	_ufile_hpp_ 1
#include <vector>
#include <ubit/unode.hpp>
namespace ubit {
  
  /* ==================================================== ======== ======= */
  /** File status returned by functions that open files.
   */
  struct UFileStat {
    enum {
      UnknownType   = -5,
      MiscError     = -4,
      NoMemory      = -3,
      InvalidData   = -2,
      CannotOpen    = -1,
      NotOpened     =  0,
      Opened        =  1
    };
  };
  
  /** synonym of UFilestat.
   */
  typedef UFileStat UFilestat;
  
  /* ==================================================== ======== ======= */
  /** file mode.
   */
  class UFileMode {
  public:
    UFileMode(const UStr& pathname);
    
    bool isValid() const;
    bool isDir()  const;
    bool isFile() const;
    bool isLink() const;
    bool isExec() const;
    
    static const int DIR, FILE, LINK; ///< modes
    int getMode() const {return mode;}
    
  protected:
    UFileMode() {}
    int mode;  //mode_t mode;
  };
  
  /* ==================================================== ======== ======= */
  /** file info.
   */
  class UFileInfo : public UFileMode {
  public:
    UFileInfo(const UStr& path);
    ///< sets fileinfo data for this path and stores path as a name.
    
    UFileInfo(const char* fullpath, const char* fname);  
    ///< sets fileinfo data for fullpath and stores fname as a name.
    
    void setPath(const UStr& path);
    ///< resets fileinfo data for this path.
    
    const UStr* getFileName() const {return pname;}
    ///< returns the file name (without the path).
    
    unsigned long getSize() const {return size;}
    //< returns the size of the file.
    
    unsigned long getModTime() const {return modtime;}
    //< returns the time when file data was last modified.
    
    UIma& getIconImage() const;
    /**< return an image that characterizes this file type.
     * the returned image is shared and can't be deleted.
     */
    
    UIma& getMiniIconImage() const;
    /**< return an mini image that characterizes this file type.
     * the returned image is shared and can't be deleted.
     */
    
    void getInfoString(UStr& str) const;
    ///< copies infos in str.
    
    enum PrefixType {LOCAL, SSH, HTTP, FTP};  
    static int parsePrefix(const UStr& path);
    ///< returns the PrefixType.
    
  protected:
    friend class UFileDir;
    friend class UFilebox;
    unsigned long size;
    unsigned long modtime;
    uptr<UStr> pname;
    void stat(const char* path);
  };
  
  /** file info container.
   */
  typedef std::vector<UFileInfo*> UFileInfos;
  
  /* ==================================================== ======== ======= */
  /** File Directory.
   */
  class UFileDir {
  public:
    UFileDir();
    UFileDir(const UStr& path);
    virtual ~UFileDir();
    
    void read(const UStr& path);
    /**< reads this directory and gets file entries.
     * @see read(const UStr& path, const UStr& prefix, const UStr& filter, bool with_dot_files)
     */
    
    void read(const UStr& path, const UStr& prefix, const UStr& filter, bool with_dot_files);
    /**< reads this directory and gets file entries.
     * Args:
     * - prefix: get files that start witrh this prefix
     * - filter: get files with this filter.
     *   filter syntax: "C++ Files (*.cpp; *.hpp)" or "*.cc; *.hh" or "*.*" 
     * _ with_hidden_files: get files starting with a dot.
     */
    
    void readRemote(const UStr& path, const UStr& prefix, const UStr& filter, bool with_dot_files);
    /**< reads a remote directory and gets file entries.
     * same as read() but SSH is used to acces a remote directory
     */
    
    const UStr& getPath() const;
    /**< returns the full pathname of the directory
     * note that the path arg. given to the constr, read(), readRemote()
     * is expanded, so that getPath() may return a different string.
     */
    
    const UFileInfos& getFileInfos() const {return file_infos;}
    ///< returns the (filtered) file entries of the directory.
    
    static void expandDirPath(UStr& dirpath);
    ///< expands the directory path (if there are path symbols in the dirpath).
    
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  private:
    friend class UIconbox;
    UFileInfo dir_info;
    UFileInfos file_infos;
    std::vector<UStr*> filters;
    static bool compareEntries(const UFileInfo*, const UFileInfo*);  
    static void parseFilter(std::vector<UStr*>& filters, const UStr& filter);
  };
  
  /* ==================================================== ======== ======= */
  /** file cache for SSH.
   */
  class UFileCache {
  public:
    static void createCache();
    static void cleanCache();
    
    static const UStr& getCachePath();
    
    static UStr* createFullPath(const UStr& path);
    //  expands ~, ssh:, http:, ftp:.
    
    static const UStr* getOrCreateFullPath(const UStr& path);
    // same as createFullPath() but fullpath may be shared with path
    
    static bool getCachePath(const UStr& url, int path_type,
                             UStr& server, UStr& path, UStr& cachepath);
  protected:
    static UStr cache;
  };
  
}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
