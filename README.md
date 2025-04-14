This is a fairly simple platformer game I made with the main gimmick being the player’s ability to redirect their current velocity in different directions. The game was made using [SFML](https://www.sfml-dev.org/) . It doesn’t have any sprites or audio since I was more interested in figuring out how to make game physics and playing around with the mechanics than actually making a full game.

The game was made using the open sans font, which is not included as part of the repository. The game is programmed to look for “fonts/OpenSans-Regular.ttf”, downloading the open sans font and adding it to a “fonts” folder in the same directory should be sufficient.

The Makefile is currently set up to cross-compile the game from Linux to Windows, so it may require some modifications to build things successfully. It’s set up this way because I prefer to develop on Linux, but primarily do so using WSL on my Windows laptop. Linux GUI programs don’t run great on WSL so I chose to cross-compile and run the programs as Windows executables.

If you do choose to build and run the game, the currently existing starting level with the most content would be “test1”. A level builder is also included, however the documentation for it is currently out of date. The old documentation can be found in lvlBuilding.txt, the main differences are as follows:
1. Modes will repeat after completing all steps, escape can be used to return to none at any time.
2. There is now only one delete button ‘d’, which will delete the object closest to the mouse cursor, shown in red, when the left mouse button is pressed.
3. Camera objects and triggers can be placed by pressing ‘c’, although they currently cannot be deleted from within the level builder.
4. The “start \<level name\>” command adds the specified level as a starting level which can be selected when playing the game.
