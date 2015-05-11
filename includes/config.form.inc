<?php

function islandora_ui_admin_config_form($form, &$form_state, $machine_name) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  // Not sure what this is for, but whatever.
  $filter_to_add = FALSE;

  // AJAX callback handling

  if (isset($form_state['triggering_element'])) {
    if ($form_state['triggering_element']['#name'] == 'islandora-ui-filters-remove-selected') {
      $rows_to_remove = array();
      foreach ($form_state['values']['islandora_ui_filters']['table_wrapper']['table'] as $key => $row) {
        if ($row !== 0) {
          $rows_to_remove[] = $key;
        }
      }
      foreach ($rows_to_remove as $key) {
        unset($form_state['complete form']['islandora_ui_filters']['table_wrapper']['table']['#options'][$key]);
      }
    }

    if ($form_state['triggering_element']['#name'] == 'islandora-ui-filters-add-filter') {
      $filter_to_add = array(
        'predicate' => $form_state['values']['islandora_ui_filters']['table_wrapper']['add_fieldset']['predicate'],
        'value' => $form_state['values']['islandora_ui_filters']['table_wrapper']['add_fieldset']['value'],
        );
    }

  }

// BUILD THE FORM.
  $form = array(
    '#tree' => TRUE,
  );
  $form['islandora_ui_configuration_machine_name'] = array(
    '#type' => 'value',
    '#value' => $machine_name,
  );

  $configuration = islandora_ui_retrieve_configuration($machine_name);

  $form['islandora_ui_label'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration Label'),
    '#description' => t('Human-readable label for this configuration'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );
  $form['islandora_ui_label']['label'] = array(
    '#type' => 'textfield',
    //'#description' => t('A field within solr. Should probably be one that is split text, or edged text.'),
    '#size' => 105,
    //'#autocomplete_path' => 'islandora_solr/autocomplete_luke',
    '#default_value' => $configuration['label'],
  );
  $form['islandora_ui_search_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search field'),
    '#description' => t('Solr field in which to search in this configuration'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );
  $form['islandora_ui_search_field']['search_field'] = array(
    '#type' => 'textfield',
    '#description' => t('A field within solr. Should probably be one that is split text, or edged text.'),
    '#size' => 105,
    '#autocomplete_path' => 'islandora_solr/autocomplete_luke',
    '#default_value' => $configuration['search_field'],
  );
  // FILTERS
  $form['islandora_ui_filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filters'),
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
  );
  $form['islandora_ui_filters']['table_wrapper'] = array(
    '#prefix' => '<div id="islandora-ui-filters-wrapper">',
    '#suffix' => '</div>',
  );
  $filters = array();
  // If there are values in the form_state use them for persistence in case of
  // AJAX callbacks, otherwise grab fresh values from the database.
  if (isset($form_state['values'])) {
    if (isset($form_state['values']['islandora_ui_filters']['table_wrapper']['table'])) {
      $filters = $form_state['complete form']['islandora_ui_filters']['table_wrapper']['table']['#options'];
    }
  }
  else {
      if (is_array($configuration['filters'])) {
        foreach($configuration['filters'] as $filter) {
          $key = $filter['predicate'] . ':' . $filter['value'];
          $filters[$key] = array('predicate' => $filter['predicate'], 'value' => $filter['value']);
        }
      }
  }

  if ($filter_to_add !== FALSE) {
    $key = $filter_to_add['predicate'] . ':' . $filter_to_add['value'];
    $filters[$key] = $filter_to_add;
  }


  $form['islandora_ui_filters']['table_wrapper']['table'] = array(
    '#type' => 'tableselect',
    '#header' => array(
      'predicate' => array('data' => t('Solr Field')),
      'value' => array('data' => t('Value')),
    ),
    '#options' => $filters,
    '#empty' => t('No filters associated.'),
  );
  if (count($filters)) {
    $form['islandora_ui_filters']['table_wrapper']['remove_selected'] = array(
      '#type' => 'button',
      '#value' => t('Remove selected'),
      '#name' => 'islandora-ui-filters-remove-selected',
      '#ajax' => array(
        'callback' => 'islandora_ui_filters_ajax',
        'wrapper' => 'islandora-ui-filters-wrapper',
      ),
    );
  }
  $form['islandora_ui_filters']['table_wrapper']['add_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add filter and value'),
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
  );
  $form['islandora_ui_filters']['table_wrapper']['add_fieldset']['predicate'] = array(
    '#type' => 'textfield',
    '#title' => t('Filter field'),
    '#description' => t('A field from within Solr'),
    '#size' => 105,
    '#autocomplete_path' => 'islandora_solr/autocomplete_luke',
    '#default_value' => '',
  );
  $form['islandora_ui_filters']['table_wrapper']['add_fieldset']['value'] = array(
    '#type' => 'textfield',
    '#title' => 'Value',
    '#description' => t('The value this field should have. Can include wildcards such as *.'),
    '#size' => 105,
    '#default_value' => '',
  );
  $form['islandora_ui_filters']['table_wrapper']['add_fieldset']['add_field'] = array(
    '#type' => 'button',
    '#value' => t('Add'),
    '#name' => 'islandora-ui-filters-add-filter',
    '#ajax' => array(
      'callback' => 'islandora_ui_filters_ajax',
      'wrapper' => 'islandora-ui-filters-wrapper',
    ),
  );

  $form['islandora_ui_save'] = array(
    '#type' => 'submit',
    '#name' => 'islandora-ui-save-button',
    '#value' => t('Save configuration'),
    '#weight' => 10,
  );
  $form['islandora_ui_delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete configuration'),
    '#name' => 'islandora-ui-delete-button',
    '#weight' => 10,
  );
  return $form;
}

