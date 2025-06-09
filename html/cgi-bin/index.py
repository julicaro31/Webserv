#!/usr/bin/env python3
import os
import sys
import random
import urllib.parse

default_fortunes = [
    "You will build an awesome webserver :rocket:",
    "Bugs are just misunderstood features :bug:",
    "Keep calm and read the RFC :scroll:"
]
method = os.environ.get("REQUEST_METHOD", "GET")
user_input = ""
if method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))

    body = sys.stdin.read(content_length)

    parsed = urllib.parse.parse_qs(body)
    user_input = parsed.get("input", ["(no fortune submitted)"])[0]

fortune = user_input if method == "POST" else random.choice(default_fortunes)

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Your Fortune</title>
</head>
<body>
  <h1>Your Fortune</h1>
  <p>Method: {method}</p>
  <p>{fortune}</p>
</body>
</html>
""")
