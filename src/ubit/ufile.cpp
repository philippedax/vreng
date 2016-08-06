/************************************************************************
 *
 *  ufile.cpp: useful classes and functions for managing directories and file entries
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DIstrIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDIstrIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/ubit_features.h>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/ustr.hpp>
#include <ubit/upix.hpp>
#include <ubit/uappli.hpp>
using namespace std;
namespace ubit {
  
  const int UFileMode::DIR  = S_IFDIR;
  const int UFileMode::FILE = S_IFREG;
  const int UFileMode::LINK = S_IFLNK;
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  struct UFileType {
    enum {NONE, FOLDER, TXT, HTML, CODE };
    int   type;
    UStr  type_name;
    UIma* icone_ima;
  };
  
  class UFileTypeMap {
  public:
    UFileTypeMap();
    void add(const UFileType*);  // adds or replaces
    const UFileType* find(const UStr& ext) const;
    
  private:
    struct Comp {
      bool operator()(const UStr* s1, const UStr* s2) const {
        return s1->compare(*s2, true) < 0;   // ignore case
      }
    };
    typedef std::map<const UStr*, UFileType*, Comp> Map;
    Map ftmap;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  UFileMode::UFileMode(const UStr& path) {
    const char* p = path.c_str();
    struct stat st;
    // lstat pour eviter de suivre les liens (ce qui peut provoquer
    // des blocages quand les fichiers sont indisponibles par nfs)
    if (p && *p && ::lstat(p, &st) == 0)     // stat ou lstat ???
      mode = st.st_mode;
    else
      mode = 0;
  }
  
  bool UFileMode::isValid() const {return mode > 0;}
  bool UFileMode::isDir()   const {return S_ISDIR(mode);}
  bool UFileMode::isFile()  const {return S_ISREG(mode);}
  bool UFileMode::isLink()  const {return S_ISLNK(mode);}
  
  bool UFileMode::isExec() const {
    // return (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0;
    return (mode & S_IXUSR) != 0;
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  UFileInfo::UFileInfo(const UStr& path) {
    setPath(path);
  }
  
  UFileInfo::UFileInfo(const char* fullpath, const char* _name) {
    pname = new UStr(_name);
    stat(fullpath);
  }
  
  void UFileInfo::setPath(const UStr& path) {
    pname = new UStr(path);
    stat(path.c_str());
  }
  
  void UFileInfo::stat(const char* p) {
    struct stat st;
    // lstat pour eviter de suivre les liens (ce qui peut provoquer
    // des blocages quand les fichiers sont indisponibles par nfs)
    if (p && *p && ::lstat(p, &st) == 0) {    // stat ou lstat ???
      mode = st.st_mode;
      modtime = st.st_mtime;
      size = st.st_size;
    }
    else {
      mode = 0;
      modtime = 0;
      size = 0;
    }
  }
  
  
  /*
   bool UFileInfo::isIma() const {
   }
   
   bool UFileInfo::isText() const {
   }
   
   bool UFileInfo::isCode() const {
   }
   
   bool UFileInfo::isHtml() const {
   }
   */
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  UIma& UFileInfo::getMiniIconImage() const {
    if (S_ISDIR(mode)) {              // directory
      return UPix::folder;
    }
    else if (S_ISREG(mode)) {          // regular file
      if (mode & S_IXUSR) return UPix::ray; // si executable
      else return UPix::doc;          // sinon
    }
    else if (S_ISLNK(mode)) {          // link
      return UPix::link;
    }
    else return UPix::question;	 // whatever
  }
  
  UIma& UFileInfo::getIconImage() const {
    if (isDir()) {
      if (*pname == "..") return UPix::bigUp;
      else return UPix::bigFolder;
    }
    
    else if (isFile() || isLink()) {
      UStr ext = pname->suffix();
      // accepter les fichiers de backup
      if (ext[-1] == '~') ext.remove(-1,1);
      
      if (ext.equals("gif",true/*ignore case*/)
          || ext.equals("jpg",true)
          || ext.equals("jpeg",true)
          || ext.equals("tif",true)
          || ext.equals("tiff",true)
          || ext.equals("png",true)
          || ext.equals("xpm",true)
          || ext.equals("xbm",true)
          || ext.equals("ras",true)
          )
        return UPix::bigIma;
      
      else if (ext.equals("txt",true)
               || ext.equals("htm",true)
               || ext.equals("html",true)
               || ext.equals("xml",true)
               || ext.equals("pdf",true)
               || ext.equals("doc",true)
               || ext.equals("ppt",true)
               || ext.equals("xls",true)
               || ext.equals("c",true)
               || ext.equals("h",true)
               || ext.equals("cc,true")
               || ext.equals("hh",true)
               || ext.equals("cpp",true)
               || ext.equals("hpp",true)
               || ext.equals("java",true)
               )
        return UPix::bigText;
      
      else if (isLink()) return UPix::link;
      
      else if (isExec()) return UPix::ray;
    }
    return UPix::bigDoc;	 // default
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  void UFileInfo::getInfoString(UStr& s) const {
    char str[200];
    
    if (S_ISDIR(mode))       // directory
      str[0] = 'd';
    else if (S_ISREG(mode))  // regular file
      str[0] = '-';
    else if (S_ISLNK(mode))  // link
      str[0] = 'l';
    else { 	            // whatever
      if (mode & S_IFIFO) //fifo (or "named pipe") special file
        str[0] = 'p';
      else if (mode & S_IFCHR) //character special file;
        str[0] = 'c';
      else if (mode & S_IFBLK) //block special file;
        str[0] = 'b';
      else   // reste:  D (door) et s (AF_UNIX address family socket)
        str[0] = '?';
    }
    
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    
    sprintf(str+10, "%10ld", size);  // 10 chars
    str[20] = ' ';
    str[21] = 0;
    
    time_t mtime = (time_t)modtime;
    struct tm* timebuf = ::localtime(&mtime);
    ::strftime(str+21, sizeof(str)-22, "%h/%d/%Y %H:%M", timebuf);
    
    s = str;
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  static bool cmpPrefix(const UStr& path, const char* prefix) {
    for (int k = 0; prefix[k]!=0; k++) {
      if (path[k] != prefix[k]) return false;
    }
    return true;
  }
  
  int UFileInfo::parsePrefix(const UStr& path) {
    if (cmpPrefix(path, "http:") || cmpPrefix(path, "https:"))
      return HTTP;
    else if (cmpPrefix(path, "ssh:"))
      return SSH;
    else if (cmpPrefix(path, "ftp:"))
      return FTP;
    else
      return LOCAL;
  }
  
  /* ==================================================== [Elc] ======= */
  /* ==================================================== ===== ======= */
  // NB: la fonction Unix 'scandir' n'est pas standard (que BSD)!
  
  UFileDir::UFileDir() : dir_info("") {}
  
  UFileDir::UFileDir(const UStr& _path) : dir_info("") {
    read(_path, "", "", false);
  }
  
  UFileDir::~UFileDir() {
    for(unsigned int k = 0; k < filters.size(); ++k) delete filters[k];
    for(unsigned int k = 0; k < file_infos.size(); ++k) delete file_infos[k];
  }
  
  const UStr& UFileDir::getPath() const {return *dir_info.getFileName();}
  
  bool UFileDir::compareEntries(const UFileInfo* e1, const UFileInfo* e2) {
    if (e1->isDir() && !e2->isDir()) return true;
    else if (!e1->isDir() && e2->isDir()) return false;
    else return strcmp(e1->pname->c_str(), e2->pname->c_str()) < 0;
  }
  
  static bool matchFilters(const char* dname, const std::vector<UStr*>& filters) {
    const char* ext = UCstr::suffix(dname);
    if (!ext || !*ext) return false;      // no suffix
    // test all filters
    for (unsigned int k = 0; k < filters.size(); ++k) {
      if (UCstr::compareN(ext, filters[k]->c_str(), filters[k]->length(), true) == 0) {
        return true;
      }
    }
    return false;
  }
  
  void UFileDir::read(const UStr& _path) {read(_path, "", "", false);}
  
  void UFileDir::read(const UStr& _path, const UStr& prefix, 
                      const UStr& filter, bool hidden_files) {
    for(unsigned int k = 0; k < file_infos.size(); ++k) delete file_infos[k];
    file_infos.clear();
    
    UStr path = _path;
    expandDirPath(path);
    dir_info.setPath(path);
    
    if (!dir_info.pname) return;
    ::DIR *dirp = ::opendir(dir_info.pname->c_str());
    if (!dirp) return;    // ce directory n'existe pas
    
    parseFilter(filters, filter);
    int pathlen = dir_info.pname->length();
    
    // fullpath = chemin d'acces complet des fichiers
    char* fullpath = new char[pathlen + 1000];
    strcpy(fullpath, dir_info.pname->c_str());
    
    // pointe sur le debut du nom de fichier dans le fullpath
    char *namepos = null;;
    if (fullpath[pathlen-1] != '/') {
      fullpath[pathlen]   = '/';
      fullpath[pathlen+1] = 0;
      namepos = fullpath + pathlen+1;
    }
    else namepos = fullpath + pathlen;
    
    if (prefix.c_str() && prefix.c_str()[0]=='.') hidden_files = true;
    
    //struct stat statstr;
    struct dirent* de = null;
    
    while (dirp) {
      if (!(de = readdir(dirp)))	// erreur ou fin de liste
        break;
      else {
        char *dname = de->d_name;
        
        if (dname[0] == 0) continue;
        else if (dname[0]=='.') {
          // skip .
          if (dname[1]==0) continue;
          // skip ..
          else if (dname[1]=='.' && dname[2]==0) continue;
          // skip files starting by . depending on mode
          else if (!hidden_files) continue;
        }
        
        // SKIP si ne correspond pas au filtre (sauf ..)
        if (prefix.c_str()) {
          if (strncmp(dname, prefix.c_str(), prefix.length()) != 0) continue;
        }
        
        // copier nom du fichier dans fullpath pour faire stat()
        strcpy(namepos, dname);
        
        //EX: lstat(fullpath, &statstr);
        UFileInfo* finfo = new UFileInfo(fullpath, dname);
        
        // SKIP si pas directory et ne correspond pas au filtre
        // NOTE: on ne filtre PAS les directories (sinon resultat illisible)
        //
        //if (filters.size() > 0 && !S_ISDIR(statstr.st_mode)) 
        if (filters.size() > 0 && !finfo->isDir() && !matchFilters(dname, filters))
          delete finfo;
        else file_infos.push_back(finfo);
      }
    }
    
    sort(file_infos.begin(), file_infos.end(), UFileDir::compareEntries);
    ::closedir(dirp);
    delete fullpath;
  }
  
  /* ==================================================== ===== ======= */
  
  void UFileDir::readRemote(const UStr& path, const UStr& prefix,
                            const UStr& filter, bool hidden_files) {
    UStr host, fpath, cachepath;
    UFileCache::getCachePath(path, UFileInfo::SSH, host, fpath, cachepath);
    
    dir_info.setPath(cachepath);
    if (!dir_info.pname) return;
    ifstream in(dir_info.pname->c_str());
    if (!in) return;
    
    parseFilter(filters, filter);
    int pathlen = dir_info.pname->length();
    
    // fullpath = chemin d'acces complet des fichiers
    char* fullpath = new char[pathlen + 1000];
    strcpy(fullpath, dir_info.pname->c_str());
    
    // pointe sur le debut du nom de fichier dans le fullpath
    char *namepos = null;;
    if (fullpath[pathlen-1] != '/') {
      fullpath[pathlen]   = '/';
      fullpath[pathlen+1] = 0;
      namepos = fullpath + pathlen+1;
    }
    else namepos = fullpath + pathlen;
    
    if (prefix.c_str() && prefix.c_str()[0]=='.') hidden_files = true;
    
    string line;
    while ((getline(in, line))) {  // 1 name by line
      if (!line.empty()) {
        const char* dname = line.c_str();
        
        if (dname[0] == 0) continue;
        else if (dname[0]=='.') {
          // skip .
          if (dname[1]==0) continue;
          // skip ..
          else if (dname[1]=='.' && dname[2]==0) continue;
          // skip files starting by . depending on mode
          else if (!hidden_files) continue;
        }
        
        // SKIP si ne correspond pas au filtre (sauf ..)
        if (prefix.c_str()) {
          if (strncmp(dname, prefix.c_str(), prefix.length()) != 0) continue;
        }
        
        // copier nom du fichier dans fullpath pour faire stat()
        strcpy(namepos, dname);
        
        UFileInfo* finfo = new UFileInfo("@ssh", dname);
        
        int ll = strlen(dname);
        if (dname[ll-1] == '/') finfo->mode = UFileInfo::DIR;
        else if (dname[ll-1] == '@') finfo->mode = UFileInfo::LINK;
        else finfo->mode = UFileInfo::FILE;
        
        // SKIP si pas directory et ne correspond pas au filtre
        // NOTE: on ne filtre PAS les directories (sinon resultat illisible)
        //
        //if (filters.size() > 0 && !S_ISDIR(/*statstr.st_*/mode)) 
        if (filters.size() > 0 && !finfo->isDir() && !matchFilters(dname, filters))
          delete finfo;
        else file_infos.push_back(finfo);
      }
    }
    
    sort(file_infos.begin(), file_infos.end(), UFileDir::compareEntries);
    delete fullpath;
  }
  
  /* ==================================================== ===== ======= */
  // format: blabla (*.html; *.xhtml; *.*)
  
  void UFileDir::parseFilter(vector<UStr*>& filters, const UStr& filter_str) {
    const char* p = filter_str.c_str();
    if (!p) return;
    
    while (*p!= 0) {
      
      if (*p == '*' && *(p+1) == '.') {
        p += 2;
        if (*p == '*') p++;  // *.* or *.*aa
        
        const char* e = p;
        for (; *e!= 0; e++) {
          if (*e == ';' || *e == ')' || *e == ' ' || *e == '\n' ||*e == '\t')
            break;
        }
        
        if (e != p) {
          UStr* s = new UStr();
          s->insert(0, p, 0, e-p);
          filters.push_back(s);
        }
        if (*e) p = e+1; else break;;
        
      }
      else p++;
    }
  }
  
  /* ==================================================== ===== ======= */
  
  void UFileDir::expandDirPath(UStr& dir) {
    // faudrait virer les blancs au debut et a la fin !!!!!
    if (dir.empty()) {
      dir = "/";
    }
    
    else if (dir.equals(".") || dir.equals("./")) {
      const char* pwd = ::getenv("PWD");  // pas de free() sur un getenv!
      if (pwd) dir = pwd; else dir = "/";
    }
    
    else {
      const char* s = dir.c_str();
      const char* p = strstr(s, "/..");
      if (p && (*(p+3)=='/' || *(p+3)==0)) {
        const char* e = p+2;
        if (p != s) {
          p--;
          while (p > s && *p != '/') p--;
          if (*p == '/') dir.remove(p-s, e-p+1);
        }
      }
    }
    
    //ajouter un / final si y'en a pas
    if (dir[-1] != '/') dir.append("/");
  }
  
  /* ==================================================== [Elc] ======= */
  /* ==================================================== ===== ======= */
  
  static void makeDir(const UStr& path) {
    if (path.empty()) return;
    UFileMode fm(path);
    if (fm.isDir()) return;  // already created
    UStr com = "mkdir " & path;
    system(com.c_str());     // eviter system
  }
  
  static UStr normSlashes(const UStr& in) {
    UStr s = in;
    for (int k = 0; k < s.length(); k++) {
      if (s[k] == '/') s.setCharAt(k,'%');
    }
    return s;
  }
  
  // ATT: ca va dependre de la platform !!!
  static UStr normURL(const UStr& in) {
    UStr s = in;
    for (int k = 0; k < s.length(); k++) {
      if (s[k] == ' ') {s.replace(k, 1, "\\ "); k++;}
      else if (s[k] == '~') {s.replace(k, 1, "%7E"); k++;}
    }
    return s;
  }
  
  /*
   static UStr normSpaces(const UStr& in) {
   UStr s = in;
   for (int k = 0; k < s.length(); k++) {
   if (s[k] == ' ') {s.replace(k, 1, "\\ "); k++;}
   }
   return s;
   }
   */
  /* ==================================================== ===== ======= */
  
  UStr UFileCache::cache;
  
  void UFileCache::cleanCache() {
    if (!cache.empty()) {
      UStr com = "rm -r " & cache;
      system(com.c_str());     // eviter system
    }
  }
  
  void UFileCache::createCache() {
    const char* user = ::getenv("USER");
    if (!user | !*user) return;
    
    cache = "/tmp/" & UAppli::getName() & "-" & user;
    cleanCache();
    
    makeDir(cache);
    cache.append('/');
    
    makeDir(cache & "http:");
    makeDir(cache & "ssh:");
    makeDir(cache & "ftp:");
  }
  
  
  const UStr& UFileCache::getCachePath() {
    if (cache.empty()) createCache();
    return cache;
  }
  
  /* ==================================================== ===== ======= */
  // "ssh://toto.enst.fr"
  // "ssh://toto.enst.fr/"
  // "ssh://toto.enst.fr:"
  // "ssh://toto.enst.fr/Public"
  // "ssh://toto.enst.fr/Public/"
  // "ssh://toto.enst.fr:Public"
  // "ssh://toto.enst.fr:Public/"
  
  bool UFileCache::getCachePath(const UStr& url, int path_type,
                                UStr& server, UStr& spath, UStr& cachepath) {
    if (cache.empty()) createCache();
    int dd = url.find("://");
    if (dd) server.insert(0, url, dd+3);   // dont copy "ssh://" prefix
    else server = "";
    
    if (server.empty()) {
      spath = "";
      cachepath = "";
      return false;
    }
    
    if (server[-1]=='/' || server[-1]==':') server.remove(-1, 1);  // remove last /
    
    const char* s = server.c_str();
    const char* p = s;
    for (; *p; p++) {      // find the spath if any
      if (*p == '/' || *p == ':') break;
    }
    
    if (path_type == UFileInfo::SSH) {
      if (*p==0 || *(p+1)==0) {
        spath = "";
        cachepath = cache & "ssh:/" & server;
      }
      else {
        spath = server.split(p-s, true);
        cachepath = cache & "ssh:/" & server & ":" & normSlashes(spath);
      }
    }
    
    else {  // HTTP
      spath = normURL(server);
      cachepath = cache & "http:/" & spath;
      cerr << "GEN " << "server: " << server << " spath " << spath << " cache "
      << cachepath << endl;
    }
    
    return true;
  }
  
  /* ==================================================== ===== ======= */
  
  const UStr* UFileCache::getOrCreateFullPath(const UStr& path) {
    if (path.empty()) return &path;
    int type = UFileInfo::parsePrefix(path);
    
    if (type == UFileInfo::LOCAL) {
      if (path[0] != '~') return &path;
      else {
        UStr* fullp = new UStr(path);
        //NB: ne PAS faire de free() sur un getenv!
        char* home = getenv("HOME");
        if (home) fullp->replace(0, 1, home); // virer ~mais pas /
        return fullp;
      }
    }
    else {
      UStr server, spath;
      UStr* cachepath = new UStr();
      getCachePath(path, type, server, spath, *cachepath);
      return cachepath;
    }
  }
  
  UStr* UFileCache::createFullPath(const UStr& path) {
    const UStr* fullp = getOrCreateFullPath(path);
    if (fullp == &path) return new UStr(path);
    else return const_cast<UStr*>(fullp);
  }
  
}
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
