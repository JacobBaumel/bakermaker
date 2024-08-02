# Error Codes

This section covers all the different error codes and what they mean in their specific contexts, as well as some
potential _simple_ fixes. In the case that the suggested fix does not work, there may be an application error that 
requires more in-depth investigation.

## Server Setup / Installation

### SSH Connection verification

- Error code -1: Invalid arguments; Check that IP address, username, and key file location was successfully entered.
- Error code -2: Application could not open a connection to the server; Check that the computer has an internet
  connection, and that the server is responding to ssh connection requests.
- Error code -3: Could not authenticate public key; Check that the correct *private* key was selected.
- Error code -4: Private key authentication rejected; Check that the correct *private* key was selected.
- Error code -5: Could not read private key; Check that the private key file path is correct, and that the key does
  not have a password set.

### Creating Admin Key, User Management

- Error code -1: Failed to generate ssh key; Error with cryptography library, try restarting program, if that fails
  try restarting computer
- Error code -2: Failed to write private key to file; Check the program has write access to disk
- Error code -3: Error creating public key; Error with cryptography library, try restarting program, if that fails
  try restarting computer
- Error code -4: Failed to write public key to file; Check the program has write access to disk

### Installing

- Error code -1: Failed to connect to server; Check internet connection and server connection info
- Error code 1: Error when installing gitolite on the server; Read installation log for details

### Syncing from server

- Error code -1: Failed to connect to server; Check internet connection and server connection info
- Error code -2: Failed to open remote directory "gitoltie-admin/keydir"; Check that gitolite was successfully installed
- Error code -3: Error downloading file from server; Check program write access and server connection
- Error code -6: Could not read private key; Try restarting application
- Error code -7: Could not create public key; Cryptography library error, try restarting application
- Error code -8: Error downloading file "gitolite.conf" from server; Check program write access and server connection
- Error code -10: Could not parse "gitolite.conf"; Check config file format, try restarting program

### Saving data to server

- Error code -1: Failed to connect to server; Check internet connection and server connection info
- Error code -2: Failed to upload key to server; Check server connectivity, and that gitolite was successfully installed
- Error code -3: Failed to upload "gitolite.conf"; Check server connectivity, and that gitolite was successfully
  installed
- Error code -4: Failed to delete repository; Check server connectivity, and that gitolite was successfully installed

### Storage Usage

- Error code -1: Failed to connect to server; Check internet connection and server connection info
- Error code -2: Error getting total storage sizes; Try restarting application
- Error code -3: Error getting individual repository sizes; Try restarting application

### Client Program Extraction

- Error code 1: Error writing files to disk; Check program write eaccess to disk