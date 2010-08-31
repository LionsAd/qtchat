<?php
// $Id: qtc.theme.inc,v 1.1.2.8 2010/08/28 11:47:25 quiptime Exp $

/**
 * @file
 * Theme functions for the qtc.module
 */

/**
 * Implementation of hook_theme().
 */
function qtc_theme($existing, $type, $theme, $path) {
  return array(
    'qtc_users_block' => array(
      'arguments' => array(
        'users' => NULL,
        'block_height' => NULL,
        'title' => NULL
      )
    ),
    'qtc_friends_block' => array(
      'arguments' => array(
        'friends' => NULL,
        'status' => NULL,
        'block_height' => NULL,
        'title' => NULL
      )
    ),
    'qtc_toggler_block' => array(
      'arguments' => array(
        'toggler' => NULL,
        'title' => NULL
      )
    ),
    'qtc_history_popup' => array(
      'arguments' => array(
        'content' => NULL
      ),
      'template' => 'qtc-history-popup',
    ),
    'qtc_barfly_title' => array(
      'arguments' => array(
        'color' => NULL
      )
    ),
    'qtc_barfly_options_header' => array(
      'arguments' => array(),
    ),
    'qtc_barfly_lists_toggler' => array(
      'arguments' => array(
        'params' => NULL
      )
    ),
    'qtc_barfly_options_toggler' => array(
      'arguments' => array(),
    ),
    'qtc_sticky_friendlist' => array(
      'arguments' => array(
        'checked' => NULL
      )
    ),
    'qtc_chat_bar_friendlist' => array(
      'arguments' => array(
        'data' => NULL,
        'count' => NULL
      )
    ),
    'qtc_chatbox_title' => array(
      'arguments' => array(
        'truncated_fn_name' => NULL,
        'uid' => NULL,
        'indicator' => NULL
      )
    )
  );
}

/**
 * Provides the display of the QTChat bar.
 *
 * @return string
 *   The rendered chat bar.
 */
