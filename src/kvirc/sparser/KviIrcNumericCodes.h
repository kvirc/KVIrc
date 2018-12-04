#ifndef _KVI_NUMERIC_H_
#define _KVI_NUMERIC_H_

//
// IRC - Internet Relay Chat, include/numeric.h
// Copyright (C) 1990 Jarkko Oikarinen
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 1, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

// Revisited by Szymon Stefanek 07 Jul 1998 (pragma at kvirc dot net)
// Re-revisited again on 11 Feb 1999 (pragma at kvirc dot net)
// Updated FSF address on 07 May 2009 (ctrlaltca at libero dot it)

// We will need 4 digit replies for the new irc protocol
// and then leave 50-100 numbers between logical groups...

// In kvirc 3.0.0 this file is newer included (at least for now: 31.07.2000)
// Today I have included it in KviIrcServerParser_numericHandlers.cpp (23.03.2001)

// ===============================================================================//
//
// E=EFnet hybrid 5.3p2
// e=EFNet hybrid 5.3p2 with #ifdef
// I=IRCnet 2.10.0p5
// i=IRCnet, reserved according to Undernet (sic) numerics header
// U=Undernet u2.10.04
// u=Undernet u2.10.04 with #ifdef
// D=DALnet 4.6.7.DF
// d=DALnet 4.6.7.DF, redundant
// O=openprojects (dancer ircd)
//

// = 000-199 ===============================================================================//
//
// Reserve numerics 000-099 for server-client connections where the client
// is local to the server. If any server is passed a numeric in this range
// from another server then it is remapped to 100-199. -avalon
//

#define RPL_WELCOME 1  /* :Welcome to the Internet Relay Network %s             [I,E,U,D] # */
#define RPL_YOURHOST 2 /* :Your host is %s, running version %s                  [I,E,U,D] # */
#define RPL_CREATED 3  /* :This server was created %s                           [I,E,U,D] # */
#define RPL_MYINFO 4   /* <server_name> <srv_version> <u_modes> <ch_modes>      [I,E,U,D] # */
#define RPL_BOUNCE 5   /* :Try server %s, port %d                               [I]       # */
#define RPL_PROTOCTL 5 /* %s :are available on this server                      [D]       # */
#define RPL_MAP 5      /* :%s%s                                                 [U]       # */
#define RPL_MAPMORE 6  /* :%s%s --> *more*                                      [U]       # */
#define RPL_MAPEND 7   /* :End of /MAP                                          [U]       # */
//[03:45:18] [dune.webchat.org][007] newbie dune.webchat.org 1037760291 :Tue, 19 Nov 2002 18:44:51 -0800

#define RPL_SNOMASK 8 /* %d :: Server notice mask (%#x)                        [U]       # */
//[03:45:18] [dune.webchat.org][008] newbie -/NAJgAxq7HQPwnkK^ 124963541 :This is your Session ID and Session Key.

#define RPL_STATMEMTOT 9 /* %u %u :Bytes Blocks                                   [U]       # */
//[03:45:18] [dune.webchat.org][009] newbie ASCII :Current character mapping.

#define RPL_STATMEM 10   /* %u %u %s                                              [U]       # */
#define RPL_BOUNCENEW 10 /* :Try server %s, port %d                               [I-NEW]   # */
//      RPL_????????   11,12,13
#define RPL_YOURCOOKIE 14 /* <cookie> :is your reconnection cookie                 [E,i?]    # */

#define RPL_CONNECTING 20 //:irc.dotsrc.org 020 * :Please wait while we process your connection.

// = 200-399 ====================================================================================//
//
// Command replies
//

