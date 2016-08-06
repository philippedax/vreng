#include "inputfile.h"

#include <list>

using namespace std;

typedef list<class Skel *> SKEL_STACK;
typedef list<class Channel *> CHANNEL_LIST;

/* This is the BVH loader */

class BVH : public InputFile {
	public:
		BVH() { state = 0;};
	~BVH() {};

	virtual Skel *LoadFile(char *filename);
	private:
	istream *thefile;

	int state;
};
