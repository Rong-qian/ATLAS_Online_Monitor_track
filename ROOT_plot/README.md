# README

## Install 
1. Install ROOT with su
2. Clone the git repository to install [libpcap](https://github.com/the-tcpdump-group/libpcap/blob/master/INSTALL.md) by:
<code>./autogen.sh
./configure --prefix path/to/install
make
make install
3. Change the path in FindPCAP.cmake and bashgcc to the `path/to/install`

