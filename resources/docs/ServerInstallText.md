### This software will now do the following on the remote server:
  * Update software with apt, and perform OS upgrade
  * Attach iSCSI storage block and mount it
  * Create and initialize swap partition
  * Create git user
  * Clone gitolite install directory
  * Run gitolite install scripts
  * Move "repositories" folder to iSCSI storage
  * Symlink "repositories" directory to the correct location

This process may take several minutes. Please do not close the software while it is processing.
