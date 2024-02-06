#! /bin/bash
echo -n "FirstName = "
echo $FirstName
echo "<br>"
echo -n "LastName = "
echo $LastName
echo "<br>"
echo "<br>"
echo "CGI Information :"
echo "<br>"
echo "FILENAME = $SCRIPT_FILENAME"
echo "<br>"
echo "QUERY_STRING = $QUERY_STRING"
echo "<br>"
echo "HTTP_USER_AGENT = $HTTP_USER_AGENT"
echo "<br>"
echo "REQUEST_METHOD = $REQUEST_METHOD"
echo "<br>"
echo "SERVER_NAME = $SERVER_NAME"