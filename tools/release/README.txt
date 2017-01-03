3D Repo GUI v 1.8.0

This is the desktop client for the 3D Repo Platform

====================== Directory Listing ========================
 > bin
	3D Repo GUI and the necessary libraries to run it 
> license
      licensing information

=========================== To Run ==============================
Run 3drepogui.exe inside the bin directory. Please note you will 
need to have a running Mongo Database to connect to in order to utilise
most of the features within the Desktop Client.

Please note that 3D Repo currently only supports Mongo DB 2.6 and 3.0.


=========================== Contact==============================
If you need any help, please contact support@3drepo.org, we
look forward to hear from you.

=========================== Improvements ========================
* (ISSUE #153) Fixed crashes with KD Tree display
* (ISSUE #163) Fixed bug where selection text is still shown after unselecting
* (ISSUE #179) When an object is selected, show its name if available
* (ISSUE #178) QT webkit is no longer a dependency
* (ISSUE #182) Improved the speed of picking
* (ISSUE #183) Changed presentation of selected geometry
* (ISSUE #196) Remove the ability to add map node as it is no longer used 
* (ISSUE #198) Changes to support 3drepobouncer refactoring

=========================== New Features ========================
* (ISSUE #164) Multi-Selection is now supported
* (ISSUE #170) IFC related ASSIMP configurations are now presented
* (ISSUE #185) A new navigation mode (Helicopter) is now added
* (ISSUE #186) Project permissions are now more fine grained
* (ISSUE #191) IFC Open Shell configurations are now included
* (ISSUE #193, #194) Some testing/administration updates
* (ISSUE #195) GUI no longer tries to import a file with invalid extension
