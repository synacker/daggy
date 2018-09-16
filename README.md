# Short description #

DataAgregator - utility that can run and save output from multiple ssh commands 
on multiple servers simultaneously.

Your can collect any remote server ssh output.

# Features
* Simple to use. If you know about json, ssh and bash, your know how to use it!
* It provides simultaneous aggregating from multiple servers and simultaneous executing multiple bash commands on each server. 
* Each standard output from bash command is writing to the separate file


# Getting started #

## Compile from source ##

### Dependencies ###
1. Qt 5.5 or above
2. libbotan-2.7-devel for Linux

### Compilation steps ###
1. `git clone https://github.com/milovidov/dataagregator.git`
2. `cd dataagregatorproject`
3. `qmake`
4. `make` or `nmake`
5. `cd Release`
6. `DataAgregator -h`

### Configuration ###

For DataAgregator start your need to configure your data sources environment.

Let's see the next example, testEnvironment.json:

```json
{
    "LinuxServer" : {
        "passwordAuthorization" : {
            "host" : "127.0.0.1",
            "login" : "LinuxUser",
            "password" : "test1234"
        },
        "commands" : {
            "messagesLog" : {
                "command" : "tail -f /var/log/messages",
                "outputExtension" : "log"
            },
            "pcapExample" : {
                "command" : "tcpdump -i eth0 port 80 -w -",
                "outputExtension" : "pcap"
            }
        }
    },
    "WindowsServer" : {
        "authorization" : {
            "host" : "127.0.0.1",
            "login" : "WindowsUser",
            "key" : "~/.ssh/id_rsa"
        },
        "commands" : {
            "tailLog1" : {
                "command" : "tail -f /cygdrive/c/ExampleLog1.log",
                "outputExtension" : "log"
            },
            "tailLog2" : {
                "command" : "tail -f /cygdrive/c/ExampleLog1.log",
                "outputExtension" : "log"
            },
            "pcapExample" : {
                "command" : "\"/cygdrive/c/Program Files/Wireshark/tshark\" -f \"port 80\" -F pcap -w -",
                "outputExtension" : "pcap"
            }
        }
    }
}
```

There are two servers with several commands each. Each command will collect it output data to separate file with next name template: serverId_commandId.outputExtension

For execution DataAgregator with tihs environment type next:
`DataAgregator testEnvironment.json`

For stopping data extraction type `CTRL+C`.

## Troubleshooting ##
The latests version of ssh-keygen utility generate new type of keys that contains next lines
```
BEGIN OPENSSH PRIVATE KEY
...
END OPENSSH PRIVATE KEY
```
This keys are not supported, and you can generate keys by the folowing commands:
```bash
openssl genrsa -out ~/.ssh/id_rsa 2048
ssh-keygen -y -f ~/.ssh/id_rsa > ~/.ssh/id_rsa.pub
```