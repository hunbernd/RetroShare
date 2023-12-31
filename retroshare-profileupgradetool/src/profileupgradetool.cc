/*
 * RetroShare Service
 * Copyright (C) 2016-2022  Gioacchino Mazzurco <gio@eigenlab.org>
 * Copyright (C) 2021-2022  Asociación Civil Altermundi <info@altermundi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-FileCopyrightText: Retroshare Team <contact@retroshare.cc>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */


#include <cmath>
#include <csignal>
#include <iomanip>
#include <atomic>

#include "retroshare/rsinit.h"
#include "retroshare/rstor.h"
#include "retroshare/rspeers.h"
#include "retroshare/rsinit.h"
#include "retroshare/rsiface.h"
#include "rsserver/rsloginhandler.h"

#include "util/stacktrace.h"
#include "util/rsprint.h"
#include "util/argstream.h"
#include "util/rskbdinput.h"
#include "util/rsdir.h"
#include "util/rsdebug.h"
#include "util/folderiterator.h"

static CrashStackTrace gCrashStackTrace;

// We should move these functions to rsprint in libretroshare

#define COLOR_GREEN  0
#define COLOR_YELLOW 1
#define COLOR_BLUE   2
#define COLOR_PURPLE 3
#define COLOR_RED    4

std::string colored(int color,const std::string& s)
{
    switch(color)
    {
    case COLOR_GREEN : return "\033[0;32m"+s+"\033[0m";
    case COLOR_YELLOW: return "\033[0;33m"+s+"\033[0m";
    case COLOR_BLUE  : return "\033[0;36m"+s+"\033[0m";
    case COLOR_PURPLE: return "\033[0;35m"+s+"\033[0m";
    case COLOR_RED   : return "\033[0;31m"+s+"\033[0m";
    default:
        return s;
    }
}

void copyFileBetweenProfiles(std::string oldid, std::string newid, std::string newfile){
	//std::cout << "--- " << oldid << "\t" << newid << "\t" << newfile << std::endl;
	std::string oldfile = newfile;
	oldfile.replace(oldfile.find(newid), newid.length(), oldid);
	std::cout << "Copying " << oldfile << " --> " << newfile << std::endl;
	RsDirUtil::copyFile(oldfile, newfile);
}

void copyFilesBetweenProfiles(std::string oldid, std::string newid, std::string newdir, std::string ending){
	//std::cout << "--- " << oldid << "\t" << newid << "\t" << newdir << std::endl;
	RsDirUtil::checkCreateDirectory(newdir);
	std::string olddir = newdir;
	olddir.replace(olddir.find(newid), newid.length(), oldid);
	std::cout << "Copying files " << olddir << " --> " << newdir << std::endl;
	librs::util::FolderIterator dirIt(olddir,false);
	if(!dirIt.isValid())
	{
		return;
	}

	for(;dirIt.isValid();dirIt.next())
	{
		if(dirIt.file_type() == librs::util::FolderIterator::TYPE_FILE)
			if(dirIt.file_name().size() >= ending.size() && dirIt.file_name().substr( dirIt.file_name().size() - ending.size()) == ending)
			{
				std::string newfile = dirIt.file_fullpath();
				newfile.replace(newfile.find(oldid), oldid.length(), newid);
				std::cout << "\t" << dirIt.file_name() << std::endl;
				RsDirUtil::copyFile(dirIt.file_fullpath(), newfile);
			}
	}
	dirIt.closedir();
}

std::string PGPpassword;	//Cache the PGP password, so it won't be asked 3 times

class RsServiceNotify: public NotifyClient
{
public:
	RsServiceNotify() = default;
	virtual ~RsServiceNotify() = default;

	virtual bool askForPassword(
	        const std::string& title, const std::string& question,
	        bool /*prev_is_bad*/, std::string& password, bool& cancel )
	{
        std::string question1 = title + colored(COLOR_GREEN,"Please enter your PGP password for key:\n    ")  + question + " :";
		password = RsUtil::rs_getpass(question1.c_str()) ;
		PGPpassword = password;
		cancel = false ;

		return !password.empty();
	}
};

static std::atomic<bool> keepRunning(true);
static int receivedSignal = 0;

void signalHandler(int signal)
{
	if(RsControl::instance()->isReady())
		RsControl::instance()->rsGlobalShutDown();
	receivedSignal = signal;
	keepRunning = false;
}


