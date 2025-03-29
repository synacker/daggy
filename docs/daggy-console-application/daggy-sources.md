---
description: How to setup data sources for Daggy
---

# Daggy Sources

## Supported formats

**Daggy** supports `yaml` and `json` formats. In yaml repeated nodes can be grouped by [yaml standard](https://yaml.org/spec/1.2/spec.html#id2785586).

## Data sources template

{% tabs %}
{% tab title="YAML" %}
```yaml
sources:
    host1:
        #....
    host2:
        #....
    #....
    hostN:
        #....
    
```
{% endtab %}
{% endtabs %}

Each config conatins map of hosts. Host (data source) parameters is next:

|    Parameter   |   Type  | Description                                                                             | Is Requiered                             |
| :------------: | :-----: | --------------------------------------------------------------------------------------- | ---------------------------------------- |
|    **type**    |  string | Type of connection to host. **Daggy** supportes `local` and `ssh` host connection types | Yes                                      |
|  **commands**  |  array  | Array of commands for simultaneous launch                                               | Yes                                      |
| **parameters** |   map   | Connection parameters                                                                   | Required for **ssh2** **type**           |
|    **host**    |  string | Host address                                                                            | No. For **ssh2** is 127.0.0.1 by default |
|  **reconnect** | boolean | true, if need reconnect connection                                                      | No                                       |

## Daggy Sources Types

**Daggy** supportes `local` and `ssh2` (remote) host connection types.

### Local type

Data aggregation via local processes

{% tabs %}
{% tab title="YAML" %}
```yaml
sources:
    localhost:
        type: local
        commands:
            pingYa:
                exec: ping ya.ru
                extension: log
```
{% endtab %}
{% endtabs %}

### SSH type

Data aggregation via ssh process

```yaml
remotehost:
    type: ssh2
    host: 192.168.1.9
    restart: false
    parameters:
      config: ~/.ssh/config
      control: /tmp/ssh-%r@%h:%p
      passphrase: {{env_PASSWORD}}
    commands:
      pingYa:
        command: ping ya.ru
        extension: log
```

#### SSH parameters

* **config**  - ssh config. By default is \~/.ssh/config
* c**ontrol** - ssh master control path. If not setted new master connection wiil created
* **passphrase** - password for ssh connection if needed

### SSH2 type

Data aggregation via ssh2 lib

{% tabs %}
{% tab title="YAML" %}
```yaml
remotehost:
    type: ssh2
    host: 192.168.1.9
    restart: false
    parameters:
      user: muxa
      key: /home/muxa/.ssh/id_rsa
    commands:
      pingYa:
        command: ping ya.ru
        extension: log
```
{% endtab %}
{% endtabs %}

#### SSH2 type additional parameters

* **host** - remote host ip address or url
* **connection** - map of ssh connection parameters. Includes next parameters

| Connection Parameter |   Type  | Description                                                                                                                                                               | Default value   |
| :------------------: | :-----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------- |
|       **user**       |  string | _ssh2_ connection login                                                                                                                                                   | Current user    |
|    **passphrase**    |  string | _ssh2_ connection password. If field is not empty, then will be used password authentication for _ssh2_ connection. In other case, will be used public key authentication |                 |
|        **key**       |  string | path to private key for _ssh2_ connection                                                                                                                                 | `~/.ssh/id_rsa` |
|     **keyphrase**    |  string | passphrase for private key file                                                                                                                                           |                 |
|       **port**       | integer | _ssh2_ connection port                                                                                                                                                    | 22              |
|      **timeout**     | integer | limit to establish _ssh2_ connection, in milliseconds                                                                                                                     | 1000            |

### Commands

{% tabs %}
{% tab title="YAML" %}
```yaml
commands:
      pingYa:
        exec: ping ya.ru
        extension: log
        restart: false
```
{% endtab %}
{% endtabs %}

Each command must contain:

* **name -** unique within host command identifier, using in **command output file** name template.
* **exec** - shell script
* **extension** - extension for **command output file**
* **restart** - restart command if it finished

## Using environment and other variables in **Data Aggregation Sources**

### Mustache syntax

YAML/JSON **Data Aggregation Sources** are support [mustache syntax](https://mustache.github.io):

* _env\_\*_ - template for environment variable, where \* is environment variable name.
* _output\_folder_ - output folder path

### Example of **using** mustache syntax in **Daggy Sources**

{% tabs %}
{% tab title="YAML" %}
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
{% endtab %}
{% endtabs %}
