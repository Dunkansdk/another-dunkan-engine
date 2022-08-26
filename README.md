# quark-engine
Quark Engine

## Configuration and build

If you fork the full source code from [here][1] (to get `App.h` and `App.cpp`), you should be able to type the following lines in a terminal at the root of the project folder.

On windows
```shell
  # on windows
  cmake  -G "Visual Studio 15 $(Version)" -S . -B ./build -DCMAKE_BUILD_TYPE=Debug ..
  cmake  --build ./build --config Debug --target app
```
On Linux
```shell
  # on linux
  mkdir build
  cd build
  cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
  cmake --build ./ --target app --config Debug
```
> **Note**:  For linux user, you might need to check [this][2] first.

## Run
You should be able to launch the executable located in the `bin` folder and see a nice (and clickable) cyan window.

```shell
./bin/Debug/app
```
