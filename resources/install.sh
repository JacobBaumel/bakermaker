#!/bin/bash

err () {
  if [ $1 -eq 0 ]; then
    echo $2
  else
    echo $3
    exit 1
  fi
}

prog () {
  echo $1 > progress
}

if [ ! -f progress ]; then
  echo 0 > progress
fi

case $(cat progress) in
  0)
    if [ $1 -eq 1 ]; then
      $2
      err $? "Connecting ISCSI drive (1/3)" "Failed to connect to ISCSI drive"
    fi

    prog 1
    ;&

  1)
    if [ $1 -eq 1 ]; then
      $3
      err $? "Connecting ISCSI drive (2/3)" "Failed to connect to ISCSI drive"
    fi

    prog 2
    ;&

  2)
    if [ $1 -eq 1 ]; then
      $4
      err $? "Connecting ISCSI drive (3/3)" "Failed to connect to ISCSI drive"
      sleep 5
    fi

    prog 3
    ;&

  3)
    if [ $1 -eq 1 ]; then
      sudo mkfs -t ext4 /dev/oracleoci/oraclevdb
      err $? "Formatting ISCSI drive" "Failed to format ISCSI drive"
    fi

    prog 4
    ;&

  4)
    if [ $1 -eq 1 ]; then
      uuid=$(sudo blkid /dev/oracleoci/oraclevdb -o value | head -n 1)
      printf '\n\nUUID="%s"\t/mnt\text4\tdefaults,noatime,_netdev\t0 2' ${uuid} | sudo tee -a /etc/fstab
      err $? "Adding ISCSI drive to /etc/fstab" "Failed to write to /etc/fstab"
    fi

    prog 5
    ;&

  5)
    if [ $1 -eq 1 ]; then
      sudo mount -a
      err $? "Mounting ISCSI drive" "Failed to mount ISCSI drive"
    fi

    prog 6
    ;&

  6)
    if [ $1 -eq 1 ]; then
      sudo chmod a+rwx /mnt -R
      err $? "Writing permissions for ISCSI drive" "Failed to write ISCSI permissions"
    else
      echo "Skipping ISCSI setup"
    fi

    prog 7
    ;&

  7)
    sudo DEBIAN_FRONTEND=noninteractive apt-get update
    err $? "Successfully updated package lists" "Error updating package lists! Stopping install"

    prog 8
    ;&

  8)
    sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y --allow-downgrades --allow-remove-essential --allow-change-held-packages
    err $? "Successfully updated system" "Error updating system"

    prog 9;
    ;&

  9)
    sudo DEBIAN_FRONTEND=noninteractive apt-get install git -y
    err $? "Successfully installed git" "Error installig Git! Stopping setup"

    prog 10
    ;&

  10)
    if id -u git > /dev/null 2>&1; then
    	echo "Git user already exists, deleting user."
    	sudo deluser --remove-home git > /dev/null

    	if [ $? -ne 0 ]; then
    		echo "Error deleting preexisting git user"
    		exit 1
    	fi
    fi

    sudo adduser --gecos "" --disabled-password git > /dev/null
    err $? "Successfully created new git user" "Error creating git user"

    prog 11
    ;&

  11)
    sudo mkdir -p /home/git/.ssh
    err $? "Created /home/git/.ssh" "Error creating .ssh folder"

    prog 12
    ;&

  12)
    sudo cp /home/ubuntu/authorized_keys /home/git/.ssh/authorized_keys
    err $? "Copied authorized_keys to git user" "Failed to copy authorized keys"

    prog 13

    ;&

  13)
    sudo chown -R git:git /home/git/.ssh
    err $? "Changed owner of ssh directory" "Failed to change ssh directory ownership"

    prog 14
    ;&

  14)
    sudo chown -R git:git /home/git/.ssh/authorized_keys
    err $? "Changed owner of authorized keys" "Failed to change authorized keys ownership"

    prog 15
    ;&

  15)
    sudo chmod 700 /home/git/.ssh
    err $? "Changed permissions of ssh directory" "Failed to change ssh directory permissions"

    prog 16
    ;&

  16)
    sudo chmod 644 /home/git/.ssh/authorized_keys
    err $? "Changed permission of authorized keys" "Failed to change permission of authorized keys"

    prog 17
    ;&

  17)
    sudo chmod 644 /home/git/.ssh/authorized_keys
    err $? "Changed permission of authorized keys" "Failed to change permission of authorized keys"

    prog 18
    ;&

  18)
    sudo chmod 644 /home/git/.ssh/authorized_keys
    err $? "Changed permission of authorized keys" "Failed to change permission of authorized keys"

    prog 19
    ;&

  19)
    mkdir -p .ssh
    err $? "Creted ubuntu ssh directory" "Could not create ubuntu ssh directory"

    prog 20
    ;&

  20)
    mkdir -p .ssh
    err $? "Creted ubuntu ssh directory" "Could not create ubuntu ssh directory"

    prog 21
    ;&

  21)
    echo -e 'Host gito\n\tStrictHostKeyChecking=no\n\tHostName 127.0.0.1\n\tUser git\n\tIdentityFile ~/.ssh/gito' > .ssh/config
    err $? "Wrote ssh config file" "Failed to write config file"

    prog 22
    ;&

  22)
    mv ./gito ./.ssh/gito
    err $? "Moved private ssh key to location" "Failed to move private ssh key"

    prog 23
    ;&

  23)
    scp ./gituserinstall.sh gito:~/gituserinstall.sh
    ssh gito "chmod +x ~/gituserinstall.sh"
    err $? "Uploaded second setup script" "Failed to upload second setup script"

    prog 24
    ;&

  24)
    ssh gito "~/gituserinstall.sh $1"
    err $? "Completed git user setup" "Failed to set up git user"

    prog 25
    ;&

  25)
    git clone gito:gitolite-admin
    err $? "Cloned administrative repository" "Failed to clone administrative repository"

    prog 26
    ;&

  26)
    git config --global user.name "admin"
    err $? "Set git name" "Failed to set git name"

    prog 27
    ;&

  27)
    git config --global user.email "admin"
    err $? "Set git email" "Failed to set git email"

    prog 28
    ;&

  28)
    echo -e 'repo gitolite-admin\n\tRW+\t=\tadmin' > gitolite-admin/conf/gitolite.conf
    err $? "Wrote to gitolite config file" "Failed to write to gitolite config file"

    prog 29
    ;&

  29)
    /home/ubuntu/commitall.sh
    err $? "Committed changes" "Failed to commit changes"

    prog 30
    ;&

  30)
    sudo rm -rf /home/git/repositories/testing.git

    ;;
esac

echo "Finished installing git server"