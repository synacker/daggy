---
description: How to setup data sources for Daggy
---

# Data Aggregation Sources

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

{% tab title="JSON" %}
```javascript
{
    "sources": {
        "host1" : {
        },
        "host2" : {
        },
        "hostN" : {
        }
    }
}
```
{% endtab %}
{% endtabs %}

Each config conatins map of hosts. Host \(data source\) parameters is next:

<table>
  <thead>
    <tr>
      <th style="text-align:center">Parameter</th>
      <th style="text-align:center">Type</th>
      <th style="text-align:left">Description</th>
      <th style="text-align:left">Is Requiered</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td style="text-align:center"><b>type</b>
      </td>
      <td style="text-align:center">string</td>
      <td style="text-align:left">
        <p></p>
        <p>Type of connection to host. <b>Daggy</b> supportes <code>local</code> and <code>ssh</code> host
          connection types</p>
      </td>
      <td style="text-align:left">Yes</td>
    </tr>
    <tr>
      <td style="text-align:center"><b>commands</b>
      </td>
      <td style="text-align:center">array</td>
      <td style="text-align:left">Array of commands for simultaneous launch</td>
      <td style="text-align:left">Yes</td>
    </tr>
    <tr>
      <td style="text-align:center"><b>connection</b>
      </td>
      <td style="text-align:center">map</td>
      <td style="text-align:left">Connection parameters</td>
      <td style="text-align:left">Required for ssh2 <b>type</b>
      </td>
    </tr>
    <tr>
      <td style="text-align:center"><b>host</b>
      </td>
      <td style="text-align:center">string</td>
      <td style="text-align:left">Host address (ip or url)</td>
      <td style="text-align:left">Required for ssh2 <b>type</b>
      </td>
    </tr>
    <tr>
      <td style="text-align:center"><b>reconnect</b>
      </td>
      <td style="text-align:center">boolean</td>
      <td style="text-align:left">true, if need reconnect connection</td>
      <td style="text-align:left">No</td>
    </tr>
  </tbody>
</table>## Data Sources Types

**Daggy** supportes `local` and `ssh2` \(remote\) host connection types.

### Local type

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

{% tab title="JSON" %}
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
{% endtab %}
{% endtabs %}

### SSH2 type

{% tabs %}
{% tab title="YAML" %}
```yaml
remotehost:
    type: ssh2
    host: 192.168.1.9
    restart: false
    connection:
      user: muxa
      key: /home/muxa/.ssh/id_rsa
    commands:
      pingYa:
        command: ping ya.ru
        extension: log
```
{% endtab %}

{% tab title="JSON" %}


```javascript
{
    "localhost": {
        "type": "ssh",
        "host": "192.169.1.9",
        "restart": false,
        "connection": {
            "login": "muxa",
            "key": "/home/muxa/.ssh/id_rsa"
        },
        "commands": {
            "pingYa": {
                "command": "ping ya.ru",
                "extension": "log"
            }
        }
    }
}
```
{% endtab %}
{% endtabs %}

#### SSH type additional parameters

* **host** - remote host ip address or url
* **connection** - map of ssh connection parameters. Includes next parameters

| Connection Parameter | Type | Description | Default value |
| :---: | :---: | :--- | :--- |
| **user** | string | _ssh2_ connection login | Current user |
| **passphrase** | string | _ssh2_ connection password. If field is not empty, then will be used password authentication for _ssh2_ connection. In other case, will be used public key authentication  |  |
| **key** | string | path to private key for _ssh2_ connection | `~/.ssh/id_rsa` |
| **keyphrase** | string | passphrase for private key file |  |
| **port** | integer | _ssh2_ connection port | 22 |
| **timeout** | integer | limit to establish _ssh2_ connection, in milliseconds | 1000 |

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

{% tab title="JSON" %}
```javascript
"commands": {
            "pingYa": {
                "command": "ping ya.ru",
                "extension": "log"
            }
        }
```
{% endtab %}
{% endtabs %}

Each command must contain:

* **name -** unique within host command identifier, using in **command output file** name template.
* **exec** - shell script
* **extension** - extension for **command output file**
* **restart** - restart command if it finished

  

