#include "inputfile.h"

#include <list>

using namespace std;

typedef list<class Skel *> SKEL_STACK;
typedef list<class Channel *> CHANNEL_LIST;

/* This is the Bvh loader */

class Bvh : public InputFile {
	public:
		Bvh() { state = 0;};
	~Bvh() {};

	virtual Skel *LoadFile(char *filename);
	private:
	istream *thefile;

	int state;
};
