# Introduction #

Here you are going to learn the basic steps required to translate Qwit's GUI to your language. The most important steps will be described and some suggestions will be given.

# Details #

## Before you start ##

  1. Ensure that you've got the _latest version of the source code_.
  1. Download an _icon_ for your translation from http://www.famfamfam.com/lab/icons/flags/
  1. Install and run _Qt Linguist_, a user-friendly application used to locate messages visible to the user and prepare the translations.

## Translating the user interface ##

First, go to the `translations` directory and copy English translation (`qwit_en_US.ts`) under an appropriate name (e.g. `qwit_pl_PL.ts`). If the translation was already there, you will probably want to run `lupdate qwit.pro` from the project's root directory, as far as something might have changed since you last updated your working directory.

Next, open the `*.ts` file in Qt Linguist and use it to go through all of the messages and names visible to the user, writing new ones or correcting any mistakes. Qt Linguist has got a very nice help (with a simple tutorial), so I won't dive into details here.

The icon with your country's flag will be in the `png` directory inside of a ZIP file you can download from the address given above.

## What to do next? ##

When the translation is finished, release it (via the `lrelease` command or Qt Linguist's menu), add resources to the project, e.g.:
  * `translations/qwit_pl_PL.qm`
  * `images/countries/pl.png`
and then rebuild it.