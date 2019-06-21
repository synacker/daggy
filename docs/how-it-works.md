---
description: How Daggy works
---

# How it works

## Work schema

![Daggy work schema](.gitbook/assets/daggy-scheme.svg)

## Work schema description

### **Set Data Sources.** 

**Daggy** accept data sources as argument:

```bash
daggy -h
Usage: daggy [options] sourceFile
Daggy - Data Aggregation Utility. Application that can run multiple commands on remote servers simultaneously and save output locally.

Options:
  -v, --version          Displays version information.
  -o, --output <folder>  Set output folder
  -f, --format <format>  Source format
  -h, --help             Displays this help.

Arguments:
  sources                Data sources
```

Supported data sources formats are `yaml` and `json`.

If **sourceFile** is not exists in current working directory, **daggy** will load it from `~/.daggy` directory.

### Create Output Folder

By default **daggy** creates output folder with next template - `current_date-sourcefilenname` in `PWD` folder, but you can change output folder via **daggy** option:

```bash
 -o, --output <folder>  Set output folder
```

#### Process control

Each command, taken from **local type** specification runing and controling by **daggy** application such as separate process in localhost:

```yaml
localhost:
      type: local
      commands:
        - name: pingYa
          command: ping ya.ru
          extension: log
        - name: pingGoo
          command: ping goo.gl
          extension: log
```

### SSH Connection

**Daggy** establish ssh connection with each remote server, specified in config:

```yaml
type: ssh
```

SSH connection parameters taken from the `connection` field:

```yaml
connection:
      login: muxa
      key: /home/muxa/.ssh/id_rsa
```

SSH host taken from `host` field:

```yaml
host: 192.168.1.9
```

Each command running by **daggy** in ssh channel

```yaml
remotehost:
    type: ssh
    host: 192.168.1.9
    connection:
      login: muxa
      key: /home/muxa/.ssh/id_rsa
    commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
      - name: pingGoo
        command: ping goo.gl
        extension: log
```

### Streaming commands output

Each command, runing remotely or locally, streams its standart output to a separate file in output folder - **command output file**, that **daggy** created at start. Filename for each command forming by next template:

```text
hostname_commandname.extension
```

Hostname is a key of data sources set and extension is extension parameter from data source. For example, in next data source config

```yaml
sources:
  localhost:
    type: local
    commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
```

command `pingYa` will streams own standard output to `localhost_pingYa.log` file

### Command completion

Type `CTRL+C` for interrupt commands execution. If command is not stopped before, SIGTERM signal will be send for each command. 

Type `CTRL+C` twice for interrupt ssh connection without SIGTERM signal.

Daggy quits after the last command is completed.

