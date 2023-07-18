# Compilation instructions
	## Import the project
		Make sure you have the latest Android Studio installed. 
		Open the folder with Android Studio and the software will automatically recognize it as a project folder.

	## Create a signing key
		Due to the security measures of the Android system, not signing the APK file will cause it to be unable to be installed on the real Android machine. 
		Click the "Builder" option at the top of the toolbar and select "Genarate Singed Bundle/APK" from the drop-down menu.

		The build window will ask the developer to enter the relevant key file and password. 
		If you can not create a new key file, click "Create new..." to open the dialog box of creating a new key file, and enter the corresponding information in the dialog box.

	## Generate the APK installation package
		Continue to sign the APK with the newly created key file in the build window to get the signed APK file. 
		Send the obtained APK file to the Android real machine to install and use.
		
	## The location of the APK file
		realise：ManufactureAPP\app\release\ManufactureHome.apk
		De-bug: ManufactureAPP\app\build\outputs\apk\debug\app-debug.apk

	## Config
		1. You need to place your root certificate under Manufacture\APP\app\src\main\assets and name it CA.crt
		2. You need to change your server IP in line 88 of the file  "ManufactureAPP\app\src\main\res\layout\activity_login.xml" 