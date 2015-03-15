Thanks to Ehtyar, now we have win32 builds of the latest version:
  * http://ehti.org/qwit/qwit-1.1-pre2-win32-portable.zip
  * http://ehti.org/qwit/qwit-1.1-pre2-win32.zip


The latest version of Qwit is 1.1-pre. If everything will be ok, it will become 1.1. This version has some minor bugfixes as well as experimental OAuth support for Twitter. Also, because of OAuth, this version depends on QOAuth (http://wiki.github.com/ayoy/qoauth/), which in turn depends on QCA. Binary archives contain everything needed, users, compiling Qwit from source will need appropriate libraries.

Latest version features:
  * depends on Qt4 only;
  * KDE native look;
  * http-proxy support;
  * userpics caching;
  * clickable links in messages;
  * public, replies, custom user timelines, search;
  * caching messages between sessions;
  * replies, retweets and direct messages support;
  * customizable interface;
  * twitpic support;
  * customizable automatic update of all tabs;
  * url shorteners support;
  * multiple accounts support;
  * identi.ca, aitu.kz and other twitter-compatible services support (not tested as much as needed);
  * OAuth support.

Official Qwit microblog, where all major (and some minor) changes will be posted: http://twitter.com/_qwit_

Qwit also has a page on KDE-Apps.org: http://www.kde-apps.org/content/show.php/Qwit?content=90792

And I announce all releases and its' features in my blog: http://arti.kz/category/projects/qwit/ (it's in Russian, but you always can look at [Google-translated-version](http://translate.google.com/translate?client=tmpg&hl=en&u=http%3A%2F%2Farti.kz%2Fcategory%2Fprojects%2Fqwit%2F&langpair=ru|en))

**Note**. Primary Qwit's platform is Linux. Despite this I always compile it for Windows (and some guys also compile it for MacOS), but now I cannot guarantee that it works well and looks good there, so use it on your own risk.

Debian users can use these repositories to install Qwit (thanks to Salvatore Ansani):

  * deb http://ansani.it/debian/ stable contrib
  * deb http://ansani.it/debian/ testing contrib
  * deb http://ansani.it/debian/ unstable contrib
  * deb http://ansani.it/debian/ experimental contrib

Also, Debian users can find Qwit in unstable distribution (Sid) (thanks to Carlos Galisteo).

Mandriva users can use these packages (thanks to tigger-gg):

  * [ftp://ftp.mandrivauser.de/rpm/GPL/2009.1/i586/release/qwit-r162-1mud2009.1.i586.rpm](ftp://ftp.mandrivauser.de/rpm/GPL/2009.1/i586/release/qwit-r162-1mud2009.1.i586.rpm)
  * [ftp://ftp.mandrivauser.de/rpm/GPL/2009.1/x86_64/release/qwit-r162-1mud2009.1.x86_64.rpm](ftp://ftp.mandrivauser.de/rpm/GPL/2009.1/x86_64/release/qwit-r162-1mud2009.1.x86_64.rpm)


Qwit-1.1-pre2 under Gentoo, KDE4

![http://pics.kz/s1/bd/8b/02/bd8b02b385303d3b41d50e914e6b4208.png](http://pics.kz/s1/bd/8b/02/bd8b02b385303d3b41d50e914e6b4208.png)

Qwit-0.8 under MacOS

![http://arti.kz/wp-content/uploads/2009/05/qwit-0.8-macos.jpg](http://arti.kz/wp-content/uploads/2009/05/qwit-0.8-macos.jpg)

## Prerequisites ##

To successfully compile Qwit you should have development version of Qt4.

## Compiling & Installing Qwit ##

Usual way

  1. Checkout or download and unpack Qwit-source into, let's say $TMP\_DIR
  1. Change current directory to $TMP\_DIR
  1. If you want to change default installation prefix (/usr/bin), put it to qwit.pro (target.path variable)
  1. run `qmake` - this will create Makefile
  1. run `make` - this will compile Qwit sources
  1. run `make install` - this will install Qwit into chosen directory
  1. to start Qwit run `qwit` from directory you have chosen for installation

CMake way

  1. Checkout or download and unpack Qwit-source into, let's say $TMP\_DIR
  1. mkdir $TMP\_DIR/build && cd $TMP\_DIR/build
  1. run `cmake ..` - this will create Makefile
  1. run `make` - this will compile Qwit sources
  1. run `make install` - this will install Qwit into chosen directory
  1. to start Qwit run `qwit`

Or just use binary package.

## CONTACTS ##

Project's homepage: http://code.google.com/p/qwit/

For submitting issues, please, use: http://code.google.com/p/qwit/issues/

If you want to contact author or provide some help, use my email or GTalk / Jabber: artem.iglikov@gmail.com

Also, some additional information can be found on my blog (mostly in Russian): http://arti.kz/

## LICENSE ##

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.