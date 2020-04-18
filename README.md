<p align="center">
  <img width="256" height="256" src="daggy_logo.svg">
  <br/>
  <b>Daggy - Data Aggregation Utility</b>
</p>

**Daggy - Data Aggregation Utility**. Server-less remote or local data aggregation and streaming.

**Daggy** main goals are simplicity and ease-of-use.

**Daggy** is server-less, cross-platform solution and don't require installation on remote servers. Aggregation and streaming work under SSH transport protocol or via local processes execution.

**Daggy** can be helpful for developers, QA, DevOps and engineers for debug, analyze and control distributed network systems, for example, based on micro-service architecture.

# Getting Started

## Getting Daggy

### Windows

Download installation package (.exe) or portable (.zip) from last [release](https://github.com/synacker/daggy/releases/) page and install it

### MacOS

```bash
brew install https://raw.githubusercontent.com/synacker/daggy_homebrew/master/daggy.rb
```

### Fedora 30/31/32

```bash
sudo dnf install daggy daggy-devel
```

### Linux

Download .rpm/.deb or portable .zip package from last [release](https://github.com/synacker/daggy/releases/) page and install it

_All daggy dependencies (Qt5, libssh2, libyaml-cpp) are included into separate dir and does not effect system applications._

### Build from source

#### Environment requirenments

[Conan](https://conan.io/), [cmake](https://cmake.org/), [git](https://git-scm.com/) and C++17 compiler.

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

#### Create simple.yaml

```yaml
sources:
    localhost:
        type: local
        commands:
            pingYa:
                exec: ping ya.ru
                extension: log
```

#### Run daggy
```bash
daggy simple.yaml
```

#### Check console output

```text
23:07:23:977 | AppStat  | Start aggregation in 01-04-20_23-07-23-977_simple
23:07:23:977 | ProvStat | localhost       | New state: Started
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Starting
23:07:23:977 | CommStat | localhost       | pingYa          | New state: Started
```

_There are all commands from **simple.yaml/simple.json** are streams in **01-04-20\_23-07-23-977\_simple** with output files_

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

_Type **CTRL+C** for stopping data aggregation and streaming. Type **CTRL+C** twice for hard stop application, without waiting cancelation of child local and remote processes._

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
        user: {{env_USER}}
        passphrase: {{env_PASSWORD}}
            
sources:
    localhost:
        type: local
        commands: *my_commands
    remotehost:
        host: 192.168.1.9
        type: ssh2
        parameters: *ssh_auth
        commands: *my_commands
    remotehost2:
        host: 192.168.1.9
        type: ssh2
        parameters: *ssh_auth
        commands: *my_commands
    remotehost3:
        host: 192.168.1.9
        type: ssh2
        parameters: *ssh_auth
        commands: *my_commands
```