#define RPL_TRACELINK 200       /* [I,E,U,D] "Link <version> <destination> <next server> <next-server-TS-version><zflag> <link-time> <sendQ> <prev-server-sendQ>" [I,E,U,D] */
#define RPL_TRACECONNECTING 201 /* [I,E,U,D] "Try. %d %s"*/
#define RPL_TRACEHANDSHAKE 202  /* [I,E,U,D] "H.S. %d %s"*/
#define RPL_TRACEUNKNOWN 203    /* [I,E,U,D] "???? %d %s %d"*/
#define RPL_TRACEOPERATOR 204   /* [I,E,U,D] "Oper %d %s"*/
#define RPL_TRACEUSER 205       /* [I,E,U,D] "User %d %s"*/
#define RPL_TRACESERVER 206     /* [I,E,U,D] "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server> <TS-version><zflag>" [I,E,U,D] */
#define RPL_TRACENEWTYPE 208    /* [I,E,U,D] "<newtype> 0 %s"*/
#define RPL_TRACECLASS 209      /* [I,E,U,D] "Class %d %d"*/
#define RPL_TRACERECONNECT 210  /* [I] ??? */
#define RPL_STATSLINKINFO 211   /* [I,E,U,D] */
#define RPL_STATSCOMMANDS 212   /* [I,E,U,D] "%s %u %u"*/
#define RPL_STATSCLINE 213      /* [I,E,U,D] "%c %s * %s %d %d"*/
#define RPL_STATSNLINE 214      /* [I,E,U,D] "%c %s * %s %d %d"*/
#define RPL_STATSILINE 215      /* [I,E,U,D] "%c %s * %s %d %d"*/
#define RPL_STATSKLINE 216      /* [I,E,U,D] */
#define RPL_STATSPLINE 217      /* [U] */
#define RPL_STATSQLINE 217      /* [I,E,U,D] */
#define RPL_STATSYLINE 218      /* [I,E,U,D] */
#define RPL_ENDOFSTATS 219      //	[I,E,U,D] */
// #define RPL_????       220
#define RPL_UMODEIS 221     /* <nick> <mode>                                            [I,E,U,D] # */
#define RPL_STATSBLINE 222  /* [E] */
#define RPL_SQLINE_NICK 222 /* [D] */
#define RPL_STATSELINE 223  /* [E] */
#define RPL_STATSFLINE 224  /* [E] */
#define RPL_STATSDLINE 225  /* [E] */
//      RPL_??????????   226-230
#define RPL_SERVICEINFO 231   /* [I,U,D] */
#define RPL_ENDOFSERVICES 232 /* [I,U,D] */
#define RPL_SERVICE 233       /* [I,U,D] */
#define RPL_SERVLIST 234      /* [I,E,U,D] */
#define RPL_SERVLISTEND 235   /* [I,E,U,D] */
//      RPL_??????????   236-238
#define RPL_STATSIAUTH 239    /*                                                              [I] # */
#define RPL_STATSVLINE 240    /*                                                              [I] # */
#define RPL_STATSLLINE 241    /*                                                        [I,E,U,D] # */
#define RPL_STATSUPTIME 242   /*                                                        [I,E,U,D] # */
#define RPL_STATSOLINE 243    /*                                                        [I,E,U,D] # */
#define RPL_STATSHLINE 244    /*                                                        [I,E,U,D] # */
#define RPL_STATSSLINE 245    /*                                                        [I,E,u,D] # */
#define RPL_STATSPING 246     /*                                                              [I] # */
#define RPL_STATSTLINE 246    /*                                                              [U] # */
#define RPL_STATSBLINE_I 247  /*                                                              [I] # */
#define RPL_STATSGLINE 247    /*                                                              [U] # */
#define RPL_STATSXLINE 247    /*                                                              [D] # */
#define RPL_STATSDEFINE 248   /*                                                              [I] # */
#define RPL_STATSULINE 248    /*                                                            [U,D] # */
#define RPL_STATSDEBUG 249    /*                                                        [I,E,U,D] # */
#define RPL_STATSCONN 250     /*                                                        [e,U,D,O] # */
#define RPL_STATSDLINE_I 250  /*                                                              [I] # */
#define RPL_LUSERCLIENT 251   /* ":There are %d users and %d invisible on %d servers" [I,E,U,D,O] # */
#define RPL_LUSEROP 252       /* "%d :IRC Operators online"                           [I,E,U,D,O] # */
#define RPL_LUSERUNKNOWN 253  /* "%d :unknown connection(s)"                          [I,E,U,D,O] # */
#define RPL_LUSERCHANNELS 254 /* "%d :channels formed"                                [I,E,U,D,O] # */
#define RPL_LUSERME 255       /* ":I have %d clients and %d servers"                  [I,E,U,D,O] # */
#define RPL_ADMINME 256       /* ":Administrative info about %s"       [I,E,U,D] */
#define RPL_ADMINLOC1 257     /* ":%s"                        [I,E,U,D] */
#define RPL_ADMINLOC2 258     /* ":%s"                            [I,E,U,D] */
#define RPL_ADMINEMAIL 259    /* ":%s"                         [I,E,U,D] */
//      RPL_?????????        260
#define RPL_TRACELOG 261  /* [I,E,U,D] */
#define RPL_TRACEEND 262  /* [I,E] */
#define RPL_TRACEPING 262 /* [U] */
//[14:28:51] [ircd.tin.it][263] [newbie] LINKS :Please wait a while and try again.
#define RPL_TRYAGAIN 263 /* [I] */
#define RPL_LOAD2HI 263  /* ":Server load is temporarily too heavy. Please wait a while and try again." [E] */
//      RPL_???????          264
#define RPL_LOCALUSERS 265  /* ":Current local users: %d Max: %s"                       [e,D,O] # */
#define RPL_GLOBALUSERS 266 /* ":Current global users: %d Max: %d"                      [e,D,O] # */
#define RPL_SHAREDUSERS 267 /* ":Current shared users: %d Max: %d"                      [I]     # */
//      RPL_???????      267-270
#define RPL_SILELIST 271      /* [U,D] */
#define RPL_ENDOFSILELIST 272 /* [U,D] */
#define RPL_STATSDELTA 274    /* "<timedelta>" [i,E] */
#define RPL_STATSDLINE_U 275  /* [U,D] */
#define RPL_WHOISSSL 275      /* [AzzurraNET] %s :is using a secure connection (SSL)              # */
//      RPL_???????      276-279
#define RPL_GLIST 280      /* [U] */
#define RPL_ENDOFGLIST 281 /* [U] */
//      RPL_???????      282-289
#define RPL_HELPHDR 290 /* [D] */
#define RPL_HELPOP 291  /* [D] */
#define RPL_HELPTLR 292 /* [D] */
#define RPL_HELPHLP 293 /* [D] */
#define RPL_HELPFWD 294 /* [D] */
#define RPL_HELPIGN 295 /* [D] */
//      RPL_???????      296-299
#define RPL_NONE 300     /* [I,E,U,D] */
#define RPL_AWAY 301     /* ":fontana.openprojects.net 301 Pragma Nick :sleep :) [I,E,U,D] # */
#define RPL_USERHOST 302 /* ":up to 5 userhost replies"                          [I,E,U,D] # */
#define RPL_ISON 303     /* ":<list of present nicks>"                           [I,E,U,D] # */
#define RPL_TEXT 304     /* [I,E,U,D] */
#define RPL_UNAWAY 305   /*                                                      [I,E,U,D] # */
#define RPL_NOWAWAY 306  /*                                                      [I,E,U,D] # */
//#define RPL_USERIP           307 /* [U] */
#define RPL_WHOISREGNICK 307 /* "%s :is a registered nick"                                 [D] # */
#define RPL_WHOISADMIN 308   /* "???"                                                      [d] # */
#define RPL_WHOISSADMIN 309  /* "???"                                                      [d] # */
#define RPL_WHOISHELPOP 310  /* "???"                                                      [D] # */

