# clitocybe
---

---
# Usage
- Creating a tcp server binding to all interfaces at port 8080. SListen() abstract socket(), bind(), listen()
```c
listener_t ln = SListen("tcp", ":8080");
if (ln.err != 0) {
    // error
}
```

---
HTTP/1.1 web server
- Thread Pool
- Socket Programming
- Keep-Alive Connection handle
- HTTP request parser
- Static file serving
- Routing
- Logger
- Error handling