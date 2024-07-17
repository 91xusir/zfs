


#ifndef FILE_ERROR_H 
#define FILE_ERROR_H 
#include <iostream>
#include <string>

using std::string;

class CFileErr
{
public:
	virtual void OnInit()=0;
	virtual void OnClear()=0;

	virtual CFileErr& operator <<(const string vMessage)=0;
};


class CFileErrStd:public CFileErr
{
public:
	virtual void OnInit(){};
	virtual void OnClear(){};

	virtual CFileErr& operator <<(const string vMessage){std::cout<<vMessage;return *this; };

};




#endif//PACK_ERROR_H