#define RPL_WHOISUSER 311       /* "%s %s %s * :%s"                                     [I,E,U,D] # */
#define RPL_WHOISSERVER 312     /* "%s %s :%s"                                          [I,E,U,D] # */
#define RPL_WHOISOPERATOR 313   /* "%s :is an IRC Operator"                             [I,E,U,D] # */
#define RPL_WHOWASUSER 314      /* "%s %s %s * :%s"                                     [I,E,U,D] # */
#define RPL_ENDOFWHO 315        /* "<channel> :End of /WHO list."                       [I,E,U,D] # */
#define RPL_WHOISCHANOP 316     /* "???"                                                  [I,E,D] # */
#define RPL_WHOISIDLE 317       /* "%s %ld %ld :seconds idle, signon time"              [I,E,U,D] # */
#define RPL_ENDOFWHOIS 318      /* "%s :End of /WHOIS list."                            [I,E,U,D] # */
#define RPL_WHOISCHANNELS 319   /* "%s :%s"                                             [I,E,U,D] # */
#define RPL_WHOISIDENTIFIED 320 /* %s :is an identified user                            [O]       # */
#define RPL_LISTSTART 321       /* Channel :Users Name                                  [I,E,U,D] # */
#define RPL_LIST 322            /* <channel> <users> :<topic>                           [I,E,U,D] # */
#define RPL_LISTEND 323         /* :End of /LIST                                        [I,E,U,D] # */
#define RPL_CHANNELMODEIS 324   /* "<channel> +<mode>"                                  [I,E,U,D] # */
#define RPL_CHANNELPASSIS 325   /* "<channel> <password>" [i] */
#define RPL_NOCHANPASS 326      /* "<channel> :No channel password" [i] */
#define RPL_CHPASSUNKNOWN 327   /* [i] */
#define RPL_CHANURL 328         /* [D] [services.dal.net][328] Pragma #linux :http://dalnet.linuxguru.net */
//[04:25:18] [services.dal.net][328] rettter #cebu :none.
#define RPL_CREATIONTIME 329 /* "<channel> %lu"                                        [E,U,D] # */
#define RPL_WHOISAUTH 330    /* %s %s :is authed as ....                            [Quakenet] # */
#define RPL_NOTOPIC 331      /* "<channel> :No topic is set."                        [I,E,U,D] # */
#define RPL_TOPIC 332        /* "<channel> :%s"                                      [I,E,U,D] # */
#define RPL_TOPICWHOTIME 333 /* "<channel> <whoset> %lu"                               [e,U,D] # */
#define RPL_LISTUSAGE 334    /* [U] */
#define RPL_LISTSYNTAX 335   /* [D] */
//      RPL_??????????   336-337
#define RPL_CHANPASSOK 338  /* "<channel> :Channel password OK" [i] */
#define RPL_BADCHANPASS 339 /* "<channel> :Invalid channel password" [i] */
#define RPL_EQUALNICKS 339
//      RPL_??????????       340
#define RPL_INVITING 341  /* [I,E,U,D] */
#define RPL_SUMMONING 342 /* [I,E,D] */
//      RPL_?????????    343-345
#define RPL_INVITELIST 346      /* "<channel> <exceptionmask>"                                [I] # */
#define RPL_ENDOFINVITELIST 347 /* "<channel> :End of Channel Invite List"                    [I] # */
#define RPL_EXCEPTLIST 348      /* "<channel> <exceptionmask>"                                [I] # */
#define RPL_ENDOFEXCEPTLIST 349 /* "<channel> :End of Channel Exception List"                 [I] # */
//      RPL_??????????       350
#define RPL_VERSION 351   /* "%s.%s %s :%s"                                       [I,E,U,D] # */
#define RPL_WHOREPLY 352  /* "<chn> <usr> <hst> <srv> <nck> <stat> :<hops> <real>"[I,E,U,D] # */
#define RPL_NAMREPLY 353  /* "= <channel> :<space_separated_list_of_nicks>"       [I,E,U,D] # */
#define RPL_WHOSPCRPL 354 /* [U] */
//      RPL_?????????    355-360
#define RPL_KILLDONE 361     /* [I,E,U,D] */
#define RPL_CLOSING 362      /* [I,E,U,D] */
#define RPL_CLOSEEND 363     /* [I,E,U,D] */
#define RPL_LINKS 364        /* "<host> <parent> :<hops> <description>"              [I,E,U,D] # */
#define RPL_ENDOFLINKS 365   /* %s :End of /LINKS"                                   [I,E,U,D] # */
#define RPL_ENDOFNAMES 366   /* "%s :End of /NAMES list."                            [I,E,U,D] # */
#define RPL_BANLIST 367      /* "<channel> <banmask>"                                [I,E,U,D] # */
#define RPL_ENDOFBANLIST 368 /* "<channel> :End of channel Ban List"                 [I,E,U,D] # */
#define RPL_ENDOFWHOWAS 369  /* "%s :END of /WHOWAS"                                 [I,E,U,D] # */
#define RPL_INFO 371         /* ":%s"                                                [I,E,U,D] # */
#define RPL_MOTD 372         /* ":- %s"                                              [I,E,U,D] # */
#define RPL_INFOSTART 373    /* ":Server INFO"                                       [I,E,U,D] # */
#define RPL_ENDOFINFO 374    /* ":End of /INFO list."                                [I,E,U,D] # */
#define RPL_MOTDSTART 375    /* ":- %s Message of the Day - "                        [I,E,U,D] # */
#define RPL_ENDOFMOTD 376    /* ":End of /MOTD command."                             [I,E,U,D] # */
#define RPL_MOTD2 377        /* ":- %s"                                                    [?] # */
#define RPL_MOTD3 378        /* ":- %s"                                              [Austnet] # */
//      RPL_?????????    378-380
#define RPL_YOUREOPER 381      /* [I,E,U,D] */
#define RPL_REHASHING 382      /* [I,E,U,D] */
#define RPL_YOURESERVICE 383   /* [I,D] */
#define RPL_MYPORTIS 384       /* [I,E,U,D] */
#define RPL_NOTOPERANYMORE 385 /* [I,E,U,D] */
//      RPL_?????????    386-390
#define RPL_TIME 391       /* [I,E,U,D] */
#define RPL_USERSSTART 392 /* [I,E,D] */
#define RPL_USERS 393      /* [I,E,D] */
#define RPL_ENDOFUSERS 394 /* [I,E,D] */
#define RPL_NOUSERS 395    /* [I,E,D] */
#define RPL_HOSTHIDDEN 396
//      RPL_?????????    397-399

