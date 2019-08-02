---
description: Data aggregation via command line snippets
---

# Data Aggregation Snippets

## About Data Aggregation Snippets

Data Aggregation Snippet - tempate that allows aggregate data via command line

## Useful Data Aggregation Snippets

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



