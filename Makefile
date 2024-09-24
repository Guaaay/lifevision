tool: tool.o support.o
    g++ -g -o tool tool.o support.o -L/sw/lib/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint \
        -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lz -L/sw/lib -lfreetype -lz -Wl,-framework,CoreServices \
        -Wl,-framework,ApplicationServices -pthread -Wl,-rpath,/sw/lib/root -lm -ldl

tool.o: tool.cc support.hh
    g++ -g  -c -pthread -I/sw/include/root tool.cc

support.o: support.hh support.cc
    g++ -g -c -pthread -I/sw/include/root support.cc