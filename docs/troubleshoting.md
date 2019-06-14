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

