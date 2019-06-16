---
description: Daggy troubleshoting
---

# Troubleshoting

## OpenSSH key format

If you use key files for remote access to servers, and your key started from next line:

```text
-----BEGIN OPENSSH PRIVATE KEY-----
```

This key is not supported in **daggy**, convert your key to rsa format:

```bash
ssh-keygen -p -m PEM -f ~/.ssh/id_rsa
```

### Slow ssh connection

If you have very slow ssh connection, **daggy** can interrupt ssh connection. For avoiding this, set timeout in connection parameters \(in seconds\) in **data sources file**:

```yaml
connection:
    timeout: 10
```

### SSH protocol incompatibility

If **daggy** cannot connect to remote server by ssh try to disable conformance check:

```yaml
connection:
    strictConformance: false
```

### Daggy does not use default proxy for ssh connection

Set `ignoreProxy` connection parameter to false:

```yaml
connection:
    ignoreProxy: false
```



