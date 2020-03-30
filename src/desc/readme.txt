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

Data aggregation sources example:
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
