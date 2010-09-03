/* $Id:*/

/**
 * @file qtc.js
 * The script implements the client-side functions to realize the QTChat chat function.
 */

var windowFocus = true;
var un; // username
var urn; // userrealname
var chatHeartbeatCount = 0;
var minChatHeartbeat = 2000;
var maxChatHeartbeat = 30000;
var chatHeartbeatTime = minChatHeartbeat;
var heartbeatFriendlistTime = minChatHeartbeat;
var originalTitle;
var blinkOrder = 0;
var cookieName = 'qtc_window';
var cookieNameChatbar = 'qtc_chatbar';
var date = new Date();
var chatboxFocus = new Array();
var newMessages = new Array();
var newMessagesWin = new Array();
var chatBoxes = new Array();
var chatCookies = new Array();

$(document).ready(function() {
 originalTitle = document.title;
 minChatHeartbeat = Drupal.settings.qtc.minHeartbeat;
 maxChatHeartbeat = Drupal.settings.qtc.minHeartbeat;
 chatHeartbeatTime = minChatHeartbeat;
 heartbeatFriendlistTime = Drupal.settings.qtc.friendHeartbeat;

 startChatSession();
 loadChatBarFunc();

 $([window, document]).blur(function() {
  windowFocus = false;
 }).focus(function() {
  windowFocus = true;
  document.title = originalTitle;
 });
});

function loadChatBarFunc() {
 if (Drupal.settings.qtc.chatBarOptions) {
  $('div#qtc-bar-onoffline').click(function() {
   $('div#qtc-barfly').toggle().toggleClass('qtc-barfly-show');
  });
 
  $('div#qtc-barfly-title').click(function() {
   $('div#qtc-barfly').toggle().toggleClass('qtc-barfly-show');
  });
 
  $('div#qtc-barfly-options-toggler').click(function() {
   $('div#qtc-barfly').toggleClass('barfly-options-visible'); $('div#qtc-barfly-options').toggle().toggleClass('qtc-barfly-options-show');
  });
 }
 $('div.qtc-barfly-option-close').click(function() {
  $('div#qtc-barfly').toggleClass('barfly-options-visible'); $('div#qtc-barfly-options').toggle().toggleClass('qtc-barfly-options-show');
 });
 // Sticky friendlist option.
 var sticky = '';
 if (Drupal.settings.qtc.friend_m != 'no') {
  $('#qtc-barfly-sticky-friendlist-wrapper input:checkbox').click(function() {
   if ($(this).attr('checked') == true) {
    sticky = 'sticky';
   }
   if ($(this).attr('checked') == false) {
    sticky = 'unsticky';
   }
   $.cookie(cookieNameChatbar, sticky, { expires: 365, path: '/' });
  });
 }
 if (Drupal.settings.qtc.friend_m == 'no') {
  sticky = 'unsticky';
  $.cookie(cookieNameChatbar, sticky, { expires: 365, path: '/' });
 }
}

function restructureChatBoxes() {
 align = 0;
 for (x in chatBoxes) {
  if ($("div#chatbox_"+chatBoxes[x]).css('display') != 'none') {
   if (align == 0) {
    if (Drupal.settings.qtc.chatBar) {
     $("div#chatbox_"+chatBoxes[x]).css({'right': '185px', 'bottom': '0'});
    }
    else {
     $("div#chatbox_"+chatBoxes[x]).css({'right': '20px', 'bottom': '0'});
    }
   }
   else {
    if (Drupal.settings.qtc.chatBar) {
     width = (align)*(245)+19+165;
    }
    else {
     width = (align)*(245+7)+20;
    }
    $("div#chatbox_"+chatBoxes[x]).css({'right': width+'px', 'bottom': '0'});
   }
   align++;
  }
 }
}

function chatWith(t_id,chatuser,name) {
 createChatBox(false,t_id,chatuser,name);
 $("div#chatbox_"+t_id+" .chatboxtextarea").focus();
}

function emptyHistory(t_id) {
 $('div#chatbox_'+t_id+' .chatboxcontent .chatboxmessage').remove();
 $.post(Drupal.settings.qtc.baseUrl + "/qtc/history/empty");
}

//
function createChatBox(data,t_id,t_un,t_name,m,t,minimizeChatBox) {
 if ($("div#chatbox_"+t_id).length > 0) {
  if ($("div#chatbox_"+t_id+".collapsed")) {
   $('div#chatbox_'+t_id).removeClass('collapsed');
   $('div#chatbox_'+t_id).addClass('collapsible');
   $("div#chatbox_"+t_id).css({display: 'block'});
   $('div#chatbox_'+t_id+' .chatboxsubhead').css('display', 'block');
   $('div#chatbox_'+t_id+' .chatboxcontent').css('display', 'block');
   $('div#chatbox_'+t_id+' .chatboxinput').css('display', 'block');

   $("div#chatbox_"+t_id+'.collapsible .chatboxhead').css({background: Drupal.settings.qtc.color});

   reorganizeMinimizedCookie('remove',t_id);
   //restructureChatBoxes();
  }

  $("div#chatbox_"+t_id+" .chatboxtextarea").focus();
  return;
 }

 if (data == false) {
  $.ajax({
   type: "POST",
   url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/data",
   data: "t_id="+t_id,
   cache: false,
   dataType: "json",
   success: function(chatWith) {
    _createChatBox(chatWith,t_id,t_un,t_name,m,t,minimizeChatBox);
  }});
 }
 else {
  _createChatBox(data,t_id,t_un,t_name,m,t,minimizeChatBox);
 }
}

