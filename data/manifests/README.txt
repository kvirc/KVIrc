This stuff is needed to successfully
build a KVIrc distribution with Microsoft Visual C++ 2005 Express edition.

- The manifest file Microsoft.VC80.CRT.manifest MUST be installed
  in the application directory (e.g. C:\Program Files\KVIrc\)
- The manifest file Microsoft.VC80.CRT.modules.manifest MUST be installed
  as Microsoft.VC80.CRT.manifest in the modules directory
  (e.g. C:\Program Files\KVIrc\modules).
- The file msvcr80.dll must be placed in the application directory.

You MIGHT need to edit the version number inside the manifests in order
to match the requested version that is embedded in the KVIrc manifest file...

Yep.. this is ugly :D
