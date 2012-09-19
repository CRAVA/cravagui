These are the release notes for cravagui

Cravagui is a graphical user interface for the CRAVA seismic inversion application. This is only the graphical user interface, it is intended to be used in counjunction with the seismic inversion program.

ON WHAT PLATFORMS DOES IT RUN?
Cravagui runs on any platform supported by Qt. Cravagui support both qmake and cmake build systems. The qmake build option is convenient for cross-platform compilations as the cmake build currently only support GNU/Linux. To build with qmake you can use and IDE such as qdevelop.

DOWNLOAD
git clone git://github.com/CRAVA/cravagui.git

BUILD INSTRUCTIONS
To build cravagui using cmake, run
 cd cravagui/trunk
 mkdir build
 cd build
 cmake ..
 make
You will find the cravagui binary in the build folder.
