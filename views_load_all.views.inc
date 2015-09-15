<?php
/**
 * @file
 *
 *  Provides the views plugin information.
 */

/**
 * Implements hook_views_plugins().
 */
function views_load_all_views_plugins() {
  return array(
    'module' => 'views_load_all',
    'pager' => array(
      'load_all' => array(
        'title' => t('Load All Pager'),
        'help' => t('views_load_all'),
        'help topic' => '',
        'handler' => 'views_plugin_pager_load_all',
        'uses options' => TRUE,
        'parent' => 'full',
      ),
    ),
  );
}
