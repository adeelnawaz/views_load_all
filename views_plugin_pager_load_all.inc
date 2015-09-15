<?php

/**
 * @file
 *
 * The plugin to handle Load All pager.
 *
 * @ingroup views_pager_plugins
 */

class views_plugin_pager_load_all extends views_plugin_pager_full {

  /**
   * Summary title overwrite.
   */
  function summary_title() {
    if (!empty($this->options['offset'])) {
      return format_plural($this->options['items_per_page'], 'Load all pager, @count item, skip @skip', 'Load all pager, @count items, skip @skip', array('@count' => $this->options['items_per_page'], '@skip' => $this->options['offset']));
    }
    return format_plural($this->options['items_per_page'], 'Load all pager, @count item', 'Load all pager, @count items', array('@count' => $this->options['items_per_page']));
  }

  /**
   * Options definition overwrite.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['waypoint'] = array(
      'contains' => array(
        'infinite' => array('default' => FALSE),
      ),
    );
    $options['all_button_text'] = array(
      'default' => t('Load all'),
      'translatable' => TRUE,
    );
    $options['all_button_empty_text'] = array(
      'default' => '',
      'translatable' => TRUE,
    );
    $options['advance'] = array(
      'contains' => array(
        // @todo change name to content_selector
        'content_class' => array('default' => ''),
        'pager_selector' => array('default' => ''),
      ),
    );
    $options['effects'] = array(
      'contains' => array(
        'type' => array('default' => 'none'),
        'speed' => array('default' => ''),
      ),
    );
    return $options;
  }

  /**
   * Options form overwrite.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Keep items per page as the first form element on the page followed by
    // the option to change the 'load all' button text
    $form['items_per_page']['#weight'] = -2;

    // Option for users to specify the text used on the 'load all' button.
    $form['all_button_text'] = array(
      '#type' => 'textfield',
      '#title' => t('All link text'),
      '#description' => t('The text that will be displayed on the link that is used to load all elements. For example "Show me all"'),
      '#default_value' => $this->options['all_button_text'] ? $this->options['all_button_text'] : t('Load all'),
      '#weight' => -1,
    );
    // Option for users to specify the text used on the 'load all' button when no mor result is found.
    $form['all_button_empty_text'] = array(
      '#type' => 'textfield',
      '#title' => t('All link text when empty'),
      '#description' => t('The text that will replace the link that is used to load all elements when there is no more element to load. For example "No more results"'),
      '#default_value' => $this->options['all_button_empty_text'] ? $this->options['all_button_empty_text'] : '',
      '#weight' => -1,
    );

    if (module_exists('waypoints')) {
      $form['waypoint'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
        '#title' =>  t('Waypoint Support'),
        '#input' => TRUE,
        '#description' => t('Waypoints is a small jQuery plugin that makes it easy to execute a function whenever you scroll to an element.'),
      );
      $form['waypoint']['infinite'] = array(
        '#type' => 'checkbox',
        '#title' => t('Infinite scrolling'),
        '#description' => t('Load all content when the user reaches the bottom of the page.'),
        '#default_value' => $this->options['waypoint']['infinite'],
      );
    }

    // Advanced options, override default selectors.
    $form['advance'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      '#title' =>  t('Advanced Options'),
      '#input' => TRUE,
      '#description' => t('Configure advanced options.'),
    );
    // Option to specify the content_class, which is the wrapping div for views
    // rows.  This allows the JS to both find new rows on next pages and know
    // where to put them in the page.
    // @todo change name to content_selector
    $form['advance']['content_class'] = array(
      '#type' => 'textfield',
      '#title' => t('Content selection selector'),
      '#description' => t('jQuery selector for the rows wrapper, relative to the view container.  Use when overriding the views markup.  Note that Views Load All requires a wrapping element for the rows.  Unless specified, Views Load All will use <strong><code>&gt; .view-content</code></strong>.'),
      '#default_value' => $this->options['advance']['content_class'],
    );
    // Option to specify the pager_selector, which is the pager relative to the
    // view container.
    $form['advance']['pager_selector'] = array(
      '#type' => 'textfield',
      '#title' => t('Pager selector'),
      '#description' => t('jQuery selector for the pager, relative to the view container.  Use when overriding the pager markup so that Views Load All knows where to find and how to replace the pager.  Unless specified, Views Load All will use <strong><code>.pager-load-all</code></strong>.'),
      '#default_value' => $this->options['advance']['pager_selector'],
    );
    // Affect the way that Views Load All adds new rows
    $form['effects'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      '#title' =>  t('JQuery Effects'),
      '#input' => TRUE,
    );
    $form['effects']['type'] = array(
      '#type' => 'select',
      '#options' => array(
        'none' => t('None'),
        'fade' => t('Fade'),
      ),
      '#default_vaue' => 'none',
      '#title' => t('Effect Type'),
      '#default_value' => $this->options['effects']['type'],
    );
    $form['effects']['speed'] = array(
      '#type' => 'select',
      '#options' => array(
        'slow' => t('Slow'),
        'fast' => t('Fast'),
      ),
      '#states' => array(
        'visible' => array(
          '#edit-pager-options-effects-type' => array('value' => t('fade')),
        ),
      ),
      '#title' => t('Effect Speed'),
      '#default_value' => $this->options['effects']['speed'],
    );
  }

  /**
   * render overwrite.
   */
  function render($input) {
    if ($this->current_page > 0) {
      return;
    }
    if (module_exists('waypoints') && $this->options['waypoint']['infinite'] == 1) {
      $viewsLoadAll = array();
      $waypoint_opts = array(
        'offset' => '100%',
      );
      drupal_alter('views_load_all_waypoint_opts', $waypoint_opts, $this->view);
      $viewsLoadAll[$this->view->name . '-' . $this->view->current_display] = array(
        'view_name' => $this->view->name,
        'view_display_id' => $this->view->current_display,
        'waypoints' => 'infinite',
        'opts' => $waypoint_opts,
      );
      drupal_add_library('waypoints', 'waypoints');
      drupal_add_js(array('viewsLoadAll' => $viewsLoadAll), 'setting');
    }

    $pager_theme = views_theme_functions('views_load_all_pager', $this->view, $this->display);

    $vars = array(
      'parameters' => $input,
      'element' => $this->options['id'],
      'all_button_text' => $this->options['all_button_text'],
      'all_button_empty_text' => $this->options['all_button_empty_text'],
    );

    return theme($pager_theme, $vars);
  }
  
  public function query()
  {
    parent::query();

    if ($this->current_page > 0) {
      $this->view->query->set_limit(0);
      $this->view->query->set_offset(0);
    }
  }

}
