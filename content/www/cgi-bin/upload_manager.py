#!/usr/bin/env python3
import os
import sys
import re

import cgitb
cgitb.enable()

def parse_headers(raw_headers):
	headers = {}
	for line in raw_headers.split('\r\n'):
		if ': ' in line:
			key, value = line.split(': ', 1)
			headers[key.lower()] = value.strip()
	return headers

def get_content_type(filepath):
	_, ext = os.path.splitext(filepath)
	if ext in ['.jpg', '.jpeg', '.png', '.gif']:
		return 'image'
	elif ext in ['.mp4', '.webm']:
		return 'video'
	return None

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
	filepath = f"content/www/uploads/{filename}"
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

	# return f"<h2>File '{filename}' uploaded successfully.</h2>"
	file_type = get_content_type(filepath)
	# if file_type == 'image':
	#     return f"<h2>Image '{filename}' uploaded successfully.</h2><img src='/content/uploads/{filename}' alt='{filename}' />"
	# elif file_type == 'video':
	#     return f"<h2>Video '{filename}' uploaded successfully.</h2><video controls src='/content/uploads/{filename}'></video>"
	# else:
	#     return f"<h2>File '{filename}' uploaded successfully.</h2>"
			
	responseStr = f"""<!DOCTYPE html>
		<html style="font-size: 16px" lang="en">
		<head>
			<meta name="viewport" content="width=device-width, initial-scale=1.0" />
			<meta charset="utf-8" />
			<meta
			name="keywords"
			content="Design Studio, What we Do, Our Features &amp;amp; Services, ​We turn your design into a live Webflow project."
			/>
			<meta name="description" content="" />
			<title>Web Server Nginx 2.0</title>
			<!-- favicon -->
			<link rel="apple-touch-icon" sizes="180x180" href="../assets/images/favicon/apple-touch-icon.png">
			<link rel="icon" type="image/png" sizes="32x32" href="../assets/images/favicon/favicon-32x32.png">
			<link rel="icon" type="image/png" sizes="16x16" href="../assets/images/favicon/favicon-16x16.png">
			<link rel="manifest" href="../assets/images/favicon/site.webmanifest">
			<link
			rel="stylesheet"
			href="../assets/css/image-colors.css"
			media="screen"
			/>
			<script
			class="u-script"
			type="text/javascript"
			src="../assets/js/jquery.js"
			defer=""
			></script>
			<script
			class="u-script"
			type="text/javascript"
			src="../assets/js/javascript.js"
			defer=""
			></script>
			<meta name="referrer" content="origin" />
			<link
			id="u-theme-google-font"
			rel="stylesheet"
			href="https://fonts.googleapis.com/css?family=Roboto:100,100i,300,300i,400,400i,500,500i,700,700i,900,900i|Open+Sans:300,300i,400,400i,500,500i,600,600i,700,700i,800,800i"
			/>
			<link
			id="u-page-google-font"
			rel="stylesheet"
			href="https://fonts.googleapis.com/css?family=Raleway:100,100i,200,200i,300,300i,400,400i,500,500i,600,600i,700,700i,800,800i,900,900i"
			/>
			<!--    bootstrap-->
			<!-- CSS only -->
			<link
			href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css"
			rel="stylesheet"
			integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65"
			crossorigin="anonymous"
			/>

			<script type="application/ld+json">
			{{
				"@context": "http://schema.org",
				"@type": "Organization",
				"name": ""
			}}
			</script>
			<meta name="theme-color" content="#029fe7" />
			<meta property="og:title" content="Home" />
			<meta property="og:type" content="website" />
		</head>
		<body class="u-body u-xl-mode" data-lang="en">
			<header class="u-clearfix u-header u-header" id="sec-4ed0">
			<div class="u-clearfix u-sheet u-sheet-1">
				<p class="u-text u-text-default u-text-1 logo" style="margin-top: 15px;"><a href="./index.html" >WebServer Nginx 2.0</a></p>
				<nav
				class="u-menu u-menu-hamburger u-offcanvas u-menu-1"
				data-responsive-from="XL"
				>
				<div
					class="menu-collapse"
					style="font-size: 1rem; letter-spacing: 0px; font-weight: 700"
				>
					<a
					class="u-button-style u-custom-border u-custom-border-color u-custom-borders u-custom-left-right-menu-spacing u-custom-padding-bottom u-custom-text-active-color u-custom-text-color u-custom-top-bottom-menu-spacing u-nav-link u-text-active-palette-1-base u-text-hover-palette-2-base"
					href="#"
					>
					<svg class="u-svg-link" viewBox="0 0 24 24">
						<use
						xmlns:xlink="http://www.w3.org/1999/xlink"
						xlink:href="#menu-hamburger"
						></use>
					</svg>
					<svg
						class="u-svg-content"
						version="1.1"
						id="menu-hamburger"
						viewBox="0 0 16 16"
						x="0px"
						y="0px"
						xmlns:xlink="http://www.w3.org/1999/xlink"
						xmlns="http://www.w3.org/2000/svg"
					>
						<g>
						<rect y="1" width="16" height="2"></rect>
						<rect y="7" width="16" height="2"></rect>
						<rect y="13" width="16" height="2"></rect>
						</g>
					</svg>
					</a>
				</div>
				<div class="u-nav-container">
					<ul class="u-nav u-spacing-20 u-unstyled u-nav-1">
					<li class="u-nav-item">
						<a
						class="u-button-style u-nav-link u-text-active-palette-2-base u-text-grey-90 u-text-hover-palette-2-base"
						href="./index.html"
						style="padding: 10px"
						>Home</a
						>
					</li>
					<li class="u-nav-item">
						<a
						class="u-button-style u-nav-link u-text-active-palette-2-base u-text-grey-90 u-text-hover-palette-2-base"
						href="./index.html"
						target="_blank"
						style="padding: 10px"
						>About</a
						>
					</li>
					<li class="u-nav-item">
						<a
						class="u-button-style u-nav-link u-text-active-palette-2-base u-text-grey-90 u-text-hover-palette-2-base"
						href="./index.html"
						style="padding: 10px"
						>Contact</a
						>
					</li>
					</ul>
				</div>
				<div class="u-nav-container-collapse">
					<div
					class="u-black u-container-style u-inner-container-layout u-opacity u-opacity-95 u-sidenav"
					>
					<div class="u-inner-container-layout u-sidenav-overflow">
						<div class="u-menu-close"></div>
						<ul
						class="u-align-center u-nav u-popupmenu-items u-unstyled u-nav-2"
						>
						<li class="u-nav-item">
							<a class="u-button-style u-nav-link" href="./index.html"
							>Home</a
							>
						</li>
						<li class="u-nav-item">
							<a
							class="u-button-style u-nav-link"
							href="./index.html"
							target="_blank"
							>About</a
							>
						</li>
						<li class="u-nav-item">
							<a class="u-button-style u-nav-link" href="./index.html"
							>Contact</a
							>
						</li>
						</ul>
					</div>
					</div>
					<div class="u-black u-menu-overlay u-opacity u-opacity-70"></div>
				</div>
				</nav>
			</div>
			</header>
			<section class="u-clearfix u-section-6" id="sec-4e6b">
			<div class="u-clearfix u-sheet u-sheet-1">
				<div
				class="u-clearfix u-expanded-width u-gutter-30 u-layout-wrap u-layout-wrap-1"
				>
				<div class="u-gutter-0 u-layout">
					<div class="u-layout-row">
					<div
						class="u-size-100-lg u-size-100-xl u-size-100-md u-size-100-sm u-size-100-xs"
					>
						<div class="u-layout-col">
						<div
							class="u-container-style u-layout-cell u-size-100 u-layout-cell-1"
						>
							<div class="u-container-layout u-container-layout-1">
							<h3
								class="u-text u-text-default u-text-palette-2-base u-text-1"
								style="color: #bc66c8 !important;"
							>
								
								This is what you have uploaded 
							</h3>"""
	if file_type == 'image':
		responseStr += f""" <img
								class="u-expanded-width u-image u-image-round u-radius-20 u-image-1"
								src="../uploads/{filename}"
								data-image-width="1000"
								data-image-height="854"
							/>"""
	elif file_type == 'video':
		responseStr += f""" <video width="100%" height="100%" controls>
						<source src="../uploads/{filename}" type="video/mp4">
						Your browser does not support the video tag.
						</video>"""
	else:
		responseStr += f"""<p class="u-text u-text-default u-text-2">File '{filename}' uploaded successfully.</p>"""
	responseStr += f"""
							<a href="../uploads/{filename}" class="u-active-palette-2-light-1 u-btn u-btn-round u-button-style u-hover-palette-2-light-1 u-palette-4-base u-radius-10 u-text-active-white u-text-hover-white u-btn-2">
								Download
							</a>
							<a href="./index.html" class="u-active-palette-2-light-1 u-btn u-btn-round u-button-style u-hover-palette-2-light-1 u-palette-4-base u-radius-10 u-text-active-white u-text-hover-white u-btn-2">
								Home
							</a>
							</div>
						</div>
						</div>
					</div>
					<div class="u-size-30-lg u-size-30-xl u-size-60-md u-size-60-sm u-size-60-xs">
						<div class="u-layout-col">
						<div
							class="u-container-style u-layout-cell u-size-60 u-layout-cell-1"
							id="plot"
						>
							<div class="u-container-layout u-container-layout-1" style="margin-left: 2rem !important;">
							<img
								class="u-expanded-width u-image u-image-round u-radius-20 u-image-1"
								src="../assets/images/img1.png"
								data-image-width="1000"
								data-image-height="854"
							/>
							</div>
						</div>
						</div>
					</div>
					</div>
				</div>
				</div>
			</div>
			</section>

			<footer
			class="u-align-center u-clearfix u-footer u-grey-80 u-footer"
			id="sec-410c"
			>
			<div class="u-clearfix u-sheet u-sheet-1">
				<p class="u-small-text u-text u-text-variant u-text-1">
				<p class="copyright">&copy; Copyright <strong>Anas Ajaanan & Abderahman Tukka</strong>. All Rights Reserved</p>
				</p>
			</div>
			</footer>
		</body>
		</html>"""

	return responseStr

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