// = 400-599 ====================================================================================//
//
// Errors are in the range from 400-599 currently and are grouped by what
// commands they come from.
//

//      ERR_?????????        400
#define ERR_NOSUCHNICK 401       /* "%s :No such nick/channel"                           [I,E,U,D] # */
#define ERR_NOSUCHSERVER 402     /* "%s :No such server"                                 [I,E,U,D] # */
#define ERR_NOSUCHCHANNEL 403    /* "%s :No such channel" [I,E,U,D] */
#define ERR_CANNOTSENDTOCHAN 404 /* "%s :Cannot send to channel" [I,E,U,D] */
#define ERR_TOOMANYCHANNELS 405  /* "%s :You have joined too many channels" [I,E,U,D] */
#define ERR_WASNOSUCHNICK 406    /* "%s :There was no such nickname                      [I,E,U,D] # */
#define ERR_TOOMANYTARGETS 407   /* "%s :Duplicate recipients. No message delivered" [I,E,U,D] */
#define ERR_NOSUCHSERVICE 408    /* "%s :No such service" [I,D] */
#define ERR_NOORIGIN 409         /* ":No origin specified" [I,E,U,D] */
//      ERR_??????????       410
#define ERR_NORECIPIENT 411  /* ":No recipient given (%s)" [I,E,U,D] */
#define ERR_NOTEXTTOSEND 412 /* ":No text to send" [I,E,U,D] */
#define ERR_NOTOPLEVEL 413   /* "%s :No toplevel domain specified" [I,E,U,D] */
#define ERR_WILDTOPLEVEL 414 /* "%s :Wildcard in toplevel Domain" [I,E,U,D] */
#define ERR_BADMASK 415      /* ??? [I] */
//#define ERR_TOOMANYTARGETS 416 /* Can not understand this ... but I've found it in some docs [I] */
#define ERR_QUERYTOOLONG 416 /* "<target> <erroneous command> :Too many lines in the output, please restrict your query [U] */
//      ERR_??????????   417-420
#define ERR_UNKNOWNCOMMAND 421 /* "%s :Unknown command"                                [I,E,U,D] # */
#define ERR_NOMOTD 422         /* ":MOTD File is missing" [I,E,U,D] */
#define ERR_NOADMININFO 423    /* "%s :No administrative info available" [I,E,U,D] */
#define ERR_FILEERROR 424      /* ":File error doing %s on %s" [I,E,U,D] */
//      ERR_??????????   425-430
#define ERR_NONICKNAMEGIVEN 431  /* ":No nickname given"                                 [I,E,U,D] */
#define ERR_ERRONEUSNICKNAME 432 /* "%s :Erroneus Nickname"                              [I,E,U,D] # */
#define ERR_NICKNAMEINUSE 433    /* "%s :Nickname is already in use."                    [I,E,U,D] # */
#define ERR_SERVICENAMEINUSE 434 /* ??? [I,D] */
#define ERR_BANONCHAN 435        /* "Guest70103 BlindSight ##linux :Cannot change nickname while banned on channel" [I,D] */
#define ERR_NICKCOLLISION 436    /* "%s :Nickname collision KILL"                        [I,E,U,D] # */
#define ERR_UNAVAILRESOURCE 437  /* ???                                                        [I] # */
//#define ERR_BANNICKCHANGE  437 /* ??? duplicate [U,D] */
#define ERR_NICKTOOFAST 438 /* ???                                                        [U] # */
//#define ERR_NCHANGETOOFAST 438 /* ??? duplicate [D] */
#define ERR_TARGETTOOFAST 439    /* ??? [U,D] */
#define ERR_SERVICESDOWN 440     /* ??? [D] */
#define ERR_USERNOTINCHANNEL 441 /* "%s %s :They aren't on that channel" [I,E,U,D] */
#define ERR_NOTONCHANNEL 442     /* "%s :You're not on that channel" [I,E,U,D] */
#define ERR_USERONCHANNEL 443    /* "%s %s :is already on channel" [I,E,U,D] */
#define ERR_NOLOGIN 444          /* "%s :User not logged in" [I,E,U,D] */
#define ERR_SUMMONDISABLED 445   /* ":SUMMON has been removed/disabled" [I,E,U,D] */
#define ERR_USERSDISABLED 446    /* ":USERS has been removed/disabled" [I,E,U,D] */
//      ERR_??????????   447-450
#define ERR_NOTREGISTERED 451 /* ":You have not registered" [I,E,U,D] */
#define ERR_IDCOLLISION 452   /* ":ID collision KILL" [i,?] */
#define ERR_NICKLOST 453      /* "<nick> :Nickname collision; please enter a new nick" [i,?] */
//      ERR_??????????       454
#define ERR_HOSTILENAME 455 /* ??? [D] */
//      ERR_??????????   456-460
#define ERR_NEEDMOREPARAMS 461   /* "%s :Not enough parameters" [I,E,U,D] */
#define ERR_ALREADYREGISTRED 462 /* ":You may not reregister" [I,E,U,D] */
#define ERR_NOPERMFORHOST 463    /* ":Your host isn't among the privileged" [I,E,U,D] */
#define ERR_PASSWDMISMATCH 464   /* ":Password Incorrect" [I,E,U,D] */
#define ERR_YOUREBANNEDCREEP 465 /* ":You are banned from this server- %s" [I,E,U,D] */
//[04:42:04] [forever.va.us.mystical.net][465] Pragma :You have been Autokilled.
#define ERR_YOUWILLBEBANNED 466 /* ??? [I,E,U,D] */
#define ERR_KEYSET 467          /* "%s :Channel key already set" [I,E,U,D] */
#define ERR_INVALIDUSERNAME 468 /* ??? [U,?] */
//#define ERR_ONLYSERVERSCANCHANGE 468 /* ??? [D] */
//      ERR_???????????  469
#define ERR_LINKCHANNEL 470    /* "%s :Forwarding to another channel" */
#define ERR_CHANNELISFULL 471  /* "%s :Cannot join channel (+l)" [I,E,U,D]                       # */
#define ERR_UNKNOWNMODE 472    /* "%c :is unknown mode char to me"  [I,E,U,D] */
#define ERR_INVITEONLYCHAN 473 /* "%s :Cannot join channel (+i)" [I,E,U,D]                       # */
#define ERR_BANNEDFROMCHAN 474 /* "%s :Cannot join channel (+b)" [I,E,U,D]                       # */
#define ERR_BADCHANNELKEY 475  /* "%s :Cannot join channel (+k)" [I,E,U,D]                       # */
#define ERR_BADCHANMASK 476    /* "%s :Bad Channel Mask" [I,E,U,D] */
#define ERR_MODELESS 477       /* ??? [U] */
//#define ERR_????           477 /* [irc.tin.it][477] [Pragma] 8 :Channel doesn't support modes [I] */
#define ERR_NEEDREGGEDNICK 477 /* ??? [U] */
//[03:48:45] [tiscali.uk.eu.dal.net][477] PragTest #italia :You need to identify to a registered nick to join that channel.
#define ERR_BANLISTFULL 478 /* ??? [U,D] */
//      ERR_???????????  479-480
#define ERR_NOPRIVILEGES 481     /* ":Permission Denied- You're not an IRC operator" [I,E,U,D] */
#define ERR_CHANOPRIVSNEEDED 482 /* "%s :You're not channel operator" [I,E,U,D] */
#define ERR_CANTKILLSERVER 483   /* ":You can't kill a server!" [E,U,D] */
#define ERR_DESYNC 484           /* ??? [E] */
#define ERR_ISCHANSERVICE 484    /* ??? [U] */
#define ERR_CHANTOORECENT 487    /* "<channel> :Channel created too recently" [i] */
#define ERR_TSLESSCHAN 488       /* "<channel> :Mode <mode> not available on TS-less channel" [i] */
//      ERR_???????????  489-490
#define ERR_NOOPERHOST 491    /* ":Only few of mere mortals may try to enter the twilight zone" [I,E,U,D] */
#define ERR_NOSERVICEHOST 492 /* ??? [I,D] */
//      ERR_???????????  493-500
#define ERR_UMODEUNKNOWNFLAG 501 /* ":Unknown MODE flag" [I,E,U,D] */
#define ERR_USERSDONTMATCH 502   /* ":Can't change mode for other users" [I,E,U,D] */
#define ERR_GHOSTEDCLIENT 503    /* ":Message could not be delivered to %s" [E] */
#define ERR_LAST_ERR_MSG 504     /* ":Last Error Message" [E] */
//      ERR_???????????  505-510
#define ERR_SILELISTFULL 511 /* ??? [U,D] */
#define ERR_NOSUCHGLINE 512  /* ??? [U] */
#define ERR_TOOMANYWATCH 513 /* ??? [D] */
#define ERR_BADPING 514      /* ??? [U] */
#define ERR_NEEDPONG 515     /* ??? [D] */
#define ERR_LISTSYNTAX 516   /* ??? [D] */
// DalNet (excellent) extension that substitutes the ISON ping
#define RPL_LOGON 600      /* <nick> <user> <host> <logintime> :logged online             [D] # */
#define RPL_LOGOFF 601     /* <nick> <user> <host> <logintime> :logged offline            [D] # */
#define RPL_WATCHOFF 602   /* <nick> <user> <host> <logintime> :stopped watching          [D] # */
#define RPL_WATCHSTAT 603  /* :You have <n> and are on <n> WATCH entries                  [D] # */
#define RPL_NOWON 604      /* <nick> <user> <host> <logintime> :is online                 [D] # */
#define RPL_NOWOFF 605     /* <nick> <user> <host> 0 :is offline                          [D] # */
#define RPL_WATCHLIST 606  /* ?                                                           [D] # */
#define RPL_ENDOFWATCH 607 /* :End of WATCH <char l=list s=stats>                         [D] # */
// STARTTLS extension
#define RPL_STARTTLSOK 670   /* :test2.chatspike.net 670 nickname :STARTTLS successful, go ahead with TLS handshake */
#define RPL_STARTTLSFAIL 691 /* :test2.chatspike.net 671 nickname :STARTTLS failure */
// WeNet codepage extension
#define RPL_CODEPAGESET 700 /* :irc.wenet.ru 700 Pragma CP1251 :is now your translation scheme # */
#define RPL_WHOISSCHEME 703 /* :interdacom.vgd.wenet.ru 703 Alexey Begemot KOI8-R :translation scheme */
#define RPL_TARGUMODEG 716  /* :graviton.oftc.net 716 BlindSight BlindSight2 :is in +g mode (server side ignore) */
#define RPL_TARGNOTIFY 717  /* :graviton.oftc.net 717 BlindSight BlindSight2 :has been informed that you messaged them. */
#define RPL_UMODEGMSG 718   /* :graviton.oftc.net 718 BlindSight BlindSight2[ [~kvirc64@host]] :is messaging you, and you are umode +g or +G. */
// Quiet ban listing (freenode)
#define RPL_QUIETLIST 728    /* :sendak.freenode.net 728 CtrlAltCa #kde q *!*@* sendak.freenode.net 1436979239 */
#define RPL_QUIETLISTEND 729 /* :sendak.freenode.net 729 CtrlAltCa #kde q :End of Channel Quiet List */
//SASL EXTENSION
#define RPL_SASLLOGIN 900           /* :jaguar.test 900 jilles jilles!jilles@localhost.stack.nl jilles :You are now logged in as jilles. */
#define RPL_SASLSUCCESS 903         /* :jaguar.test 903 jilles :SASL authentication successful  */
#define RPL_SASLFAILED 904          /* :lindbohm.freenode.net 904 * :SASL authentication failed */
#define RPL_SASLERROR 905           // :server 905 <nick> :SASL message too long
#define RPL_SASLABORT 906           // :server 906 <nick> :SASL authentication aborted
#define ERR_SASLALREADY 907         // :server 907 <nick> :You have already completed SASL authentication
#define RPL_SASLMECHS 908           // :server 908 <nick> <mechanisms> :are available SASL mechanisms
#define RPL_ENDOFSPAMFILTERLIST 940 // <nick> <channel> :End of channel spamfilter list
#define RPL_SPAMFILTERLIST 941      // <nick> <channel> <spamfilter>

