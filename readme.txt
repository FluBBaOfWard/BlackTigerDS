BlackTigerDS V0.1.10
--------------------------------------------------------------------------------
This is a Black Tiger & Black Dragon - Arcade emulator for the NDS.
It has no tile priority, and no screenflip support.
You should still be able to enjoy the game though. =)

--------------------------------------------------------------------------------
How to use:
--------------------------------------------------------------------------------
First DLDI patch the emulator for your flashcard.
Create a folder named "acds" in either the root of your flash card or in the
data folder. Now put blktiger.zip, blkdrgon.zip and/or blkdrgonb.zip into a
folder where you have arcade roms.

When the emulator starts, you can either press L+R or tap on the screen to open
up the menu (the emulator tries to load Black Tiger automagically on startup).
Now you can use the cross or touchscreen to navigate the menus, A or double tap
to select an option, B or the top of the screen to go back a step.

To select between the tabs use R & L or the touchscreen.

--------------------------------------------------------------------------------
Menu:
--------------------------------------------------------------------------------
File:
-----
	Load Game: Select a game to load.
	Load State: Load a previously saved state for the currently running game.
	Save State: Save a state for the current game.
	Save Settings: Save the current settings.
	Reset Game: Reset the currently running game.

Options:
--------
	Controller:
		Autofire: Select if you want autofire.
		Controller: 2P start a 2 player game.
		Swap A/B: Swap which NDS button is mapped to which arcade button.
	Display:
		Display: Here you can select if you want scaled or unscaled screenmode.
			Unscaled mode:  L & R buttons scroll the screen up and down.
		Scaling: Here you can select if you want flicker or barebones lineskip.
		Gamma: Lets you change the gamma ("brightness").
		Disable Foreground: Turn on/off foreground rendering.
		Disable Background: Turn on/off background rendering.
		Disable Sprites: Turn on/off sprite rendering.
	Settings:
		Speed: Switch between speed modes.
			Normal: Game runs at it's normal speed.
			200%: Game runs at double speed.
			Max: Game runs at 4 times normal speed (might change in the future).
			50%: Game runs at half speed.
		Autoload State: Toggle Savestate autoloading.
			Automagically load the savestate associated with the selected game.
		Autosave Settings: This will save settings when
			leaving menu if any changes are made.
		Autopause Game: Toggle if the game should pause when opening the menu.
		Powersave 2nd Screen: If graphics/light should be turned off for the
			GUI screen when menu is not active.
		Emulator on Bottom: Select if top or bottom screen should be used for
			emulator, when menu is active emulator screen is allways on top.
		Debug Output: Show an FPS meter for now.
		Autosleep: Doesn't work.
	Dipswitches:
		Lot of settings for the actual arcade game, difficulty/lives etc.

About:
------
	Some dumb info about the game and emulator...

--------------------------------------------------------------------------------
Credits:
--------------------------------------------------------------------------------
Huge thanks to Loopy for the incredible PocketNES, without it this emu would
probably never have been made.
Thanks to:
Dwedit for help and inspiration with a lot of things.
Reesy for help with the Z80 emu core.
Paul Leaman, for the MAME driver.


--------------------------------------
Fredrik Ahlström
Twitter @TheRealFluBBa
http://www.github.com/FluBBaOfWard
--------------------------------------

