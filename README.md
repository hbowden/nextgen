# nextgen
A Genetic File, Sycall and Network Fuzzer for Unix systems.

<a href="https://scan.coverity.com/projects/2trill2spill-nextgen">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/9072/badge.svg"/>
</a>
[![Build Status](https://travis-ci.org/2trill2spill/nextgen.svg?branch=master)](https://travis-ci.org/2trill2spill/nextgen)

Real README.md coming soon, I just need to fix a few bugs and add a few features!
I will start adding the missing features and known bugs to the issue tracker.

In the mean time `FreeBSD`, `Linux` and `OSX` users can build and "use" nextgen.

# Build-FreeBSD
To build `nextgen` on `FreeBSD` you will need to install a few dependencies and have the `FreeBSD` source tree installed at `/usr/src` as well. Use the command below to install the needed dependencies.

    sudo pkg install gmake autotools cmake
    
Use the command below if you don't have the `FreeBSD` source tree installed. The command below assumes your running `FreeBSD` 10.3, if you are not change the command accordingly.

    sudo svnlite co https://svn0.us-west.freebsd.org/base/release/10.3.0/ /usr/src

Next run the setup script, this will build any dependencies needed for nextgen on `FreeBSD`.

    ./setup.sh

Create a directory called build and change to it.

    mkdir build && cd build
    
Run `cmake` and pass the path to the main source directory.

    cmake ..
    
Finally use `make` or `gmake` to build `nextgen`.

    make

# Build-OSX

To build nextgen on `OSX` or `macOS` you will need to install command line tools if they are not already installed on your system. Run the command below in terminal to install the command line tools. Note this command only works for `OSX` 10.9 and later.

    xcode-select --install
    
Now you will need to install a few dependencies before building nextgen. The recommended way to get these dependencies is to use homebrew. To install homebrew use.

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    
Next install cmake and automake using homebrew.

    brew install cmake automake
    
Run the setup script to build the nextgen dependencies.

    ./setup.sh

Create a directory called build and change to it.

    mkdir build && cd build
    
Run `cmake` and pass the path to the main source directory.

    cmake ..
    
Finally use `make` or `gmake` to build `nextgen`.

    make

# Build-Linux

To build nextgen on `Linux` you will first need to install `aclocal-1.15`. If your distributions packgage manager dosen't
have `aclocal-1.15`, the easiest way to get it is to run the get-aclocal script that comes packaged with nextgen. This script
needs to be run with root privileges and installs the `automake-1.15` package to `/usr/local/bin/` and includes `aclocal-1.15`.

    sudo ./get-aclocal.sh
    
Next run the setup script, this will build any dependencies needed for nextgen on `Linux`.

     ./setup.sh
     
Create a directory called build and change to it.

    mkdir build && cd build
    
Run `cmake` and pass the path to the main source directory.

    cmake ..
    
Finally use `make` to build `nextgen`.

    make
    
On some distributions of `Linux` such as ubuntu you will have to raise the ulimit on your system because it is set low.
    
    ulimit -n 15743

# Install

Finally after building nextgen for your platform you can install nextgen with `sudo make install`. This will install the required dynamic libraries built earlier.

To run nextgen use the binary in the nextgen directory, if you run nextgen outside it's directory, nextgen won't find it's plugin directory. This limitation will be fixed later.

# Documentation

To build the documentation for nextgen you will need `doxygen` installed on your system, then just run...

    doxygen Doxyfile
    
Then you can open the file index.html in the newly generated folder html in your browser.

# Example
Below is an example of running nextgen as a syscall fuzzer.

`sudo ./nextgen --syscall --out /file/path/to/create/output/directory`

To use the syscall fuzzer without probes and the genetic algo pass --dumb.

`sudo ./nextgen --syscall --dumb --out /file/path/to/create/output/directory`

Nextgen will be ported to other operating systems besides `FreeBSD`, `Linux` and `OSX` and will be implemented in this order `illumos`, `OpenBSD`, and finally `NetBSD`. Porting `Nextgen` is mainly a matter of having a working libdtrace available on your platform. 
