#!/bin/bash

sudo DEBIAN_FRONTEND=noninteractive apt-get update

if [ $? -eq 0 ];  then
	echo "Successfully updated package lists."
else
	echo "Error updating package lists! Stopping install."
	exit 1
fi

sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y --allow-downgrades --allow-remove-essential --allow-change-held-packages

if [ $? -eq 0 ]; then
	echo "Successfully updated system."
else
	echo "Error updating system! Stopping install."
	exit 1
fi

sudo DEBIAN_FRONTEND=noninteractive apt-get install git -y

if [ $? -eq 0 ]; then
	echo "Successfully installed Git."
else
	echo "Error installig Git! Stopping setup."
	exit 1
fi


if id -u git > /dev/null 2>&1; then
	echo "Git user already exists, deleting user."
	sudo deluser --remove-home git > /dev/null

	if [ $? -ne 0 ]; then
		echo "Error deleting preexisting git user. Stopping setup."
		exit 1

	fi
fi

sudo adduser --gecos "" --disabled-password git > /dev/null

if [ $? -eq 0 ]; then
	echo "Successfully created new git user"
else
	echo "Error creating git user. Stopping setup."
	exit 1
fi

sudo mkdir -p /home/git/.ssh

if [ $? -eq 0 ]; then
	echo "Created /home/git/.ssh"
else
	echo "Error creating .ssh folder. Stopping install."
	exit 1
fi

sudo cp /home/ubuntu/authorized_keys /home/git/.ssh/authorized_keys

if [ $? -eq 0 ]; then
	echo "Copied authorized_keys to git user"
else
	echo "Failed to copy authorized keys!"
	exit 1
fi

sudo chown -R git:git /home/git/.ssh

if [ $? -eq 0 ]; then
	echo "Changed owner of ssh directory"
else
	echo "Failed to change ssh directory ownership"
	exit 1
fi

sudo chown -R git:git /home/git/.ssh/authorized_keys

if [ $? -eq 0 ]; then
	echo "Changed owner of authorized keys"
else
	echo "Failed to change authorized keys ownership"
	exit 1
fi


sudo chmod 700 /home/git/.ssh

if [ $? -eq 0 ]; then
	echo "Changed permissions of ssh directory"
else
	echo "Failed to change ssh directory permissions"
	exit 1
fi

sudo chmod 644 /home/git/.ssh/authorized_keys

if [ $? -eq 0 ]; then
	echo "Changed permission of authorized keys"
else
	echo "Failed to change permission of authorized keys"
	exit 1
fi

mkdir -p .ssh

if [ $? -eq 0 ]; then
	echo "Creted ubuntu ssh directory"
else
	echo "Could not create ubuntu ssh directory"
	exit 1
fi

echo -e 'Host gito\n\tStrictHostKeyChecking=no\n\tHostName 127.0.0.1\n\tUser git\n\tIdentityFile ~/.ssh/gito' > .ssh/config

if [ $? -eq 0 ]; then
	echo "Wrote ssh config file"
else
	echo "Failed to write config file"
	exit 1
fi

mv ./gito ./.ssh/gito

if [ $? -eq 0 ]; then
	echo "Moved private ssh key to location"
else
	echo "Failed to move private ssh key"
	exit 1
fi
