# xorlock
A top-down shooter made mostly for my own personal enjoyment.

# Profiling
I use cachegrind:
```
valgrind --tool=cachegrind ./build/xorlock

# then, once that's done:

cg_annotate ./cachegrind.1234
```

cachegrind output is stored in [the profiling folder](profiling/)

# TODO
- [ ] Generate a map
- [ ] Find ways to define edges in the map
- [ ] Create a zombie NPC
  - [ ] It should lumber slowly toward the player
  - [ ] It should do melee damage

