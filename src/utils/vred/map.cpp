#include "vred.hpp"
#include "map.hpp"
#include "gif.hpp"


int Map::defaultTexSize = 128;

//#define VERBOSE 1

Map::Map(const char* const _url)
{
  data = NULL;
  width = 0;
  height = 0;
  error = false;
  url = (char *) strdup((char *) _url);
}

Map::~Map()
{
  if (data && !error) free(data);
  if (url) free(url);
}

char* httpGet(char* serverName, char* chemin, int& pos)
{
  struct hostent *hostptr;
  struct sockaddr_in serv_addr;

#ifdef VERBOSE
  printf("url server=%s path=%s\n", serverName, chemin);
#endif

  if ((hostptr = gethostbyname(serverName)) == NULL) {
#ifdef VERBOSE
    printf("No DNS for host\n");
#endif
    return 0;        
  }
  
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *) &serv_addr.sin_addr, (char *)hostptr->h_addr, hostptr->h_length);
  serv_addr.sin_port = htons(80);

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
#ifdef VERBOSE
    printf("socket error 1\n");
#endif
    return 0;
  }

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
#ifdef VERBOSE
    printf("socket error 2\n");
#endif
    return 0;
  } 

  int res = 0;
  int size = 512;
  pos = 0;

  char* temp = (char*) malloc(size);

  sprintf(temp,"GET /%s HTTP/1.0\r\n\r\n", chemin);
  write(sockfd, temp, strlen(temp));

#ifdef VERBOSE
  printf("waiting for reply\n");
#endif

  res = read(sockfd, temp, size);
  if (res == -1) {
#ifdef VERBOSE
    printf("erreur !\n"); 
#endif
    close(sockfd);
    free(temp);
    return NULL;
  }  

  int marker;
  for (marker=0; marker < size-1; marker++) {
    if (temp[marker] == '\n' && temp[marker+1] == '\r') break;
  }
  if (marker == size-1) {
#ifdef VERBOSE
    printf("erreur !\n"); 
#endif
    free(temp);
    close(sockfd);
    return 0;    
  }

  marker += 3;
#ifdef VERBOSE
  printf("Marker found=%d\n", marker);
#endif

  for (int i=marker; i < res; i++) {
    temp[i-marker] = temp[i];
  }

  pos = res-marker;

  if (res == 0) {
    close(sockfd);
    return temp;
  }

  //Voila la suite
  while (1) {   

    size += 512;
    temp = (char*) realloc(temp, size);
    res = read(sockfd, temp+pos, 512);
    pos += res;
    
    if (res == -1) break;
    if (res == 0) break;
  }

  if (res == -1) {
#ifdef VERBOSE
    printf("erreur !\n"); 
#endif
    close(sockfd);
    free(temp);
    return NULL;
  }
#ifdef VERBOSE
  printf("Ok, fini, size=%d\n", pos);
#endif

  close(sockfd);
  return temp;
}

const GLubyte* const Map::getData()
{
  if (error) return 0;
  if (data != 0) return data;

  unsigned char rgbpalette[3*256];
  int sizeGif;
  char* fromWeb = NULL;

#ifdef VERBOSE
  printf("url '%s'\n", url);
#endif 

  // parsing

  if (strncmp(url, "file://", 7) == 0) {

#ifdef VERBOSE
    printf("File url %s\n", url);
#endif    
    FILE* in = fopen(url+7, "rb");
    if (in == NULL) {
      printf("file not found !\n");
      free(url);
      url = NULL;
      error = true;
      data = 0;
      return 0;
    }
    fseek(in, 0, SEEK_END);
    sizeGif = ftell(in);
    fseek(in, 0, SEEK_SET);
#ifdef VERBOSE
    printf("filesize=%d\n", sizeGif);
#endif

    fromWeb = (char *) malloc(sizeGif);
                                            
    if (fromWeb == NULL) {
      free(url);
      url = NULL;
      error = true;
      data = 0;
      fclose(in);
      return 0;
    }

    int size = fread(fromWeb, 1, sizeGif, in);
    if (size != sizeGif) {
	printf("Error while loading the file\n");
	free(fromWeb);
	fclose(in);
	free(url);
	url = NULL;
	error = true;
	data = 0;
	return 0;
    }
    fclose(in);

  } else if (strncmp(url, "http://", 7) == 0) {
    int i;
    for (i=7; url[i] != 0; i++) {
      if (url[i] == '/') 
	break;
    }
    if (url[i] == 0) {
      printf("bad http url %s\n", url);
      free(url); url = NULL;
      error = true;
      data = 0;
      return 0;    
    }
    url[i] = 0;
    
    //Get http
    fromWeb = httpGet(url+7, url+i+1, sizeGif);
    if (fromWeb == 0) {
      free(url); url = NULL;
      error = true;
      data = 0;
      return 0;
    }
  }
  else if (url[0] == '/') {
    int i;
    for (i=1; url[i]!=0; i++) {
      if (url[i] == '/') 
	break;
    }
    if (url[i] == 0) {
      printf("bad http url %s\n", url);
      free(url); url = NULL;
      error = true;
      data = 0;
      return 0;    
    }
    url[i] = 0;
    
    //Get http
    fromWeb = httpGet(url+1, url+i+1, sizeGif);
    if (fromWeb == 0) {
      free(url); url = NULL;
      error = true;
      data = 0;
      return 0;
    }
  }
  else {
#ifdef VERBOSE
    printf("url unknown '%s'\n", url);
#endif
    free(url); url = NULL;
    error = true;
    data = 0;
    return 0;
  }
  free(url); url = NULL;

  // uncompress
  int w, h;
#ifdef VERBOSE
  unsigned char* gif = load_gif((unsigned char*)fromWeb, sizeGif, rgbpalette, w, h, 1);
  printf("size x=%d, y=%d\n", w, h);
#else
  unsigned char* gif = load_gif((unsigned char*)fromWeb, sizeGif, rgbpalette, w, h, 0);
#endif
  width = w;
  height = h;
  if (gif == 0) {
    printf("Gif: error\n"); 
    error = true;
    data = 0;
    return 0;
  }

  if (data != 0) free(data);
  data = (GLubyte*) malloc(width*height*3);
  if (data == 0) { 
    free(gif);
#ifdef VERBOSE
    printf("Gif: error, not enough memory\n");
#endif
    error = true;
    data = 0;
    return 0;
  }

  // unpaletize
  for (int j=0; j< height; j++) {
    for (int i=0; i< width;i++) {
      int car = int(gif[j*width+i]);
      data[3*(j*width+i)]   = rgbpalette[car*3];
      data[3*(j*width+i)+1] = rgbpalette[car*3+1];
      data[3*(j*width+i)+2] = rgbpalette[car*3+2];
    }
  }
  free(gif);

  // scaling
  void* finaltex = malloc(defaultTexSize*defaultTexSize*3);
  gluScaleImage(GL_RGB, width, height, GL_UNSIGNED_BYTE , (void*) data,
		defaultTexSize, defaultTexSize, GL_UNSIGNED_BYTE, finaltex);
  width = defaultTexSize;
  height = defaultTexSize;
  free(data);
  data = (GLubyte*) finaltex;
#ifdef VERBOSE
  printf("Gif: OK %d %d\n", width, height);
#endif

  return data;
}

int Map::getWidth() const
{
  return width;
}

int Map::getHeight() const
{
  return height;
}
