## Installing Node-RED

```shell
bimalka98@LAP-BIMALKA98:~$ sudo apt install build-essential git curl
bimalka98@LAP-BIMALKA98:~$ bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered)

Running Node-RED update for user bimalka98 at /home/bimalka98 on ubuntu


This can take 20-30 minutes on the slower Pi versions - please wait.

  Stop Node-RED                       ✔
  Remove old version of Node-RED      ✔
  Remove old version of Node.js       ✔
  Install Node.js 16 LTS              ✔   v16.17.0   Npm 8.15.0
  Clean npm cache                     ✔
  Install Node-RED core               ✔   3.0.2
  Move global nodes to local          -
  Npm rebuild existing nodes          ✔
  Install extra Pi nodes              -
  Add shortcut commands               ✔
  Update systemd script               ✔


Any errors will be logged to   /var/log/nodered-install.log
All done.
You can now start Node-RED with the command  node-red-start
  or using the icon under   Menu / Programming / Node-RED
Then point your browser to localhost:1880 or http://{your_pi_ip-address}:1880

Started :  Wed Sep 21 18:13:55 +0530 2022
Finished:  Wed Sep 21 18:18:13 +0530 2022

You may want to run   node-red admin init
to configure your initial options and settings.

bimalka98@LAP-BIMALKA98:~$
```