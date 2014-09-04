# Harmonic Motion

_Harmonic Motion_ is a free open source toolkit for artists, musicians and designers working with gestural data. It provides a robust interface for rapid prototyping of patches to process gestural data and a framework through which approaches may be encapsulated, reused and shared with others. It can also be used as a C++ library without any user interface.

This project has been developed at the Centre for Digital Music, Queen Mary University of London.

The project is released under an MIT licence meaning you can do what you like except hold us responsible if things don't work (see the LICENCE file for the full text). However, we would really appreciate hearing from anyone using the software what you're getting up to (contact details below), and for any academics using it within research to cite the paper mentioned below. This helps us to evaluate whether developing software like this was money well spent.

More information about _Harmonic Motion_ may be found at http://harmonicmotion.timmb.com and also within publication about the project at the 2014 New Interfaces for Musical Express (NIME) conference:

http://nime2014.org/proceedings/papers/273_paper.pdf

## Current limitations

- At the moment, this only builds on Mac OSX. The Windows version will be coming shortly however.

- In theory, Harmonic Motion should run on any version of OSX from 10.7 upwards. However, issues have been reported on 10.7, and nearly all testing has been done on 10.9.

- The only option for getting skeleton tracking on OSX is to use NITE from PrimeSense which, unfortunately, has been removed from public availability after Apple bought out PrimeSense. Harmonic Motion supports NITE if it's installed but we're not allowed to distribute it. There are copies around on the internet but obviously it's down to you to ensure you do not use software without the appropriate licence. If you don't have NITE (and SensorKinect, etc) installed then the OpenNI node will not be able to find a Kinect.

- At the moment, documentation is limited to tooltips. Providing better in-app help is a top priority at the moment.


## Build instructions

Harmonic Motion is currently split into two modules: __Core__ and __Gui__. __Core__ is a C++ static library without any user interface that may be accessed directly from another C++ application. __Gui__ provides a visual patch-based user interface for working with __Core__. Patches (or 'pipelines' as we often refer to them) created in __Gui__ can be exported as JSON files and then imported directly in __Core__.

Harmonic Motion is built using CMake 3.0.0, and is dependent upon Cinder 0.8.5 (with Boost 1.48) for the Core library and additionally Qt 5.3 for the user interface. Currently it only builds on OSX:

### OSX

1. Clone the git repository to a folder, e.g. ~/HarmonicMotion.
2. Download and install CMake: http://www.cmake.org/cmake/resources/software.html
3. Download and install Qt 5.3: http://qt-project.org/downloads *NB* You need to install Qt to its default location in your home folder otherwise many annyoing problems will arise when building.
4. Download and install Cinder 0.8.0 either using the Mac OSX package or by cloning it from github: http://libcinder.org/download/
5. Open CMake. In the box marked _Where is the source code_, put in _HarmonicMotion_/src where _HarmonicMotion_ is the folder you cloned to in step 1. In the box marked _Where to build the binaries_, put in _HarmonicMotion_/xcode. Below there is a list of settings - you can leave all of these as they are except _CinderDir_ which you need to set to the location of your Cinder installation from step 4.
6. Click Configure. Click Done if this then shows a dialog. When it's finished configuring, press Generate.
7. You should now have an Xcode project in _HarmonicMotion_/xcode. Open this and select HarmonicMotionGui from the list of schemes (on the upper left, which will initially be set to ALL_BUILD). Press the big play button to build and run the toolkit.
8. At present, the schemes that CMake creates are set to build in Debug mode which will be slower than necessary for general use. To change it to the faster Release mode: click on the list of schemes, select Edit Scheme, choose HarmonicMotionGui from the menu at the top, click on Run HarmonicMotionGui on the list on the left side, then change the Build Configuration option from Debug to Release. Press OK then click on the play button again.
9. The app bundle will be placed in _HarmonicMotion_/bin/Debug or _HarmonicMotion_/bin/Release.


## Contact

Tim Murray-Browne, July 2014.

http://timmb.com
hi ...at...... timmb dot com
http://twitter.com/timmurraybrowne

## Acknowledgements

Harmonic Motion was developed by Tim Murray-Browne and Mark D. Plumbley at the Centre for Digital Music, Queen Mary University of London. The project was funded by the UK Engineering and Physical Sciences Research Council (EPSRC) under the Platform Grant (EP/K009559/1) and builds on work completed during Murray-Browne's PhD at the Centre for Digital Music which was funded under an EPSRC Doctoral Training Account (EP/P504031/1).