// Helper to create chatbox.
function _createChatBox(data,t_id,t_un,t_name,m,t,minimizeChatBox) {
 var avatar = data.avatar;
 var m_time;
 var status_online = '';
 var status_offline = '';
 var online_toggle = '';
 var offline_toggle = '';
 var chatboxClass = 'chatbox';
 var mainColor = Drupal.settings.qtc.color;
 var message = '';
 var messageClass = '';
 var clickName = '';
 var lineBreak = '';

 if (data.online == 0 || data.online == 3) {
  message = Drupal.t('Offline');
  messageClass = 'offline';
  mainColor = '#D8D8D8';
 }
 if (data.online == 1) {
  offline_toggle = 'none';
  message = Drupal.t('Online');
  messageClass = 'online';
 }
 if (data.online == 4) {
  message = Drupal.t('Busy');
  messageClass = 'busy';
 }
 status_online = '<div id="qtc-bar-user-stat-online_'+t_id+'" style="display: '+online_toggle+';" class="user-stat-'+messageClass+' chat-message">'+message+'</div>';

 if (Drupal.settings.qtc.chatBar) {
  chatboxClass = 'chatbar-on qtc-chatbox';
 }
 else {
  chatboxClass = 'chatbar-off qtc-chatbox';
 }

 // Clickable names.
 if (Drupal.settings.qtc.clickableChWname == 2) {
  clickName = ' clickable-yes';
 }
 if (Drupal.settings.qtc.clickableChWname == 1) {
  clickName = ' clickable-no';
 }

 $("<div />").attr({id: "chatbox_"+t_id, tid: t_id}).addClass(chatboxClass)
  .html('<div class="chatboxhead'+clickName+'" style="background-color: '+mainColor+';"> <div class="chatbox-user-show"><div class="chatboxtitle clear-block">'+data.path+'</div></div> <a class="chatbox-toggler" title="'+ Drupal.t('Hide/unhide') +'" href="javascript:void(0)" onclick="javascript:toggleChatBoxGrowth(\''+t_id+'\', \''+t_un+'\')"><div class="chatbox-hide"> <div class="chatbox-toggle">-</div></div></a> <div class="chatboxoptions"><a class="chatbox-close" title="'+ Drupal.t('Close') +'" href="javascript:void(0)" onclick="javascript:closeChatBox(\''+t_id+'\', \''+t_un+'\')"> X </a></div></div><div style="display: none;" class="message-incoming">New message</div>'+chatboxSubhead(avatar,status_online,status_offline,t_id)+'<div class="chatboxcontent-wrapper"><div class="chatboxcontent"></div><div class="chatboxinput clear-block"><div id="chatbox-inputimage"></div><div id="chatbox-inputarea"><textarea class="chatboxtextarea" onkeydown="javascript:return checkChatBoxInputKey(event,this,\''+t_id+'\', \''+t_un+'\', \''+t_name+'\');"></textarea></div></div></div>')
  .appendTo($("body"));

 if (m && typeof(m) != 'object') {
  $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxmessageheader"><span class="chatboxmessagefrom">'+truncateString('name',t_name)+':&nbsp;&nbsp;</span>&nbsp;<span class="chatboxmessagetime">'+t+'</span></span<<span class="chatboxmessagecontent">'+m+'</span></div>');
 }
 else if (m && typeof(m) == 'object') {
  for (i in m) {
   if (m[i].i == t_id) {
    m[i].fn = Drupal.t('Me');
   }
   else {
    if (m[i].fn.length > (Drupal.settings.qtc.truncateChBname + 7)) {
     lineBreak = 'display: block;';
    }
   }
   m_time = messageDate('short', m[i].t);
   $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxmessageheader"><span class="chatboxmessagefrom" style="'+lineBreak+'">'+truncateString('name',m[i].fn)+':&nbsp;&nbsp;</span>&nbsp;<span class="chatboxmessagetime">'+m_time+'</span></span><span class="chatboxmessagecontent">'+m[i].m+'</span></div>');
  }
 }

 if (data.online == 1) {
  $('div#chatbox_'+t_id).addClass('chatbox-user-online');
 }
 if (data.online == 4) {
  $('div#chatbox_'+t_id).addClass('chatbox-user-busy');
 }
 if (data.online == 0 || data.online == 3) {
  $('div#chatbox_'+t_id).addClass('chatbox-user-offline');

  $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage-offline"><span class="chatboxmessagefrom">'+Drupal.t('Chat')+':&nbsp;&nbsp;</span><span class="chatboxmessagecontent">'+Drupal.t('User is offline.')+'</span>&nbsp;<span class="chatboxmessagetime">'+messageDate('short')+'</span></div>');
 }

 // If a chat box crated: All time - the chat box class is collapsible. Add the color later. See the line below.
 $('div#chatbox_'+t_id).addClass('collapsible');

 $("div#chatbox_"+t_id).css('bottom', '0px');

 chatBoxeslength = 0;
 for (x in chatBoxes) {
  if ($("div#chatbox_"+chatBoxes[x]).css('display') != 'none') {
   chatBoxeslength++;
  }
 }

 if (chatBoxeslength == 0) {
  if (Drupal.settings.qtc.chatBar) {
    $("div#chatbox_"+t_id).css({'right': '184px', 'bottom': '0'});
  }
  else {
   $("div#chatbox_"+t_id).css({'right': '20px', 'bottom': '0'});
  }
 }
 else {
  if (Drupal.settings.qtc.chatBar) {
   width = (chatBoxeslength)*(245)+19+165;
  }
  else {
   width = (chatBoxeslength)*(245+7)+20;
  }
  $("div#chatbox_"+t_id).css({'right': width+'px', 'bottom': '0'});
 }

 chatBoxes.push(t_id);
 chatBoxeslength += 1;

 if (minimizeChatBox == 1) {
  minimizedChatBoxes = new Array();
  if ($.cookie('chatbox_minimized')) {
   minimizedChatBoxes = $.cookie('chatbox_minimized').split(/\|/);
  }
  minimize = 0;
  for (j=0;j<minimizedChatBoxes.length;j++) {
   if (minimizedChatBoxes[j] == t_id) {
    minimize = 1;
   }
  }
  if (minimize == 1) {
   $('div#chatbox_'+t_id).addClass('collapsed');
   $('div#chatbox_'+t_id).removeClass('collapsible');
   $('div#chatbox_'+t_id+' .chatboxsubhead').css('display', 'none');
   $('div#chatbox_'+t_id+' .chatboxcontent').css('display', 'none');
   $('div#chatbox_'+t_id+' .chatboxinput').css('display', 'none');
  }
  else {
   $('div#chatbox_'+t_id).addClass('collapsible');
   $('div#chatbox_'+t_id).removeClass('collapsed');

   $("div#chatbox_"+t_id+' .chatboxhead').css({background: Drupal.settings.qtc.color});
  }
 }

 // Add the chat box color.
 // If a cat box crated: All time of a collapsibe chat box is the color the QTC color.
 $("div#chatbox_"+t_id+'.collapsible .chatboxhead').css({background: Drupal.settings.qtc.color});

 chatBoxes.sort(numComparisonAsc);
 chatboxFocus[t_id] = false;

 $("div#chatbox_"+t_id+" .chatboxtextarea").blur(function() {
  chatboxFocus[t_id] = false;
  //$("div#chatbox_"+t_id+" .chatboxtextarea").removeClass('chatboxtextareaselected');
 }).focus(function() {
  chatboxFocus[t_id] = true;
  newMessages[t_id] = false;
  //$('div#chatbox_'+t_id+' .chatboxhead').removeClass('chatboxblink');
  $("div#chatbox_"+t_id+" .chatboxtextarea").addClass('chatboxtextareaselected');
 });

 $("div#chatbox_"+t_id).click(function() {
  if ($('div#chatbox_'+t_id+' .chatboxcontent').css('display') != 'none' && $('div#chatbox_'+t_id+'.chatbox-user-online').css('')) {
   $("div#chatbox_"+t_id+" .chatboxtextarea").focus();
  }
 });
 $("div#chatbox_"+t_id).show();

 var elem = $("div#chatbox_"+t_id+" .chatboxcontent");
 if (elem[0]) {
  $("div#chatbox_"+t_id+" .chatboxcontent").scrollTop($("#chatbox_"+t_id+" .chatboxcontent")[0].scrollHeight);
 }
}

