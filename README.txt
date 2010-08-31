// $Id: README.txt,v 1.1.2.6 2010/08/02 23:11:13 quiptime Exp $

QTChat - a Facebook style chat.

Requirements
--------------------------------------------------------------------------------
- This module is written for Drupal 6.0+.

- The "QTChat Views" module (qtc_views) provides Views module integration.

Installation
--------------------------------------------------------------------------------
Copy the QTChat module folder to your module directory and then enable on the
admin modules page.
 
Administration
--------------------------------------------------------------------------------
1. Administer the QTChat settings.

   - Delete chat messages

     The module database contains all chat messages. To delete the messages you
     can configure a time value.

     The functionality to delete chat messages requires a cron task.

   - Delete chat histories

     Chat histories are session based.

     If a chat window is closed then deleted the chat histories according to
     this timing - this refers to the user who has chatted.

2. Administer the QTChat permissions.

Documentation
--------------------------------------------------------------------------------
Module documentation: http://qtchat.quiptime.com/

Credits
--------------------------------------------------------------------------------
The qtc.js script is inspired by an idea by Anant Garg.
The module code is inspired by code of r0kawa and smd1704 (Google code).
Thanks for the inspiration.

Maintainer
--------------------------------------------------------------------------------
Quiptime Group
Siegfried Neumann
www.quiptime.com
quiptime [ at ] gmail [ dot ] com

Fork
--------------------------------------------------------------------------------

for www.info-notes.com
