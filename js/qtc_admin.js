/* $Id:*/

Drupal.behaviors.QTCAdminister = function (context) {
 // Administer friend modules options.
 var friendModule = $('#edit-qtc-chat-friend-module').val();
 $('#edit-qtc-chat-friend-module').change(function() {
  var friendModule2 = $('#edit-qtc-chat-friend-module').val();
  if (friendModule2 != 0 && friendModule != friendModule2) {
   $('#friend_module-select').show();
  }
  else {
   $('#friend_module-select').hide();
  }
 });
 // Editable CTChat main color.
 $('#color-use').farbtastic('#edit-qtc-color');
}
