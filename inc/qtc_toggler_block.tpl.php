<?php
// $Id: qtc_toggler_block.tpl.php,v 1.1.2.2 2010/08/22 18:41:20 quiptime Exp $

/**
 * @file
 * Template file for the QTChat toggler block.
 */

/**
 * Displays an block to display and allow users to switch their online status.
 *
 *   Currently the toggle function not available.
 *   Only the user status are displayed.
 *
 * Available variables:
 * 
 * @param string $toggler
 *   The rendered toggler content.
 * @param string $title
 *   Optional an title for the block content.
 *
 * @see theme_qtc_toggler_block()
 */
?>
<?php if (isset($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php print $toggler; ?>