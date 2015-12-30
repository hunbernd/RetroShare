----------------------------------
Disclaimer:
----------------------------------
Always do backup from your data folder!
This version contains modifications, but fully compatible with the official stable version, except one thing.
Distant chat does not work with the most recent unstable versions, but works with this version, the official stable version, and every other older versions.

----------------------------------
Changed things from the official version:
----------------------------------
*based on a version where distant chat and some other things were not fucked yet
+backported from recent version:
	!crash from distant chat request from new clients
	!double selection bug in settings window when vista style is active
	+/me
	+>greentext
	+quote text in chat
	+spoiler text
	+save images from chat and forums
	*contains the emoticons from both the old and recent versions
	*emoticon selector widget size adapts to the number of emoticons
	
-quoting the content of the last post when making a new comment in forums
+loading external emoticons
+loading extensions from RetroShare\extensions6 folder too
-VoIP extension
+PaintChat extension
+ExportImport extension
+FriendMap extension
+SocialCompass extension
+Lua4RS extension


+added, -removed, *changed, !fixed

----------------------------------
Installation instructions:
----------------------------------
Windows (portable):
	1. Remove all .dll from RetrosShare folder and from Data\extensions6 folder, if you keep something, it may cause instability
	2. Extract the RetroShare-Windows-Portable-date-v0.6.0-Qt-5.5.0-Base-anmo.7z file into the RetroShare folder, roughly 200 MB
	3. Extract the RetroShare-Windows-Portable-date-v0.6.0-RC2-xxx-version-Qt-5.5.0-Diff-anmo.7z file into RetroShare folder
	
	To update, repeat step 3 only and write over the files
	
Windows (installed version):
	1. Remove all .dll from ProgramFiles\RetrosShare folder and from %appdata%\RetroShare\extensions6 folder, if you keep something, it may cause instability
	2. Extract the RetroShare-Windows-Portable-date-v0.6.0-Qt-5.5.0-anmo-Base.7z file into the ProgramFiles\RetroShare folder, around 200 MB
	3. Delete the file called "portable"
	4. Extract the RetroShare-Windows-Portable-date-v0.6.0-RC2-xxx-version-Qt-5.5.0-Diff-anmo.7z file into ProgramFiles\RetroShare folder
	
	To update, repeat step 3 only and write over the files
	
Linux
	1. Build it for yourself, the source is here: https://github.com/hunbernd/RetroShare.git
	2. If you need the additional emoticons, then copy the emoticons folder next to the RetrosShare06 executable from here: RetroShare-Windows-Portable-date-v0.6.0-Qt-5.5.0-Base-anmo.7z