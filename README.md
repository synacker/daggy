<p align="center">
  <img width="256" height="256" src="daggy_logo.svg">
  <br/>
  <b>Daggy - Data Aggregation Utility</b>
</p>

# Short description #
Daggy - application that can run multiple commands on remote servers simultaneously and save output locally.

## Simple to use ##
Configure commands and servers via json/yaml config.
Run command via ssh/bash/powershell 

## Simple to support ##
No installations are required on remote servers.
Only ssh connection are required for remote servers.

## Simple to search and read ##
Each command output saving in separate file at runtime.

# Getting started #

## Download and install ##
See [releases](https://github.com/synacker/daggy/releases) and get installation package for your OS (Windows, Fedora, Ubuntu, Debian, MacOS).

## Or Compile from source ##

### Dependencies ###
1. Qt Core and Qt Network >= 5.5
2. libbotan >= 2.7
3. yamp-cpp >= 0.6.0

### Compilation steps ###
1. `git clone https://github.com/milovidov/dataagregator.git`
2. `cd dataagregatorproject`
3. `qmake`
4. `make` or `nmake`
5. `cd Release`
6. `DataAgregator -h`

# Servers commands configuration #

Let's see the next example, test.yaml:

```yaml
aliases:
    - &pingYa
      name: pingYa
      command: ping -n 5 ya.ru
      extension: log
      restart: false
    - &pingYaInfinite
      name: journalLast
      command: ping ya.ru
      extension: log
      restart: false

sources:
    linux_localhost:
        type: local
        commands:
            - *pingYa
            - *pingYaInfinite
    linux_remote:
        type: ssh
        host: 127.0.0.1
        authorization:
            login: muxa
            key: /home/muxa/.ssh/id_rsa
        commands:
            - *pingYa
            - *pingYaInfinite
```

`daggy test.yaml`

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