function chatboxSubhead(avatar,status_online,status_offline,t_id) {
 var output = '';
 output = '<div class="chatboxsubhead clear-block">';
 output += '<span class="chat-avatar">'+avatar+'</span>';
 output += '<div class="chatbox-actions clear-block">';
 output += status_online+''+status_offline;
 output += '<a href="javascript:void(0)" onclick="javascript:emptyHistory(\''+t_id+'\')" class="history-empty-link" title="'+Drupal.t('Empty history')+'"><span>'+Drupal.t('Empty history')+'</span></a>';
 output += '<a href="'+Drupal.settings.qtc.baseUrl+'/qtc/history/show/'+t_id+'?popup=1" onclick="var w=window.open(this.href, \'chat_history_window\', \'width=400,height=500,scrollbars,resizable\'); w.focus(); return false;" class="history-show-link" title="'+Drupal.t('Chat history')+'"><span>'+Drupal.t('Chat history')+'</span></a>';
 output += '</div></div>';
 return output;
}

function chatHeartbeat() {
 var itemsfound = 0;
 if (windowFocus == false) {
  var blinkNumber = 0;
  var titleChanged = 0;
  for (x in newMessagesWin) {
   if (newMessagesWin[x] == true) {
    ++blinkNumber;
    if (blinkNumber >= blinkOrder) {
     document.title = x+' '+Drupal.t('says ...');
     titleChanged = 1;
     break;
    }
   }
  }
  if (titleChanged == 0) {
   document.title = originalTitle;
   blinkOrder = 0;
  }
  else {
   ++blinkOrder;
  }
 }
 else {
  for (x in newMessagesWin) {
   newMessagesWin[x] = false;
  }
 }
 for (x in newMessages) {
  if (newMessages[x] == true) {
   if (chatboxFocus[x] == false) {
    // FIXME: Add toggle all or none policy, otherwise it looks funny.
    $('div#chatbox_'+x+' .chatboxhead').toggleClass('chatboxblink');
   }
  }
 }

 $.ajax({
  type: "GET",
  url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/heartbeat",
  data: "t_id="+Drupal.settings.qtc.uid,
  cache: false,
  dataType: "json",
  success: function(data) {
   $.each(data.items, function(i,item) {
    if (i > 0) {
     return ;
    }
    if (item) { // Fix strange IE bug.
     t_un = item.f;
     t_id = item.i;

     if ($("div#chatbox_"+t_id).length <= 0) {
      createChatBox(false,t_id,t_un,item.fn,item.m,item.t);
     }
     if ($("div#chatbox_"+t_id).css('display') == 'none') {
      $("div#chatbox_"+t_id).css('display', 'block');
      restructureChatBoxes();
     }
     if (item.s == 1) {
      item.f = un;
     }
     if (item.s == 2) {
      $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxinfo">'+item.m+'</span></div>');
     }
     else {
      newMessages[item.fn] = true;
      newMessagesWin[item.fn] = true;
      $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxmessageheader"><span class="chatboxmessagefrom">'+item.fn+':&nbsp;&nbsp;</span>&nbsp;<span class="chatboxmessagetime">'+item.t+'</span></span><span class="chatboxmessagecontent">'+item.m+'</span></div>');

      if ($('div#chatbox_'+t_id+' .chatboxcontent').css('display') == 'none') {
       $('div#chatbox_'+t_id+' .message-incoming').show();
       $('div#chatbox_'+t_id+' .chatbox-hide').click(function() {
        $('div#chatbox_'+t_id+' .message-incoming').hide();
       });
      }
     }
     var elem = $("div#chatbox_"+t_id+" .chatboxcontent");
     if (elem[0]) {
      $("div#chatbox_"+t_id+" .chatboxcontent").scrollTop($("div#chatbox_"+t_id+" .chatboxcontent")[0].scrollHeight);
     }
     itemsfound += 1;
    }
    ++i;
   });

   chatHeartbeatCount++;

   if (itemsfound > 0) {
    chatHeartbeatTime = minChatHeartbeat;
    chatHeartbeatCount = 1;
   }
   else if (chatHeartbeatCount >= Drupal.settings.qtc.heartbeat_1) {
    chatHeartbeatTime *= 3;
    chatHeartbeatCount = 1;
   }
   else if (chatHeartbeatCount >= Drupal.settings.qtc.heartbeat_2) {
    chatHeartbeatTime *= 4;
    chatHeartbeatCount = 1;
    if (chatHeartbeatTime > maxChatHeartbeat) {
     chatHeartbeatTime = maxChatHeartbeat;
    }
   }
   setTimeout('chatHeartbeat();', chatHeartbeatTime);
 }});
}

