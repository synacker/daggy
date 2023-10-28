---
description: How Daggy aggregate data via command line
---

# How it works

## Work schema

![](.gitbook/assets/daggy_scheme_white.svg)

## Work schema description

### **Set Data Sources**

**Daggy** accept **Data Aggregation Config** as argument or from _stdin:_

```bash
daggy -h
Usage: ./daggy [options] sourceFile
Daggy - Data Aggregation Utility. Application that can run multiple commands on remote servers simultaneously and save output locally.

Options:
  -v, --version          Displays version information.
  -o, --output <folder>  Set output folder
  -f, --format <format>  Source format
  -i, --stdin            Read data sources from stdin
  -h, --help             Displays this help.

Arguments:
  sourceFile             Data sources
```

Supported data aggregation config formats are `yaml` and `json`.

#### Set Data Aggregation Config from file

```bash
daggy simple.yaml
```

If **sourceFile** \(for example simple.yaml\) is not exists in current working directory, **daggy** will load it from `~/.daggy` directory.

#### From Data Aggregation Config from stdin

```bash
cat simple.yaml | daggy -f yaml -i
```

## Data Aggregation Session

At start **Daggy** create **Data Aggregation Session** that include:

1. Create **Data Aggregation Storage**
2. Run **Data Aggregation Snippets** from **Data Aggregation Config** by **processes execution** or via **ssh connection**
3. Write **Data Aggregation Streams** at runtime

### Create Output Folder

By default **daggy** creates output folder - **Data Aggregation Storage** - with next template - `current_date-sourcefilenname` in `PWD` folder, but you can change output folder via **daggy** option:

```bash
 -o, --output <folder>  Set output folder
```

### Processes execution

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

### Streaming Data Aggregation Snippets

Each command, runing remotely or locally, streams its standart output to a separate file in output folder - **Data Aggregation Snippet** output file, that **daggy** created at start. Filename for each command forming by next template:

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

## How to stop Data Aggregation Session

Type `CTRL+C` for interrupt commands execution. If command is not stopped before, SIGTERM signal will be send for each command. 

Type `CTRL+C` twice for interrupt ssh connection without SIGTERM signal.

Daggy quits after the last command is completed.

