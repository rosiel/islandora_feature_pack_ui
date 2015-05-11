<?php


/**
 * Get all configurations available.
 *
 * @return array
 *   An array of associative arrays, each of which contains:
 *   - name: The name of the given configuration, possibly translated via
 *     i18n_string.
 *   - id: The ID of the given configuration.
 */
function islandora_ui_get_configurations() {
  $configurations = array();
  $result = db_query('SELECT machine_name, label, filters, search_field FROM {islandora_ui_autocomplete}');
  foreach ($result as $record) {
    $config_name = $record->label;
    if (module_exists('i18n_string')) {
      $i18n_id = islandora_solr_metadata_config_i18n_id($record->machine_name);
      $config_name = i18n_string_translate($i18n_id, $config_name);
    }
    // Format this as key-less arrays in order to pass it into JS as a 'list of lists'.


    $configurations[] = array(
      'name' => $config_name,
      'machine_name' => $record->machine_name,
      'search_field' => $record->search_field,
      'filters' => unserialize($record->filters),
    );
  }
  return $configurations;
}

/**
 * Create a new configuration.
 *
 * @param string $configuration_name
 *   The name of a configuration to add the the DB.
 * @param string $machine_name
 *   The unique machine name to add the the DB.
 *
 * @return int
 *   The ID with which the configuration was created in the database.
 */
function islandora_ui_add_configuration($configuration_name, $machine_name) {
  $id = db_insert('islandora_ui_autocomplete')
    ->fields(
      array(
        'label' => $configuration_name,
        'machine_name' => $machine_name,
      )
    )
    ->execute();

  if (module_exists('i18n_string')) {
    $i18n_id = islandora_solr_metadata_config_i18n_id($id);
    i18n_string_update($i18n_id, $configuration_name);
  }

  return $id;
}

/**
 * Updates existing fields in a configuration given configuration ID.
 *
 * @param int $machine_name
 *   The configuration ID of the configuration which to add to.
 * @param bool $translate
 *   Whether the output needs to be translated.
 *
 * @return array
 *   An associative array containing:
 *   -description_field: The Solr field used to generate the description.
 *   -description_label: The display label associated with the description.
 */
function islandora_ui_retrieve_configuration($machine_name) {
  $result = db_query('SELECT machine_name, label, search_field, filters FROM {islandora_ui_autocomplete} WHERE machine_name = :machine_name', array(':machine_name' => $machine_name));
  $config = $result->fetchAssoc();
  if ($config['filters']) {
    $config['filters'] = unserialize($config['filters']);
  }
  return $config;
}


/**
 * Delete the given configuration indicated by the given ID.
 *
 * @param int $machine_name
 *   The ID for a configuration to remove.
 */
function islandora_ui_delete_configuration($machine_name) {
  // FIXME IMplement i18n
  /*
  if (module_exists('i18n_string')) {
    $fields = islandora_solr_metadata_get_fields($machine_name, FALSE);
    foreach (array_keys($fields) as $field) {
      $i18n_id = islandora_solr_metadata_field_i18n_id($machine_name, $field);
      i18n_string_remove($i18n_id);
    }
  }
  */
  db_delete('islandora_ui_autocomplete')
    ->condition('machine_name', $machine_name)
    ->execute();
}
