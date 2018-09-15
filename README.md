# Short description #

Data aggregation utility is a program for extracting information from multiple remote servers via ssh simultaneously.

Your can collect any remote server ssh output.

#Features#
* Simple to use. If you know about json, ssh and bash, your know how to use it!
* It provides simultaneous aggregating from multiple servers and simultaneous executing multiple bash commands on each server. 
* Each standard output from bash command is writing to the separate file


## How do I get set up? ##

### Download ###

For windows - https://bitbucket.org/milovidov/dataagregatorproject/downloads/DataAgregator-0.9.4-Win32.7z

For other operatrion systems you can compile from source.

### Compile from source ###
1. `hg clone https://milovidov@bitbucket.org/milovidov/dataagregatorproject`
2. `cd dataagregatorproject`
3. `qmake` or `qmake CONFIG+=UseSystemBotan`
4. `make` or `nmake`
5. `cd Release`
6. `DataAgregator -h`

### Configuration ###

For DataAgregator start your need to configure your data sources environment.

Let's see the next example, testEnvironment.json:

```
#!json

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
        "passwordAuthorization" : {
            "host" : "127.0.0.1",
            "login" : "WindowsUser",
            "password" : "test1234"
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

## Dependencies ##

Qt 5.5 or above

libbotan-1.10 for compilation with `qmake CONFIG+=UseSystemBotan`