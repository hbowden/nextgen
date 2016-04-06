# nextgen
A Genetic File, Sycall and Network Fuzzer for Unix systems.

Real README.md coming soon, I just need to fix a few bugs and add a few features!
I will start adding the missing features and known bugs to the issue tracker.

In the mean time `FreeBSD` and `OSX` users can build and "use" nextgen.

# Install-freebsd
To install `nextgen` on `FreeBSD` you will need to install a few dependencies and have the `FreeBSD` source tree installed at `/src` as well. Use the command below to install the needed dependencies.

    sudo pkg install gmake autotools
    
Use the command below if you don't have the `FreeBSD` source tree installed. The command below assumes your running `FreeBSD` 10.3, if you are not change the command accordingly.

    sudo svnlite co https://svn0.us-west.freebsd.org/base/release/10.3.0/ /usr/src

Due to a regression in `concurrencykit` a `nextgen` dependency you will have to go into `deps` and build `concurrencykit` manually. Run `./configure && gmake` in the `ck-0.5.1` directory to build `concurrencykit`.

Now that your done building `concurrencykit`, run `gmake` to build the rest of `nextgen`.

Use `gmake quick` if you don't wan't to build the dependencies and only want to build nextgen, note you will have to build the dependencies at least once.

`gmake build-test` will build the test suite.

Use `gmake test` to test nextgen and it's dependencies otherwise use `gmake test-quick` to only test nextgen.

Install nextgen with `sudo make install`. This will install the required dynamic libraries built earlier.

To run nextgen use the binary in the nextgen directory, if you run nextgen outside it's directory, nextgen won't find it's plugin directory. This limitation will be fixed later.

#Example
Below is an example of running nextgen as a syscall fuzzer.

`sudo ./nextgen --syscall --out /file/path/to/create/output/directory`

To use the syscall fuzzer without probes and the genetic algo pass --dumb.

`sudo ./nextgen --syscall --dumb --out /file/path/to/create/output/directory`

Nextgen will be ported to other operating systems besides `FreeBSD` and `OSX` and will be implemented in this order `illumos`, `Linux`, `OpenBSD`, and finally `NetBSD`. Porting Nextgen is mainly a matter of on having a working libdtrace available on your platform. 

