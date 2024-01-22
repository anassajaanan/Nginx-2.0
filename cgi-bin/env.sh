#! /bin/bash
echo -n "x = "
echo $x
echo -n "y = "
echo $y

echo "CGI Information :"
echo "FILENAME = $SCRIPT_FILENAME"
echo "QUERY_STRING = $QUERY_STRING"
echo "HTTP_USER_AGENT = $HTTP_USER_AGENT"
echo "REQUEST_METHOD = $REQUEST_METHOD"
echo "SERVER_NAME = $SERVER_NAME"
echo "CONTENT_TYPE = $CONTENT_TYPE"
echo "CONTENT_LENGHT = $CONTENT_LENGHT"
