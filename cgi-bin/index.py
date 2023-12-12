#!/usr/bin/env python3

import cgi

# print("Content-type: text/html\n")
print("<html><body>")
print("<h1>This is our Python CGI</h1>")

form = cgi.FieldStorage()
if "user_input" in form:
    print("<p>You entered: " + cgi.escape(form["user_input"].value) + "</p>")
print("<p>please enter your name:</p>")
print("""
<form method="POST">
    <input type="text" name="username">
    <input type="submit" value="Send">
</form>
""")

print("</body></html>")