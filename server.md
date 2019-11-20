# Installation

1. Download and install Etcher https://www.balena.io/etcher/
2. Download DietPi image from https://dietpi.com/
3. Flash SD card with image, and insert into RPi
4. Boot RPi

# Configuration

## Ssh into DietPi

`ssh root@[ip xxx.xxx.xxx.xxx]`

Password: `dietpi`

## Install software

Available software listed on the [wiki](https://github.com/MichaIng/DietPi/wiki/DietPi-Software-list).

Can also install all at once: `dietpi-software install 9 130 122 123`.

### **Node.js** | package manager

`dietpi-software install 9`

### **Python pip** | package manager

`dietpi-software install 130`

### **Node-red**

`dietpi-software install 122`

### **Mosquitto** | MQTT support

`dietpi-software install 123`

# Connect to the node-red instance

Surf to the node-red instance at [http://[ip xxx.xxx.xxx.xxx]:1880](http://[ip xxx.xxx.xxx.xxx]:1880).
