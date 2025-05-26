#!/usr/bin/env python3

import random

fortunes = [
    "You will build an awesome webserver 🚀",
    "Bugs are just misunderstood features 🐛",
    "Keep calm and read the RFC 📜"
]

fortune = random.choice(fortunes)


print(f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Your Fortune</title>
</head>
<body>
  <h1>Your Fortune</h1>
  <p>{fortune}</p>
</body>
</html>
""")
