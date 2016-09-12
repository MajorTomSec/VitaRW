# VitaRW
Enable RW access to "read-only" partitions.
This is a PoC, and should be used by devs who like the risk.

Oh, sa0 isn't unmounted correctly there, fix it if you care.

No build provided, just in case any Indiana Jones kid would like to play with os0.

Thanks to SMOKE for taking the risk of modding some files in vs0 (That doesn't seem to cause any problem, btw.)

## Partitions mounted in RW

0x200= os0
0x300= vs0
0x400= vd0
0x500= tm0
0x700= ud0
0x800= ux0??
0x900= gro0
0xC00= pd0

## Hall of Fame

Try not to end up here.

```
<Moolie> well I found what a brick looks like xdxd
<Lupo511> did it brick?
<Moolie> yep
<Lupo511> what happened?
<Moolie> I fucked up os0
<Lupo511> yeah, I mean what did actually happen
<Moolie> well I deleted some stuff in a folder
<Moolie> then now its stuck on ps sign
[...]
<Moolie> xyz: I deleted everything in os0:sm/
```
