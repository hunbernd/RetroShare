#include "reencryptor.h"

#include "util/rsprint.h"
#include "util/folderiterator.h"

ReEncryptor::ReEncryptor()
{
	oldid = AuthSSL::instance().OwnId().toStdString();
}

ReEncryptor::~ReEncryptor()
{
	for (std::list<FileData*>::iterator it=files.begin(); it != files.end(); ++it){
		FileData* fd = *it;
		delete[] fd->data;
		delete fd;
	}
	files.clear();
}

void ReEncryptor::LoadCfgFile(std::string filename)
{
	std::string cfgFname = filename;
	std::string cfgFnameBackup = cfgFname + ".tmp";

	std::string signFname = filename +".sgn";
	std::string signFnameBackup = signFname + ".tmp";

	FileData* fd;
	if((fd = AttemptLoadCfgFile(cfgFname, signFname)) != nullptr)
	{
		std::cerr << "Successfully loaded configfile " << cfgFname << std::endl;
		files.push_back(fd);
	} else if((fd = AttemptLoadCfgFile(cfgFnameBackup, signFnameBackup)) != nullptr)
	{
		std::cerr << "Successfully loaded configfile from backup " << cfgFnameBackup << std::endl;
		fd->filename = cfgFname; //Keep the original filename, insteadof the backup file
		files.push_back(fd);
	} else {
		std::cerr << "Cannot load configfile, will not be migrated " << cfgFname << std::endl;
	}
}

void ReEncryptor::LoadCfgFiles(std::string folder)
{
	std::string ending = ".cfg";
	librs::util::FolderIterator dirIt(folder,false);
	if(!dirIt.isValid())
	{
		return;
	}

	for(;dirIt.isValid();dirIt.next())
	{
		if(dirIt.file_type() == librs::util::FolderIterator::TYPE_FILE)
			if(dirIt.file_name().size() >= ending.size() && dirIt.file_name().substr( dirIt.file_name().size() - ending.size()) == ending)
			{
				LoadCfgFile(dirIt.file_fullpath());
			}
	}
	dirIt.closedir();
}

void ReEncryptor::SaveFiles()
{
	std::string newid = AuthSSL::instance().OwnId().toStdString();
	for (auto it = files.begin(); it != files.end(); ++it)
	{
		FileData* fd = *it;
		std::string newfile = fd->filename;
		newfile.replace(newfile.find(oldid), oldid.length(), newid);

		//------------
		std::string cfgFname = newfile;
		std::string signFname = newfile + ".sgn";

		std::cerr << "(II) Saving configuration file " << cfgFname << std::endl;

		uint32_t bioflags = BIN_FLAGS_HASH_DATA | BIN_FLAGS_WRITEABLE;
		BinEncryptedFileInterface *cfg_bio = new BinEncryptedFileInterface(cfgFname.c_str(), bioflags);

		cfg_bio->senddata(fd->data, fd->size);

		/* store the hash */
		RsFileHash strHash = cfg_bio->gethash();
		delete cfg_bio;

		/* sign data */
		std::string signature;
		AuthSSL::getAuthSSL()->SignData(strHash.toByteArray(),strHash.SIZE_IN_BYTES, signature);

		/* write signature to configuration */
		BinMemInterface *signbio = new BinMemInterface(signature.c_str(),
				signature.length(), BIN_FLAGS_READABLE);

		signbio->writetofile(signFname.c_str());
		delete signbio;
		//------------
	}
}

FileData* ReEncryptor::AttemptLoadCfgFile(std::string cfgFname, std::string signFname)
{
	uint32_t bioflags = BIN_FLAGS_HASH_DATA | BIN_FLAGS_READABLE;

	BinFileInterface *bio = new BinFileInterface(cfgFname.c_str(), bioflags);

	char* encryptedData = NULL;
	int encrypDataLen = 0;

	uint64_t encrypDataLen64 = bio->getFileSize();

	if(encrypDataLen64 > uint64_t(~(int)0))
	{
		std::cerr << __PRETTY_FUNCTION__ << ": cannot decrypt files of size > " << ~(int)0 << std::endl;
		return nullptr;
	}
	encrypDataLen = (int)encrypDataLen64 ;
	encryptedData = new char[encrypDataLen];

	// make sure assign was successful
	if(encryptedData == NULL)
		return nullptr;

	if(-1 == bio->readdata(encryptedData, encrypDataLen))
	{
		delete[] encryptedData;
		return nullptr;
	}

	int size = 0;
	char* decrypteddata;
	if((encrypDataLen > 0) && (encryptedData != NULL))
	{
		if(!AuthSSL::getAuthSSL()->decrypt((void*&)(decrypteddata), size, encryptedData, encrypDataLen))
		{
			delete[] encryptedData;
			return nullptr;
		}
		delete[] encryptedData;
	}

	//--- Hash check ---

	// In order to check the signature that is stored on disk, we compute the hash of the current data (which should match the hash of the data on disc because we just read it),
	// and validate the signature from the disk on this data. The config file data is therefore hashed twice. Not a security issue, but
	// this is a bit inelegant.

	std::string signatureRead;
	RsFileHash strHash = bio->gethash();

	BinFileInterface bfi(signFname.c_str(), BIN_FLAGS_READABLE);

	if(bfi.getFileSize() == 0)
		return nullptr;

	RsTemporaryMemory mem(bfi.getFileSize()) ;

	if(!bfi.readdata(mem,mem.size()))
		return nullptr;

	// signature is stored as ascii so we need to convert it back to binary

	RsTemporaryMemory mem2(bfi.getFileSize()/2) ;

	if(!RsUtil::HexToBin(std::string((char*)(unsigned char*)mem,mem.size()),mem2,mem2.size()))
	{
		std::cerr << "Input string is not a Hex string!!"<< std::endl;
		return nullptr ;
	}

	bool signature_checks = AuthSSL::getAuthSSL()->VerifyOwnSignBin(strHash.toByteArray(), RsFileHash::SIZE_IN_BYTES,mem2,mem2.size());

	std::cerr << "(II) checked signature of config file " << cfgFname << ": " << (signature_checks?"OK":"Wrong!") << std::endl;

	if(signature_checks){
		FileData* fd = new FileData();
		fd->data = new char[size];
		memcpy(fd->data, decrypteddata, size);
		free(decrypteddata);
		fd->filename = cfgFname;
		fd->size = size;
		return fd;
	} else {
		free(decrypteddata);
		return nullptr;
	}
}
