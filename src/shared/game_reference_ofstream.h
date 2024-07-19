


#ifndef GAME_REFERENCE_OFSTREAM_H 
#define GAME_REFERENCE_OFSTREAM_H

#include <string>
#include <fstream>


using std::string;

class RefOfstream
{
public:
	RefOfstream(const string& vrFileName)
		:mRef(1),mFile(vrFileName.c_str(),std::ios::app){};
	~RefOfstream(){};

	int AddRef(){return ++mRef;}
	int DelRef(){return --mRef;}

	std::ofstream&  Get()
	{
		return mFile; 
	};
private:
	int				mRef;
	std::ofstream	mFile;
};

#endif//GAME_REFERENCE_OFSTREAM_H