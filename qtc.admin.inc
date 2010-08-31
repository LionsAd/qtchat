<?php
// $Id: qtc.admin.inc,v 1.1.2.11 2010/08/27 23:25:20 quiptime Exp $

/**
 * @file
 * Administer QTChat.
 */

/**
 * Functions for the administration QTChat.
 *
 *  The settings areas:
 *   - Global
 *     - Messages
 *     - Module settings
 *     - Login fix
 *   - Chat
 *     - Status status
 *     - Chat bar settings
 *     - Friend modules
 *     - Other settings
 *   - Heartbeat
 */
function qtc_admin_form_global() {
  _qtc_admin_css();

  $destination = drupal_get_destination();
  $messages_count = db_result(db_query('SELECT COUNT(zid) as count FROM {qtc}'));

  $form = array();
  $description = array();

  _qtc_display_missing_permission();

  $form['messages'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Messages'),
    '#collapsible' => false,
    '#collapsed'   => false
  );
  $description['m'] = t('Age for chat messages to be deleted. The functionality to delete old chat messages requires a cron task.') .'<br />';
  $description['m'] .= '<strong>'. t('Important') .':</strong> ';
  $description['m'] .= t('Make sure that is not shorter %messages than %history.', array('%messages' => t('Delete messages'), '%history' => t('Delete histories')));
  $form['messages']['qtc_old_messages'] = array(
    '#type'          => 'select',
    '#title'         => t('Delete messages'),
    '#description'   => $description['m'],
    '#options'       => _qtc_admin_time_options(),
    '#default_value' => variable_get('qtc_old_messages', 86400),
  );

  $form['messages']['messages_info'] = array(
    '#type'  => 'markup',
    '#value' => t('The database contains <strong>!count</strong> chat messages.', array('!count' => $messages_count)),
  );
  $form['messages']['cron'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Cron'),
    '#collapsible' => false,
    '#collapsed'   => false
  );
  
  $value = t('You can <a href="@cron">run cron manually</a>.', array(
    '@cron' => url('admin/reports/status/run-cron', array('query' => $destination)))
  );
  $form['messages']['cron']['cron_run'] = array(
    '#type'  => 'markup',
    '#value' => $value,
  );
  $form['messages']['qtc_history'] = array(
    '#type'          => 'select',
    '#title'         => t('Delete histories'),
    '#description'   => t('Age for chat histories to be deleted. If a chat window is closed then deleted the chat histories according to this timing - this refers to the user who has chatted. It is a good idea, this time choose short as one believes that a user needs the history.'),
    '#options'       => _qtc_admin_time_options(),
    '#default_value' => variable_get('qtc_history', 43200),
  );

  $form['module'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Module settings'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );

  $form['module']['js_file'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('jQuery file variant'),
    '#description'   => t('The QTChat jQuery main file can be used in two variants. Recommended is the minified version.'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $form['module']['js_file']['qtc_js_file_variant'] = array(
    '#type'          => 'radios',
    '#title'         => t('Main file compression level'),
    '#description'   => t('Choose QTChat jQuery main file compression level.'),
    '#options'       => array(
      1 => t('Production (Minified)'),
      2 => t('Development (Uncompressed code)'),
    ),
    '#default_value' => variable_get('qtc_js_file_variant', 1),
  );

  $form['module']['login_fix'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Login fix'),
    '#description'   => $description_login_fix,
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $form['module']['login_fix']['qtc_login_fix'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Login fix'),
    '#size'          => 15,
    '#description'   => t('This entry is a path. Without beginning and ending slash. Leave this entry blank if not needed.'),
    '#default_value' => variable_get('qtc_login_fix', ''),
  );

  return system_settings_form($form);
}

/**
 * Administer the QTChat chat settings.
 */
function qtc_admin_form_chat() {
  $path = drupal_get_path('module', 'qtc');
  drupal_add_css($path .'/css/qtc_admin.css');
  drupal_add_js($path . '/js/qtc_admin.js');
  drupal_add_js('misc/farbtastic/farbtastic.js');
  drupal_add_css('misc/farbtastic/farbtastic.css');

  $form = array();
  $description = array();
  $friend_modules = _qtc_check_friend_modules();

  _qtc_display_missing_permission();
  
  // Status settings.
  $description['status_settings'] = t('Using the functionality to change the online status.') .'<br />';
  $description['status_settings'] .= t('This functionality is only available if the option <em>Yes, accept users status</em> is chosen and if enabled <em>Allow invisible status</em> and/or <em>Allow busy status</em>.');
  $form['status'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Status settings'),
    '#description' => $description['status_settings'],
    '#collapsible' => false,
    '#collapsed'   => false,
  );

  // Status variant.
  $form['status']['status_variant'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Status variant'),
    '#collapsible' => true,
    '#collapsed'   => true,
  );
  
  $description['status_variant'] = t('If <em>Yes, accept users status</em> selected: Users may change their online status. Status changes are visualized for other users depending on the heartbeat settings.') .'<br />';
  $description['status_variant'] .= t('If <em>No, use Drupals default</em> selected: Users may not change their online status. Status changes are visualized for other users depending on the <em>Online status time</em> setting.');
  $form['status']['status_variant']['qtc_online_status_variant'] = array(
    '#type'          => 'select',
    '#title'         => t('Online status variant'),
    '#description'   => $description['status_variant'],
    '#options'       => array(1 => t('Yes, accept users status'), 0 => t('No, use Drupals default')),
    '#default_value' => variable_get('qtc_online_status_variant', false),
  );
  $form['status']['status_variant']['qtc_online_status_variant_old'] = array(
    '#type'  => 'value',
    '#value' => variable_get('qtc_online_status_variant', false),
  );

  $description['status_time'] = t('Choose the time after which a user status is offline.') .' ';
  $description['status_time'] .= t('Note: The corresponding time of the %user-stats module is 15 minutes.', array('%user-stats' => t('User Stats'))) .'<br />';
  $description['status_time'] .= t('If selected the option <em>Accept users status</em> from the setting %status-variant, then has the setting of the user priority.', array('%status-variant' => t('Online status variant')));
  $form['status']['status_variant']['online_status']['qtc_online_status_time'] = array(
    '#type'          => 'select',
    '#title'         => t('Online status time'),
    '#description'   => $description['status_time'],
    '#options'       => _qtc_online_time_options(),
    '#default_value' => variable_get('qtc_online_status_time', 900),
  );

  $description['invisible_busy'] = t('If disabled no, or only one of the two options:') .'<br />';
  $description['invisible_busy'] .= t('Users can set your online status with the activated options.') .'<br />';
  $description['invisible_busy'] .= '<p>'. t('Both options are disabled:') .'<br />';
  $description['invisible_busy'] .= t('Users can not using the function to set their status to invisible or busy.') .'</p>';
  $form['status']['invisible_busy'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Invisible and busy'),
    '#description' => $description['invisible_busy'],
    '#collapsible' => true,
    '#collapsed'   => true,
  );
  // Invisible status.
  $form['status']['invisible_busy']['qtc_invisible_status_allow'] = array(
    '#type'          => 'select',
    '#title'         => t('Allow invisible status'),
    '#description'   => t('If enabled users can choose the status %invisible.', array('%invisible' => t('Invisible'))),
    '#options'       => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => variable_get('qtc_invisible_status_allow', 1),
  );
  $form['status']['invisible_busy']['qtc_invisible_status_allow_old'] = array(
    '#type'  => 'value',
    '#value' => variable_get('qtc_invisible_status_allow', 1),
  );

  // Busy status.
  $form['status']['invisible_busy']['qtc_busy_status_allow'] = array(
    '#type'          => 'select',
    '#title'         => t('Allow busy status'),
    '#description'   => t('If enabled users can choose the status %busy.', array('%busy' => t('Busy'))),
    '#options'       => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => variable_get('qtc_busy_status_allow', 1),
  );
  $form['status']['invisible_busy']['qtc_busy_status_allow_old'] = array(
    '#type'  => 'value',
    '#value' => variable_get('qtc_busy_status_allow', 1),
  );

  // Chat settings.
  $form['chat'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Chat bar settings'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $form['chat']['qtc_chat_bar'] = array(
    '#type'          => 'select',
    '#title'         => t('Chat bar'),
    '#description'   => t('Choose whether to use the chat bar.'),
    '#options'       => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => variable_get('qtc_chat_bar', 0),
  );

  // Chat bar visibility settings.
  $form['chat']['user_vis_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('User specific visibility settings'),
    '#collapsible' => true,
    '#collapsed'   => true,
  );
  $form['chat']['user_vis_settings']['qtc_chatbar_user_vis'] = array(
    '#type'          => 'radios',
    '#title'         => t('Custom visibility settings'),
    '#options'       => array(
                          0 => t('Users cannot control whether or not they see the Chat bar.'),
                          1 => t('Show the Chat bar by default, but let individual users hide it.'),
                          2 => t('Hide the Chat bar by default but let individual users show it.')
                        ),
    '#description'   => t('Allow individual users to customize the visibility of the Chat bar in their account settings.'),
    '#default_value' => variable_get('qtc_chatbar_user_vis', 0),
  );
  
  $form['chat']['page_vis_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Page specific visibility settings'),
    '#collapsible' => true,
    '#collapsed'   => true,
  );
  $access = user_access('use PHP for block visibility');
  if (variable_get('qtc_chatbar_page_vis', 0) == 2 && !$access) {
    $form['chat']['page_vis_settings'] = array();
    $form['chat']['page_vis_settings']['visibility'] = array('#type' => 'value', '#value' => 2);
    $form['chat']['page_vis_settings']['pages'] = array('#type' => 'value', '#value' => variable_get('qtc_chatbar_vis_list', ''));
  }
  else {
    $options = array(
      t('Show on every page except the listed pages.'),
      t('Show on only the listed pages.')
    );
    $description['vis_list'] = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));
    if ($access) {
      $options[] = t('Show if the following PHP code returns <code>true</code> (PHP-mode, experts only).');
      $description['vis_list'] .= ' '. t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    $form['chat']['page_vis_settings']['qtc_chatbar_page_vis'] = array(
      '#type'          => 'radios',
      '#title'         => t('Show Chat bar on specific pages'),
      '#options'       => $options,
      '#default_value' => variable_get('qtc_chatbar_page_vis', 0),
    );
    $form['chat']['page_vis_settings']['qtc_chatbar_vis_list'] = array(
      '#type'          => 'textarea',
      '#title'         => t('Pages'),
      '#default_value' => variable_get('qtc_chatbar_vis_list', ''),
      '#description'   => $description['vis_list'],
    );
  }

  // Chat bar friend list length.
  if (count($friend_modules)) {
    $description['qtc_chatbar_friend_list_height'] = t('Maximum number of currently online friends to display in the chat bar.') .'<br />';
    $description['qtc_chatbar_friend_list_height'] .= t('The block uses a scrollable list if the number of friends is greater than the length of the list.');
    $form['chat']['qtc_chatbar_friend_list_height'] = array(
      '#type'          => 'select',
      '#title'         => t('Friend list length'),
      '#description'   => $description['qtc_chatbar_friend_list_height'],
      '#options'       => drupal_map_assoc(array(2, 4, 6, 8, 10, 12)),
      '#default_value' => variable_get('qtc_chatbar_friend_list_height', 6),
    );
  }

  // Chat bar alternativ counter
  if (module_exists('qtc_views')) {
    $bar_counter = array(0 => t('Don\'t use an view'));
    $options_bar_counter = $bar_counter + qtc_get_views('users');

    $description['user_counter'] = t('Choose an view display to get an counter result of online users. The counter should count the online users or the online friends of the current user.') .' ';
    $description['user_counter'] .= t('The counter is used mainly in the chat bar.');
    $form['chat']['qtc_chat_user_counter'] = array(
      '#type'          => 'select',
      '#title'         => t('User counter'),
      '#description'   => $description['user_counter'],
      '#options'       => $options_bar_counter,
      '#default_value' => variable_get('qtc_chat_user_counter', false),
    );
  }

  // Friend modules.
  $collaps_fm = false;
  if (!count($friend_modules)) {
    $collaps_fm = true;
  }
  $form['friend_modules'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Friend modules'),
    '#collapsible' => $collaps_fm,
    '#collapsed'   => $collaps_fm,
  );

  // Friend modules available.
  if (count($friend_modules)) {
    $friend_module_options = array('no' => t('Do not use friend modules')) + $friend_modules;
    $form['friend_modules']['qtc_chat_friend_module'] = array(
      '#type'          => 'select',
      '#title'         => t('Friend modules'),
      '#description'   => t('Choose a friend module.'),
      '#options'       => $friend_module_options,
      '#default_value' => variable_get('qtc_chat_friend_module', 'no'),
      '#suffix'        => '<div id="friend_module-select" class="warning" style="display: none;">'. t('Changes made in the option %modules must be saved with %submit it can be configured the related options.', array('%modules' => t('Friend modules'),'%submit' => t('Save configuration'))) .'</div>',
    );
  }
  $form['friend_modules']['qtc_chat_friend_module_old'] = array(
    '#type'  => 'value',
    '#value' => variable_get('qtc_chat_friend_module', 'no')
  );

  if (!count($friend_modules)) {
    $upported = qtc_friend_modules();
    $no_friend_module = theme('item_list', $upported, t('Supported friend modules'));
    $form['friend_modules']['no_friend_module'] = array(
      '#type'  => 'markup',
      '#value' => $no_friend_module,
    );
    $form['friend_modules']['qtc_chat_friend_module'] = array(
      '#type'  => 'value',
      '#value' => 'no',
    );
  }

  if (count($friend_modules)) {
    if (variable_get('qtc_chat_friend_module', 'no') == 'flag_friend') {
      $result = db_query("SELECT fid, name, title FROM {flags} WHERE content_type = 'user' AND global = 0");
      $ff_flags = array();
      while ($type = db_fetch_object($result)) {
        $ff_flags[$type->fid] = array('name' => $type->name, 'title' => $type->title);
        $ff_options[$type->fid] = $type->title;
      }
      
      $ff_multiple = count($ff_options);
      $form['friend_modules']['qtc_chat_friend_module_flag_friend'] = array(
        '#type'          => 'select',
        '#title'         => t('Friend flags'),
        '#description'   => t('Choose one or more friend flags. At least one flag is required for this to work the friends online counter and the friends list function.'),
        '#options'       => $ff_options,
        '#multiple'      => true,
        '#size'          => $ff_multiple ? min(3, count($ff_options)) : 0,
        '#default_value' => variable_get('qtc_chat_friend_module_flag_friend', array()),
      );
    }
    elseif (variable_get('qtc_chat_friend_module', 'no') == 'user_relationships_api') {
      $ur_flags = user_relationships_types_load();
      if (count($ur_flags)) {
        foreach ($ur_flags as $key => $rel) {
          $ur_options[$rel->rtid] = $rel->plural_name;
        }

        $ur_multiple = count($ur_options);
        $form['friend_modules']['qtc_chat_friend_module_user_relationships'] = array(
          '#type'          => 'select',
          '#title'         => t('Relationships'),
          '#description'   => t('Choose one ore more user relationship types. At least one relationship is required for this to work the friends online counter and the friends list function.'),
          '#options'       => $ur_options,
          '#multiple'      => true,
          '#size'          => $ur_multiple ? min(3, count($ur_options)) : 0,
          '#default_value' => variable_get('qtc_chat_friend_module_user_relationships', array()),
        );
      }
    }
  }

  // Other settings.
  $form['other'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Other settings'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $form['other']['qtc_chat_invite_title'] = array(
    '#type'          => 'select',
    '#title'         => t('Invite title'),
    '#description'   => t('Select the text that is used to chat with a user. Example for use: Mouse over text of user links.'),
    '#options'       => array(
                          1 => t('Chat with user'),
                          2 => t('Invite to chat'),
                          3 => t('Chat'),
                        ),
    '#default_value' => variable_get('qtc_chat_invite_title', 1),
  );
  $description['login_again_status'] = t('Choose the online status when a user login again.') .'<br />';
  $description['login_again_status'] .= t('Last status: The status with which a user has logged out.');
  $form['other']['qtc_login_again_status'] = array(
    '#type'          => 'select',
    '#title'         => t('Login again status'),
    '#description'   => $description['login_again_status'],
    '#options'       => array(
                          1 => t('Status available'),
                          2 => t('Last status'),
                        ),
    '#default_value' => variable_get('qtc_login_again_status', 1),
  );

  $form['other']['truncate'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Truncate names'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $description['truncate_chatbar_name'] = t('The names of the chat messages and friends list in the chat bar.') .' ';
  $description['truncate_chatbar_name'] .= t('Please choose the character length for the name if desired.');
  $form['other']['truncate']['qtc_truncate_chat_name'] = array(
    '#type'          => 'select',
    '#title'         => t('Message and chat bar names'),
    '#description'   => $description['truncate_chatbar_name'],
    '#options'       => drupal_map_assoc(array(15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25)),
    '#default_value' => variable_get('qtc_truncate_chat_name', 20),
  );
  $description['truncate_chatwindow_name'] = t('The names in the title of a chat window.') .' ';
  $description['truncate_chatwindow_name'] .= t('Please choose the character length for the name if desired.');
  $form['other']['truncate']['qtc_truncate_chatwindow_name'] = array(
    '#type'          => 'select',
    '#title'         => t('Chat window names'),
    '#description'   => $description['truncate_chatwindow_name'],
    '#options'       => drupal_map_assoc(array(15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30)),
    '#default_value' => variable_get('qtc_truncate_chatwindow_name', 25),
  );
  $description['clickabe_chatwindow_name'] = t('The names in the title of a chat window.') .' ';
  $description['clickabe_chatwindow_name'] .= t('Clickable names refer to the user\'s account.');
  $form['other']['qtc_clickable_chatwindow_name'] = array(
    '#type'          => 'select',
    '#title'         => t('Clickable chat window names'),
    '#description'   => $description['clickabe_chatwindow_name'],
    '#options'       => array(1 => t('Not clickable'), 2 => t('Clickable')),
    '#default_value' => variable_get('qtc_clickable_chatwindow_name', 1),
  );
  $form['other']['qtc_clickable_chatwindow_avatar'] = array(
    '#type'          => 'select',
    '#title'         => t('Clickable chat window avatars'),
    '#description'   => t('Clickable avatars refer to the user\'s account.'),
    '#options'       => array(2 => t('Clickable'), 1 => t('Not clickable')),
    '#default_value' => variable_get('qtc_clickable_chatwindow_avatar', 2),
  );

  $form['other']['color'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('QTChat color'),
    '#collapsible' => true,
    '#collapsed'   => true,
  );
  $suffix_qtc_color = '<div class="colors-more">'. t('QTChat default color') .': <span id="qtc-reset-color">#F77B05</span></div>';
  $suffix_qtc_color .= '<div class="colors-more">'. t('The Facebook color') .': <span id="qtc-facebook-color">#6D84B4</span></div>';
  $form['other']['color']['qtc_color'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Color'),
    '#description'   => t('Enter a new color value or use the interface to change the color value.'),
    '#size'          => 10,
    '#maxlength'     => 7,
    '#default_value' => variable_get('qtc_color', '#F77B05'),
    '#prefix'        => '<div id="color-use"></div>',
    '#suffix'        => '<div id="more-colors">'. $suffix_qtc_color . '</div>',
  );

  unset($description);

  return system_settings_form($form);
}

/**
 * Administer the QTChat heartbeat settings.
 */
function qtc_admin_form_heartbeat() {
  _qtc_admin_css();
  
  $form = array();
  $description = array();

  _qtc_display_missing_permission();
  
  // Heatbeat settings.
  $description['heartbeat'] = t('The heartbeat time is the frequency with which the clients (browsers) connects to the server.') .'<br />';
  $description['heartbeat'] .= t('Be very careful with this settings, as it can quickly eat your server resources.');
  $form['heartbeat']= array(
    '#type'        => 'fieldset',
    '#title'       => t('Heartbeat'),
    '#description' => $description['heartbeat'],
    '#collapsible' => false,
    '#collapsed'   => false,
  );

  $form['heartbeat']['qtc_heartbeat_min']= array(
    '#type'          => 'select',
    '#title'         => t('Time min'),
    '#description'   => t('Minimum heartbeat time.'),
    '#options'       => array(2000 => '2 '. t('seconds'), 4000 => '4 '. t('seconds'), 6000 => '6 '. t('seconds'), 8000 => '8 '. t('seconds'), 9990 => '10 '. t('seconds')),
    '#default_value' => variable_get('qtc_heartbeat_min', 6000),
  );
  $form['heartbeat']['qtc_heartbeat_max']= array(
    '#type'          => 'select',
    '#title'         => t('Time max'),
    '#description'   => t('Maximum heartbeat time.'),
    '#options'       => array(40000 => '40 '. t('seconds'), 30000 => '30 '. t('seconds'), 25000 => '25 '. t('seconds'), 20000 => '20 '. t('seconds')),
    '#default_value' => variable_get('qtc_heartbeat_max', 30000),
  );
  $description['count_1'] = t('The number of client connects, after which time shall be extended for the first time.').'<br />';
  $description['count_1'] .= t('After this number is extended the heartbeat time with the factor') .' 3.';
  $form['heartbeat']['qtc_heartbeat_count_1']= array(
    '#type'          => 'select',
    '#title'         => t('Time count one'),
    '#description'   => $description['count_1'],
    '#options'       => array(15 => 15, 20 => 20, 25 => 25, 30 => 30),
    '#default_value' => variable_get('qtc_heartbeat_count_1', 15),
  );
  $description['count_2'] = t('The number of client connects to increase the time count one, as result after which time shall be extended for the second time.').'<br />';
  $description['count_2'] .= t('After this number is extended the heartbeat time with the factor') .' 4.';
  $form['heartbeat']['qtc_heartbeat_count_2']= array(
    '#type'          => 'select',
    '#title'         => t('Time count second'),
    '#description'   => $description['count_2'],
    '#options'       => array(30 => 30, 25 => 25, 20 => 20, 15 => 15),
    '#default_value' => variable_get('qtc_heartbeat_count_2', 30),
  );

  $form['heartbeat']['userdata'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('User data'),
    '#description' => t('Heartbeat time to get user data. They are user and friends lists, user and friends counter'),
    '#collapsible' => false,
    '#collapsed'   => false,
  );
  $form['heartbeat']['userdata']['qtc_heartbeat_userdata']= array(
    '#type'          => 'select',
    '#title'         => t('Data time'),
    '#description'   => t('By this time, updates the user data.'),
    '#options'       => array(60000 => '1 '. t('minute'), 90000 => '1,5 '. t('minutes'), 120000 => '2 '. t('minutes'), 150000 => '2,5 '. t('minutes'), 180000 => '3 '. t('minutes')),
    '#default_value' => variable_get('qtc_heartbeat_userdata', 120000),
  );

  unset($description);

  return system_settings_form($form);
}

/**
 * Realize reset of status settings.
 */
function qtc_settings_admin_form_chat_submit($form, $formstate) {
  // Reset status settings of all users if the corresponding setting option are disabled.
  // Reset invisible and busy.
  if (($formstate['values']['qtc_invisible_status_allow_old'] != $formstate['values']['qtc_invisible_status_allow'] && $formstate['values']['qtc_invisible_status_allow'] == 0)
     && ($formstate['values']['qtc_busy_status_allow_old'] != $formstate['values']['qtc_busy_status_allow'] && $formstate['values']['qtc_busy_status_allow'] == 0)
  ) {
    db_query('UPDATE {qtc_stats} SET status = 1 WHERE status = 3 OR status = 4');
  }
  // Reset only invisible.
  elseif ($formstate['values']['qtc_invisible_status_allow_old'] != $formstate['values']['qtc_invisible_status_allow']
     && $formstate['values']['qtc_invisible_status_allow'] == 0
  ) {
    db_query('UPDATE {qtc_stats} SET status = 1 WHERE status = 3');
  }
  // Reset only busy.
  elseif ($formstate['values']['qtc_busy_status_allow_old'] != $formstate['values']['qtc_busy_status_allow'] 
     && $formstate['values']['qtc_busy_status_allow'] == 0
  ) {
    db_query('UPDATE {qtc_stats} SET status = 1 WHERE status = 4');
  }

  // Make sure that the availability of the function to change the online status
  // is changed in the user account.
  if (($formstate['values']['qtc_online_status_variant_old'] != $formstate['values']['qtc_online_status_variant'])
     || ($formstate['values']['qtc_invisible_status_allow_old'] != $formstate['values']['qtc_invisible_status_allow'] && $formstate['values']['qtc_busy_status_allow_old'] != $formstate['values']['qtc_busy_status_allow'])
  ) {
    menu_rebuild();
  }
}

function _qtc_admin_css() {
  $path = drupal_get_path('module', 'qtc');
  drupal_add_css($path .'/css/qtc_admin.css');
}

/**
 * Display an message has no role the permission "access qtchat".
 */
function _qtc_display_missing_permission() {
  if (!qtc_access_qtchat_rules()) {
    $message = t('No role has the permission %access-qtchat.', array('%access-qtchat' => t('access qtchat')));
    drupal_set_message($message, 'error');
  }
}

/**
 * Help function to examine, what supported friend modules are enabled.
 *
 * @return array
 *   An associative array contains the available friend modules.
 */
function _qtc_check_friend_modules() {
  $friend_modules = array();

  foreach (qtc_friend_modules() as $module => $title) {
    if (module_exists($module)) {
      $friend_modules[$module] = t($title);
    }
    else {
      if (variable_get('qtc_chat_friend_module_'. $module, array())) {
        variable_del('qtc_chat_friend_module_'. $module);
      }
    }
  }

  // Functions to see:
  // flag_friend_get_friends($uid, $reset = NULL)
  // flag_friend_get_friend_count($uid, $reset = NULL)

  // user_relationship_defaults_load($reset = false); user_relationship_defaults.module
  // user_relationship_default_load($param = array()); user_relationship_defaults.module
  // user_relationships_types_load($param = array(), $reset = NULL); user_relationships_api.api.inc
  // user_relationships_type_load($param = array(), $reset = NULL); user_relationships_api.api.inc

  return $friend_modules;
}

/**
 * Help function. Provides time options.
 *
 * @return array
 *   An associative array contains the time options.
 */
function _qtc_admin_time_options() {
  return array(
    3600 => '1 '. t('hour'),
    7200 => '2 '. t('hours'),
    10800 => '3 '. t('hours'),
    21600 => '6 '. t('hours'),
    43200 => '12 '. t('hours'),
    64800 => '18 '. t('hours'),
    86400 => '24 '. t('hours'),
    172800 => '2 '. t('days'),
    259200 => '3 '. t('days'),
    345600 => '4 '. t('days'),
    432000 => '5 '. t('days'),
    518400 => '6 '. t('days'),
    604800 => '7 '. t('days'),
  );
}
