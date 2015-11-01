#!/usr/bin/python3

# https://scan.coverity.com/faq#frequency doesn't allow every push to be scanned.
# So this script pushes kvirc/KVIrc to kvirc/coverity no more than once a day, with modified .travis.yml

# This script is shamelessly copied from znc/znc (I wrote that one first), but please keep them synchronized...

import datetime
import subprocess
import sys
import yaml

subprocess.check_call(['git remote add coverity github:kvirc/coverity.git'], shell=True)
subprocess.check_call(['git fetch coverity'], shell=True)
commits_today = subprocess.check_output(['git log --since=today.midnight --oneline coverity/coverity_scan || true'], shell=True)
if len(commits_today) > 0:
    print('There were already commits today in coverity_scan')
    sys.exit()

# Get "install:" from .travis.yml, the rest from .travis-coverity.yml
with open('.travis.yml') as f:
    t = yaml.load(f)
with open('.travis-coverity.yml') as f:
    c = yaml.load(f)
# Travis project of kvirc/coverity is not multi-os
c['install'] = ['export TRAVIS_OS_NAME=linux'] + t['install']

with open('.travis.yml', 'w') as f:
    print(yaml.dump(c, default_flow_style=False), file=f)

subprocess.check_call(['git checkout -b coverity_scan'], shell=True)
subprocess.check_call(['git commit .travis.yml -m"Automatic coverity scan for {}"'.format(datetime.date.today())], shell=True)
subprocess.check_call(['git push coverity coverity_scan -f'], shell=True)