function chatHeartbeatFriendlist(){
 var onlineBoxes = new Array();
 // Get open chat boxes.
 var boxes = $('div.qtc-chatbox');
 var openBoxes = new Array();
 $.each(boxes, function(x,item) {
  var box = item.id;
  var tid = box.substr(8);
  openBoxes[x] = tid;
 });

 var inviteTitle = Drupal.t('Chat with user');
 if (Drupal.settings.qtc.invite_title == 2) {
  inviteTitle = Drupal.t('Invite to chat');
 }
 if (Drupal.settings.qtc.invite_title == 3) {
  inviteTitle = Drupal.t('Chat');
 }

 var dataGet = false;
 if (!Drupal.settings.qtc.os_variant || Drupal.settings.qtc.os_variant == 0 || Drupal.settings.qtc.friend_m == 'no') {
  dataGet = "data_get="+openBoxes;
 }

 var friends = '';
 var friendsTruncated = '';
 var userName = '';

  $.ajax({
  type: "POST",
  url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/heartbeat-userdata",
  data: dataGet,
  cache: true, // @TODO Check GET and true with many online friends and IE
  dataType: "json",
  success: function(data) {
   data.stamp = Number(new Date());
   $(window).data('userList', data); // @TODO use this

   var statusClass = '';
   $.each(data.items, function(i,item) {
    if (item) {
     onlineBoxes[i] = i;
     userName = item.name;
     if (item.realname) {
      userName = item.realname;
     }

     if (item.status == 0 || item.status == 3) {
      statusClass = 'status-offline';
      $('div#chatbox_'+i+' .warning-busy').remove();
     }
     if (item.status == 1) {
      statusClass = 'status-online';
      $('div#chatbox_'+i+' .warning-busy').remove();
     }
     if (item.status == 4) {
      statusClass = 'status-busy';
     }

     // Used with chat bar friends list.
     friendsTruncated += '<a href="javascript:void(0)" title="'+inviteTitle+'" class="qtc-chat-invite '+statusClass+'" style="" onclick="javascript:chatWith(\''+i+'\', \''+item.name+'\', \''+item.realname+'\')"><span class="truncate-friendname">'+truncateString('name',userName)+'</span></a>';
     // Used with friends block.
     friends += '<a href="javascript:void(0)" title="'+inviteTitle+'" class="qtc-chat-invite '+statusClass+'" style="" onclick="javascript:chatWith(\''+i+'\', \''+item.name+'\', \''+item.realname+'\')"><span class="truncate-friendname">'+userName+'</span></a>';
    }
   });

   if (data.count == 0) {
    friendsTruncated = '<span id="qtc-friends-empty">'+Drupal.t('No online friends.')+'</span>';
   }

   // Chat bar friends list.
   var chatbarListHeight = 0;
   if (data.count <= Drupal.settings.qtc.chatbarFriendsListHeight) {
    $('div#qtc-barfly-friends-list-height-wrapper').removeClass();
    $('div#qtc-barfly-friends-list-height-wrapper').addClass('block-list-height-auto');
   }
   if (data.count > Drupal.settings.qtc.chatbarFriendsListHeight) {
    chatbarListHeight = Drupal.settings.qtc.chatbarFriendsListHeight;
    $('div#qtc-barfly-friends-list-height-wrapper').removeClass();
    $('div#qtc-barfly-friends-list-height-wrapper').addClass('block-list-height-'+chatbarListHeight);
   }
   // Insert current/updated list.
   $('div#qtc-barfly-friends-list-height-wrapper').html(friendsTruncated);
   // Insert counter.
   $('div.qtc-bar-user-online-counter').text(data.count);

   // Friends block friends list.
   var blockListHeight = 0;
   var friendLineHeight = $('#qtc-friends-block-list-wrapper a.qtc-chat-invite').height();
   if (data.count == 0) {
    friends = '<span id="qtc-friends-empty">'+Drupal.t('There are no online friends.')+'</span>';
   }
   // Provides flexible list height.
   if (data.count <= Drupal.settings.qtc.friendsListHeight) {
    $('div#qtc-friends-block-list-height-wrapper').removeClass();
    $('div#qtc-friends-block-list-height-wrapper').addClass('block-list-height-auto');
   }
   if (data.count > Drupal.settings.qtc.friendsListHeight) {
    blockListHeight = Drupal.settings.qtc.friendsListHeight;
    $('div#qtc-friends-block-list-height-wrapper').removeClass();
    $('div#qtc-friends-block-list-height-wrapper').addClass('block-list-height-'+blockListHeight);
   }
   // Insert current/updated list.
   $('div#qtc-friends-block-list-height-wrapper').html(friends);

   $.each(openBoxes, function(b,item) {
    if (!onlineBoxes[openBoxes[b]]) {
     if ($('div#chatbox_'+openBoxes[b]+'.collapsed').length) {
      $('div#chatbox_'+openBoxes[b]+' .chatboxhead').css({background: '#D8D8D8'});
     }
     $('span#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-online'); 
     $('span#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-invisible');
     $('span#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-busy');
     $('span#qtc-bar-user-stat-online_'+openBoxes[b]).addClass('user-stat-offline');
     $('span#qtc-bar-user-stat-online_'+openBoxes[b]).html(Drupal.t('Offline'));

     $('span#qtc-bar-user-stat-offline_'+openBoxes[b]).css('display', 'block');

     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-online'); 
     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-busy');
     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-invisible');
     $('div#chatbox_'+openBoxes[b]).addClass('chatbox-user-offline');

     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-online'); 
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-invisible');
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-busy');
     $('span#chatboxtitle-status_'+openBoxes[b]).addClass('chat-status-offline');

     // Provides an chat window message "User is offline"
     $('#chatbox_'+openBoxes[b]+' .chatboxmessage-offline').remove();
     $("div#chatbox_"+openBoxes[b]+" .chatboxcontent")
     .append('<div class="chatboxmessage-offline"><span class="chatboxmessagefrom">'+Drupal.t('Chat')+':&nbsp;&nbsp;</span><span class="chatboxmessagecontent">'+Drupal.t('User is offline.')+'</span>&nbsp;<span class="chatboxmessagetime">'+messageDate('short')+'</span></div>');
    }
    else {
     var headerClass = '';
     var statusClass = '';
     var indicatorClass = '';
     var message = '';
     if (data.items[openBoxes[b]]['status'] == 0) {
      headerClass = 'user-stat-offline';
      statusClass = 'chatbox-user-offline';
      indicatorClass = 'chat-status-offline';
      message = Drupal.t('Offline');
     }
     if (data.items[openBoxes[b]]['status'] == 1) {
      headerClass = 'user-stat-online';
      statusClass = 'chatbox-user-online';
      indicatorClass = 'chat-status-online';
      message = Drupal.t('Online');
      //$('div#chatbox_'+openBoxes[b]+'.collapsed .chatboxhead').css({background: ''});
     }
     // Map invisible to offline.
     if (data.items[openBoxes[b]]['status'] == 3) {
      headerClass = 'user-stat-offline';
      statusClass = 'chatbox-user-offline';
      indicatorClass = 'chat-status-offline';
      message = Drupal.t('Offline');
      //$('div#chatbox_'+openBoxes[b]+'.collapsed .chatboxhead').css({background: ''});
     }
     if (data.items[openBoxes[b]]['status'] == 4) {
      headerClass = 'user-stat-busy';
      statusClass = 'chatbox-user-busy';
      indicatorClass = 'chat-status-busy';
      message = Drupal.t('Busy');
      //$('div#chatbox_'+openBoxes[b]+'.collapsed .chatboxhead').css({background: ''});
     }
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).css('display', 'block');
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).show();

     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-online');
     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-offline');
     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-invisible');
     $('div#chatbox_'+openBoxes[b]).removeClass('chatbox-user-busy');
     $('div#chatbox_'+openBoxes[b]).addClass(statusClass);
     
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-online');
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-offline');
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-invisible');
     $('span#chatboxtitle-status_'+openBoxes[b]).removeClass('chat-status-busy');
     $('span#chatboxtitle-status_'+openBoxes[b]).addClass(indicatorClass);

     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-online');
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-offline');
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-invisible');
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).removeClass('user-stat-busy');
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).addClass(headerClass);
     $('div#qtc-bar-user-stat-online_'+openBoxes[b]).html(message);

     $('#chatbox_'+openBoxes[b]+' .chatboxmessage-offline').remove();
    }
   });
   setTimeout('chatHeartbeatFriendlist();', heartbeatFriendlistTime);
 }});
}

