# Server setup instructions

This section covers how to use the software to configure and install gitolite components to the target server.

See the error code page for information on failures.

## ISCSI options

If using ISCSI attached storage, ensure the checkbox is checked, and paste in the 3 connection commands noted in the
server creation stage. Hit the submit button to confirm. The submit button can be pushed again if the commands change.

## Verify server connection information

Enter in the IP address, port, and username for the server ssh connection. Then, click "Browse for server private key"
to open a file browser, and select the server key file downloaded in the server creation step. Click submit to
verify the connection.

This may take a few seconds, as the software will attempt to open an ssh connection with the server.

If successful, this step will also copy the server private key to the working folder of the application.

## Gitolite admin key creation

This step generates the administrator key for gitolite. This key is seperate from the key used to connect via ssh to
the server. This key will be located at "keys/admin" and "keys/admin.pub". It is not used elsewhere, besides for
gitolite.

## Install

Press the install button to begin the installation process. This will take several minutes. While the program is
installing, you can push "Show/Hide Command Outputs" to view the progress of the installation. This progress will
also be written to install.log in the working directory.

If the installation fails, it will display an error code, and subsequent presses of the install button will continue
where the program left off.

When finished, a success message will show. Push the "Ok" button to continue.

During installation, the software will:
- Update the operating system
- Attach iSCSI storage block and mount it (if applicable)
- Create and initialize swap partition
- Create git user
- Clone gitolite install directory
- Run gitolite install scripts
- Move "repositories" folder to iSCSI storage (if applicable)
- Symlink "repositories" directory to the correct location (if applicable)