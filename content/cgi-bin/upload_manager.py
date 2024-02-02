#!/usr/bin/env python3
import os
import sys
import re

def parse_headers(raw_headers):
    headers = {}
    for line in raw_headers.split('\r\n'):
        if ': ' in line:
            key, value = line.split(': ', 1)
            headers[key.lower()] = value.strip()
    return headers

def parse_multipart_form_data(stdin, boundary, buffer_size=1000000):
    # Read and accumulate chunks until we get the headers
    accumulated_chunks = bytearray()
    while True:
        chunk = stdin.buffer.read(buffer_size)
        accumulated_chunks.extend(chunk)
        if b'\r\n\r\n' in accumulated_chunks:
            break

    # Extract headers
    headers_part, rest_of_data = accumulated_chunks.split(b'\r\n\r\n', 1)
    raw_headers = headers_part.decode('utf-8')
    headers = parse_headers(raw_headers)

    # Extract filename
    content_disposition = headers.get('content-disposition', '')
    filename = re.findall('filename="([^"]+)"', content_disposition)
    if not filename:
        return "<h2>Filename not found in headers.</h2>"
    filename = filename[0]

    # Find the start of the file data
    file_start = rest_of_data.find(boundary.encode()) - 4  # -4 to account for \r\n before the boundary
    file_data = rest_of_data[:file_start] if file_start > 0 else rest_of_data

    # Write file data
    filepath = f"/Users/aajaanan/Desktop/Ngi/uploads/{filename}"
    with open(filepath, 'wb') as f:
        f.write(file_data)
        while True:
            chunk = stdin.buffer.read(buffer_size)
            if not chunk:
                break
            end_of_file_data = chunk.find(boundary.encode())
            if end_of_file_data >= 0:
                f.write(chunk[:end_of_file_data - 4])  # -4 to remove \r\n-- before boundary
                break
            f.write(chunk)

    return f"<h2>File '{filename}' uploaded successfully.</h2>"

def main():
    content_type = os.environ.get('CONTENT_TYPE', '')
    boundary = content_type.split('boundary=')[-1]
    if not boundary:
        print("Content-Type: text/plain\n")
        print("No boundary found in Content-Type header.")
        return

    result = parse_multipart_form_data(sys.stdin, boundary)
    # print("Content-Type: text/plain\n")
    print(result)

if __name__ == "__main__":
    main()