function closeChatBox(t_id,f_id) {
 $('div#chatbox_'+t_id).remove();
 $.post(Drupal.settings.qtc.baseUrl + "/qtc/ajax/close", { t_id: t_id, chatbox: f_id} , function(data) {
 });
 reorganizeMinimizedCookie('remove',t_id);
 for (i=0;i<chatBoxes.length;i++) {
  if (chatBoxes[i] == t_id) {
   chatBoxes.splice(i,1);
   chatBoxeslength -= 1;
  }
 }
 restructureChatBoxes();
}

function toggleChatBoxGrowth(t_id,t_un) {
 var newCookie;
 if ($('div#chatbox_'+t_id+' .chatboxcontent').css('display') == 'none') {
  if ($.cookie('chatbox_minimized')) {
   minimizedChatBoxes = $.cookie('chatbox_minimized').split(/\|/);
  }
  newCookie = '';
  for (i=0;i<minimizedChatBoxes.length;i++) {
   if (minimizedChatBoxes[i] == t_id) {
    minimizedChatBoxes.splice(i,1);
   }
  }
  newCookie = minimizedChatBoxes.join('|');
  $.cookie('chatbox_minimized', newCookie, { expires: 365, path: '/' });

  $('div#chatbox_'+t_id).removeClass('collapsed'); 
  $('div#chatbox_'+t_id).addClass('collapsible');

  $('div#chatbox_'+t_id+' .chatboxhead').css({background: Drupal.settings.qtc.color});

  $('div#chatbox_'+t_id+' .chatboxsubhead').css('display', 'block');
  $('div#chatbox_'+t_id+' .chatboxcontent').css('display', 'block');
  $('div#chatbox_'+t_id+' .chatboxinput').css('display', 'block');
  $("div#chatbox_"+t_id+" .chatboxcontent").scrollTop($("#chatbox_"+t_id+" .chatboxcontent")[0].scrollHeight);

 }
 else {
  newCookie = t_id;
  if ($.cookie('chatbox_minimized')) {
   newCookie += '|'+$.cookie('chatbox_minimized');
  }
  $.cookie('chatbox_minimized', newCookie, { expires: 365, path: '/' });

  $('div#chatbox_'+t_id).addClass('collapsed'); 
  $('div#chatbox_'+t_id).removeClass('collapsible');

  $('div#chatbox_'+t_id+' .chatboxhead').removeAttr('style');

  $('div#chatbox_'+t_id+' .chatboxsubhead').css('display', 'none');
  $('div#chatbox_'+t_id+' .chatboxcontent').css('display', 'none');
  $('div#chatbox_'+t_id+' .chatboxinput').css('display', 'none');
 }
}