function qtc_chat_bar($check_vis = true) {
  if (!variable_get('qtc_chat_bar', 0) || !_qtc_chatbar_vis() || !_qtc_chatbar_show()) {
    return '';
  }

  global $user;

  $settings = array();
  $params = array();
  $options = '';

  $settings['friend_module']          = variable_get('qtc_chat_friend_module', 'no');
  $settings['chat_bar']               = variable_get('qtc_chat_bar', 0);
  $settings['status_variant']         = variable_get('qtc_online_status_variant', false);
  $settings['invisible_status_allow'] = variable_get('qtc_invisible_status_allow', 1);
  $settings['busy_status_allow']      = variable_get('qtc_busy_status_allow', 1);

  $params['display_options']     = false;
  $params['display_options_adv'] = false;

  $params['online_status']      = qtc_online_status('view', $user->uid);
  $params['qtc_chatbar_cookie'] = $_COOKIE['qtc_chatbar'];

  $params['qtc_color'] = variable_get('qtc_color', '#F77B05');

  // Check are enabled an views display for chat counter.
  if (variable_get('qtc_chat_bar_counter', false)) {
    $settings['view'] = variable_get('qtc_chat_bar_counter', false);
    $to_embed = explode('|', $settings['view']);
    $params['counter'] = views_embed_view($to_embed[0], $to_embed[1]);
  }
  elseif (!variable_get('qtc_chat_bar_counter', false) && $settings['friend_module'] != 'no') {
    $friends_data = qtc_chat_user_online_data();
    $params['counter'] = $friends_data['counter'];
  }

  // Check user has enabled sticky friend list display.
  if (!$params['qtc_chatbar_cookie'] || $params['qtc_chatbar_cookie'] == 'unsticky') {
    $params['barfly_class'] = '';
    $params['barfly_style'] = 'display: none;';
  }
  if ($params['qtc_chatbar_cookie'] == 'sticky') {
    $params['barfly_class'] = 'qtc-barfly-show';
    $params['barfly_style'] = '';
  }

  // Provides internal parameters.
  // 1 online/available, 3 (invisible) offline, 4 busy
  if ($settings['chat_bar']) {
    if ($params['online_status'] == 1) {
      $params['online_toggle'] = 'inline';
      $params['invisible_toggle'] = 'none';
      $params['busy_toggle'] = 'none';
    }
    if ($params['online_status'] == 3 && $settings['invisible_status_allow']) {
      $params['online_toggle'] = 'none';
      $params['invisible_toggle'] = 'inline';
      $params['busy_toggle'] = 'none';
    }
    if ($params['online_status'] == 4 && $settings['busy_status_allow']) {
      $params['online_toggle'] = 'none';
      $params['invisible_toggle'] = 'none';
      $params['busy_toggle'] = 'inline';
    }
  }

  // Parameter to specify whether the base options are shown.
  // The base options are only available if the option "Yes, accept users status" is chosen and
  // if enabled "Allow invisible status" and/or "Allow busy status".
  if ($settings['status_variant']) {
    if (($settings['invisible_status_allow'] || $settings['busy_status_allow'])) {
      $params['display_options'] = true;
    }
  }

  if ($settings['friend_module'] != 'no') {
    $params['display_options_adv'] = true;
  }

  if (!$params['display_options'] && !$params['display_options_adv']) {
    $params['display_options_class'] = 'barfly-options-none';
  }
  else {
    $params['display_options_class'] = 'barfly-options-exist';
  }

  // Friend list options.
  // Part of flying chat bar content.
  // Currently no functions developed.
  $list_options = '';
  $get_list_options = qtc_chat_bar_list_options();

  if (count($get_list_options)) {
    $params['list_options_toggle'] = 'inline';
    // Coming soon.
  }
  else {
    $params['list_options_toggle'] = 'none';
  }

  // Options.
  // Part of flying chat bar content.
  $options = '<div id="qtc-barfly-options" style="display: none;">';

  $options .= qtc_barfly_options_header($params);

  $options .= '<div id="qtc-barfly-options-wrapper">';

  // The base option;
  // Allow users to change their online status.
  $options .= qtc_chat_bar_options($params);

  // Advanced options;
  // At the moment only sticky/unsticky friendlist option.
  $options .= qtc_chat_bar_options_adv($settings, $params);

  $options .= '</div>';
  $options .= '</div>';

  // Build the flying chat bar content.
  $barfly = '<div id="qtc-barfly" style="'. $params['barfly_style'] .'" class="'. $params['barfly_class'] .' '. $params['display_options_class'] .'">'; // barfly

  $barfly .= qtc_barfly_title($settings, $params); // barffly-title

  $barfly .= '<div id="qtc-barfly-header" class="clear-block">'; // barfly-header
  $barfly .= '<div class="clear-block">';
  // Toggle the friends list options.
  // Options coming soon.
  $barfly .= qtc_barfly_lists_toggler($params);
  // Toggle the base options.
  // - Online status
  // - Sticky/unsticky chat bar friend list.
  $barfly .= qtc_barfly_options_toggler($params);
  $barfly .= '</div>';
  $barfly .= $options;
  $barfly .= '</div>'; // barfly-header

  $barfly .= '<div id="qtc-barfly-body" style="height: auto;" class="clear-block">'; // barfly-body
  $barfly .= '<div id="qtc-barfly-body-content" class="clear-block">'; // barfly-body-content

  // Additional content.
  // Upcoming base option features.
  if ($additional_content) {
    $barfly .= '<div id="qtc-barfly-additional-content" class="clear-block">';
    $barfly .= '';
    $barfly .= '</div>';
  }

  // Chat bar online friends list.
  $barfly .= qtc_chat_bar_friendlist($settings);

  $barfly .= '<div class="qtc-barfly-friends-list-message" style="display: none;">'. t('Loading data ...') .'</div>';

  $barfly .= '</div>'; // barfly-body-content
  $barfly .= '</div>'; // barfly-body

  $barfly .= '</div>'; // barfly

  // Build chat bar content.
  $bar = '<div id="qtc-bar-onoffline" class="'. $params['display_options_class'] .'">';
  $bar .= '<div id="qtc-bar-onoffline-wrapper" class="clear-block">';

  // This div contains (CSS) the user online/offline icon.
  $bar .= '<div id="qtc-bar-online-indicator" class="qtc-user-online" style="display: '. $params['online_toggle'] .'; float: left"> </div>';
  $bar .= '<div id="qtc-bar-online-indicator" class="qtc-user-invisible" style="display: '. $params['invisible_toggle'] .'; float: left"> </div>';
  $bar .= '<div id="qtc-bar-online-indicator" class="qtc-user-busy" style="display: '. $params['busy_toggle'] .'; float: left"> </div>';

  $bar .= '<div class="qtc-bar-user-online-text" style="display: inline; float: left">'. t('Chat').': </div>';

  // This div contains (CSS) the user online counter.
  $bar .= '<div class="qtc-bar-user-online-counter" style="display: inline; float: left">'. $params['counter'] .'</div>';

  $bar .= '</div>';
  $bar .= '</div>';

  // Add flying content.
  $bar .= $barfly;

  $output = '<div id="qtc-bar-container">';
  $output .= '<div id="qtc-bar-sub">';
  $output .= '<div id="qtc-bar">';
  $output .= $bar;
  $output .= '</div>';
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Provides an online/offline toggle to allow an user to switch their online status.
 *
 *   The toggle status function mainly usable in blocks.
 * 
 * @param string $id
 *   An CSS id part.
 *   This CSS id part is usable in the corresponding jQuery functionality.
 * @return string
 *   The rendered toggle content.
 *
 * @see qtc_friends_block()
 *
 * @TODO Reactivate this.
 */
function qtc_onoffline_toggler($id) {
  global $user;

  $settings = array();
  $params = array();

  $settings['status_variant'] = variable_get('qtc_online_status_variant', false);
  $settings['invisible_status_allow'] = variable_get('qtc_invisible_status_allow', 1);
  $settings['busy_status_allow'] = variable_get('qtc_busy_status_allow', 1);

  $params['online_status'] = qtc_online_status('view', $user->uid);

  $output .= '<div id="'. $id .'-onoffline-wrapper" class="clear-block">';

  // 1 online/available, 3 invisible, 4 busy
  if ($params['online_status'] == 1 && ($settings['status_variant'] == 0 || $settings['status_variant'] == 1)) {
    $params['toggle_online'] = 'block';
    $params['toggle_invisible'] = 'none';
    $params['toggle_busy'] = 'none';
  }
  if ($params['online_status'] == 3 && $settings['invisible_status_allow']) {
    $params['toggle_online'] = 'none';
    $params['toggle_invisible'] = 'block';
    $params['toggle_busy'] = 'none';
  }
  if ($params['online_status'] == 4 && $settings['busy_status_allow']) {
    $params['toggle_online'] = 'none';
    $params['toggle_invisible'] = 'none';
    $params['toggle_busy'] = 'block';
  }

  // The block content.
  $output .= '<div id="'. $id .'-toggler-off">';

  $output .= '<div class="qtc-toggler-status-message-online" style="display: '. $params['toggle_online'] .';">'. l(t('You are available'), 'my-chat-status', array('attributes' => array('title' => t('Choose your chat status')))) .'</div>';
  $output .= '<div class="qtc-toggler-status-message-invisible" style="display: '. $params['toggle_invisible'] .';">'. l(t('You are invisible'), 'my-chat-status', array('attributes' => array('title' => t('Choose your chat status')))) .'</div>';
  $output .= '<div class="qtc-toggler-status-message-busy" style="display: '. $params['toggle_busy'] .';">'. l(t('You are busy'), 'my-chat-status', array('attributes' => array('title' => t('Choose your chat status')))) .'</div>';
  // Div contains the status icon.
  $output .= '<div id="qtc-toggler-online-indicator" class="'. $id .'-online" style="display: '. $params['toggle_online'] .';"> </div>';
  $output .= '<div id="qtc-toggler-online-indicator" class="'. $id .'-invisible" style="display: '. $params['toggle_invisible'] .';"> </div>';
  $output .= '<div id="qtc-toggler-online-indicator" class="'. $id .'-busy" style="display: '. $params['toggle_busy'] .';"> </div>';
  
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Provides the QTC users block content.
 *
 * @param array $items
 *   The array contains the online users.
 * @param array $params
 *   The array contains values to configure the block content.
 * @param string $title
 *   Optional an title for the user list.
 *
 * @see qtc_block()
 */
function qtc_users_block($items, $params, $title = false) {
  global $user;

  $settings = array();
  $users = '';

  $settings['chat_invite_title'] = _qtc_chat_invite_title(variable_get('qtc_chat_invite_title', 1));

  // Build the user list content.
  $users .= '<ul>'."\n";

  if (empty($items)) {
    $users .= '<li id="qtc-user-empty">'. t('There are no active users.') .'</li>'."\n";
  }
  else {
    foreach ($items as $i) {
      $name_toggle = qtc_user_name($i->name, $i->realname);
      if ($i->name != $user->name) {
        $users .= '<li id="qtc-user">';
        $users .= '<a class="qtc-chat-invite" href="javascript:void(0)" title="'. $settings['chat_invite_title'] .'" onclick="javascript:chatWith(\''. addslashes($i->uid) .'\', \''. addslashes($i->name) .'\', \''. $name_toggle .'\')">'. $name_toggle .'</a>';
        $users .= '</li>'."\n";
      }
      else {
        $users .= '<li id="qtc-user">'. $name_toggle .'</li>'."\n";
      }
    }
  }
  $users .= '</ul>'."\n";

  return theme('qtc_users_block', $users, $params['block_list_height'], $title);
}

/**
 * Provides the online friends block.
 *
 *   If no friend module enabled the block is no available.
 *
 * @param array $items
 *   The array contains the online friends.
 * @param array $params
 *   The array contains values to configure the block content.
 * @param string $title
 *   Optional an title for the friends list.
 *
 * @see qtc_block()
 */
function qtc_friends_block($items, $params, $title = false) {
  global $user;

  $friends = '';
  $params = array();
  $params['status'] = '';
  $settings = array();

  $settings['chat_invite_title'] = _qtc_chat_invite_title(variable_get('qtc_chat_invite_title', 1));
  $settings['chat_bar'] = variable_get('qtc_chat_bar', 0);

  // Provides the online status of the current user if the chat bar not displayed.
  if (!$settings['chat_bar'] || !_qtc_chatbar_vis()) {
    // Don't change the function parameter.
    $params['status'] = qtc_onoffline_toggler('qtc-friends-block');
  }

  if (empty($items)) {
    $friends .= '<div id="qtc-friends-empty">'. t('There are no online friends.') .'</div>'."\n";
  }
  else {
    foreach ($items as $i) {
      $name_toggle = qtc_user_name($i['name'], $i['realname']);
      if ($i['name'] != $user->name) {
        $friends .= '<a class="qtc-chat-invite" href="javascript:void(0)" style="display: block;" title="'. $settings['chat_invite_title'] .'" onclick="javascript:chatWith(\''. addslashes($i['uid']) .'\', \''. addslashes($i['name']) .'\', \''. $name_toggle .'\')">'. $name_toggle .'</a>';
      }
    }
  }

  return theme('qtc_friends_block', $friends, $params['status'], $params['block_list_height'], $title);
}

/**
 * Provides an block to allow users to switch their online status.
 *
 * @param string $title
 *   Optional an title for the block content.
 *
 * @see qtc_block()
 */
function qtc_toggler_block($title = false) {
  // Don't change the function parameter.
  $toggler = qtc_onoffline_toggler('qtc-toggler-block');

  return theme('qtc_toggler_block', $toggler, $title);
}

/**
 * Provides the toggle function to display in the chat bar options.
 *
 * @see qtc_chat_bar()
 */
function qtc_barfly_lists_toggler($params) {
  return theme('qtc_barfly_lists_toggler', $params);
}

/**
 * Provides the toggle function to display the chat bar options.
 *
 * @see qtc_chat_bar()
 */
function qtc_barfly_options_toggler($params) {
  $output = '';

  if (!$params['display_options'] && !$params['display_options_adv']) {
    return $output;
  }

  $output = theme('qtc_barfly_options_toggler');

  return $output;
}

/**
 * Provides the chat bar friends list options.
 *
 * @return array
 *   The array contains the available rendered list options.
 *
 * @see qtc_chat_bar()
 */
function qtc_chat_bar_list_options() {
  $list_options = array();

  // Coming soon.

  return $list_options;
}

/**
 * Provides the chat bar base options.
 *
 *   This options allow users to change their online status.
 *
 * @return array
 *   The array contains the available rendered options.
 *
 * @see qtc_chat_bar()
 */
function qtc_chat_bar_options($params) {
  if (!$params['display_options'] || !$params['display_options_adv']) {
    return false;
  }

  global $user;

  $output = '';
  $options = array();
  $settings = array();
  $params = array();

  $settings['chat_bar'] = variable_get('qtc_chat_bar', 0);

  // Option online status.
  $settings['status_variant'] = variable_get('qtc_online_status_variant', false);

  $settings['invisible_status_allow'] = variable_get('qtc_invisible_status_allow', 1);
  $settings['busy_status_allow'] = variable_get('qtc_busy_status_allow', 1);

  $settings['online_status'] = qtc_online_status('view', $user->uid);

  // 1 online/available, 3 invisible (offline), 4 busy
  if ($settings['chat_bar']) {
    if ($settings['online_status'] == 1 && ($settings['status_variant'] == 0 || $settings['status_variant'] == 1)) {
      $params['online_toggle'] = 'none';
      $params['online_toggle2'] = 'block';
      $params['invisible_toggle'] = 'block';
      $params['invisible_toggle2'] = 'none';
      $params['busy_toggle'] = 'block';
      $params['busy_toggle2'] = 'none';
    }
    if ($settings['online_status'] == 3 && $settings['invisible_status_allow']) {
      $params['online_toggle'] = 'block';
      $params['online_toggle2'] = 'none';
      $params['invisible_toggle'] = 'none';
      $params['invisible_toggle2'] = 'block';
      $params['busy_toggle'] = 'block';
      $params['busy_toggle2'] = 'none';
    }
    if ($settings['online_status'] == 4 && $settings['busy_status_allow']) {
      $params['online_toggle'] = 'block';
      $params['online_toggle2'] = 'none';
      $params['invisible_toggle'] = 'block';
      $params['invisible_toggle2'] = 'none';
      $params['busy_toggle'] = 'none';
      $params['busy_toggle2'] = 'block';
    }
  }

  $output_online = '<a id="qtc-barfly-online-indicator-on" title="'. t('Make available status') .'" style="display: '. $params['online_toggle'] .';" class="qtc-status-go-online" onclick="javascript:setUserStatus(\'1\', \'1\', \''. $user->uid .'\')" href="javascript:void(0)"><div>'. t('Be available') .'</div></a>';
  $output_online .= '<div class="qtc-status-go-offline" style="display: '. $params['online_toggle2'] .';"><div>'. t('Available') .'</div></div>';
  // Status 3, invisible
  $output_invisible = '<a id="qtc-barfly-invisible-indicator-on" title="'. t('Make invisible status') .'" style="display: '. $params['invisible_toggle'] .';" class="qtc-status-go-invisible-off" onclick="javascript:setUserStatus(\'3\', \'3\', \''. $user->uid .'\')" href="javascript:void(0)"><div>'. t('Be invisible') .'</div></a>';
  $output_invisible .= '<div class="qtc-status-go-invisible-on" style="display: '. $params['invisible_toggle2'] .';"><div>'. t('Invisible') .'</div></div>';
  // Status 4, busy
  $output_busy = '<a id="qtc-barfly-busy-indicator-on" title="'. t('Make busy status') .'" style="display: '. $params['busy_toggle'] .';" class="qtc-status-go-busy-off" onclick="javascript:setUserStatus(\'4\', \'4\', \''. $user->uid .'\')" href="javascript:void(0)"><div>'. t('Be busy') .'</div></a>';
  $output_busy .= '<div class="qtc-status-go-busy-on" style="display: '. $params['busy_toggle2'] .';"><div>'. t('Busy') .'</div></div>';

  if ($settings['status_variant'] == 0 || $settings['status_variant'] == 1) {
    $options['online_status'] = '<div class="qtc-barfly-action-status-toggle">'. $output_online .'</div>';
  }
  if ($settings['invisible_status_allow']) {
    $options['invisible_status'] = '<div class="qtc-barfly-invisible-status-toggle">'. $output_invisible .'</div>';
  }
  if ($settings['busy_status_allow']) {
    $options['busy_status'] = '<div class="qtc-barfly-busy-status-toggle">'. $output_busy .'</div>';
  }

  if (count($options) > 1) {
    $output = '<div id="qtc-barfly-options-base">';
    $output .= implode($options);
    $output .= '</div>';
  }
  
  return $output;
}

/**
 * Provides the chat bar advanced options.
 *
 *   - Sticky friendlist
 * 
 * @return array
 *   The array contains the available rendered options.
 *
 * @see qtc_chat_bar()
 */
function qtc_chat_bar_options_adv($settings, $params) {
  $options = array();

  if ($settings['friend_module'] != 'no') {
    $qtc_chatbar_cookie = $_COOKIE['qtc_chatbar'];
    
    if ($qtc_chatbar_cookie == 'sticky') {
      $checked = 'checked="checked"';
    }
    if (!$qtc_chatbar_cookie || $qtc_chatbar_cookie == 'unsticky') {
      $checked = '';
    }

    $options['sticky_friendlist'] = theme('qtc_sticky_friendlist', $checked);
  }

  if (count($options)) {
    $output .= '<hr class="list-divider" />';
    $output .= '<div id="qtc-barfly-options-adv">';
    $output .= implode($options);
    $output .= '</div>';
  }

  return $output;
}

/**
 * Provides the chat bar friends list.
 *
 * @param array $settings
 *   An associative array contains settings values.
 * @return string
 *   The rendered list of online friends.
 *
 * @see qtc_chat_bar()
 */
function qtc_chat_bar_friendlist($settings) {
  $output = '';

  if ($settings['friend_module'] == 'no') {
    return $output;
  }

  $settings = array();
  $params = array();

  $settings['chatbar_friend_list_height'] = variable_get('qtc_chatbar_friend_list_height', 6);

  $params['invite_title'] = _qtc_chat_invite_title(variable_get('qtc_chat_invite_title', 1));
  $friends_online = _qtc_chat_friends_online();

  $i = 0;
  foreach($friends_online['friends'] as $uid => $friend) {
    // Provides various internal parameters.
    if ($friend['status'] == 1) {
      $params['status'] = 'online';
    }
    if ($friend['status'] == 0 || $friend['status'] == 3) {
      // Invisible is the same as offline.
      $params['status'] = 'offline';
    }
    if ($friend['status'] == 4) {
      $params['status'] = 'busy';
    }
    
    // The friends to include in the list to display.
    if ($friend['status'] == 1 || $friend['status'] == 4) {
      $friends .= '<a onclick="javascript:chatWith(\''. $uid .'\', \''. $friend['name'] .'\', \''. $friend['realname'] .'\')" style="" class="qtc-chat-invite status-'. $params['status'] .'" title="'. $params['invite_title'] .'" href="javascript:void(0)">';
      $friends .= '<span class="truncate-friendname">'. qtc_truncate($friend['realname'], variable_get('qtc_truncate_chat_name', 20)) .'</span>';
      $friends .= '</a>';

      ++$i;
    }
  }

  if (!count($friends)) {
    $friends = '<span id="qtc-friends-empty">'. t('No online friends.') .'</span>';
    $params['display_friends_list_class'] = 'display-friends-list-none';
  }
  else {
    $params['display_friends_list_class'] = 'display-friends-list-exist';
  }

  // Provides height value for scrollable list functionality;
  if ($i <= $settings['chatbar_friend_list_height']) {
    $params['chatbar_friends_list_height'] = 'auto';
  }
  if ($i > $settings['chatbar_friend_list_height']) {
    $params['chatbar_friends_list_height'] = $settings['chatbar_friend_list_height'];
  }

  $output = '<div id="qtc-barfly-friends-list" class="clear-block '. $params['display_friends_list_class'] .'">';
  $output .= '<div id="qtc-barfly-friends-list-wrapper">';
  // Scrollable list function.
  $output .= '<div id="qtc-barfly-friends-list-height-wrapper" class="block-list-height-'. $params['chatbar_friends_list_height'] .'">';

  $output .= theme('qtc_chat_bar_friendlist', $friends, $i);
  $output .= '</div>';
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Provides the chat bar options header.
 *
 * @see qtc_chat_bar()
 */
function qtc_barfly_options_header($params) {
  $output = '';

  if (!$params['display_options'] && !$params['display_options_adv']) {
    return $output;
  }

  $output = '<div id="qtc-barfly-options-header" class="'. $params['display_options_class'] .'">';
  $output .= theme('qtc_barfly_options_header');
  $output .= '</div>';

  return $output;
}

/**
 * Provides the chat bar flying content title.
 *
 * @see qtc_chat_bar()
 */
function qtc_barfly_title($settings, $params) {
  $output = '';

  if (!$params['display_options'] && !$params['display_options_adv']) {
    return $output;
  }

  $output = theme('qtc_barfly_title', $params['qtc_color']);

  return $output;
}

/**
 * Theme function to display the chat box user name.
 *
 * @param string $truncated_fn_name
 *   Truncated user name or realname.
 * @param int $t_id
 *   ID of user to chat with (To ID).
 * @param string $indicator
 *  An CSS class part to make an class unique.
 *
 * @see qtc_ajax_data()
 */
function theme_qtc_chatbox_title($truncated_fn_name, $t_id, $indicator) {
  $output = '';

  // Contains the status icon.
  $output .= '<span id="chatboxtitle-status_'. $t_id .'" class="chat-status-'. $indicator .' online-status" style="float: left"></span>';
  // Name of user to chat with.
  $output .= '<span class="chatboxtitle-name">'. $truncated_fn_name .'</span>';

  return $output;
}

/**
 * Displays the toggle function to display the chat bar options.
 */
function theme_qtc_barfly_lists_toggler($params) {
  $output = '';

  $output = '<div id="qtc-barfly-lists-toggler" style="display: '. $params['list_options_toggle'] .'; float: left;">'. t('Friend lists') .'</div>';

  return $output;
}

/**
 * Displays the toggle function to display the chat bar options.
 */
function theme_qtc_barfly_options_toggler() {
  $output = '';

  $output = '<div id="qtc-barfly-options-toggler" style="display: inline; float: left;">'. t('Options') .'</div>';

  return $output;
}

/**
 * Displays the chat bar flying content title.
 *
 * @param string $color
 *   An CSS color value.
 */
function theme_qtc_barfly_title($color) {
  $output = '';
  
  $output = '<div id="qtc-barfly-title" style="background-color: '. $color .';">';
  $output .= '<div id="qtc-barfly-title-wrapper" class="clear-block">';
  $output .= '<div class="qtc-barfly-header-title">'. 'Chat' .'</div>';
  $output .= '<div class="qtc-barfly-apps-hide"> - </div>';
  $output .= '</div>';
  $output .= '</div>';
  
  return $output;
}

/**
 * Displays the chat bar options header.
 */
function theme_qtc_barfly_options_header() {
  $output = '';

  $output = '<div id="qtc-barfly-options-header-wrapper" class="clear-block">';
  $output .= '<div class="qtc-barfly-option-close"> X </div>';
  $output .= '</div>';

  return $output;
}

/**
 * Displays the sticky/unsticky option in the chat bar.
 * 
 * @param string $checked
 *   An check box value.
 * 
 * @see qtc_chat_bar_options_adv()
 */
function theme_qtc_sticky_friendlist($checked) {
  $output = '';

  $output = '<div id="qtc-barfly-sticky-friendlist" class="clear-block">';
  $output .= '<div id="qtc-barfly-sticky-friendlist-wrapper">';
  $output .= '<div class="input-box-sticky">';
  $output .= '<input type="checkbox" '. $checked .' id="qtc-setting-sticky-friendlist" name="qtc-setting-sticky-friendlist">';
  $output .= '</div>';
  $output .= '<label for="qtc-setting-sticky-friendlist">'. t('Friends window leave open') .'</label>';
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Displays the list of online friends in the chat bar.
 *
 * @param array $data
 *   An associative array contains the rendered friends list or
 *   the rendered information "No online friends.".
 * @param int $count
 *   An counter value of online friends.
 *
 * @see qtc_chat_bar_friendlist()
 */
function theme_qtc_chat_bar_friendlist($data, $count) {
  $output = '';

  $output = $data;
  $output .= '<span id="data-loading-message" style="display: none; margin: 8px;">'. t('Loading data ...') .'</span>';

  return $output;
}

/**
 * Displays the QTC users block.
 *
 * @param string $users
 *   The rendered online user list.
 * @param int $block_height
 *   An CSS value to configure the height of the user list.
 * @param string $title
 *   Optional an title for the user list.
 *
 * @see qtc_users_block()
 */
function theme_qtc_users_block($users, $block_height, $title = false) {
  $output = '';

  $output = '<div class="item-list">'."\n";

  if ($title) {
    $output .= '<h3>' . $title . '</h3>'."\n";
  }
  // Scrollable list functionality.
  $output .= '<div id="qtc-user-block-list-height-wrapper" class="block-list-height-'. $block_height .'">'."\n";

  $output .= $users;
  $output .= '</div>'."\n";
  $output .= '</div>'."\n";

  return $output;
}

/**
 * Displays the online friends block.
 *
 *   If no friend module enabled this block is no available.
 *
 * @param string $friends
 *   The rendered online friends list.
 * @param string $status
 *  The rendered content of online status of the current user if the chat bar not displayed.
 * @param int $block_height
 *   An CSS value to configure the height of the user list.
 * @param string $title
 *   Optional an title for the friends list.
 *
 * @see qtc_friends_block()
 */
function theme_qtc_friends_block($friends, $status, $block_height, $title = false) {
  $output = '';
  
  $output = '<div class="item-list">'."\n";

  if ($title) {
    $output .= '<h3>' . $title . '</h3>'."\n";
  }

  $output .= $status;

  $output .= '<div id="qtc-friends-block-list-wrapper">'."\n";
  $output .= '<div id="qtc-friends-block-list-height-wrapper" class="block-list-height-'. $block_height .'">'."\n";

  $output .= $friends;

  $output .= '</div>'."\n";
  $output .= '</div>'."\n";
  $output .= '</div>'."\n";
  
  return $output;
}

/**
 * Displays an block to display and allow users to switch their online status.
 *
 *   Currently the toggle function not available.
 *   Only the user status are displayed.
 *
 * @param string $toggler
 *   The rendered toggler content.
 * @param string $title
 *   Optional an title for the block content.
 *
 * @see qtc_toggler_block()
 */
function theme_qtc_toggler_block($toggler, $title = false) {
  $output = '';

  if ($title) {
    $output .= '<h3>' . $title . '</h3>'."\n";
  }

  $output .= $toggler;

  return $output;
}

/**
 * Fill in a bunch of page variables for our specialized popup page.
 */
function template_preprocess_qtc_history_popup(&$variables) {
  global $theme;

  // Construct page title.
  if (drupal_get_title()) {
    $head_title = array(strip_tags(drupal_get_title()), variable_get('site_name', 'Drupal'));
  }
  else {
    $head_title = array(variable_get('site_name', 'Drupal'));
    if (variable_get('site_slogan', '')) {
      $head_title[] = variable_get('site_slogan', '');
    }
  }

  drupal_add_css(drupal_get_path('module', 'qtc') . '/css/history-popup.css');

  $variables['head_title']        = implode(' | ', $head_title);
  $variables['base_path']         = base_path();
  $variables['front_page']        = url();
  $variables['head']              = drupal_get_html_head();
  $variables['language']          = $GLOBALS['language'];
  $variables['language']->dir     = $GLOBALS['language']->direction ? 'rtl' : 'ltr';
  $variables['css']               = drupal_add_css();
  $css = drupal_add_css();

  // Remove theme css.
  foreach ($css as $media => $types) {
    if (isset($css[$media]['theme'])) {
      $css[$media]['theme'] = array();
      //$css[$media]['module'] = array();
    }
  }

  $variables['styles']            = drupal_get_css($css);
  //$variables['scripts']           = drupal_get_js();
  $variables['title']             = drupal_get_title();
  // Closure should be filled last.
  $variables['closure']           = theme('closure');
}
