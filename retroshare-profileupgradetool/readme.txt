This tool lets you to create a new account while keeping all your old data.
Useful if you cannot access to your old accound, because retroshare complains about the old SSL keys with new OenSSL version.

-----------------------------------------------------------------

How to use:

MAKE SURE YOU HAVE A BACKUP.
Extract the files somewhere.
Open the terminal and run the program.
	If installed RetroShare to the default location, then just run:
		retroshare-profileupgradetool.exe
	If you have portable RetroShare, then you have to specifiy the data folder with -c option:
		retroshare-profileupgradetool.exe -c where\retroshare\is\Data
Select which account to upgrade.
Login with your password.
Name your new location.

Your downloads and partials folder won't be migrated by this tool.
If they are under your profile folder, you may copy/move the files manually, and change the folders under Preferences --> Files --> Directories.

You should have outgoing connections immediately.
For incoming connections you have to send your new certificate to your friends, or wait until they get the certificate through other mechanisms built into retroshare.

-----------------------------------------------------------------

How does it work:

- loads retroshare core, but just the crypto modules, and extracts the SSL password (it is randomly generated, different from you PGP password)
- decrypts all config files into the memory
- creates a new account with new SSL keys, but with the old PGP keys and SSL password
- copies over not encrypted files
- copies over GXS databases (they use the SSL password, which is the same for both accounts)
- loads the new account, and SSL keys
- encrypts the config files in memory and saves them to the disk

-----------------------------------------------------------------

Source code available from here:
https://github.com/hunbernd/RetroShare/tree/accountupgradetool
	branch: accountupgradetool
https://github.com/hunbernd/libretroshare/tree/sslseclevel
	branch: sslseclevel
