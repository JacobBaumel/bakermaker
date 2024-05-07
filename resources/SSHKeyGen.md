In this step, you will configure what users you want and what permission level they have. Afterward, libssh will
generate key files for each of these users. The generation step may take several minutes depending on how many users are
configured.

There are 3 permission levels to choose from:
  * __Normal:__ Basic permissions required to read/write to a repository. This level is intended for normal students who
only need basic access to repositories. 
    * The exact repositories they can access are configured later.
    * This permission level disables force pushes, and pushes directly to the master branch
  * __Advanced:__ Higher permission level for scrum masters or trusted students who may need to be able to force push, edit
the master branch, or complete other advanced operations.
  * __Admin:__ Highest permission level, and is assigned only to the user of this software. This level allows full access to
all repositories, as well as administrative access to the server itself.

Other important notes:
  * There cannot be two users with the same name. If two students have the same first name, be sure to include other
characters to make sure they are still differentiated.
  * Once the keys have been generated, it will be possible to create the user-side setup program. This program will
install git to the target system, along with the corresponding key. 
  * User names are up to 64 characters long
