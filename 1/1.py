#!/usr/bin/python
import urllib
import urllib2

url = "http://www.baidu.com"

req = urllib2.Request(url)
print req

res_data = urllib2.urlopen(req)
res = res_data.read()
print res
