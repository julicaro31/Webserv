#!/usr/bin/env python3

import cgi
import cgitb
import os
from urllib.parse import parse_qs

cgitb.enable()

query_string = os.environ.get('QUERY_STRING', '')

if not query_string:
    # No query string: show input form
    print('''
    <html>
        <head><title>Input Form</title></head>
        <body>
            <h1>Enter Data</h1>
            <form method="GET" action="">
                Surfer's Name: <input type="text" name="name"><br>
                Surfer's Age: <input type="text" name="age"><br>
                Surfer's Favorite Beach: <input type="text" name="beach"><br>
                <input type="submit" value="Submit">
            </form>
        </body>
    </html>
    ''')
else:
    # Parse the query string
    parsed_data = parse_qs(query_string)

    print('''
    <html>
        <head><title>Parsed Query String</title></head>
        <body>
            <h1>Parsed Query String Variables</h1>
            <ul>
    ''')

    for key, values in parsed_data.items():
        for value in values:
            print(f"<li><strong>{key}</strong>: {value}</li>")

    print('''
            </ul>
            <a href="?">Go Back</a>
        </body>
    </html>
    ''')
