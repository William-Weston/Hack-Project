# Hack-Project
Attempt to recreate various programs from the Nand2Tetris Course for educational purposes.

## Suppression File Usage:

`LSAN_OPTIONS=suppressions=suppression.txt ./<program>`

## Building
-  requires vcpkg
- Requires the `mold` linker


## TODO
- [ ] proper build instructions
- [ ] move setting of vcpkg toolchain file out of `CMakePresets.json` and into a User presets file not subject to source control as per best practices
- [ ] polish gui interface
- [ ] fix problem building with CPPCHECK enabled
- [ ] finish `TODO` list :)

