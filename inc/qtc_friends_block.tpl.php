<?php
// $Id: qtc_friends_block.tpl.php,v 1.1.2.2 2010/08/22 18:41:20 quiptime Exp $

/**
 * @file
 * Template file for the QTChat friends block.
 */

/**
 * The block displays the online friends of the current user.
 *
 *   The listed friends are automatically refreshed without a reload of the page.
 *
 * Available variables:
 *
 * @param string $friends
 *   The rendered online friends as list with clickable users.
 * @param string $status
 *  The rendered content of online status of the current user if the chat bar not displayed.
 * @param int $block_height
 *   An CSS value to configure the height of the user list.
 * @param string $title
 *   Optional an title for the friends list.
 *
 * @see theme_qtc_friends_block()
 */
?>
<div class="item-list">
  <?php if (isset($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <?php print $status; ?>
  <div id="qtc-friends-block-list-wrapper">
    <div id="qtc-friends-block-list-height-wrapper" class="block-list-height-<?php print $block_height; ?>">
      <?php print $friends; ?>
    </div>
  </div>
</div>