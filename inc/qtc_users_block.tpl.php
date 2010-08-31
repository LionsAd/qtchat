<?php
// $Id: qtc_users_block.tpl.php,v 1.1.2.2 2010/08/22 18:41:20 quiptime Exp $

/**
 * @file
 * Template file for the QTChat users block.
 */

/**
 * The block displays the online users.
 * 
 *   The listed users are only refreshed with a reload of the page.
 * 
 * Available variables:
 *
 * @param string $users
 *   The rendered online user list.
 * @param int $block_height
 *   An CSS value to configure the height of the user list.
 * @param string $title
 *   Optional an title for the user list.
 *
 * @see theme_qtc_users_block()
 */
?>
<div class="item-list">
  <?php if (isset($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <div id="qtc-user-block-list-height-wrapper" class="block-list-height-<?php print $block_height; ?>">
    <?php print $users; ?>
  </div>
</div>