# lets_dance
a dancing game which you can learn to dance.

# Just Starting this respo.currently opencv static lib cmake from source is ok.

# How to use
> git clone https://github.com/F-Quasimo/lets_dance.git</br>
> cd lets_dance</br> 
> cmake -S . -B build</br>
> cmake --build build --target all -j12

or
> mkdir build<br/>
> cd build<br/>
> cmake ..<br/>
> make -j12

P.S: Building opencv may take some time .. you could increase number of threads
> cmake --build build --target all -j12

 make sure the LD_LIBRARY_PATH is updated while running your exectable in the bin/ folder.

# TBD:
1. config ncnn from *.a or src
2. config c++ GUI api,such as https://github.com/cycfi/elements

# Reference
https://github.com/BBO-repo/cpp-opencv-cmake-superbuild