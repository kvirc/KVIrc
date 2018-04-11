# KVIrc

## [5.0.0-beta1-50-g4f0c52b](https://github.com/kvirc/KVIrc/tree/4f0c52b9e20bfc6695b58586d952add98c370511) (2018-04-07)

[Full Changelog](https://github.com/kvirc/KVIrc/compare/5.0.0-beta1...4f0c52b9e20bfc6695b58586d952add98c370511)

Alexander Pozdnyakov (1):

- PPA: fix script

Alexander Pozdnyakvo (4):

- PPA update: revert description
- PPA update: update version
- PPA update: lintian fix
- PPA update:

Benjamin Staneck (10):

- use the StdUtils NSIS plugin (#2369)
- fix doc folder cleanup fallout (#2370)
- Docs folder cleanup (#2366)
- don't use the NSIS plugin
- remove unused CheckUserInstallRights function
- Remove Winamp installer option
- remove autostart related functions
- Fixed the finish page so it will launch KVIrc as the user that ran the install before UAC privilege escalation.
- AppVeyor: grab gettext from vcpkg and clean up a bit (#2364)
- fix appveyor build

Bernhard M. Wiedemann (2):

- doc: Allow to override build date
- Use cmake TIMESTAMP

Dessa (2):

- update dicts
- push and pull po{t}s, unbreak a string in german

IceN9ne (18):

- serverdb.kvc: Applied general cleanup and network fixes
- KviAntiSpam: Remove undefined behavior
- KviInputEditor: Fix home key not clearing selection in certain situations
- Appveyor: Updated OpenSSL version to 1.0.2o
- KviChannelWindow: Fix bug in modes with parameters not being updated properly
- Revert "Remove gzipped logs as an option"
- Lag Meter: Change units to the more standard millisecond. Fixes #1577
- spellchecker: Stop trying to load invalid dictionaries.
- Add option to prioritize nick tab completion by last action time. Implements #2216
- KviChannelWindow: Fix encrypted own CTCP ACTION messages being sent to the wrong split view.
- Adjust defaults for avatar settings. Fixes #2340
- QRegExp: Enable greedy quantifiers in regex pattern matching.
- KviModeWidget: Remove commented out code
- KviModeWidget: Fix two bugs and refactor class
- KviWindowStack: Fix bug when attempting to focus undocked windows Fixes #2109
- KviNotifyList: Fix bug in notify property parsing with multiple consecutive spaces
- General code cleanup for kernel code
- Move Class Window List option to its proper location

Matt (1):

- KviOptions: Set the default bantype to *!*@host.name (#2343)

OmegaPhil (1):

- Remove gzipped logs as an option

Static (3):

- KviIrcContext: Re-add missing `return`
- KviIconManager: Use double-braces for array initialization
- IrcConnection: Use initiailizer lists and member initializers

Vladimir Panteleev (3):

- Support 64-bit X509 certificate serial numbers (#2363)
- CMakeLists: Detect and use libenchant 2.x
- I/O themes: Fix highlights not being logged in I/O themes

craftwar (2):

- AliasEditorWindow: Add "Export All to ..." button for more consistent UI
- AppVeyor: get zlib from vcpkg

funkydude (2):

- Update Freenode server locations for "asimov", "adams" & "pratchett". Replace dead "zelazny" with "niven" (London). (#2348)
- orwell.freenode.net is in Amsterdam, not Helsinki (#2347)

wodim (1):

- fix typo in libkvidcc.cpp

