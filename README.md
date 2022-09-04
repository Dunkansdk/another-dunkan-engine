# another-dunkan-engine
Another dunkan engine


## Unix dependencies
```shell
sudo apt-get install libgl1-mesa-dev libudev-dev libopenal-dev libvorbis-dev libsndfile1-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libfreetype6-dev
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
