
## Prerequisites
**opencv**:

Can be installed on linux with 

```
$ sudo apt install libopencv-dev
```

## Compiling
Makefile in progress. For the moment:

```
$ g++ lifevision.cpp -o lifevision.out `pkg-config --cflags --libs opencv4`
```
