# CopperOS
I wanted to understand operating systems, so I (the coolest guy ever) started building one.

<img width="1865" height="1012" alt="CopperOS_v 1 0 0" src="https://github.com/user-attachments/assets/f3fe0f31-f76f-48ba-b66e-15df48babc3f" />


## What makes it Special?
From the graphics perspective it's very similar to MS-DOS as it's just a custom shell OS but with the feature of a built-in search engine which under the hood is an html to text converter but it still searches so don't underrate my work

## Current stand 
Right now it can "browse" the world wide web by using the IP of the QEMU and is equipped with various commands. Under the hood the "browsing" is just an HTTP request for the html file, then an html to text converter converts into text but don't 
browsers like Chrome do a similar method?

# How to install/run
First you'll need to clone this repository into your system by pasting this command into your terminal: `git clone https://github.com/Niedrigerregen/CopperOS.git`

After that you'll need to open the Folder in your Terminal with `cd CopperOS` and install the requiered packages

## How to install the required packages
You have to call a different installation file based on which package manager you have. If you have the apt package manager just run `bash bash/apt_install.sh` 
there are also scripts for the dnf manager and the pacman manager where you just have to insert your package manager in the placeholder "name" `bash bash/name_install.sh`

## Compiling the OS
This step is not really necessary as my latest binaries are also comitted into the repository if you want to use them. Anyways if you want to compile it yourself, don't worry about my existing binaries as you
can just overwrite my binaries with your new ones by inserting `bash bash/make.sh` after opening the CopperOS folder if you didn't already

## Optional: Generating a .iso image
There already are .iso images in the releases but if you want to generate your own .iso image you can do it with `grub-mkrescue -o CopperOS.iso iso`

> Note: 
> If you're on Fedora or your package manager is dnf, there might be problems with generating the .iso image with grub, which is why i recommend using a Debian/Ubuntu based system to generate your image

## Running CopperOS
This is also pretty easy with `bash bash/run.sh` which runs the OS in the terminal. Now you're finished!

## Future
Expand package manager
make a more user friendly file system (home/user/etc.)