function chatBoxDisplay() {
 var boxes = $.cookie('chatbox_minimized').split(/\|/);
 for (i=0;i<boxes.length;i++) {
  $('div#chatbox_'+boxes[i]+' .chatboxsubhead').css('display', 'none');
  $('div#chatbox_'+boxes[i]+' .chatboxcontent').css('display', 'none');
  $('div#chatbox_'+boxes[i]+' .chatboxinput').css('display', 'none');
 }
}

function reorganizeMinimizedCookie(op,t_id) {
 var boxes = $.cookie('chatbox_minimized');
 // No minimized chat box exist.
 if (boxes == null) {
  return;
 }
 
 boxes = $.cookie('chatbox_minimized').split(/\|/);
 for (i=0;i<boxes.length;i++) {
  if (boxes[i] == t_id) {
   boxes.splice(i,1);
  }
 }
 newCookie = boxes.join('|');
 $.cookie('chatbox_minimized', newCookie, { expires: 365, path: '/' });
}

function checkChatBoxInputKey(event,chatboxtextarea,t_id,t_un,to_name) {
 var currentBrowser = $.browsers.browser();
 var currentBrowserVersion = $.browsers.version.string();
 var variableHeight = 18;
 var baseHeight = 18;
 var lowerHeight = '';
 var baseBoxContentHeight = 198;
 var variableBoxContentHeight = crossBoxContentHeight(t_id);
 var warningBusy = '';
 if (event.keyCode == 13 && event.shiftKey == 0)  {
  message = $(chatboxtextarea).val();
  message = message.replace(/^\s+|\s+$/g,"");

  urn = Drupal.t('Me');
  var mTime = messageDate('short');

  $(chatboxtextarea).val('');
  $("div#chatbox_"+t_id).focus();

  if (message != '') {
   $.post(Drupal.settings.qtc.baseUrl + "/qtc/ajax/send", {t_id: t_id, to: t_un, to_name: to_name, message: message} , function(data) {
    message = message.replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/\"/g, "&quot;");

    if ($("div#chatbox_"+t_id+".chatbox-user-busy").length) {
     warningBusy = '<div class="warning-busy">'+Drupal.t('User busy. Please respect his status.')+'</div>';
    }
    $('div.warning-busy').remove();
    $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxmessageheader"><span class="chatboxmessagefrom">'+urn+':&nbsp;&nbsp;</span>&nbsp;<span class="chatboxmessagetime">'+mTime+'</span></span><span class="chatboxmessagecontent">'+message+'</span></div>'+warningBusy);

    $("div#chatbox_"+t_id+" .chatboxcontent").scrollTop($("div#chatbox_"+t_id+" .chatboxcontent")[0].scrollHeight);
   });

   $(chatboxtextarea).css('height', '18px');
   $(chatboxtextarea).css('overflow' , 'hidden');
   $("div#chatbox_"+t_id+" .chatboxcontent").height(198);
  }
  chatHeartbeatTime = minChatHeartbeat;
  chatHeartbeatCount = 1;
  return false;
 }
 if (crossScrollHeight(chatboxtextarea) > variableHeight) {
  variableHeight = crossScrollHeight(chatboxtextarea);
  variableBoxContentHeight = baseBoxContentHeight - crossScrollHeight(chatboxtextarea) + baseHeight;

  if (crossScrollHeight(chatboxtextarea) > 54) {
   $(chatboxtextarea).css('height', 54 +'px');
   $(chatboxtextarea).css('overflow-y' , 'auto');
   $("div#chatbox_"+t_id+" .chatboxcontent").height(162);
   if (currentBrowser == 'Internet Explorer' && currentBrowserVersion >= 8.0 && currentBrowserVersion < 9) {
    $("div#chatbox_"+t_id+" .chatboxcontent").height(163);
   }
  }
  else {
   $(chatboxtextarea).css('height', variableHeight +'px');
   $("div#chatbox_"+t_id+" .chatboxcontent").height(variableBoxContentHeight);
   if (currentBrowser == 'Internet Explorer' && currentBrowserVersion >= 8.0 && currentBrowserVersion < 9) {
    $("div#chatbox_"+t_id+" .chatboxcontent").height(variableBoxContentHeight + 1);
   }
  }
 }
 if (crossScrollHeight(chatboxtextarea) < variableHeight) {
  lowerHeight = (crossScrollHeight(chatboxtextarea) + 4) - baseHeight;
  variableBoxContentHeight = baseBoxContentHeight - (crossScrollHeight(chatboxtextarea) + 4) + (baseHeight * 2);
  $(chatboxtextarea).css('height', lowerHeight +'px');
  $("div#chatbox_"+t_id+" .chatboxcontent").height(variableBoxContentHeight);

  if (crossScrollHeight(chatboxtextarea) < 54) {
   $(chatboxtextarea).css('overflow' , 'hidden');
  }
 }
 if (crossScrollHeight(chatboxtextarea) == 18) {
  $(chatboxtextarea).css('height', '18px');
  $(chatboxtextarea).css('overflow' , 'hidden');
  $("div#chatbox_"+t_id+" .chatboxcontent").height(198);
 }
}