RsInit::LoadCertificateStatus attemptLogin(const RsPeerId& account, const std::string& password)
{
	if(rsLoginHelper->isLoggedIn()) return RsInit::ERR_ALREADY_RUNNING;

	{
		if(!RsAccounts::SelectAccount(account))
			return RsInit::ERR_UNKNOWN;

		if(!password.empty())
		{
			rsNotify->cachePgpPassphrase(password);
			rsNotify->setDisableAskPassword(true);
		}
		std::string _ignore_lockFilePath;
		RsInit::LoadCertificateStatus ret = RsInit::LockAndLoadCertificates(false, _ignore_lockFilePath);

		bool is_hidden_node = false;
		bool is_auto_tor = false ;
		bool is_first_time = false ;

		RsAccounts::getCurrentAccountOptions(is_hidden_node,is_auto_tor,is_first_time);

		return ret;
	}
}

int main(int argc, char* argv[])
{
	signal(SIGINT,   signalHandler);
	signal(SIGTERM,  signalHandler);
#ifdef SIGBREAK
	signal(SIGBREAK, signalHandler);
#endif // ifdef SIGBREAK

#ifdef WINDOWS_SYS
	// Enable ANSI color support in Windows console
	{
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#endif

		HANDLE hStdin = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdin) {
			DWORD consoleMode;
			if (GetConsoleMode(hStdin, &consoleMode)) {
				if ((consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0) {
					if (SetConsoleMode(hStdin, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
						std::cout << "Enabled ANSI color support in console" << std::endl;
					} else {
						RsErr() << "Error getting console mode" << std::endl;
					}
				}
			} else {
				RsErr() << "Error getting console mode" << std::endl;
			}
		} else {
			RsErr() << "Error getting stdin handle" << std::endl;
		}
	}
#endif

	RsInfo() << "\n" <<
	    "+================================================================+\n"
	    "|     o---o                                             o        |\n"
	    "|      \\ /           - Retroshare Service -            / \\       |\n"
	    "|       o                                             o---o      |\n"
	    "+================================================================+"
	         << std::endl << std::endl;

	RsInit::InitRsConfig();
	RsControl::earlyInitNotificationSystem();

#ifdef __APPLE__
	// TODO: is this still needed with argstream?
	/* HACK to avoid stupid OSX Finder behaviour
	 * remove the commandline arguments - if we detect we are launched from
	 * Finder, and we have the unparsable "-psn_0_12332" option.
	 * this is okay, as you cannot pass commandline arguments via Finder anyway
	 */
	if ((argc >= 2) && (0 == strncmp(argv[1], "-psn", 4))) argc = 1;
#endif

	std::string prefUserString = "list";
	RsConfigOptions conf;
	conf.sslLevel = 0;

	argstream as(argc,argv);
	as >> option( 's', "stderr", conf.outStderr,
	              "output to stderr instead of log file." )
       >> parameter( 'c',"base-dir", conf.optBaseDir, "directory", "Set base directory.", false )
       >> parameter( 'l', "log-file", conf.logfname, "logfile", "Set Log filename.", false )
	   >> parameter( 'd', "debug-level", conf.debugLevel, "level", "Set debug level.", false );
	as >> parameter( 'U', "user-id", prefUserString, "ID",
	                 "[node Id] Selected account to use and asks for passphrase"
	                 ". Use \"-U list\" in order to list available accounts.",
	                 false );

	as >> help( 'h', "help", "Display this Help" );
	as.defaultErrorHandling(true, true);

	conf.main_executable_path = argv[0];

	int initResult = RsInit::InitRetroShare(conf);

	if(initResult != RS_INIT_OK)
	{
		RsFatal() << "Retroshare core initalization failed with: " << initResult
		          << std::endl;
		return -initResult;
	}

	RsLoginHelper::Location selectedaccount;
		if(prefUserString == "list")
		{
			std::vector<RsLoginHelper::Location> locations;
			rsLoginHelper->getLocations(locations);

            if(locations.size() == 0)
            {
                RsErr() << colored(COLOR_RED,"No available accounts. You cannot use option -U list") << std::endl;
                return -RsInit::ERR_NO_AVAILABLE_ACCOUNT;
            }

            std::cout << std::endl << std::endl
                      << colored(COLOR_GREEN,"Available accounts:") << std::endl<<std::endl;

            int accountCountDigits = static_cast<int>( ceil(log(locations.size())/log(10.0)) );

			for( uint32_t i=0; i<locations.size(); ++i )
                std::cout << colored(COLOR_GREEN,"  [" + RsUtil::NumberToString(i+1,false,'0',accountCountDigits)+"]") << " "
                          << colored(COLOR_YELLOW,locations[i].mLocationId.toStdString())<< " "
                          << colored(COLOR_BLUE,"(" + locations[i].mPgpId.toStdString()+ "): ")
                          << colored(COLOR_PURPLE,locations[i].mPgpName + " (" + locations[i].mLocationName + ")" )
				          << std::endl;

            std::cout << std::endl;
            uint32_t nacc = 0;
			while(keepRunning && (nacc < 1 || nacc >= locations.size()))
			{
                std::cout << colored(COLOR_GREEN,"Please enter account number: ");
				std::cout.flush();

				std::string inputStr;
				std::getline(std::cin, inputStr);

				nacc = static_cast<uint32_t>(atoi(inputStr.c_str())-1);
				if(nacc < locations.size())
				{
					prefUserString = locations[nacc].mLocationId.toStdString();
					selectedaccount = locations[nacc];
					break;
				}
				nacc=0; // allow to continue if something goes wrong.
			}
		}


		RsPeerId ssl_id(prefUserString);
		if(ssl_id.isNull())
		{
            RsErr() << colored(COLOR_RED,"Invalid User location id: a hexadecimal ID is expected.")
			        << std::endl;
			return -EINVAL;
		}

		RsServiceNotify* notify = new RsServiceNotify();
		rsNotify->registerNotifyClient(notify);

		// supply empty passwd so that it is properly asked 3 times on console
		RsInit::LoadCertificateStatus result = attemptLogin(ssl_id, "");

		switch(result)
		{
		case RsInit::OK: break;
		case RsInit::ERR_ALREADY_RUNNING:
			RsErr() << "Another RetroShare using the same profile is already "
			           "running on your system. Please close that instance "
			           "first." << std::endl << "Lock file: "
			        << RsInit::lockFilePath() << std::endl;
			return -RsInit::ERR_ALREADY_RUNNING;
		case RsInit::ERR_CANT_ACQUIRE_LOCK:
			RsErr() << "An unexpected error occurred when Retroshare tried to "
			           "acquire the single instance lock file." << std::endl
			        << "Lock file: " << RsInit::lockFilePath() << std::endl;
			return -RsInit::ERR_CANT_ACQUIRE_LOCK;
		case RsInit::ERR_UNKNOWN: // Fall-throug
		default:
			RsErr() << "Cannot login. Check your passphrase." << std::endl
			        << std::endl;
			return -result;
		}        

// Successful login, start the profile upgrade
		std::cout << "Successful login, start the profile upgrade" << std::endl;
		RsPgpId PGPId = selectedaccount.mPgpId;

		bool is_hidden_node = false;
		bool is_auto_tor = false ;
		bool is_first_time = false ;
		RsAccounts::getCurrentAccountOptions(is_hidden_node,is_auto_tor,is_first_time);

		std::string genLoc = selectedaccount.mLocationName + " upgraded"; // TODO make configurable

		rsNotify->cachePgpPassphrase(PGPpassword);
		rsNotify->setDisableAskPassword(true);
		std::string sslPasswd;
		RsLoginHandler::getSSLPassword(selectedaccount.mLocationId,true,sslPasswd);


		RsPeerId sslId;
		std::cerr << "GenCertDialog::genPerson() Generating SSL cert with gpg id : " << PGPId << std::endl;
		std::string err;


		std::cout << "RsAccounts::GenerateSSLCertificate" << std::endl;
		rsNotify->cachePgpPassphrase(PGPpassword);
		rsNotify->setDisableAskPassword(true);
		bool okGen = RsAccounts::createNewAccount(PGPId, "", genLoc, "", is_hidden_node, is_auto_tor, sslPasswd, sslId, err);

		if (okGen)
		{
			std::cout << std::endl << "New location SSL ID: " << sslId << std::endl;
			std::string newid = sslId.toStdString();
			std::string oldid = selectedaccount.mLocationId.toStdString();

			//Copy stuff from the old to the new location
			std::string path;
			//SSL password
			path = RsAccounts::AccountKeysDirectory() + "/" + "ssl_passphrase.pgp";
			copyFileBetweenProfiles(oldid, newid, path);
			//GXS databases
			path = RsAccounts::AccountDirectory() + "/gxs";
			copyFilesBetweenProfiles(oldid, newid, path, "_db");
			//GUI settings
			copyFileBetweenProfiles(oldid, newid, RsAccounts::AccountDirectory() + "/RetroShare.conf");
			copyFileBetweenProfiles(oldid, newid, RsAccounts::AccountDirectory() + "/RSPeers.conf");
			//

//			/* complete the process */
//			RsInit::LoadPassword(sslPasswd);
//			if (Rshare::loadCertificate(sslId, false)) {
//			}
		}
		else
		{
			std::cerr << "Failed to generate new profile" << std::endl;
		}

		std::cout << "Profile upgrade end" << std::endl; //TODO
// End of profile upgarde

	rsControl->setShutdownCallback([&](int){keepRunning = false;});
	rsControl->rsGlobalShutDown();
	while(keepRunning)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	return 0;
}
