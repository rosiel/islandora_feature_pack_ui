<?php
/**
 * @file
 * Houses the confirm_form for the islandora_ui configuration form.
 */

/**
 * The delete confirm_form for the islandora_ui module.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array containing the Drupal form state.
 * @param int $machine_name
 *   The ID of the configuration we are currently editing.
 *
 * @return array
 *   An array representing the configuration form.
 */
function islandora_ui_delete_config_form($form, &$form_state, $machine_name) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  $form_state['configuration_machine_name'] = $machine_name;
  $configuration = islandora_ui_retrieve_configuration($machine_name);
  $form_state['configuration_label'] = $configuration['label'];
  return confirm_form($form,
    t('Are you sure you want to delete the @label autocomplete configuration?', array('@label' => $configuration['label'])),
    "admin/islandora/tools/ui/config/$machine_name",
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for the islandora_ui delete confirm_form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_ui_delete_config_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_ui', 'includes/db');
  islandora_ui_delete_configuration($form_state['configuration_machine_name']);
  $form_state['redirect'] = 'admin/islandora/tools/ui';
  drupal_set_message(t('The @configuration_name autocomplete configuration has been deleted!', array('@configuration_name' => $form_state['configuration_label'])));
}
