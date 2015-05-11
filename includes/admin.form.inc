<?php

/**
 * @file
 * Contains the administration form for the Islandora UI Feature Pack module.
 */


function islandora_ui_admin_form($form, &$form_state) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  $configurations = islandora_ui_get_configurations();
  $form = array();
  $rows = array();
  foreach($configurations as $configuration) {
    $rows[] = array(
      l($configuration['name'], "admin/islandora/tools/ui/config/{$configuration['machine_name']}"),
      $configuration['machine_name'],
      $configuration['search_field'],
      print_r($configuration['filters'], 1),
      );
  }
  $form['table'] = array(
    '#title' => t('Islandora autocomplete configurations'),
    '#markup' => theme('table', array(
      'header' => array(
        t('Edit'),
        t('Machine name'),
        t('Search field'),
        t('Filters'),
      ),
      'rows' => $rows,
      'empty' => t('No configurations currently present.'),
    )),
  );
  $form['add_configuration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a configuration'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['add_configuration']['configuration_name'] = array(
    '#type' => 'textfield',
    '#size' => 100,
    '#title' => t('Configuration name'),
  );
  $form['add_configuration']['machine_name'] = array(
    '#type' => 'textfield',
    '#size' => 100,
    '#title' => t('Machine name'),
    '#description' => t('A unique machine name used in the exportation of features'),
  );
  $form['add_configuration']['save_content_model'] = array(
    '#type' => 'submit',
    '#value' => t('Add configuration'),
    '#name' => 'islandora_solr_metadata_add_configuration', // OMG FIX ME
  );

  return $form;
}

/**
 * Submit handler for the islandora_ui administration form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_ui_admin_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  islandora_ui_add_configuration($form_state['values']['configuration_name'], $form_state['values']['machine_name']);
  drupal_set_message(t('A new empty configuration has been created for @config_name', array('@config_name' => $form_state['values']['configuration_name'])));
  $form_state['redirect'] = '/admin/islandora/tools/ui/config/' . $form_state['values']['machine_name'];
}
