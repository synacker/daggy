---
description: Useful command snippets
---

# Command snippets

### Dump entire network traffic for remote host

```yaml
name: entireNetwork
command: tcpdump -i any -s 0 port not 22 -w -
extension: pcap
```

### Journald logs streaming

```yaml
name: journaldLog
command: journalctl -f
extension: log
```



