# packet-sniffer
#### GL C/Embedded BaseCamp
#### Test assignment
-----

#### How to build
  
  ```
  make
  ```
  
#### How to run

Firstly run the daemon using: 
  
  ```
  make run
  ```
 or
  ```
 sudo ./build/packet-sniffer
  ```
It is necessary to have root privileges while using packet-sniffer.

Now daemon is running. 

The PID of this daemon is in daemon_id.txt. 
It might make work with daemon easier because you do not need to seek for daemon PID if you need it.

#### How to use

To communicate with daemon use CLI. 
  ```
  sudo ./build/cli [command]
  ```
  
Avaliable commands are:
- --help - argument is used to get more info.
- start - argument is used to start writing sniffed packet information into dump.txt.
- stop - argument is used to stop writing sniffed packet information into dump.txt.
- reset - argument is used to clean dump.txt.
- show - argument is used to show dump.txt.
- show -i [ip] - argument is used to show received packets from [ip].
- show -d [domain] - argument is used to show received packets from [domain].

To call CLI help you also can use:
  ```
  make runcli
  ```
  
#### Example of Usage

1. Run packet-sniffer. It satrt to catch packets on default interface (such as enp, eth).
```
 sudo ./build/packet-sniffer
```
2. Send command START using CLI. It starts writing catched packets info into dump.txt.
```
 sudo ./build/cli start
```
- Now packet-sniffer is writing sniffed packets into dump.txt. Just make some interactions with Internet.
You might test it using:

```
 ping -c 1 8.8.8.8
 curl google.com
```
3. Send command STOP using CLI. It starts writing catched packets info into dump.txt.
```
 sudo ./build/cli stop
```
- Packet-sniffer stoped writing sniffed packets into dump.txt. You might read dump.txt to get more info about traffic on default interface.

- To kill packet-sniffer daemon you might read PID from daemon_id.txt adnt use:
```
 sudo kill [PID]
```
Please, dont use SIGKILL, use only SIGTERM.
- You might read log.txt file to get more info about packet-sniffer daemon work session.

*If you need more info about packet-sniffer, read documantation, please. You could find it in ./doc/*
