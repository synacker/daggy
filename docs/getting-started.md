# Getting Started

## Getting Daggy

### For Fedora 28/29/30

```bash
$ sudo dnf install daggy
```

### For Windows 7/8/10

Download and install from [releases](https://github.com/synacker/daggy/releases)

### Build from sources for other systems

#### Dependency

* Qt Core and Qt Network 5.5 or newer
* libbotan-devel 2.7 or newer
* yaml-cpp 0.6.0 or newer

#### Build instractions

```bash
git clone https://github.com/synacker/daggy.git
cd daggy
make
cd Release
daggy -h
```

### First simple config

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

```bash
daggy simple.yaml
23:15:41:005 | AppStatus  | Application     | Start recivier
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
23:16:25:686 | AppStatus  | Application     | Srop recivier
```

