---
description: Getting started data aggregation with daggy
---

# Getting Started

## Getting Daggy

### Windows/MacOS

Download installation package \(exe/dmg\) from last [release](https://github.com/synacker/daggy/releases/tag/2.0.0) page and install it

### Linux

Download rpm/deb package from last [release](https://github.com/synacker/daggy/releases/tag/2.0.0) page and install it

{% hint style="info" %}
All daggy dependencies \(Qt5, libssh2, libyaml-cpp\) are included into separate dir and does not effect system applications.
{% endhint %}

### Fedora 29/30/31

```bash
sudo dnf install daggy daggy-devel
```

### Build from source

#### Environment requirenments

[Conan](https://conan.io/), [cmake](https://cmake.org/), [git](https://git-scm.com/) and C++ compiler.

#### Build steps

```bash
git clone https://github.com/synacker/daggy.git
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install ../daggy --build=missing
conan build ../daggy
cmake install
```

#### Check installation

```bash
daggy --help
Usage: daggy [options] *.yaml|*.yml|*.json

Options:
  -o, --output <folder>       Set output folder
  -f, --format <json|yaml>    Source format
  -i, --stdin                 Read data aggregation sources from stdin
  -t, --timeout <time in ms>  Auto complete timeout
  -h, --help                  Displays help on commandline options.
  --help-all                  Displays help including Qt specific options.
  -v, --version               Displays version information.

Arguments:
  file                        data aggregation sources file
```

## Getting Started with data aggregation and streaming

### Simple Data Aggregation Source

#### Create yaml or json file

{% tabs %}
{% tab title="YAML" %}
{% code title="simple.yaml" %}
```yaml
sources:
    localhost:
        type: local
        commands:
            pingYa:
                exec: ping ya.ru
                extension: log
```
{% endcode %}
{% endtab %}

{% tab title="JSON" %}
{% code title="simple.json" %}
```javascript
{
    "sources": {
        "localhost": {
            "commands": {
                "pingYa": {
                    "extension": "log",
                    "exec": "ping ya.ru"
                }
            },
            "type": "local"
        }
    }
}
```
{% endcode %}
{% endtab %}
{% endtabs %}

#### Run daggy

{% tabs %}
{% tab title="YAML" %}
```bash
daggy simple.yaml
```
{% endtab %}

{% tab title="JSON" %}
```
daggy simple.json
```
{% endtab %}
{% endtabs %}

#### Check console output

```text
23:07:23:977 | AppStat  | Start aggregation in 01-04-20_23-07-23-977_simple
23:07:23:977 | ProvStat | localhost       | New state: Started
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Starting
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Started
```

{% hint style="info" %}
There are all commands from **simple.yaml/simple.json** are streams in **01-04-20\_23-07-23-977\_simple** with output files
{% endhint %}

#### Tailing streams from Simple Data Source

```text
tail -f 01-04-20_23-07-23-977_simple/*
64 bytes from ya.ru (87.250.250.242): icmp_seq=99 ttl=249 time=21.2 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=100 ttl=249 time=18.8 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=101 ttl=249 time=23.5 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=102 ttl=249 time=18.8 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=103 ttl=249 time=18.8 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=104 ttl=249 time=17.4 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=105 ttl=249 time=17.4 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=106 ttl=249 time=20.1 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=107 ttl=249 time=25.8 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=108 ttl=249 time=35.1 ms
64 bytes from ya.ru (87.250.250.242): icmp_seq=109 ttl=249 time=21.1 ms
```

#### Stop data aggregation and streaming

{% hint style="info" %}
Type CTRL+C for stopping data aggregation and streaming
{% endhint %}

```text
23:07:23:977 | AppStat  | Start aggregation in 01-04-20_23-07-23-977_simple
23:07:23:977 | ProvStat | localhost       | New state: Started
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Starting
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Started
^C23:17:56:667 | ProvStat | localhost       | New state: Finishing
23:17:56:668 | CommStat | localhost       | pingYa          | New state: Finished. Exit code: 0
23:17:56:668 | ProvStat | localhost       | New state: Finished
23:17:56:668 | AppStat  | Stop aggregation in 01-04-20_23-07-23-977_simple
```

#### Investigate aggregated data

```bash
ls -l 01-04-20_23-07-23-977_simple/
-rw-r--r-- 1 muxa muxa 45574 апр  1 23:17 localhost-pingYa.log
```

### Example of Data Aggregation Sources with multiple commands and remote data aggregation and streaming

{% tabs %}
{% tab title="YAML" %}
{% code title="multiple\_and\_remote.yaml" %}
```yaml
aliases:  
    - &my_commands
        pingYa:
            exec: ping ya.ru
            extension: log
        pingGoo:
            exec: ping goo.gl
            extension: log
        
    - &ssh_auth
        user: muxa
        key: /home/muxa/.ssh/id_rsa
            
sources:
    localhost:
        type: local
        commands: *my_commands
    remotehost:
        host: 192.168.1.9
        type: ssh2
        connection: *ssh_auth
        commands: *my_commands
    remotehost2:
        host: 192.168.1.9
        type: ssh2
        connection: *ssh_auth
        commands: *my_commands
    remotehost3:
        host: 192.168.1.9
        type: ssh2
        connection: *ssh_auth
        commands: *my_commands
```
{% endcode %}
{% endtab %}

{% tab title="JSON" %}
{% code title="multiple\_and\_remote.json" %}
```javascript
{
    "sources": {
      "remotehost3": {
        "connection": {
          "user": "muxa", 
          "key": "/home/muxa/.ssh/id_rsa"
        }, 
        "commands": {
          "pingYa": {
            "extension": "log", 
            "exec": "ping ya.ru"
          }, 
          "pingGoo": {
            "extension": "log", 
            "exec": "ping goo.gl"
          }
        }, 
        "type": "ssh2", 
        "host": "192.168.1.9"
      }, 
      "remotehost2": {
        "connection": {
          "user": "muxa", 
          "key": "/home/muxa/.ssh/id_rsa"
        }, 
        "commands": {
          "pingYa": {
            "extension": "log", 
            "exec": "ping ya.ru"
          }, 
          "pingGoo": {
            "extension": "log", 
            "exec": "ping goo.gl"
          }
        }, 
        "type": "ssh2", 
        "host": "192.168.1.9"
      }, 
      "remotehost": {
        "connection": {
          "user": "muxa", 
          "key": "/home/muxa/.ssh/id_rsa"
        }, 
        "commands": {
          "pingYa": {
            "extension": "log", 
            "exec": "ping ya.ru"
          }, 
          "pingGoo": {
            "extension": "log", 
            "exec": "ping goo.gl"
          }
        }, 
        "type": "ssh2", 
        "host": "192.168.1.9"
      }, 
      "localhost": {
        "commands": {
          "pingYa": {
            "extension": "log", 
            "exec": "ping ya.ru"
          }, 
          "pingGoo": {
            "extension": "log", 
            "exec": "ping goo.gl"
          }
        }, 
        "type": "local"
      }
    }
  }
```
{% endcode %}
{% endtab %}
{% endtabs %}