///* 303 */	RPL_ISON, ":",
///* 304 */	RPL_TEXT, (char *)nullptr,
///* 305 */	RPL_UNAWAY, ":You are no longer marked as being away",
///* 306 */	RPL_NOWAWAY, ":You have been marked as being away",

///* 321 */	RPL_LISTSTART, "Channel :Users  Name",
///* 322 */	RPL_LIST, "%s %d :%s",
///* 323 */	RPL_LISTEND, ":End of /LIST",
///* 324 */       RPL_CHANNELMODEIS, "%???"

///* 334 */	0, (char *)nullptr,
///* 341 */	RPL_INVITING, "%s %s",
///* 342 */	RPL_SUMMONING, "%s :User summoned to irc",
///* 352 */	RPL_WHOREPLY, ,
///* 361 */	RPL_KILLDONE, (char *)nullptr,
///* 362 */	RPL_CLOSING, "%s :Closed. Status = %d",
///* 363 */	RPL_CLOSEEND, "%d: Connections Closed",
///* 364 */	RPL_LINKS, "%s %s :%d %s",
///* 365 */	RPL_ENDOFLINKS, "%s :End of /LINKS list.",
///* 367 */       RPL_BANLIST, "%s %s %s %lu",
///* 367 */	RPL_BANLIST, "%s %s",
///* 368 */	RPL_ENDOFBANLIST, "%s :End of Channel Ban List",
///* 369 */	RPL_ENDOFWHOWAS, "%s :End of WHOWAS",
///* 381 */	RPL_YOUREOPER, ":You have entered... the Twilight Zone!.",
///* 382 */	RPL_REHASHING, "%s :Rehashing",
///* 383 */	0, (char *)nullptr,
///* 384 */	RPL_MYPORTIS, "%d :Port to local server is\r\n",
///* 385 */	RPL_NOTOPERANYMORE, (char *)nullptr,
///* 391 */	RPL_TIME, "%s :%s",
///* 392 */	RPL_USERSSTART, ":UserID   Terminal  Host",
///* 393 */	RPL_USERS, ":%-8s %-9s %-8s",
///* 394 */	RPL_ENDOFUSERS, ":End of Users",
///* 395 */	RPL_NOUSERS, ":Nobody logged in.",
///* 216 */	RPL_STATSKLINE, "%c %s * %s %s",
///* 216 */	RPL_STATSKLINE, "%c %s %s %s %d %d",
///* 217 */	RPL_STATSQLINE, "%c %s * %s %d %d",
///* 218 */	RPL_STATSYLINE, "%c %d %d %d %d %ld",
///* 219 */	RPL_ENDOFSTATS, "%c :End of /STATS report",
///* 221 */	RPL_UMODEIS, "%s",
///* 222 */	RPL_STATSBLINE, "%c %s * %s %d %d",
///* 223 */	RPL_STATSELINE, "%c %s * %s %d %d",
///* 224 */	RPL_STATSFLINE, "%c %s * %s %d %d",
///* 225 */	RPL_STATSDLINE, "%c %s %s",
///* 231 */	0, (char *)nullptr,
///* 232 */	0, (char *)nullptr,
///* 233 */	0, (char *)nullptr,
///* 234 */	RPL_SERVLIST, (char *)nullptr,
///* 235 */	RPL_SERVLISTEND, (char *)nullptr,
///* 241 */	RPL_STATSLLINE, "%c %s * %s %d %d",
///* 242 */	RPL_STATSUPTIME, ":Server Up %d days, %d:%02d:%02d",
///* 243 */	RPL_STATSOLINE, "%c %s * %s %d %d",
///* 244 */	RPL_STATSHLINE, "%c %s * %s %d %d",
///* 245 */	RPL_STATSSLINE, "%c %s * %s %d %d",
///* 250 */ RPL_STATSCONN,":Highest connection count: %d (%d clients)"
///* 261 */	RPL_TRACELOG, "File %s %d",
///* 262 */	RPL_ENDOFTRACE, "%s :End of TRACE",
//[21:04:58] [oltrelinux.azzurra.org][275] Pragma Pragma :is using a secure connection (SSL)
#endif // _KVI_NUMERIC_H_
