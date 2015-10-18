#!/usr/bin/python

# An example how to parse the KVIrc documentation with python.
# Do whatever you want with it :D

import re
import os

filelist = []

print "Step 1:"
print "Searching files ",

for root, dirs, files in os.walk("/src/pragma/kvirc-git/KVIrc/"):
	for f in files:
		r = re.compile("(.*)\.(h|cpp|c)$")
		if r.match(f):
			filelist.append(os.path.join(root, f))
			print ".",

outfile = open("index.html","w")

print ""
print "Step 2:"
print "Extracting comments",

# compile regular expressions

regex_comment = re.compile("(\/\*.*?\@doc:[\t ]+(.*?)\n.*?\*\/)", re.M and re.S);
regex_category = re.compile("^[\t ]+\@(type|title|short|syntax|description|doc):(.*)$")
regex_line = re.compile("^[\t ]+(.*)$")

# loop through all files

for filename in filelist:
	# first we go through the file
	comments = {}
	category = ""

	# read file
	data = open(filename).read();

	# get all comments
	comments_found = regex_comment.findall(data)

	for match in comments_found:
		print ".",
		comment = {}
		# we use @doc: to identify and save the comment
		doc_name = match[1]

		# if it already exists append our data
		if comments.has_key(doc_name):
			comment = comments[doc_name]

		# go through the comment line by line
		for m in match[0].splitlines():
			line_result = regex_category.match(m)
			if line_result:
				category = line_result.group(1)
				comment[category] = line_result.group(2)
			else:
				line_result = regex_line.match(m)
				if line_result:
					tmp = line_result.group(1)
					tmp = tmp.replace("<","&lt;")
					tmp = tmp.replace(">","&gt;")
					if category != "":
						if comment.has_key(category):
							comment[category] = comment[category] + tmp
						else:
							comment[category] = tmp

		# add the comment to our comments dict
		comments[doc_name] = comment

	# then we write all parsed comments to our file
	# keeping all stuff in memory created weird result in the dictonaries

	for key in comments:
		outfile.write("<h1>" + key + "</h1>\n")
		for subkey in comments[key]:
			outfile.write("<div style=\"border: 1px dotted green\">\n")
			outfile.write("<b><pre>" + subkey + "</pre></b>\n")
			outfile.write("<pre>" + comments[key][subkey] + "</pre>\n")
			outfile.write("</div><br>\n")

outfile.close()
