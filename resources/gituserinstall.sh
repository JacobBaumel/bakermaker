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
    mkdir ~/bin
    mv .ssh/authorized_keys ~/admin.pub
    err $? "Moved admin.pub" "Failed to move files"

    prog 1
    ;&

  1)
    git clone https://github.com/sitaramc/gitolite
    err $? "Cloned gitolite repository" "Failed to clone gitolite repository"

    prog 2
    ;&

  2)
    gitolite/install -ln /home/git/bin
    err $? "Installed gitolite binaries" "Failed to install gitolite binaries"

    prog 3
    ;&

  3)
    bin/gitolite setup -pk admin.pub
    err $? "Set up gitolite admin key" "Failed to set gitolite admin key"

    prog 4
    ;&

  4)
    if [ $1 -eq 1 ]; then
      mv ./repositories /mnt/repositories
      err $? "Moving repositories to ISCSI drive" "Failed to move repositories to ISCSI drive"
    fi

    prog 5
    ;&

  5)
    if [ $1 -eq 1 ]; then
      ln -s /mnt/repositories /home/git/repositories
      err $? "Linked repositories folder to correct location" "Failed to create repositories link"
    fi

    ;;

esac

echo "Finished setting up git user"

#mkdir ~/bin
#
#mv .ssh/authorized_keys ~/admin.pub
#err $? "Moved admin.pub" "Failed to move files"

#git clone https://github.com/sitaramc/gitolite
#err $? "Cloned gitolite repository" "Failed to clone gitolite repository"

#gitolite/install -ln /home/git/bin
#err $? "Installed gitolite binaries" "Failed to install gitolite binaries"

#bin/gitolite setup -pk admin.pub
#err $? "Set up gitolite admin key" "Failed to set gitolite admin key"

#mv ./repositories /mnt/repositories
#err $? "Moving repositories to ISCSI drive" "Failed to move repositories to ISCSI drive"

#ln -s /mnt/repositories /home/git/repositories
#err $? "Linked repositories folder to correct location" "Failed to create repositories link"