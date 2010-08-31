<?php
// $Id: qtc.author-pane.inc,v 1.1.2.4 2010/08/15 16:57:26 quiptime Exp $

/**
 * @file
 * Author Pane module compatibility.
 *
 *   Provides
 *   - An link "Chat with user".
 *   - An QTChat like user online status indicator.
 */

/**
 * Implementation of hook_preprocess_author_pane().
 */
function qtc_preprocess_author_pane(&$variables) {
  $name = $variables['account']->realname ? $variables['account']->realname : $variables['account']->name;
  $variables['account_name'] = l($name, 'user/'. $variables['account']->uid, array('attributes' => array('title' => t('View user profile.'))));
  $variables['qtc_invite_to_chat_link'] = _qtc_invite_to_chat_author_pane($variables['account']);
  $variables['qtc_user_status'] = _qtc_user_status_author_pane($variables['account']);
}

/**
 * Help function to get chat link.
 *
 * @see qtc_preprocess_author_pane()
 */
function _qtc_invite_to_chat_author_pane($account) {
  $invite_title = variable_get('qtc_chat_invite_title', 1);
  $link_text = _qtc_chat_invite_title($invite_title);
  $chat_link = qtc_chat_link($account->uid, $account->name, $account->realname, $link_text, TRUE);

  return $chat_link;
}

/**
 * Help function to get QTChat like user online status.
 *
 * @see qtc_preprocess_author_pane()
 */
function _qtc_user_status_author_pane($account) {
  $status = qtc_online_status('view', $account->uid);
  $indicator_class = '';
  $message = '';

  switch ($status) {
    case '0':
      $indicator_class = 'user-offline';
      $message = t('Offline');
      break;
    case '1':
      $indicator_class = 'user-online';
      $message = t('Available');
      break;
    case '3':
      // Status invisible is the same as offline.
      $indicator_class = 'user-offline';
      $message = t('Offline');
      break;
    case '4':
      $indicator_class = 'user-busy';
      $message = t('Busy');
      break;
  }
  
  $online_status = '<span class="'. $indicator_class .'"> '. $message .'</span>';

  return $online_status;
}