function islandora_ui_admin_config_form_validate($form, $form_state) {
  // Add Field. Consider this for add filter.

  if (!$form_state['values']['islandora_ui_label']['label']) {
    form_set_error('islandora_ui_label][label', t('Label must be set.'));
  }
  if ($form_state['triggering_element']['#name'] == 'islandora-ui-filters-add-filter') {
    module_load_include('inc', 'islandora_solr_search', 'includes/luke');
    $solr_luke = islandora_solr_get_luke();
    $luke_fields = array_keys($solr_luke['fields']);
    $predicate_to_add = $form_state['values']['islandora_ui_filters']['table_wrapper']['add_fieldset']['predicate'];
    $value_to_add = $form_state['values']['islandora_ui_filters']['table_wrapper']['add_fieldset']['value'];

    // Check if the new value is a real field.
    if (!in_array($predicate_to_add, $luke_fields)) {
      form_set_error('islandora_ui_filters][table_wrapper][add_fieldset][predicate', t('The field @field is not a valid field within Solr!', array('@field' => $predicate_to_add)));
    }
    else {
      // Then, check if it's already in the list.
      $added_values = array();
      if (isset($form_state['values']['islandora_ui_filters']['table_wrapper']['table'])) {
        foreach ($form_state['values']['islandora_ui_filters']['table_wrapper']['table'] as $key => $delete) {
          if (!$delete) {
            $added_values[] = $key;
          }

        }
      }
      if (in_array($predicate_to_add . ":" . $value_to_add, $added_values)) {
        form_set_error('islandora_ui_filters][table_wrapper][add_fieldset', t('The filter @filter already exists in this configuration!', array('@filter' => $predicate . ":" . $value)));
      }
    }
  }


  if ($form_state['triggering_element']['#name'] == 'islandora-ui-filters-remove-selected') {
    $rows_to_remove = array();
    foreach ($form_state['values']['islandora_ui_filters']['table_wrapper']['table'] as $key => $row) {
      if ($row !== 0) {
        $rows_to_remove[] = $key;
      }
    }
    if (count($rows_to_remove) === 0) {
      form_set_error('islandora_ui', t('Must select at least one entry to remove!'));
    }
  }
}

/**
 * Submit handler for the islandora_ui configuration form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_ui_admin_config_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  $machine_name = $form_state['values']['islandora_ui_configuration_machine_name'];

  if ($form_state['clicked_button']['#name'] == 'islandora-ui-save-button') {
    // Grab existing entries first for comparison.
    $configuration = islandora_ui_retrieve_configuration($machine_name);

    // set stuff from form_state.
    $configuration_fs = array(
      'label' => '',
      'machine_name' => '',
      'filters' => NULL,
      'search_field' => '',
    );
    $configuration_fs['search_field'] = $form_state['values']['islandora_ui_search_field']['search_field'];
    $configuration_fs['machine_name'] = $form_state['values']['islandora_ui_configuration_machine_name'];
    $configuration_fs['label'] = $form_state['values']['islandora_ui_label']['label'];

    if (isset($form_state['values']['islandora_ui_filters']['table_wrapper']['table'])) {
      $filters_fs = $form_state['values']['islandora_ui_filters']['table_wrapper']['table'];
    }
    else {
      $filters_fs = array();
    }
    $filters = array();
    foreach($filters_fs as $key => $deleted) {
      $filters[] = $form_state['complete form']['islandora_ui_filters']['table_wrapper']['table']['#options'][$key];

    }
    $configuration_fs['filters'] = $filters;
    drupal_write_record('islandora_ui_autocomplete', $configuration_fs, 'machine_name');
    drupal_set_message(t('The Solr metadata display configuration options have been saved.'));
  }

  if ($form_state['clicked_button']['#name'] == 'islandora-ui-delete-button') {
    drupal_goto("admin/islandora/tools/ui/config/delete/$machine_name");
  }
}

/**
 * AJAX callback for the content models portion of the configuration form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array representing the Drupal form state.
 *
 * @return array
 *   An array representing the portion of the form we are rendering.
 */
function islandora_ui_filters_ajax(&$form, &$form_state) {
  return $form['islandora_ui_filters']['table_wrapper'];
}
