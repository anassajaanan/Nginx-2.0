# **Nginx 2.0**

## **Introduction**

Nginx 2.0 is a cutting-edge, event-driven web server designed with efficiency, scalability, and HTTP protocol compliance at its core. Inspired by the original Nginx architecture, our goal was to create a web server that matches its predecessor in performance, flexibility, and ease of use. Developed through the collaborative efforts of myself and [Tukka](https://github.com/tukka25), Nginx 2.0 embodies our commitment to innovation, offering a robust platform for both static and dynamic web content, optimized for modern web applications and services.

---

## **Table of Contents**


- [Features at a Glance](#feature-deep-dive)
    - [HTTP Protocol Compliance](#http-protocol-compliance)
	- [Custom Parsing Strategy](#custom-parsing-strategy)
	- [Cross-Platform I/O Multiplexing](#cross-platform-io-multiplexing)
	- [Efficient Large Response Handling and Video Streaming](#efficient-large-response-handling-and-video-streaming)
	- [Dynamic Content Execution with CGI Support](#dynamic-content-execution-with-cgi-support)
	- [Configurable Logging Framework](#configurable-logging-framework)
- [Getting Started with Nginx 2.0](#getting-started-with-nginx-20)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
    - [Running Nginx 2.0](#running-Nginx-2.0)
    - [Clean Build](#clean-build)
    - [Advanced Usage](#advanced-usage)
- [Configuration Guide for Nginx 2.0](#configuration-guide-for-nginx-20)
    - [Directive Details](#directive-details)
- [Project Structure](#project-structure)
- [Resources & Further Reading](#resources--further-reading)
- [Special Thanks](#special-thanks)
- [Contributing to Nginx 2.0](#contributing-to-nginx-20)


---

## **Feature Deep Dive**

Our journey in developing Nginx 2.0 has been driven by a commitment to innovation, performance, and the pursuit of excellence in web serving technology. Here, we delve into the core features that embody our ambitions and technical prowess, showcasing the strides we've made in redefining the capabilities of a modern web server.

### **[HTTP Protocol Compliance](#http-protocol-compliance)**

In crafting Nginx 2.0, we prioritized strict adherence to the HTTP/1.1 standard, ensuring our server robustly supports essential HTTP methods such as GET, HEAD, POST, and DELETE. This commitment not only aligns with our goal for broad compatibility but also reflects our dedication to providing a solid, reliable foundation for web serving.

### **[Custom Parsing Strategy](#custom-parsing-strategy)**

The complexity and diversity of web server configurations led us to implement a recursive descent parser, mirroring the hierarchical model seen in Nginx. This strategy enhances configuration management, making it intuitive and manageable while preserving the flexibility needed for complex setups.

### **[Cross-Platform I/O Multiplexing](#cross-platform-io-multiplexing)**

Understanding the diverse environments in which our server operates, we developed a custom abstraction layer for I/O multiplexing that seamlessly integrates with both kqueue (macOS) and epoll (Linux). This cross-platform approach is a testament to our commitment to optimizing performance across different systems, ensuring Nginx 2.0 performs efficiently under various operational conditions.

### **[Efficient Large Response Handling and Video Streaming](#efficient-large-response-handling-and-video-streaming)**

Our focus on efficiency and performance is particularly evident in how Nginx 2.0 handles large responses and video streaming. By supporting Transfer-Encoding: chunked and Range requests, we've optimized the delivery of large content, ensuring minimal resource usage while maintaining smooth, uninterrupted video playback. This feature is a direct outcome of our dedication to enhancing user experiences, addressing common challenges in content delivery with innovative solutions.

### **[Dynamic Content Execution with CGI Support](#dynamic-content-execution-with-cgi-support)**

To extend the server's capabilities beyond serving static content, we integrated comprehensive CGI support into Nginx 2.0. This allows for the execution of external programs for dynamic content generation and form processing, among other tasks. This integration reflects our vision for a versatile web server that can cater to a wide range of web application requirements, offering the flexibility needed for developing interactive, personalized web experiences.

### **[Configurable Logging Framework](#configurable-logging-framework)**

The development of a configurable logging framework within Nginx 2.0 stems from our recognition of the critical role that logging plays in understanding and optimizing server operations. By implementing a system that supports multiple log levels and allows for dynamic configuration of log outputs, we've provided ourselves with a powerful tool for monitoring, debugging, and improving server performance. This framework embodies our commitment to transparency and control, ensuring that we can always keep a pulse on the server's health and efficiency.

---

# **Getting Started with Nginx 2.0**

Welcome to Nginx 2.0, an event-driven web server designed for efficiency, scalability, and compliance with the HTTP/1.1 standard. This guide will walk you through the steps to install and build Nginx 2.0 on your system.

## **Prerequisites**

Before you begin, ensure your system meets the following requirements:

- A Linux or macOS operating system
- The Clang compiler (clang++) installed
- Makefile support

## **Installation**

Nginx 2.0 uses a Makefile for building from source. Follow these steps to clone the repository and build the server:

1. **Clone the Repository**
    
    Start by cloning the Nginx 2.0 repository to your local machine:
    
    ```bash
    git clone https://github.com/anassajaanan/Nginx-2.0
    
    cd nginx-2.0
    ```
    
    
2. **Build the Project**
    
    You can build the project in two configurations: **debug** for development and **release** for production.
    
    - **Debug Build:**
        
        The debug build includes additional debug symbols and is compiled with address and undefined behavior sanitizers (on macOS) or with strong protection and overflow checks (on Linux) for development and testing purposes.
        
        ```bash
        make debug
        ```
        
    - **Release Build:**
        
        The release build is optimized for performance with **`-O3`** optimization, native architecture targeting, and link-time optimization. This is the recommended configuration for deployment.
        
        ```bash
        make prod
        ```
        
3. **Running Nginx 2.0**
    
    To start the server, specify a configuration file path if desired. If no path is provided, the server will use the default configuration located at  **`[conf/nginx.conf]`**
    
    ```bash
    ./webserver [configfile_path]  # For release build
    ```
    
    Replace **`[configfile_path]`** with the path to your configuration file. If omitted, Nginx 2.0 will use the default configuration.
    
	 For a debug build:
    
    ```bash
    ./webserver_debug [configfile_path]  # For debug build
    ```

    

## **Clean Build**

To clean up build artifacts and start fresh, use the **`clean`** or **`fclean`** commands:

- **Clean Objects and Dependencies:**
    
    ```bash
    make clean
    ```
    
- **Full Clean (Including Binaries):**
    
    ```bash
    make fclean
    ```
    

## **Advanced Usage**

- **Valgrind Memory Check:**
    
    For Linux users, run your debug build with Valgrind to check for memory leaks:
    
    ```bash
    make valgrind
    ```
    
    Ensure Valgrind is installed on your system for this to work.



---

# **Configuration Guide for Nginx 2.0**

This section outlines the directives available in Nginx 2.0, their applicable contexts, validation policies, and usage examples. This structured approach ensures a clear understanding of how to configure your web server effectively.

### **Directive Details**

### **`root`**

- **Contexts Allowed:** **`server`**, **`location`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    server {
        root /var/www/html; # Document root
    }
    ```
    

### **`listen`**

- **Contexts Allowed:** **`server`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    server {
        listen 8080; # Server listens on port 8080
    }
    ```
    

### **`autoindex`**

- **Contexts Allowed:** **`server`**, **`location`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    location /images {
        autoindex on; # Enables directory listing
    }
    ```
    

### **`server_name`**

- **Contexts Allowed:** **`server`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    server {
        server_name example.com;
    }
    ```
    

### **`client_max_body_size`**

- **Contexts Allowed:** **`http`**, **`server`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    http {
        client_max_body_size 20M; # Limits request body size
    }
    ```
    

### **`error_page`**

- **Contexts Allowed:** **`http`**, **`server`**, **`location`**
- **Validation Policy:** Supports two or more arguments.
- **Example:**
    
    ```bash
    server {
        error_page 404 /404.html;
    }
    ```
    

### **`try_files`**

- **Contexts Allowed:** **`server`**, **`location`**
- **Validation Policy:** Must be unique within its context, supports two or more arguments. The last argument is treated as a fallback.
- **Example:**
    
    ```bash
    location / {
        try_files $uri $uri/ /index.html;
    }
    ```
    

### **`index`**

- **Contexts Allowed:** **`http`**, **`server`**, **`location`**
- **Validation Policy:** Supports one or more arguments. The server will use the first found file as the index. The last argument is treated as a fallback if it starts with a slash. If no index is found, a directory listing is shown.
- **Example:**
    
    ```bash
    location / {
        index index.html index.htm /fallback;
    }
    ```
    

### **`return`**

- **Contexts Allowed:** **`server`**, **`location`**
- **Validation Policy:** Supports either one argument as a status code to return a predefined status message, or two arguments where the first is the status code and the second is a URL for redirection or text to return as the body. When used for redirection, common status codes are 301 (permanent redirect) or 302 (temporary redirect).
- **Example 1: Returning a Status Code with Text:**
    
    ```bash
    location /gone {

        return 410 "The resource is no longer available";

    }
    ```
    
    This configuration returns a 410 status code with a custom message indicating that the resource is no longer available.
    
- **Example 2: Redirection:**
    
    ```bash
    location /oldpage {

        return 301 http://example.com/newpage;

    }
    ```
    
    This directive redirects requests for **`/oldpage`** to a new URL with a 301 status code, indicating a permanent redirect.
    

### **`limit_except`**

- **Contexts Allowed:** **`location`**
- **Validation Policy:** Must be unique within its context, supports one or more arguments to specify allowed HTTP methods.
- **Example:**
	This directive restricts the allowed methods for the **`/api`** endpoint to GET and POST, denying all other methods.
    
    ```bash
    location /api {
    	limit_except GET POST;
	}
    ```
    

### **`keepalive_timeout`**

- **Contexts Allowed:** **`http`**, **`server`**
- **Validation Policy:** Must be unique within its context.
- **Example:**
    
    ```bash
    server {
        keepalive_timeout 15; # Keep connections alive for 15 seconds
    }
    ```
    

### **`cgi_extension`**

- **Contexts Allowed:** **`server`**
- **Validation Policy:** Must be unique within its context, supports one or more arguments. Specifies the file extensions to be treated as CGI scripts.
- **Example:**
    
    ```bash
    server {
        cgi_extension .cgi .pl .py .sh .extension; # Handle .cgi .pl .py files as CGI scripts
    }
    ```
    

### **Complete Configuration Example**

This comprehensive example demonstrates a server setup with nested contexts and multiple directives, showcasing a realistic configuration for Nginx 2.0.

```bash
http {
    client_max_body_size 20M; # Apply to all servers

	keepalive_timeout 15; # Connection keep-alive timeout

    server {
        listen 8080;
        server_name localhost;

        root /var/www/example;
        index index.html index.htm index.php;

        # Serve static files directly
        location / {
            try_files $uri $uri/ /fallback;
        }

        # Enable directory listing for /images
        location /images {
            autoindex on;
            root /var/www/example;
        }

        # Custom error pages
        error_page 404 /404.html;
        error_page 500 502 /50x.html;

        # API endpoint with method restrictions
        location /api {
            limit_except GET POST DELETE;
        }

        # CGI script execution for specific extensions
        cgi_extension .cgi .pl;

    }
}

```

This guide and example should equip you with the knowledge to configure Nginx 2.0 effectively, ensuring your web server is tailored to your specific requirements and operational contexts.


---

## **Project Structure**

Below is an overview of the Nginx 2.0 project structure, providing insight into the organization of the codebase and the purpose of each directory and key files:

```bash
/web-server-project
├── src                     # Source files
│   ├── config              # Configuration-related classes and files
│   │   ├── BaseConfig.cpp
│   │   ├── BaseConfig.hpp
│   │   ├── LocationConfig.cpp
│   │   ├── LocationConfig.cpp
│   │   ├── MimeTypeConfig.cpp
│   │   ├── MimeTypeConfig.hpp
│   │   ├── ReturnDirective.cpp
│   │   ├── ReturnDirective.hpp
│   │   ├── ServerConfig.cpp
│   │   ├── ServerConfig.hpp
│   │   ├── TryFilesDirective.cpp
│   │   └── TryFilesDirective.hpp
│   │
│   ├── cgi                 # CGI handling classes
│   │   ├── CgiDirective.hpp
│   │   ├── CgiDirective.cpp
│   │   ├── CgiHandler.hpp
│   │   └── CgiHandler.cpp
│   │
│   ├── http                # HTTP protocol handling classes
│   │   ├── HttpRequest.hpp
│   │   ├── HttpRequest.cpp
│   │   ├── HttpResponse.hpp
│   │   ├── HttpResponse.cpp
│   │   ├── HttpRequest.cpp
│   │   ├── RequestHandler.hpp
│   │   └── RequestHandler.cpp
│   │
│   ├── logging             # Logging functionality
│   │   ├── Logger.hpp
│   │   └── Logger.cpp
│   │
│   ├── parsing             # Dedicated parsing logic
│   │   ├── ConfigLoader.cpp
│   │   ├── ConfigLoader.hpp
│   │   ├── ConfigNode.cpp
│   │   ├── ConfigNode.hpp
│   │   ├── ConfigParser.cpp
│   │   ├── ConfigParser.hpp
│   │   ├── ConfigTokenizer.cpp
│   │   ├── ConfigTokenizer.hpp
│   │   ├── ContextNode.cpp
│   │   ├── ContextNode.hpp
│   │   ├── DirectiveNode.cpp
│   │   ├── DirectiveNode.hpp
│   │   ├── LogicValidator.cpp
│   │   ├── LogicValidator.hpp
│   │   ├── MimeTypeParser.cpp
│   │   ├── MimeTypeParser.hpp
│   │   ├── SyntaxValidator.cpp
│   │   ├── SyntaxValidator.hpp
│   │   ├── TreeBuilder.cpp
│   │   └── TreeBuilder.hpp
│   │
│   ├── event_polling       # Abstraction over kqueue and Epoll
│   │   ├── EpollManager.cpp
│   │   ├── EpollManager.hpp
│   │   ├── EventPoller.cpp
│   │   ├── EventPoller.hpp
│   │   ├── KqueueManager.cpp
│   │   └── KqueueManager.hpp
│   │
│   ├── server              # Core server functionality
│   │   ├── ClientState.cpp
│   │   ├── ClientState.hpp
│   │   ├── ResponseState.cpp
│   │   ├── ResponseState.hpp
│   │   ├── Server.cpp
│   │   ├── Server.hpp
│   │   ├── ServerManager.cpp
│   │   └── ServerManager.hpp
│   │
│   └── main.cpp            # Entry point of the application
│
├── conf                    # Configuration files (e.g., nginx.conf, mime.types)
├── content                 # Static content served by the server
├── logs                    # Log files generated by the server
├── uploads                 # Directory for handling uploaded files
└── Makefile                # Build instructions for your project

```

This structure is designed to enhance maintainability and scalability, ensuring that anyone can easily navigate and contribute to the project.

---

## **Resources & Further Reading**

To aid in further exploration and mastery of web server development, networking, and programming concepts, we recommend the following curated list of resources:

### **Web Server Development and Networking**

- [Building Web: Sockets and Servers for Dummies](https://levelup.gitconnected.com/building-the-web-sockets-and-servers-for-dummies-886d1595a4f8)
- [Socket Programming in C/C++](https://www.geeksforgeeks.org/socket-programming-cc/)
- [Set Socket Nonblocking](https://jameshfisher.com/2017/04/05/set_socket_nonblocking/)
- [A Simple HTTP Server from Scratch](https://trungams.github.io/2020-08-23-a-simple-http-server-from-scratch/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/#client-server-background)

### **HTTP and HTTPS**

- [The HTTP/3 Protocol (RFC 9112)](https://www.rfc-editor.org/rfc/rfc9112.html)
- [HTTP Headers for Dummies](https://code.tutsplus.com/http-headers-for-dummies--net-8039)
- [HTTP Status Codes Explained](https://www.hostinger.com/tutorials/http-status-codes)

### **Nginx Specific**

- [Understanding the Nginx Configuration File Structure and Configuration Contexts](https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts#the-core-contexts)
- [Understanding Nginx Server and Location Block Selection Algorithms](https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms)
- [Nginx Documentation - Core Module](http://nginx.org/en/docs/http/ngx_http_core_module.html)

### **Programming and Asynchronous Development**

- [libhttpserver for Creating HTTP Servers](https://github.com/etr/libhttpserver)
- [Go HTTP Programming](https://fideloper.com/go-http)

### **Video Lectures**

- [Introduction to Networking | TCP IP & Socket Programming](https://www.youtube.com/watch?v=2-hmRHmsVCM) by Abdelaziz Eroui, part of The Missing Semester series.
- [Missing Semester 0x01 | Introduction to Networking | Introduction to Async Programming](https://www.youtube.com/watch?v=gEmiqMWUbg8) by Mehdi Cheracher, offering insights into asynchronous programming and networking.

### **Additional Learning Resources**

- [Understanding WebSockets and Servers (YouTube Playlist)](https://www.youtube.com/watch?v=bqj4dWG7v3c&list=PLhnN2F9NiVmAMn9iGB_Rtjs3aGef3GpSm)
- [Analysis of High-Performance Web Server Architectures (Academic Paper)](https://arrow.tudublin.ie/cgi/viewcontent.cgi?article=1255&context=scschcomdis)
- [The C10k Problem](https://en.wikipedia.org/wiki/C10k_problem)

## **Special Thanks**

Special thanks are extended to Abdelaziz Eroui for his informative lecture on TCP/IP and socket programming, part of The Missing Semester series, which provided deep insights into the fundamentals of networking critical to our project's success.

We would also like to express our gratitude to Mehdi Cheracher for his lecture on networking and asynchronous programming. His teachings have been instrumental in guiding our approach to handling network communications efficiently.

Their contributions to the field and dedication to education have been invaluable to both our project and the wider community.

## **Contributing to Nginx 2.0**

We warmly welcome contributions from the community and are thrilled to have you join us in improving Nginx 2.0! Whether you're fixing bugs, adding new features, or improving documentation, your contributions are invaluable to making Nginx 2.0 better for everyone.

### **Seeking Help?**

If you have any questions or need assistance, feel free to reach out by opening an issue. We're here to help and look forward to your contributions!