# Bakermaker (gitolitesetuper)

-----

## Introduction
Bakermaker is an open-source utility for creating and managing
[gitolite](https://gitolite.com/gitolite/index.html) servers. It provides a 
simple interface for installing gitolite on a fresh server, and managing
repositories and users all through a single ssh connection. Bakermaker is
intended only for **simple** configurations, meaning that the program does not
support gitolite's [group definitions](https://gitolite.com/gitolite/conf.html#group-definitions),
any [access rules](https://gitolite.com/gitolite/conf.html#access-rules) other than the provided
defaults, or anything else beyond simple repository creation, deletion, user creation, deletion,
and basic access rights. 

This project is designed for my videogame programming teacher to assist with creating a remote git
repository solution that works within FriscoISD's restrictive network and computer policies, hence
some of the strange architecture with libraries; namely the banned access to all reasonable cloud git
providers like github itself, gitlab, and bitbucket. This project is designed to replicate the work I
did for my class to provide a remote repository with these restrictions. However, this project will work
outside of this specific setting, as long as any advanced gitolite config is required.

This project is licensed under the [MIT license](https://mit-license.org/).

## Restrictions
This project assumes all https access to the remote server is blocked, and there are no preinstalled
libraries or resources available on the target machine. This project also assumes the resulting executable
will have to be distributed as a single file, and does not expect the user to have to set up the runtime
environment. To do this, this project uses [libromfs](https://github.com/werwolv/libromfs) to package all
resources into the executable. This includes a copy of the 
[client program](https://github.com/jacobbaumel/bakermakersetuper), as well as some extra libraries needed
at runtime that could not be statically linked.

## Compiling Bakermaker
Bakermaker is entirely written in C++ 20, and all compilation happens through cmake and the **MSVC** compiler.
Bakermaker will not work with the MingW compiler, as this project relies on [DLL delay loading](
https://learn.microsoft.com/en-us/cpp/build/reference/linker-support-for-delay-loaded-dlls?view=msvc-170). 

This project submodules [GLFW](https://github.ccom/GLFW/GLFW), [Dear ImGui](https://github.com/ocornut/imgui),
[libromfs](https://github.com/werwolv/libromfs), and [miniz](https://github.com/richgel999/miniz) into the [libs](
./libs) folder. In addition, it requires [string theory](https://github.com/zrax/string_theory), [libssh](
https://libssh.org), [openssl](https://github.com/openssl/openssl), and [zlib](https://zlib.net) to be available on
the system, such as through [vcpkg](https://vcpkg.io).

Before building, the project requires a few user-provided files present:
  - A copy of the [client program](https://github.com/jacobbaumel/bakermakersetuper) located at [resources/setupclient.exe](
./libs/setupclient.exe)
  - A zip archive located at [resources/libs.zip](resources/libs.zip) with the following files:
    - ssh.dll
    - libcrypto-3-x64.dll
    - ucrtbased.dll
    - vcruntime140d.dll
  - A font, provided as "font-bold.ttf" and "font-regular.ttf", both in the resources folder
The final output will be located in the build folder, named gitolitesetuper.exe.

## Using Bakermaker
There is a built-in manual tab that hosts all documentation for the program (WIP). Once finalized, all documentation 
files will be located in [resources/docs](resources/docs).

## gitolite.conf output
Bakermaker only supports a specific subset format of the normal gitolite configuration file. Bakermaker can only 
read configuration files in a specific format:

The first two lines read:

```
repo gitolite-admin
    RW+ = admin
```

Following this, all other repositories are listed as so, with one line break between them:
```
repo <reponame>
    RW+ = admin <admin users>
    - master    = <non admin users>
    RW  = <non admin users>
```

No other gitolite configuration options are currently supported.

## Reporting Bugs
Feel free to open a github issue if you find a bug, but I am not looking to expand this project much further past 
where it currently is. It is designed for a specific circumstance, and I am more interested in other projects, so if 
the bug is critical I will (maybe) fix it, but I most likely will not address feature requests or unusual bugs that 
would not show under normal usage.