function startChatSession() {   
 $.ajax({
  type: "POST",
  url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/data",
  data: "c_id=1",
  cache: true, // false, @TODO Check this
  dataType: "json",
  success: function(data) {
   // Initiation of Heartbeat 1.
   setTimeout('chatHeartbeat();', chatHeartbeatTime);

   $.each(data.items, function(i,item) {
    if (item) {
     var t_id = item.i;

     createChatBox(item, item.t_id, item.chatuser, item.name, item.items, false, 1);
     if (item.s == 1) {
      item.f = un;
     }
     if (item.s == 2) {
      $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxinfo">'+item.m+'</span></div>');
     }
     else {
      if (item.f == data.un) {
       item.fn = Drupal.t('Me');
      }
      $("div#chatbox_"+t_id+" .chatboxcontent").append('<div class="chatboxmessage"><span class="chatboxmessagefrom">'+item.fn+':&nbsp;&nbsp;</span><span class="chatboxmessagecontent">'+item.m+'</span></div>');
     }
    }
   });
   // Display the chat box counter if no friend module used.
   if (Drupal.settings.qtc.friend_m == 'no') {
    $('div.qtc-bar-user-online-counter').text(data.count);
   }

   // Initiation of Heartbeat 2.
   setTimeout('chatHeartbeatFriendlist();', Drupal.settings.qtc.friendHeartbeat);
  }
 });
}

function goOffOnline(op,id) {
 $.ajax({
  type: "GET",
  url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/set-online-status",
  data: "os_act="+op+"&id="+id,
  success: function() {
   if (op == 0) {
    // QTC bar flying content option.
    $('#qtc-barfly-online-indicator-on').hide();
    $('#qtc-barfly-online-indicator-off').show();
    $('#qtc-bar-online-indicator').removeClass('qtc-user-online');
    $('#qtc-bar-online-indicator').addClass('qtc-user-offline');
    // Friends block.
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-on').hide();
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-off').show();
    // Toggler block.
    $('#qtc-toggler-block-onoffline-wrapper #qtc-toggler-block-toggler-on').hide();
    $('#qtc-toggler-block-onoffline-wrapper #qtc-toggler-block-toggler-off').show();
   }
   if (op == 1) {
    // QTC bar flying content option.
    $('#qtc-barfly-online-indicator-on').show();
    $('#qtc-barfly-online-indicator-off').hide();
    $('#qtc-bar-online-indicator').addClass('qtc-user-online');
    $('#qtc-bar-online-indicator').removeClass('qtc-user-offline');
    // Friends block.
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-on').show();
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-off').hide();
    // Toggler block.
    $('#qtc-toggler-block-onoffline-wrapper #qtc-toggler-block-toggler-on').show();
    $('#qtc-toggler-block-onoffline-wrapper #qtc-toggler-block-toggler-off').hide();
   }
   // User account; edit online status form.
   $('#edit-status-status selected').remove();
   $('#edit-status-status').val(op);
  }
 });
 $.sleep(3, function() {});
}

function setUserStatus(act,op,id) {
 $.ajax({
  type: "GET",
  url: Drupal.settings.qtc.baseUrl + "/qtc/ajax/set-online-status",
  data: "act="+act+"&op="+op+"&id="+id,
  success: function() {
   // User account; edit online status form.
   $('#edit-status-status selected').remove();
   $('#edit-status-status').val(op);
   // Action id's: 1 online, 3 invisible, 4 busy
   if (act == 1) {
    // QTC bar flying content option.
    $('#qtc-barfly-online-indicator-on').hide();
    $('.qtc-status-go-offline').show();
    $('.qtc-status-go-invisible-on').hide();
    $('#qtc-barfly-invisible-indicator-on').show();
    $('.qtc-status-go-busy-on').hide();
    $('#qtc-barfly-busy-indicator-on').show();
    // Chat bar.
    $('#qtc-bar-onoffline-wrapper .qtc-user-online').show(); 
    $('#qtc-bar-onoffline-wrapper .qtc-user-invisible').hide();
    $('#qtc-bar-onoffline-wrapper .qtc-user-busy').hide();
    // Toggler block.
    $('.qtc-toggler-block-online').show();
    $('.qtc-toggler-block-invisible').hide();
    $('.qtc-toggler-block-busy').hide();
    $('.qtc-toggler-status-message-online').show();
    $('.qtc-toggler-status-message-invisible').hide();
    $('.qtc-toggler-status-message-busy').hide();
    // Friends block.
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-on').hide();
    $('#qtc-friends-block-onoffline-wrapper #qtc-friends-block-toggler-off').show();
   }
   if (act == 3) {
    $('.qtc-status-go-offline').hide();
    $('#qtc-barfly-online-indicator-on').show();
    $('#qtc-barfly-invisible-indicator-on').hide();
    $('.qtc-status-go-invisible-on').show();
    $('.qtc-status-go-busy-on').hide();
    $('#qtc-barfly-busy-indicator-on').show();
    // Chat bar.
    $('#qtc-bar-onoffline-wrapper .qtc-user-online').hide(); 
    $('#qtc-bar-onoffline-wrapper .qtc-user-invisible').show();
    $('#qtc-bar-onoffline-wrapper .qtc-user-busy').hide();
    // Toggler block.
    $('.qtc-toggler-block-online').hide();
    $('.qtc-toggler-block-invisible').show();
    $('.qtc-toggler-block-busy').hide();
    $('.qtc-toggler-status-message-online').hide();
    $('.qtc-toggler-status-message-invisible').show();
    $('.qtc-toggler-status-message-busy').hide();
    // Friends block. @TODO

   }
   if (act == 4) {
    $('.qtc-status-go-offline').hide();
    $('#qtc-barfly-online-indicator-on').show();
    $('.qtc-status-go-invisible-on').hide();
    $('#qtc-barfly-invisible-indicator-on').show();
    $('#qtc-barfly-busy-indicator-on').hide();
    $('.qtc-status-go-busy-on').show();
    // Chat bar.
    $('#qtc-bar-onoffline-wrapper .qtc-user-online').hide(); 
    $('#qtc-bar-onoffline-wrapper .qtc-user-invisible').hide();
    $('#qtc-bar-onoffline-wrapper .qtc-user-busy').show();
    // Toggler block.
    $('.qtc-toggler-block-online').hide();
    $('.qtc-toggler-block-invisible').hide();
    $('.qtc-toggler-block-busy').show();
    $('.qtc-toggler-status-message-online').hide();
    $('.qtc-toggler-status-message-invisible').hide();
    $('.qtc-toggler-status-message-busy').show();
    // Friends block. @TODO
    
   }
  }
 });
}

