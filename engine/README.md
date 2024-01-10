# xorlock
A top-down shooter made mostly for my own personal enjoyment.

# Building source
```sh
cd engine
make all
```

# Running
```sh
cd engine
./build/xorlock
```

# Profiling
I use cachegrind:
```
valgrind --tool=cachegrind ./build/xorlock

# then, once that's done:

cg_annotate ./cachegrind.1234
```

cachegrind output is stored in [the profiling folder](profiling/)

# Assets
Xorlock has everything except for the gaming assets. For example gaming assets, checkout the releases page on this repo.

# Features

## Backpack
All loot that you pick up is stored to the file system. This is then presented to you as your backpack. To open your backpack, press Tab.

## Randomized loot drops
All stats on dropped loot are randomized.

## Difficulty menu
Press `esc`. You should be greeted with a menu with different difficulties listed.
This menu is very janky but it gets the job done.

## Abilities
It is now possible to use three different (and always evolving) abilities:
- F35 Air Support:
*TODO: add gif*

- Placing Automated Turrets:
*TODO: add gif*

- Using an Aerial Drone to help you identify NPC locations:
*TODO: add gif*

# Button mapping
`1` (the key to the right of the tilde)
    - Equips your secondary (usually a pistol)
`2`
    - Equips your primary
`3`
    - Equips a frag grenade
`TAB`
    - Brings you to the backpack menu
    - When in the backpack menu, use `f`,`p`,`s` buttons to go to the Frag, Primary, and Secondary menus, respectively.
`ESC`
    - Should exit out of backpack menu
    - Will give you the difficulty menu ("baby", "easy", ... etc.)
# TODO
- [ ] Generate a map
- [ ] Find ways to define edges in the map
- [ ] Create a zombie NPC
  - [ ] It should lumber slowly toward the player
  - [ ] It should do melee damage

