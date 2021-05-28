[Desktop Entry]
Type=Application
Version=1.5
Name=KVIrc
GenericName=IRC Client
GenericName[de]=IRC Client
GenericName[pl]=Klient IRC
GenericName[pt]=Cliente IRC
GenericName[tr]=IRC İstemcisi
GenericName[es]=Cliente de IRC
Comment=Connect to Internet Relay Chat
Comment[de]=Internet Relay Chat betreten.
Comment[pl]=Połącz się z Internet Relay Chat (IRC)
Comment[pt]=Conectar ao IRC (Internet Relay Chat)
Comment[tr]=IRC'e bağlanın
Comment[es]=Conectar al chat IRC
Path=${CMAKE_KVIRC_RESOURCES_DIR}
TryExec=${KVIRC_BIN_PATH}/${KVIRC_BINARYNAME}
Exec=${KVIRC_BINARYNAME} -m %U
Icon=kvirc
Categories=Network;IRCClient;Qt;KDE;
Keywords=IM;Chat;DCC;
MimeType=application/x-kva;application/x-kvt;text/x-kvc;text/x-kvs;x-scheme-handler/irc;x-scheme-handler/irc6;x-scheme-handler/ircs;
StartupNotify=true
StartupWMClass=KVIrc
DBusActivatable=false
Terminal=false
PrefersNonDefaultGPU=false
NoDisplay=false
Hidden=false
Actions=NewSession;

[Desktop Action NewSession]
Name=Start New Session
Icon=stock_new-window
Exec=${KVIRC_BINARYNAME} -f %U
