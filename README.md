[![synacker](https://circleci.com/gh/synacker/daggy.svg?style=svg)](https://circleci.com/gh/synacker/daggy)

<p align="center">
  <img width="256" height="256" src="daggy_logo.svg">
  <br/>
  <b>Daggy - Data Aggregation Utility</b>
</p>

**Daggy** main goals are simplicity and ease-of-use. 

**Daggy** is serverless, cross-platform solution and 
don't require installation on remote servers. 
Aggregation and streaming work under SSH transport protocol 
or via local processes execution.

**Daggy** can be helpful for developers, QA, DevOps and 
engenieers for debug, analyze and control 
distributed network systems, for example, 
based on microservice architecture.

[Getting Started](https://docs.daggy.dev/getting-started)

```bash
daggy simple.yaml
```
*simple.yaml:*
```yaml
aliases:
  - &pingYa
    pingYa:
        exec: ping ya.ru
        extension: log

  - &pingGoo
    pingGoo:
        exec: ping goo.gl
        extension: log

  - &my_commands
    - *pingYa
    - *pingGoo

  - &ssh_auth
    user: muxa
    key: /home/muxa/.ssh/id_rsa

sources:
  localhost:
    type: local
    commands:
      *my_commands
  remotehost:
    host: 192.168.1.9
    type: ssh2
    parameters:
      *ssh_auth
    commands:
      *my_commands
  remotehost2:
    host: 192.168.1.9
    type: ssh2
    parameters:
      *ssh_auth
    commands:
      *my_commands
  remotehost3:
    host: 192.168.1.9
    type: ssh2
    parameters:
      *ssh_auth
    commands:
      *my_commands
```


```txt
daggy [options] *.yaml|*.yml|*.json

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
