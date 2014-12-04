Space shooter
===========

An asteroids game made using the Allegro library.


Depends on
----------
[Allegro 5](http://alleg.sourceforge.net/)


Setup on OS X
----------

If you're on Yosemite use install allegro HEAD, as the 5.0 branch has a bug that will make the app crash.

```shell

brew install allegro5 # or use brew install allegro5 --HEAD on Yosemite
cd /path/to/space_shooter
cmake .
make
./space_shooter

```

Setup on Linux (Fedora)
----------

```shell

sudo yum install allegro5-devel
cd /path/to/space_shooter
cmake .
make
./space_shooter

```
