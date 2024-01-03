#ifndef REENCRYPTOR_H
#define REENCRYPTOR_H

#include <list>
#include <string>

#include "pqi/authssl.h"

class FileData
{
public:
	int size;
	char * data;
	std::string filename;
};

class ReEncryptor
{
public:
	ReEncryptor();
	~ReEncryptor();
	void LoadCfgFile(std::string filename);
	void LoadCfgFiles(std::string folder);
	void SaveFiles();

private:
	std::list<FileData*> files;
	RsPeerId oldid;
	FileData* AttemptLoadCfgFile(std::string cfgFname, std::string signFname);
};

#endif // REENCRYPTOR_H
