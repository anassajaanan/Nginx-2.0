# **Nginx 2.0**

## **Introduction**

Nginx 2.0 is a cutting-edge, event-driven web server designed with efficiency, scalability, and HTTP protocol compliance at its core. Inspired by the original Nginx architecture, our goal was to create a web server that not only matches but surpasses its predecessor in performance, flexibility, and ease of use. Nginx 2.0 embodies our commitment to innovation, offering a robust platform for both static and dynamic web content, optimized for modern web applications and services.

---

# **Features at a Glance**

- [HTTP Protocol Compliance](#http-protocol-compliance)
- [Custom Parsing Strategy](#custom-parsing-strategy)
- [Cross-Platform I/O Multiplexing](#cross-platform-io-multiplexing)
- [Efficient Large Response Handling and Video Streaming](#efficient-large-response-handling-and-video-streaming)
- [Dynamic Content Execution with CGI Support](#dynamic-content-execution-with-cgi-support)
- [Configurable Logging Framework](#configurable-logging-framework)

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
    
    After building, you can start the server with the default configuration or specify a configuration file in conf directory:

    ```bash
    ./webserver_release  # For release build
    ```
    
    Or for a debug build:
    
    ```bash
    ./webserver  # For debug build
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
        index index.html index.htm;
    }
    ```
    

### **`return`**

- **Contexts Allowed:** **`server`**, **`location`**
- **Validation Policy:** Must be unique within its context, supports one or two arguments for redirection or setting response codes.
- **Example:**
    
    ```bash
    location =/oldpage {
        return 301 http://example.com/newpage;
    }
    ```
    

### **`limit_except`**

- **Contexts Allowed:** **`location`**
- **Validation Policy:** Must be unique within its context, supports one or more arguments. It restricts the allowed HTTP methods for a location.
- **Example:**
    
    ```bash
    location /api {
        limit_except GET POST PUT HEAD;
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

    server {
        listen 80;
        server_name example.com;

        root /var/www/example;
        index index.html index.htm;

        # Serve static files directly
        location / {
            try_files $uri $uri/ /index.html;
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

        keepalive_timeout 15; # Connection keep-alive timeout
    }
}

```

This guide and example should equip you with the knowledge to configure Nginx 2.0 effectively, ensuring your web server is tailored to your specific requirements and operational contexts.