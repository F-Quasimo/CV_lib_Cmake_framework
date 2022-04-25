# lets_dance
a dancing game which you can learn to dance.

# Just Starting this respo.currently opencv static lib cmake from source is ok.

# what it can do
a basic opencv environment with static(or dynamic) link library support. a ncnn framework deep-learning model deployment demo. I will create a dancing teaching game in the end. This base demo is easy to turn to other application.

# How to use
### Step 1:(for opencv)
step 1.1
>sudo apt install build-essential cmake git pkg-config libgtk-3-dev 
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev 
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev 
    gfortran openexr libatlas-base-dev python3-dev python3-numpy 
    libtbb2 libtbb-dev libdc1394-22-dev libopenexr-dev 
    libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev </br>

step 1.2

> git clone https://github.com/F-Quasimo/lets_dance.git</br>
> cd lets_dance</br> 
> mkdir build<br/>
> cd build<br/>
> cmake ..<br/>
> make -j12
### Step 2:(for ncnn)
if cmake from ncnn source :
>sudo apt install build-essential git cmake libprotobuf-dev protobuf-compiler libvulkan-dev vulkan-utils </br>
>https://github.com/Tencent/ncnn/wiki/how-to-build#build-for-linux

**esle:(Prefer)**
>sudo apt install mesa-vulkan-drivers </br>
>Download ncnn release package at https://github.com/Tencent/ncnn/releases with ".a" lib, unzip to thirdparty directory. which means you will get XXX/thirdparty/ncnn/bin path etc.</br>
>I prepare a ncnn release pkg in pkgs, and it will automatically unzip to thirdparty/ncnn. if you need gpu support ,just install vulkan driver is ok.

PS: change the ncnn directory name from "ncnn-XXXXXXXX-ubuntu-1804" to "ncnn"

# RUN:
> cd /lets_dance/bin </br>
> ./ocvdemo


P.S: 
1. Building opencv may take some time ..
2. Anaconda may cause some conflict. please disable anaconda by:
> conda deactivate </br>
> mv \~/anaconda3/lib \~/anaconda3/lib_bck


 make sure the LD_LIBRARY_PATH is updated while running your exectable in the bin/ folder.

# TBD:
1. ~~config ncnn from *.a or src~~(finish)
2. config c++ GUI api,such as https://github.com/cycfi/elements

# Reference
https://github.com/Tencent/ncnn/wiki/how-to-build#build-for-linux
https://github.com/Tencent/ncnn

# One more thing
For AMD GPU:</br>
https://github.com/clbr/radeontop is equal to nvidia-smi