/**
 * @param op Possible values: name, window
 * @param string The string to truncate.
 */
function truncateString(op,string) {
 var truncatedString = '';
 var truncateLength = '';
 if (op == 'name') {
   truncateLength = Drupal.settings.qtc.truncateChBname;
 }
 if (op == 'window') {
   truncateLength = Drupal.settings.qtc.truncateChWname;
 }
 if (string.length > truncateLength) {
  truncatedString = string.substring(0, truncateLength);
  truncatedString += '... ';
 }
 else {
  truncatedString = string;
 }
 return truncatedString;
}

/**
 * @param elem Element to get the height.
 */
function crossScrollHeight(elem) {
 var h = elem.scrollHeight;
 var currentBrowser = $.browsers.browser();
 var currentBrowserVersion = $.browsers.version.string();
 if (currentBrowser == 'Internet Explorer' && currentBrowserVersion >= 7.0 && currentBrowserVersion < 8) {
  h += 2;
 }
 if (currentBrowser == 'Internet Explorer' && currentBrowserVersion == 8.0 && currentBrowserVersion < 9) {
  h += 3;
 }
 return h;
}

/**
 * @param t_id An user ID of user to chat.
 */
function crossBoxContentHeight(t_id) {
 var h = $("div#chatbox_"+t_id+" .chatboxcontent").height();
 var currentBrowser = $.browsers.browser();
 var currentBrowserVersion = $.browsers.version.string();
 if (currentBrowser == 'Internet Explorer' && currentBrowserVersion == 8.0 && currentBrowserVersion < 9) {
  h -= 3;
 }
 return h;
}

/**
 * @param format String; Possible values: short, long
 * @param convert An timestamp to buid an given date.
 */
function messageDate(format,convert) {
 var mTime = '';
 var d = new Date();
 if (convert) {
  d.setTime(convert * 1000);
 }
 var day = d.getDate();
 var month = d.getMonth() + 1;
 var year = d.getFullYear();
 var hour = d.getHours();
 var minute = d.getMinutes();
 var second = d.getSeconds();
 var monthname = new Array(Drupal.t('January'), Drupal.t('February'), Drupal.t('March'), Drupal.t('April'), Drupal.t('May'), Drupal.t('June'), Drupal.t('July'), Drupal.t('August'), Drupal.t('September'), Drupal.t('October'), Drupal.t('November'), Drupal.t('December'));
 if (minute < 10) {
  minute = '0'+minute;
 }
 if (format == 'short' || !format) {
  mTime = hour+':'+minute;
 }
 if (format == 'long') {
  mTime = hour+':'+minute+' '+monthname[month]+' '+day;
 }
 return mTime;
}

function numComparisonAsc(a, b)	{
 return a-b;
}

(function($) {
 $.fn.found = function(fFunction) {
  if(this.length) {
   fFunction.call(this);
  }
 };
})(jQuery);

(function($) {
 $.fn.notfound = function(nfFunction) {
  if(!this.length) {
   nfFunction.call(this);
  }
 };
})(jQuery);

/**
 * Sleep function.
 *
 * Usage:
 * $.sleep(3, function() {
 *  alert ('I slept for 3 seconds.');
 * });
 */
(function($) {
 var sleeptimer;
 $.sleep = function (time2sleep, callback) {
  $.sleep.sleeptimer = time2sleep;
  $.sleep.cback = callback;
  $.sleep.timer = setInterval('$.sleep.count()', 1000);
 };
 $.extend ($.sleep, {
  current_i: 1,
  sleeptimer: 0,
  cback: null,
  timer: null,
  count: function() {
   if ($.sleep.current_i === $.sleep.sleeptimer) {
    clearInterval($.sleep.timer);
    $.sleep.cback.call(this);
   }
   $.sleep.current_i++;
  }
 });
})(jQuery);

(function($) {
 $.fn.getLines = function (element) {
  return $(this).val().split("\n").length;
 }
})(jQuery);
