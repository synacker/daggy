---
description: How to setup data sources for Daggy
---

# Data Aggregation Config

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
    "host1" : {
    },
    "host2" : {
    },
    "hostN" : {
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
      <td style="text-align:left">Required for ssh <b>type</b>
      </td>
    </tr>
    <tr>
      <td style="text-align:center"><b>host</b>
      </td>
      <td style="text-align:center">string</td>
      <td style="text-align:left">Host address (ip or url)</td>
      <td style="text-align:left">Required for ssh <b>type</b>
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
</table>## Data sources types

**Daggy** supportes `local` and `ssh` host connection types.

### Local type

{% tabs %}
{% tab title="YAML" %}
```yaml
sources:
  localhost:
    type: local
    commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
        restart: false
```
{% endtab %}

{% tab title="JSON" %}
```javascript
  {
  "localhost": {
    "type": "local",
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

### SSH type

{% tabs %}
{% tab title="YAML" %}
```yaml
remotehost:
    type: ssh
    host: 192.168.1.9
    restart: false
    connection:
      login: muxa
      key: /home/muxa/.ssh/id_rsa
    commands:
      - name: pingYa
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
| **login** | string | ssh connection login | **daggy** user |
| **password** | string | ssh connection password. If empty, **daggy** will expect **key** field |  |
| **key** | string | path to private key for ssh connection | `~/.ssh/id_rsa` |
| **port** | integer | ssh connection port | 22 |
| **timeout** | integer |  limit to establish ssh connection, in seconds | 2 |
| **ignoreProxy** | boolean | if true, daggy will ignore default proxy | true |
| **strictConformance** | boolean | if true, enable ssh protocol compatibility | true |
| **forceKill** | integer | kill signal for remote process before connection close. If -1 no signals will be send  | 15 \(SIGTERM\) |

### Commands

{% tabs %}
{% tab title="YAML" %}
```yaml
commands:
      - name: pingYa
        command: ping ya.ru
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
* **command** - shell script
* **extension** - extension for **command output file**
* **restart** - restart command if it finished

  

