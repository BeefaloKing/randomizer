--------------------------------------------------
 Lynn's Diablo II Randomizer
--------------------------------------------------

--------------------------------------------------
 config.txt
--------------------------------------------------
	d2path
String (Default=C:/Program Files (x86)/Diablo II)
A path to your Diablo II installation directory.

	d2se
true/false (Default=false)
Set to true if d2path points to a D2SE installation directory.

	d2se_core
String (Default=1.13c)
Ignored if d2se is false.
Defines the core folder to use under <d2path>/D2SE/CORES.

	d2se_mod
String (Default=Randomizer)
Ignored if d2se is false.
Defines the mod folder to use under <d2path>/MODS. Will create if it doesn't exist.

	seed
0-4294967295 (Default=0)
Used in all random calculations and can be shared to generate the same files.
When set to 0 will generate a different random seed every time.

	rand_npc
true/false (Default=false)
Enable to randomize all (possible) town NPC spawn locations.
Quest NPC's always remain in the same act.

	rand_mapnames
true/false (Default=false)
Enable to randomize names displayed for each area.

	rand_wpmenu
true/false (Default=false)
Enable to randomize the order waypoints are listed in the menu.
Does NOT randomize which areas have waypoints.
Town waypoints will always be at the top.

rand_music
true/false (Default=false)
Enable to randomize music played in each area.

	rand_maplinks
true/false (Default=false)
Enable to randomize which area entrances/exits lead to each other.

	force_longmeph
true/false (Default=false)
If random_stairs is enabled, the walk from travincal to Durance of Hate Level 3 can be quite
lack luster due to limited options and restrictions placed on the random map links.
Enable to force at least one additional area placed after Travincal.

--------------------------------------------------
 External
--------------------------------------------------
	Ladislav Zezula
Responsible for StormLib and made MPQs much easier to manage
http://www.zezula.net/en/mpq/stormlib.html
https://github.com/ladislav-zezula/StormLib
