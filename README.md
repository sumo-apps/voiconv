# voiconv

Conversion tool used in [Sumopaint](https://paint.sumo.app/).
It's fast and gives good compression results.

Credits to original qoi repository: [phoboslab/qoi](https://github.com/phoboslab/qoi)


Compile (requires CMake)
```sh
make
```

Example: Convert png to voi
```sh
./voiconv foobar.png foobar.voi
```

Example: Convert voi to png
```sh
./voiconv foobar.voi foobar.png
```
