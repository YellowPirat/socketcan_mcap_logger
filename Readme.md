# Installation
- Install Conan2
    - pip install conan --break-system-packages
- Detect Conan profile
    - conan profile detect
- Run conan install to setup the environment for the projekt 
    - conan install . --output-folder=build --build=missing
- Goto build folder and run cmake
    - cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
- Compile project
    - cmake --build . -v

# Usage
MCL uses gflags. Therefor it is required to parse some aruguments into the programm
- --config_file : path to the configfile of the logger
- --proto_file : path to the location where the protofiles should be stored. If you pass this argument, the logger do not start. It produces the proto-files based on the .dbc files in the config-file.

# Example
- Starting the logger
```
./MCL --config_file="~/Git/mcap_can_logger/config/config.yaml"
```


