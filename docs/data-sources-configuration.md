---
description: How to setup data sources for Daggy
---

# Data Sources Configuration

## Supported formats

**Daggy** supports `yaml` and `json` formats. In yaml repeated nodes can be grouped by [yaml standard](https://yaml.org/spec/1.2/spec.html#id2785586).

## Minimal valid config

{% tabs %}
{% tab title="YAML" %}
```yaml
sources:
    
```
{% endtab %}

{% tab title="JSON" %}
```javascript
{
}
```
{% endtab %}
{% endtabs %}

## Config parameters

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
| :--- | :--- | :--- | :--- |
| login | string | ssh connection login | **daggy** user |
| password | string | ssh connection password. If empty, **daggy** will expect **key** field |  |
| key | string | path to private key for ssh connection | `~/.ssh/id_rsa` |
| port | integer | ssh connection port | 22 |
| timeout | integer |  limit to establish ssh connection, in seconds | 2 |
| ignoreProxy | boolean | if true, daggy will ignore default proxy | true |
| strictConformance | boolean | if true, enable ssh protocol compatibility | true |
| forceKill | integer | kill signal for remote process before connection close. If -1 no signals will be send  | 15 \(SIGTERM\) |

### Commands

{% tabs %}
{% tab title="YAML" %}
```yaml
commands:
      - name: pingYa
        command: ping ya.ru
        extension: log
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

  

