Introduction
------------

This is the source code for Daggerfall Explorer Version 0.90 (Beta2).

Creating Daggerfall Explorer has been a very experimental process.
The entire project is really one large prototype. Due to this, some
parts of the source code will not reflect my highest standards, or
any standards at all for that matter. ;)

Alchemy is included with this project, but only as a static library.
I will make a decision about releasing Alchemy at a later date.

Building Daggerfall Explorer
---------------------------

In order to build this code, you must have the following development
environment.

* Visual Studio.NET
* DirectX 8.1 SDK

Before you can build Daggerfall Explorer, you must add the library
and include paths.

In Visual Studio.NET, follow these steps.

1. Click Tools > Options.
2. Select Projects > VC++ Directories.
3. Add ..\Daggerfall Explorer\Include to the "include" path list.
4. Add ..\Daggerfall Explorer\lib to "library" path list.

Before you can run Daggerfall Explorer, you must copy the contents
of ..\Daggerfall Explorer\redist to a path that will be searched
when the executable starts. This can be in the same folder as the
executable, but a system path such as %systemdrive%\WINNT or
%systemdrive%\WINDOWS is recommended.

NOTE: If you are running Visual Studio.NET, the MFC7 dlls will
already be present on your system.

That should be all you need to build and run your local version of
Daggerfall Explorer.

If you add to Daggerfall Explorer please contact me. I would love
to hear from you.


Gavin Clayton
Interkarma
interkarma@m0use.net
http://www.m0use.net/~interkarma