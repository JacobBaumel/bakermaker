# Program Usage

## Syncing and Saving from/to server

In order to save changes to the server, or download changes made by others, you must synchronize the program state with
the server. This is done with the "Sync" and "Save" buttons.

### Saving application changes to server

In order to apply configuration changes in the application to the server, you must press the "Save" button. **This
will overwrite any existing configuration**, and replace it with the configuration represented in the application.
See the technical explanation for information on details on how saving works.

Almost all configuration changes (exception noted below) **will not be applied** until the save button is pushed.

### Syncing changes from server

Syncing changes from the server works in much the same way as saving does, but in reverse. This is intended for if
another person also has access to the administrator account for gitolite, and you want to download the changes they
have made to your application. **The application will replace the local repository configuration with the server
configuration**, but will attempt to resolve missing user keys:

- If the server has a user present that the local application does not have a private key for, this user will
  still be present in the repository config, but they will not be listed in the users list, and once removed from
  a repository, they can not be added back.
- If the application has a private key for a user the server does not have, it will regenerate the public key and
  upload it to the server on the next save.

Both of these instances will be reported to the user in a modal after the sync operation is complete.

**NOTE:** The application must be "synced" directly after installation is complete, and before additional
configuration can be done.

## User management

### Adding users

New users can be added to the system by typing in their username into the text input labelled "Enter new user" and
clicking "Add". This will generate the corresponding user keys, and add them to the table of available users. Usernames
have a few restrictions:

- No user can be named "admin"
- No users can share the same name
- Usernames can be a maximum of 64 characters

### Deleting users

To delete a user, find their name in the table of users and click "delete". This will also delete their user key,
and remove them from all repositories they were previously a part of.

## Repository Management

### Creating a repository

New repositories can be added by typing a name in the "Enter New Repository Name" field, and clicking "Create". The
creation of the repository on the server will be queued for the next save. Repository names have a few restrictions:

- No repository can be named "gitolite-admin"
- No two repositories can share the same name
- Repository names are limited to 64 characters

### Managing user access to repositories

By default, no user except the gitolite admin user has access to any repository, and user permissions are set on a
per-repository basis. To select a repository to edit permissions for, select it from the dropdown (or create one
first). Then, select the user to add to the repository and press "Add". This will add the selected user with default
permissions to the repository. To give a user admin access to a repository, click the checkbox next to their name in
the table.

There are two levels of permissions for repository access:

- Default permission: This restricts all access to the master branch of the repository and disables force pushes, but
  allows edits on any other branch. This option does **not** limit merges on regular branches.
- Admin permissions: This enables force pushes, and edits to the master branch. This permission is limited in scope
  to the specific repository, and does not give higher permissions on other repositories or to the gitolite server
  as a whole.

To revoke a user's access to a repository entirely, click the "remove" button next to their name.

### Deleting a repository

A repository can be deleted by pressing the red delete button. Following a confirmation, the application will
**immediately begin a save and delete all data belonging to the repository! This data is not recoverable!** All
repository data will be immediately deleted from the server, and the repository will be removed from the server
configuration.

## Storage usage viewer

To view the storage use on the remote server, first click "refresh" to refresh the usage data. The software will
show overall utilization labelled "Total Storage Used", and per-repository usage, with percentages relative to the
total used storage (ie repositories will show their usage percent relative to each other).

## Client Setup Program

To help with setting up users to connect to the server, you can extract a client program to automate the process of
setting up the environment to use git. After clicking the "Extract" button, the application will output into a
folder called "setupclient" in the working directory with all the necessary components. This includes all the
private user keys to be installed onto user machines, as well as connection information, and the client program
itself, "setupclient.exe".

To use the setup program, copy the entire "setupclient" folder onto portable storage like a flash drive, and run the
program on each target machine. The software will ask for the username to install, as well as a location to extract
the git client itself through a file picker. The program will then set up all the required files in order to use git.

This setup is done to the Windows user only, and not to the whole machine. This means the setup software can be run
for each person who uses a piece of hardware, and a particular machine is not limited to one user.

## Debug exports

The "Create Debug Export" button will package the files necessary to hold all relevant information about a server
setup into a singular zip archive to be shared if needed. This archive includes:

- config.json
- install.log
- gitolite.conf
- keyfile
- the "keys" directory

### Sharing admin access to a server

Sharing administrative access to a server can be done in a few ways, but the simplist is to share the entire working
folder of the application. Sharing this folder will copy all necessary information, and the application will 
function as normal. Another way is to use the debug export, and have the other person extract the contents of the 
export into their own folder along with the application. Both methods work the same, but the second method could be 
used if the other person already has a copy of the application executable.