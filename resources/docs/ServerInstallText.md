### This software will now do the following on the remote server:

- Update the operating system
- Attach iSCSI storage block and mount it (if applicable)
- Create and initialize swap partition
- Create git user
- Clone gitolite install directory
- Run gitolite install scripts
- Move "repositories" folder to iSCSI storage (if applicable)
- Symlink "repositories" directory to the correct location (if applicable)

This process may take several minutes. Please do not close the software while it is processing.
