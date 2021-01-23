#!/usr/bin/env python3

import sys, os
import json
import subprocess
import urllib.request
import hashlib

# Go to the directory this script is stored in
os.chdir(os.path.dirname(os.path.realpath(__file__)))

# Download release data
releases = json.loads(str(urllib.request.urlopen('https://api.github.com/repos/nlohmann/json/releases').read(), 'utf-8'))

# Fetch all releases and the corresponding URL
release_url_map = []
for release in releases:
  for asset in release['assets']:
    if asset['name'] == 'json.hpp':
      release_url_map.append((release['tag_name'], asset['browser_download_url'], release['body']))

# List all git tags
process = subprocess.Popen(['git', 'tag'], stdout=subprocess.PIPE)
tags, _ = process.communicate()
tags = set(filter(None, str(tags, 'utf-8').split("\n")))
print("Releases already contained in this repository are " + str(tags))

# Go over the release_url_map in reverse order; if a release is not yet a Git
# tag, download the file, commit and add a tag
did_update = False
for tag, url, body in release_url_map[::-1]:
  if not tag in tags:
    print("Downloading release " + tag + " from " + url)
    os.makedirs('./include', mode=0o777, exist_ok=True)
    os.makedirs('./include/nlohmann', mode=0o777, exist_ok=True)
    data = urllib.request.urlopen(url).read();
    with open('./include/nlohmann/json.hpp', 'wb') as f:
      f.write(data)

    # Try to download the json_fwd.hpp header -- only exists since release
    # v3.1.0
    has_json_fwd = False
    try:
      json_fwd_url = 'https://github.com/nlohmann/json/raw/{}/include/nlohmann/json_fwd.hpp'.format(tag);
      print("Trying to download " + json_fwd_url)
      data = urllib.request.urlopen(json_fwd_url).read();
      with open('./include/nlohmann/json_fwd.hpp', 'wb') as f:
        f.write(data)
      has_json_fwd = True
    except:
      pass

    subprocess.call(['git', 'add', './include/nlohmann/json.hpp'])
    if has_json_fwd:
      subprocess.call(['git', 'add', './include/nlohmann/json_fwd.hpp'])

    # Update the README.md:
    subprocess.call([ 'sed', '-i', '-e', 's/GIT_TAG .*)/GIT_TAG '+ tag + ')/g', './README.md'])
    subprocess.call(['git', 'add', './README.md'])

    # Commit:
    subprocess.call(['git', 'commit', '-m', 'Upstream release ' + tag])
    subprocess.call(['git', 'tag', '-a', tag, '-m', body])

    did_update = True

# Push the updated Git repository
if did_update:
  subprocess.call(['git', 'push', '--tags'])
  subprocess.call(['git', 'push'])
