#!/usr/bin/python

from __future__ import print_function

import json
import sys
import os

print(json.dumps({
    'bin': sys.argv[1],
    'commit': os.environ['TRAVIS_COMMIT'],
}))
