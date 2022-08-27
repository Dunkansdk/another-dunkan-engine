# quark-engine
Quark Engine

## Unix dependencies
```shell
  sudo apt-get install libudev-dev
  sudo apt-get install libopenal-dev
  sudo apt-get install libvorbis-dev
  sudo apt-get install libsndfile1-dev
  sudo apt-get install libx11-dev
  sudo apt-get install libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev
  sudo apt-get install libfreetype6-dev
```




## Configuration and build

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

## Run
You should be able to launch the executable located in the `bin` folder and see a nice (and clickable) cyan window.

```shell
./bin/Debug/app
```
