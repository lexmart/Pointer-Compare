@echo off
pushd build
clang++ -g -gcodeview ../analyze.cpp -o analyze.exe -L../lib -llibclang
popd