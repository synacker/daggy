---
description: Getting Started aggregate data via command line
---

# Getting Started

## Getting Daggy

### Fedora 29/30/31

```bash
$ sudo dnf install daggy
```

### MacOS

```bash
brew install https://raw.githubusercontent.com/synacker/homebrew-core/daggy/Formula/daggy.rb
```

### Windows 7/8/10

Download and install from [releases](https://github.com/synacker/daggy/releases)

### Build from sources

#### Dependency

* Qt Core and Qt Network 5.5 or newer
* libbotan-devel 2.7 or newer
* yaml-cpp 0.6.0 or newer

#### Build instractions

```bash
git clone https://github.com/synacker/daggy.git
cd daggy
qmake && make
cd Release
daggy -h
```

## Configuration

### Simple data sources config

{% code-tabs %}
{% code-tabs-item title="simple.yaml" %}
```yaml
sources:
  localhost:
    type: local
    commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
```
{% endcode-tabs-item %}
{% endcode-tabs %}

{% file src=".gitbook/assets/simple.yaml" %}

```bash
daggy simple.yaml
23:15:41:005 | AppStatus  | Application     | Start receiver
23:15:41:005 | ConnStatus | localhost       | Server connected
23:15:41:006 | CommStatus | localhost       | pingYa          | Command started
```

In your working directory **daggy** were created directory with next template:

`{date}-{source_name}`

```bash
ls
12-06-19_23-15-41-simple  simple.yaml
```

Output folder contain file with `pingYa` command output with next template:

`{server_name}_{command_name}.{command_extension}`

```bash
ls 12-06-19_23-15-41-simple/
localhost_pingYa.log
```

The output file filling at runtime, you can tail content of the file:

```bash
tail -f 12-06-19_23-15-41-simple/localhost_pingYa.log 
```

Type CTRL+C for stop commands execution

```bash
23:16:25:685 | CommStatus | localhost       | pingYa          | Command was crashed
23:16:25:686 | ConnStatus | localhost       | Server disconnected
23:16:25:686 | AppStatus  | Application     | Stop receiver
```

### Add parallel command

Add new command to our simple config - `pingGoo`

{% code-tabs %}
{% code-tabs-item title="simple\_add\_command.yaml" %}
```yaml
sources:
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
{% endcode-tabs-item %}
{% endcode-tabs %}

{% file src=".gitbook/assets/simple\_add\_command.yaml" %}

```bash
daggy simple_add_command.yaml 
21:38:02:154 | AppStatus  | Application     | Start receiver
21:38:02:154 | ConnStatus | localhost       | Server connected
21:38:02:154 | CommStatus | localhost       | pingGoo         | Command started
21:38:02:155 | CommStatus | localhost       | pingYa          | Command started

```

`pingYa` and `pingGoo` work in parallel, and both commands filling output files  `localhost_pingGoo.log localhost_pingYa.log` at runtime.

You can tail all output files while **daggy** is working:

```bash
tail 14-06-19_21-38-02-simple_add_command/*.log
```

### Add remote server

Now, add remote server `192.168.1.9` with ssh connection with the same commands:

{% code-tabs %}
{% code-tabs-item title="simple\_add\_remote\_server.yaml" %}
```yaml
sources:
  localhost:
    type: local
    commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
      - name: pingGoo
        command: ping goo.gl
        extension: log
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
{% endcode-tabs-item %}
{% endcode-tabs %}

{% file src=".gitbook/assets/simple\_add\_remote\_server.yaml" %}

{% code-tabs %}
{% code-tabs-item title="simple\_add\_remote\_server.yaml" %}
```yaml
daggy simple_add_remote_server.yaml 
22:08:26:093 | AppStatus  | Application     | Start receiver
22:08:26:093 | ConnStatus | localhost       | Server connected
22:08:26:093 | CommStatus | localhost       | pingGoo         | Command started
22:08:26:094 | CommStatus | localhost       | pingYa          | Command started
22:08:26:131 | ConnStatus | remotehost      | Server connected
22:08:26:131 | CommStatus | remotehost      | pingGoo         | Command started
22:08:26:131 | CommStatus | remotehost      | pingYa          | Command started

```
{% endcode-tabs-item %}
{% endcode-tabs %}

`pingYa` and `pingGoo` work in parallel on both servers - localhost and remotehost.

You can tail all output files while **daggy** is working:

```bash
tail -f 14-06-19_22-11-44-simple_add_remote_server/*.log
```

### Grouping commands by name

`pingYa` and `pingGoo` are duplicated in last config. Next step, group this commands by name:

{% code-tabs %}
{% code-tabs-item title="simple\_group\_commands.yaml" %}
```yaml
aliases:
  - &pingYa
    name: pingYa
    command: ping ya.ru
    extension: log
  - &pingGoo
    name: pingGoo
    command: ping goo.gl
    extension: log

sources:
  localhost:
    type: local
    commands:
      - *pingYa
      - *pingGoo
  remotehost:
    type: ssh
    host: 192.168.1.9
    connection:
      login: muxa
      key: /home/muxa/.ssh/id_rsa
    commands:
      - *pingYa
      - *pingGoo
```
{% endcode-tabs-item %}
{% endcode-tabs %}

{% file src=".gitbook/assets/simple\_group\_commands.yaml" %}

```bash
daggy simple_group_commands.yaml 
22:19:18:690 | AppStatus  | Application     | Start receiver
22:19:18:691 | ConnStatus | localhost       | Server connected
22:19:18:691 | CommStatus | localhost       | pingGoo         | Command started
22:19:18:692 | CommStatus | localhost       | pingYa          | Command started
22:19:18:723 | ConnStatus | remotehost      | Server connected
22:19:18:723 | CommStatus | remotehost      | pingGoo         | Command started
22:19:18:723 | CommStatus | remotehost      | pingYa          | Command started

```

###  Add multiple servers

{% code-tabs %}
{% code-tabs-item title="simple\_multiple\_servers.yaml" %}
```yaml
aliases:
  - &pingYa
    name: pingYa
    command: ping ya.ru
    extension: log

  - &pingGoo
    name: pingGoo
    command: ping goo.gl
    extension: log

  - &my_commands
    - *pingYa
    - *pingGoo

  - &ssh_auth
    login: muxa
    key: /home/muxa/.ssh/id_rsa
    
sources:
  localhost:
    type: local
    commands:
      *my_commands
  remotehost:
    host: 192.168.1.9
    type: ssh
    connection:
      *ssh_auth
    commands:
      *my_commands
  remotehost2:
    host: 192.168.1.10
    type: ssh
    connection:
      *ssh_auth
    commands:
      *my_commands
  remotehost3:
    host: 192.168.1.11
    type: ssh
    connection:
      *ssh_auth
    commands:
      *my_commands
```
{% endcode-tabs-item %}
{% endcode-tabs %}

{% file src=".gitbook/assets/simple\_multiple\_servers.yaml" %}

```text
daggy simple_multiple_servers.yaml 
23:24:25:118 | AppStatus  | Application     | Start receiver
23:24:25:119 | ConnStatus | localhost       | Server connected
23:24:25:119 | CommStatus | localhost       | pingGoo         | Command started
23:24:25:120 | CommStatus | localhost       | pingYa          | Command started
23:24:25:156 | ConnStatus | remotehost2     | Server connected
23:24:25:156 | CommStatus | remotehost2     | pingGoo         | Command started
23:24:25:156 | CommStatus | remotehost2     | pingYa          | Command started
23:24:25:156 | ConnStatus | remotehost3     | Server connected
23:24:25:156 | CommStatus | remotehost3     | pingGoo         | Command started
23:24:25:156 | CommStatus | remotehost3     | pingYa          | Command started
23:24:25:156 | ConnStatus | remotehost      | Server connected
23:24:25:156 | CommStatus | remotehost      | pingGoo         | Command started
23:24:25:157 | CommStatus | remotehost      | pingYa          | Command started

```

