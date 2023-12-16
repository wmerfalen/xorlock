# Overview
The backpack holds player loot

# Project requirements
1) Must be able to load from filesystem
2) Must be able to save from filesystem
3) When user presses TAB, the backpack should display on screen
4) Backpack menu should allow user to equip 3 different slots:
    - Primary
        - SMG's, Shotguns, Assault Rifles, Sniper Rifles, DMR
    - Secondary
        - Pistols, Knives, Tasers
    - Utility
        - Frag grenades
5) When user equips to the 3 different slots, the current slot gets put into backpack
6) When user picks up loot, it should automatically go into their backpack
7) When user puts anything into their backpack, the game must immediately save and flush to disk
8) User should be able to drop items from their backpack
9) When items are dropped, they appear just as loot
10) Dropped items that are not picked up before game exit are not saved to the user's backpack


